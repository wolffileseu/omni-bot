# ============================================================
#  ET Mod Build (qagame + cgame) - Linux i386
#  Ersetzt das tote Python-2-SCons.
#  Im Ordner  0.83/GameInterfaces/ET/src  ablegen und  `make`  ausfuehren.
#
#  Targets:
#    make            -> baut beide (qagame.mp.i386.so + cgame.mp.i386.so)
#    make qagame     -> nur qagame
#    make cgame      -> nur cgame
#    make clean      -> Build-Ordner loeschen
#    make ARCH=x86_64-> 64-bit statt 32-bit bauen
# ============================================================

# --- Architektur (Standard: i386 = 32-bit, passend zum Server) ---
ARCH ?= i386
ifeq ($(ARCH),i386)
  ARCHFLAG := -m32
  SOSUFFIX := i386
else
  ARCHFLAG := -m64
  SOSUFFIX := $(ARCH)
endif

CC      ?= gcc
BUILD    = build
# -fno-strict-aliasing + -w : alter ET-Code bricht sonst / spammt Warnungen.
# -fcommon : alter Code hat mehrfach-definierte globals (GCC10+ braucht das).
CFLAGS   = $(ARCHFLAG) -fPIC -O2 -fno-strict-aliasing -fcommon -w \
           -Dstricmp=strcasecmp -D_stricmp=strcasecmp
LDFLAGS  = $(ARCHFLAG) -shared -lm

# ------------------------------------------------------------
#  Quelldateien (1:1 aus den SConscripts uebernommen)
# ------------------------------------------------------------

# --- qagame: botai + game (alle relativ zu game/) ---
QAGAME_SRC = \
  botai/ai_cmd.c botai/ai_dmgoal_mp.c botai/ai_dmnet_mp.c botai/ai_dmq3.c \
  botai/ai_main.c botai/ai_script.c botai/ai_script_actions.c botai/ai_team.c \
  game/bg_animation.c game/bg_animgroup.c game/bg_campaign.c game/bg_character.c \
  game/bg_classes.c game/bg_misc.c game/bg_pmove.c game/bg_slidemove.c \
  game/bg_sscript.c game/bg_stats.c game/bg_tracemap.c \
  game/g_active.c game/g_alarm.c game/g_antilag.c game/g_bot.c game/g_buddy_list.c \
  game/g_character.c game/g_client.c game/g_cmds.c game/g_cmds_ext.c game/g_combat.c \
  game/g_config.c game/g_fireteams.c game/g_items.c game/g_main.c game/g_match.c \
  game/g_mem.c game/g_misc.c game/g_missile.c game/g_mover.c game/g_multiview.c \
  game/g_props.c game/g_referee.c game/g_save.c game/g_script.c game/g_script_actions.c \
  game/g_session.c game/g_spawn.c game/g_stats.c game/g_sv_entities.c game/g_svcmds.c \
  game/g_syscalls.c game/g_systemmsg.c game/g_target.c game/g_team.c game/g_teammapdata.c \
  game/g_trigger.c game/g_utils.c game/g_vote.c game/g_weapon.c \
  game/q_math.c game/q_shared.c

# Falls der omnibot-Interface-Code mitkompiliert werden muss:
# game/g_etbot_interface.cpp  -> separat, weil C++ (siehe unten optional)

# --- cgame: cgame/ + einige game/ + ui/ ---
CGAME_SRC = \
  cgame/cg_atmospheric.c cgame/cg_character.c cgame/cg_commandmap.c cgame/cg_consolecmds.c \
  cgame/cg_debriefing.c cgame/cg_draw.c cgame/cg_drawtools.c cgame/cg_effects.c \
  cgame/cg_ents.c cgame/cg_event.c cgame/cg_fireteamoverlay.c cgame/cg_fireteams.c \
  cgame/cg_flamethrower.c cgame/cg_info.c cgame/cg_limbopanel.c cgame/cg_loadpanel.c \
  cgame/cg_localents.c cgame/cg_main.c cgame/cg_marks.c cgame/cg_missionbriefing.c \
  cgame/cg_multiview.c cgame/cg_newDraw.c cgame/cg_particles.c cgame/cg_players.c \
  cgame/cg_playerstate.c cgame/cg_polybus.c cgame/cg_popupmessages.c cgame/cg_predict.c \
  cgame/cg_scoreboard.c cgame/cg_servercmds.c cgame/cg_snapshot.c cgame/cg_sound.c \
  cgame/cg_spawn.c cgame/cg_statsranksmedals.c cgame/cg_syscalls.c cgame/cg_trails.c \
  cgame/cg_view.c cgame/cg_weapons.c cgame/cg_window.c \
  game/bg_animation.c game/bg_animgroup.c game/bg_character.c game/bg_classes.c \
  game/bg_misc.c game/bg_pmove.c game/bg_slidemove.c game/bg_sscript.c game/bg_stats.c \
  game/bg_tracemap.c game/q_math.c game/q_shared.c \
  ui/ui_shared.c

# ------------------------------------------------------------
#  Objekt-Listen (in build/ gespiegelt)
# ------------------------------------------------------------
QAGAME_OBJ = $(addprefix $(BUILD)/qagame/,$(QAGAME_SRC:.c=.o))
CGAME_OBJ  = $(addprefix $(BUILD)/cgame/,$(CGAME_SRC:.c=.o))

QAGAME_LIB = qagame.mp.$(SOSUFFIX).so
CGAME_LIB  = cgame.mp.$(SOSUFFIX).so

.PHONY: all qagame cgame clean
all: qagame cgame

qagame: $(QAGAME_LIB)
cgame:  $(CGAME_LIB)

$(QAGAME_LIB): $(QAGAME_OBJ)
	@echo "==> Linking $@"
	$(CC) $(QAGAME_OBJ) $(LDFLAGS) -o $@
	@echo "==> Fertig: $@"

$(CGAME_LIB): $(CGAME_OBJ)
	@echo "==> Linking $@"
	$(CC) $(CGAME_OBJ) $(LDFLAGS) -o $@
	@echo "==> Fertig: $@"

# qagame-Objekte brauchen -DGAMEDLL
$(BUILD)/qagame/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -DGAMEDLL -c $< -o $@

# cgame-Objekte brauchen -DCGAMEDLL
$(BUILD)/cgame/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -DCGAMEDLL -c $< -o $@

clean:
	rm -rf $(BUILD) $(QAGAME_LIB) $(CGAME_LIB)
