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

CC      ?= gcc
CXX     ?= g++
BUILD    = build
# omni-bot interface: qagame embeds the C++ glue (g_etbot_interface.cpp) plus the
# runtime loader (Omnibot/Common/BotLoadLibrary.cpp) so it can load omnibot_et and
# drive the bots - like the official qagame (hence libstdc++).
OB_INC   = -I../../../Omnibot/Common -I../../../Omnibot/ET

# --- Ziel: Linux (.so) ODER Windows-DLL via MinGW-Cross ---
# Linux:   make ARCH=i386|x86_64|aarch64
#            -> qagame.mp.<arch>.so   cgame.mp.<arch>.so   ui.mp.<arch>.so
# Windows: make WIN=x64 CC=x86_64-w64-mingw32-gcc CXX=x86_64-w64-mingw32-g++
#          make WIN=x86 CC=i686-w64-mingw32-gcc   CXX=i686-w64-mingw32-g++
#            -> qagame_mp_<win>.dll   cgame_mp_<win>.dll   ui_mp_<win>.dll
ifdef WIN
  # MinGW cross -> self-contained Windows DLL. Static runtime (libgcc/libstdc++
  # baked in) so the dll needs no MinGW runtime DLLs. Exports come from the .def.
  CFLAGS     = -O2 -fno-strict-aliasing -fcommon -w
  LDFLAGS    = -shared -static -static-libgcc
  # qagame + cgame are C++ (omni-bot interface / interprocess) -> bake in libstdc++.
  QAGAME_XLD = -static-libstdc++
  CGAME_XLD  = -static-libstdc++
  QAGAME_LIB = qagame_mp_$(WIN).dll
  CGAME_LIB  = cgame_mp_$(WIN).dll
  UI_LIB     = ui_mp_$(WIN).dll
  QAGAME_DEF = game/game.def
  CGAME_DEF  = cgame/cgame.def
  UI_DEF     = ui/ui.def
else ifdef MAC
  # macOS native (run on a macos-latest runner): universal x86_64+arm64 Mach-O.
  #   make MAC=1 CC=clang CXX=clang++
  # Only vmMain/dllEntry are exported (mac symbols carry a leading underscore).
  CFLAGS     = -arch x86_64 -arch arm64 -O2 -fno-strict-aliasing -fcommon -w \
               -Dstricmp=strcasecmp -D_stricmp=strcasecmp
  LDFLAGS    = -arch x86_64 -arch arm64 -dynamiclib \
               -Wl,-exported_symbols_list,mac_exports.sym
  QAGAME_XLD =                              # mac dlopen() lives in libSystem
  CGAME_XLD  =
  QAGAME_LIB = qagame_mac
  CGAME_LIB  = cgame_mac
  UI_LIB     = ui_mac
else
  ARCH ?= i386
  ifeq ($(ARCH),i386)
    ARCHFLAG := -m32
    SOSUFFIX := i386
  else ifeq ($(ARCH),x86_64)
    ARCHFLAG := -m64
    SOSUFFIX := x86_64
  else
    # cross targets (e.g. aarch64): the toolchain selects the arch, no -m flag.
    ARCHFLAG :=
    SOSUFFIX := $(ARCH)
  endif
  # -fno-strict-aliasing + -w: alter ET-Code. -fcommon: mehrfach-def. globals (GCC10+).
  CFLAGS     = $(ARCHFLAG) -fPIC -O2 -fno-strict-aliasing -fcommon -w \
               -Dstricmp=strcasecmp -D_stricmp=strcasecmp
  LDFLAGS    = $(ARCHFLAG) -shared -lm
  QAGAME_XLD = -ldl                         # dlopen() of omnibot_et.so
  CGAME_XLD  =                              # cgame is C++ (libstdc++ via g++ link)
  QAGAME_LIB = qagame.mp.$(SOSUFFIX).so
  CGAME_LIB  = cgame.mp.$(SOSUFFIX).so
  UI_LIB     = ui.mp.$(SOSUFFIX).so
endif

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
  cgame/cg_view.c cgame/cg_weapons.c cgame/cg_window.c cgame/cg_omnibot.c \
  game/bg_animation.c game/bg_animgroup.c game/bg_character.c game/bg_classes.c \
  game/bg_misc.c game/bg_pmove.c game/bg_slidemove.c game/bg_sscript.c game/bg_stats.c \
  game/bg_tracemap.c game/q_math.c game/q_shared.c \
  ui/ui_shared.c

# --- ui: ui/ + einige game/ (1:1 aus SConscript.ui, Define UIDLL) ---
UI_SRC = \
  ui/ui_atoms.c ui/ui_gameinfo.c ui/ui_loadpanel.c ui/ui_main.c \
  ui/ui_players.c ui/ui_shared.c ui/ui_syscalls.c ui/ui_util.c \
  game/bg_campaign.c game/bg_classes.c game/bg_misc.c \
  game/q_math.c game/q_shared.c

# ------------------------------------------------------------
#  Objekt-Listen (in build/ gespiegelt)
# ------------------------------------------------------------
QAGAME_OBJ = $(addprefix $(BUILD)/qagame/,$(QAGAME_SRC:.c=.o))
CGAME_OBJ  = $(addprefix $(BUILD)/cgame/,$(CGAME_SRC:.c=.o))
UI_OBJ     = $(addprefix $(BUILD)/ui/,$(UI_SRC:.c=.o))

# qagame C++ objects: the omni-bot glue (in game/) + the loader (in Omnibot/Common).
QAGAME_CPP_OBJ = $(BUILD)/qagame/game/g_etbot_interface.o \
                 $(BUILD)/qagame/ob/BotLoadLibrary.o
# cgame C++ object: the omni-bot interprocess/export glue.
CGAME_CPP_OBJ  = $(BUILD)/cgame/cgame/Omnibot_Interprocess.o

.PHONY: all qagame cgame ui clean
all: qagame cgame ui

qagame: $(QAGAME_LIB)
cgame:  $(CGAME_LIB)
ui:     $(UI_LIB)

$(QAGAME_LIB): $(QAGAME_OBJ) $(QAGAME_CPP_OBJ)
	@echo "==> Linking $@"
	$(CXX) $(QAGAME_OBJ) $(QAGAME_CPP_OBJ) $(LDFLAGS) $(QAGAME_XLD) $(QAGAME_DEF) -o $@
	@echo "==> Fertig: $@"

$(CGAME_LIB): $(CGAME_OBJ) $(CGAME_CPP_OBJ)
	@echo "==> Linking $@"
	$(CXX) $(CGAME_OBJ) $(CGAME_CPP_OBJ) $(LDFLAGS) $(CGAME_XLD) $(CGAME_DEF) -o $@
	@echo "==> Fertig: $@"

# qagame-Objekte brauchen -DGAMEDLL
$(BUILD)/qagame/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -DGAMEDLL -c $< -o $@

# qagame C++ glue (omni-bot interface) - braucht die Omnibot-Header.
$(BUILD)/qagame/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CFLAGS) $(OB_INC) -DGAMEDLL -c $< -o $@

# Loader liegt ausserhalb von src/ (in Omnibot/Common) - eigene Regel.
$(BUILD)/qagame/ob/BotLoadLibrary.o: ../../../Omnibot/Common/BotLoadLibrary.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CFLAGS) $(OB_INC) -DGAMEDLL -c $< -o $@

# cgame-Objekte brauchen -DCGAMEDLL
$(BUILD)/cgame/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -DCGAMEDLL -c $< -o $@

# cgame C++ glue (omni-bot interprocess) - braucht die Omnibot-Header.
$(BUILD)/cgame/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CFLAGS) $(OB_INC) -DCGAMEDLL -c $< -o $@

$(UI_LIB): $(UI_OBJ)
	@echo "==> Linking $@"
	$(CC) $(UI_OBJ) $(LDFLAGS) $(UI_DEF) -o $@
	@echo "==> Fertig: $@"

# ui-Objekte brauchen -DUIDLL
$(BUILD)/ui/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -DUIDLL -c $< -o $@

clean:
	rm -rf $(BUILD) $(QAGAME_LIB) $(CGAME_LIB) $(UI_LIB)
