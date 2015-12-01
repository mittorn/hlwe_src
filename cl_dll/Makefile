CC= gcc -m32
CXX = g++ -m32
SRCS +=./rendersystem/Particle.cpp
SRCS +=./rendersystem/ParticleSystem.cpp
SRCS +=./rendersystem/PSBeam.cpp
SRCS +=./rendersystem/PSBlastCone.cpp
SRCS +=./rendersystem/PSBlood.cpp
SRCS +=./rendersystem/PSBubbles.cpp
SRCS +=./rendersystem/PSBubbleTrail.cpp
SRCS +=./rendersystem/PSDrips.cpp
SRCS +=./rendersystem/PSFlatTrail.cpp
SRCS +=./rendersystem/PSGravityPart.cpp
SRCS +=./rendersystem/PSSparks.cpp
SRCS +=./rendersystem/PSSparkShower.cpp
SRCS +=./rendersystem/PSSpawnEffect.cpp
SRCS +=./rendersystem/PSStreaks.cpp
SRCS +=./rendersystem/PSTrail.cpp
SRCS +=./rendersystem/PSWallGibs.cpp
SRCS +=./rendersystem/rain.cpp
SRCS +=./rendersystem/RenderManager.cpp
SRCS +=./rendersystem/RenderSystem.cpp
SRCS +=./rendersystem/RotatingSystem.cpp
SRCS +=./rendersystem/RSBeam.cpp
SRCS +=./rendersystem/RSBeamStar.cpp
SRCS +=./rendersystem/RSCylinder.cpp
SRCS +=./rendersystem/RSDisk.cpp
SRCS +=./rendersystem/RSLight.cpp
SRCS +=./rendersystem/RSSprite.cpp
SRCS +=./rendersystem/RSTeleparts.cpp
SRCS +=./hud_icons/hud_antigrav_icon.cpp
SRCS +=./hud_icons/hud_battery.cpp
SRCS +=./hud_icons/hud_buyzone.cpp
SRCS +=./hud_icons/hud_cloak_icon.cpp
SRCS +=./hud_icons/hud_firemode.cpp
SRCS +=./hud_icons/hud_flashlight.cpp
SRCS +=./hud_icons/hud_harmor_icon.cpp
SRCS +=./hud_icons/hud_kevlar_icon.cpp
SRCS +=./hud_icons/hud_longjump_icon.cpp
SRCS +=./hud_icons/hud_money_icon.cpp
SRCS +=./hud_icons/hud_PHEV_icon.cpp
SRCS +=./hud_icons/hud_PHK_icon.cpp
SRCS +=./hud_icons/hud_pshield_icon.cpp
SRCS +=./hud_icons/hud_pt_icon.cpp
SRCS +=./hud_icons/hud_scope.cpp
SRCS +=./hud_icons/hud_Timer_icon.cpp
SRCS +=./hud_icons/hud_train.cpp
SRCS +=./hud_icons/hud_turretI_icon.cpp
SRCS +=./hud_icons/hud_turretM_icon.cpp
SRCS +=./hud_icons/hud_turretS_icon.cpp
SRCS +=./hud_icons/hud_warhead.cpp
SRCS +=./ammo.cpp
SRCS +=./ammo_secondary.cpp
SRCS +=./ammohistory.cpp
#SRCS +=./buymenu.cpp
SRCS +=./cdll_int.cpp
#SRCS +=./classmenu.cpp
SRCS +=./death.cpp
SRCS +=./demo.cpp
SRCS +=./entity.cpp
SRCS +=./ev_common.cpp
SRCS +=./game_fx.cpp
SRCS +=./GameStudioModelRenderer.cpp
SRCS +=./health.cpp
SRCS +=./hud.cpp
SRCS +=./hud_msg.cpp
SRCS +=./hud_redraw.cpp
#SRCS +=./hud_servers.cpp
SRCS +=./hud_spectator.cpp
SRCS +=./hud_update.cpp
SRCS +=./in_camera.cpp
SRCS +=./input.cpp
SRCS +=./input_stub.cpp
SRCS +=../common/interface.cpp
SRCS +=./menu.cpp
SRCS +=./message.cpp
SRCS +=./msgmgr.cpp
SRCS +=./overview.cpp
SRCS +=./parsemsg.cpp
SRCS +=./r_particle.cpp
SRCS +=./saytext.cpp
SRCS +=./statusbar.cpp
SRCS +=./studio_util.cpp
SRCS +=./StudioModelRenderer.cpp
SRCS +=./text_message.cpp
SRCS +=./tri.cpp
SRCS +=./util.cpp
SRCS +=./view.cpp
SRCS +=./scoreboard_stub.cpp

SRCS_C +=../pm_shared/pm_debug.c
SRCS_C +=../pm_shared/pm_math.c
SRCS_C +=../pm_shared/pm_shared.c

INCLUDES = -Irendersystem  -I../common -I. -I../game_shared -I../pm_shared -I../engine -I../dlls
DEFINES = -Wno-write-strings -DLINUX -D_LINUX -Dstricmp=strcasecmp -D_strnicmp=strncasecmp -Dstrnicmp=strncasecmp -DCLIENT_WEAPONS -DCLIENT_DLL -fpermissive -w
CFLAGS = -Og -ggdb
OBJS = $(SRCS:.cpp=.o) $(SRCS_C:.c=.o)
%.o : %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFINES) -fPIC -c $< -o $@

%.o : %.cpp
	$(CXX) $(CFLAGS) $(INCLUDES) $(DEFINES) -fPIC -c $< -o $@
client.so : $(OBJS)
	$(CXX) $(OBJS) -o client.so -shared -Wl,--no-undefined -fPIC -lm -ldl

clean:
	$(RM) $(OBJS)
