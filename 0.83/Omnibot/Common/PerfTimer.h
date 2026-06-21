#ifndef __OMNIBOT_PERFTIMER_H__
#define __OMNIBOT_PERFTIMER_H__

// ---------------------------------------------------------------------------
// Block-2 diagnosis: opt-in CPU timing of the per-tick hot path.
//
// Compiled in, but OFF by default. Turn it on at runtime on the server with the
// cvar:   set omnibot_perf 1     (and  set omnibot_perf 0  to stop).
// When off the only cost is a couple of bool/int tests per tick - no clock()
// calls, no allocations. Set OMNIBOT_ENABLE_PERFTIMER to 0 below to strip the
// instrumentation from the build entirely.
//
// Every ~5 s it writes one INFO log line (omni-bot log file, i.e. requires
// omnibot_logsize > 0), e.g.:
//   PERF: UpdateGame avg 2.13 ms/tick, davon Targeting 1.41 ms, Sensory 0.38 ms over 102 ticks
//
// Timing uses clock() = process CPU time on Linux (CLOCKS_PER_SEC resolution),
// which is exactly what we want to know ("how much CPU is omni-bot's tick").
// UpdateGame = the whole bot tick; Targeting/Sensory = summed time across all
// bots within that tick. (On Windows clock() is wall-clock, but the target is
// the Linux server.)
// ---------------------------------------------------------------------------

#define OMNIBOT_ENABLE_PERFTIMER 1

#if OMNIBOT_ENABLE_PERFTIMER

#include <time.h>

class IGame;

struct PerfTimerState
{
	bool   m_Enabled;        // mirrors the omnibot_perf cvar, refreshed every ~5s
	double m_UpdateGameMs;   // accumulated ms in the current window
	double m_TargetingMs;
	double m_SensoryMs;
	int    m_Ticks;
	int    m_NextPollMsec;   // game-time of the next cvar poll / flush

	PerfTimerState()
		: m_Enabled(false), m_UpdateGameMs(0.0), m_TargetingMs(0.0)
		, m_SensoryMs(0.0), m_Ticks(0), m_NextPollMsec(0)
	{
	}
};
extern PerfTimerState g_PerfTimer;

// Call once per tick from IGameManager::UpdateGame (after the UpdateGame scope).
// Counts the tick, polls the omnibot_perf cvar every ~5s and flushes the average.
void PerfTimer_Frame(IGame *_game);

// RAII accumulator. When timing is disabled it does nothing at all (no clock()).
class PerfScope
{
public:
	explicit PerfScope(double *_target)
		: m_Target(g_PerfTimer.m_Enabled ? _target : 0)
		, m_Start(0)
	{
		if(m_Target)
			m_Start = clock();
	}
	~PerfScope()
	{
		if(m_Target)
			*m_Target += (double)(clock() - m_Start) * (1000.0 / (double)CLOCKS_PER_SEC);
	}
private:
	double *m_Target;
	clock_t m_Start;
};

#define OB_PERF_SCOPE(target) PerfScope __ob_perf_scope__(&(target))

#else  // !OMNIBOT_ENABLE_PERFTIMER

#define OB_PERF_SCOPE(target) ((void)0)

#endif // OMNIBOT_ENABLE_PERFTIMER

#endif // __OMNIBOT_PERFTIMER_H__
