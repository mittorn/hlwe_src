# Microsoft Developer Studio Project File - Name="hl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=hl - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "hl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "hl.mak" CFG="hl - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "hl - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "hl - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "hl - Win32 Profile" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "hl - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Releasehl"
# PROP Intermediate_Dir ".\Releasehl\dlls"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MT /W3 /GX /Zi /O2 /I "..." /I "gamerules" /I "..\." /I "parser" /I "bots" /I "monsters" /I "projectiles" /I "weapons" /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\game_shared" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "QUIVER" /D "VOXEL" /D "QUAKE2" /D "VALVE_DLL" /D "CLIENT_WEAPONS" /Fr /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /def:".\hl.def"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\hl___Win"
# PROP BASE Intermediate_Dir ".\hl___Win"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\debughl"
# PROP Intermediate_Dir ".\debughl"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\dlls" /I "gamerules" /I "..\." /I "parser" /I "bots" /I "monsters" /I "projectiles" /I "weapons" /I "..\engine" /I "..\common" /I "..\game_shared" /I "..\pm_shared" /I "..\\" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "QUIVER" /D "VOXEL" /D "QUAKE2" /D "VALVE_DLL" /D "CLIENT_WEAPONS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\engine" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 user32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /def:".\hl.def" /implib:".\Debug\hl.lib"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "hl - Win32 Profile"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\hl___Win"
# PROP BASE Intermediate_Dir ".\hl___Win"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Profilehl"
# PROP Intermediate_Dir ".\Profilehl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MT /W3 /GX /Zi /O2 /I "..\engine" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "QUIVER" /D "VOXEL" /D "QUAKE2" /D "VALVE_DLL" /YX /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /G5 /MT /W3 /Zi /O2 /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\game_shared" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "QUIVER" /D "VOXEL" /D "QUAKE2" /D "VALVE_DLL" /D "CLIENT_WEAPONS" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /def:".\hl.def"
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /profile /debug /machine:I386 /def:".\hl.def"

!ENDIF 

# Begin Target

# Name "hl - Win32 Release"
# Name "hl - Win32 Debug"
# Name "hl - Win32 Profile"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Group "weapons"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\weapons\wpn_30mmsg.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_AK74.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_akimbogun.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_autoshotgun.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_awp.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_bandsaw.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_barett.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_BFG.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_biorifle.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_blaster.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_C4detpack.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_chronosceptor.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_crossbow.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_crowbar.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_deagle.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_devastator.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_displacer.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_egon.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_flakcannon.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_flamethrower.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_flashbang.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_froster.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_g11.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_gauss.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_glock.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_glock_akimbo.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_gluongun.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_handgrenade.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_incendiary.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_lightsaber.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_M16.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_m249.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_m72.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_machinegun.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_medkit.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_minigun.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_mp5.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_nailgun.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_photongun.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_plasmarifle.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_pulserifle.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_python.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_redeemer.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_rpg.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_satchel.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_satellite.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_shieldgun.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_shotgun.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_smartgun.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_svd.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_taucannon.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_teslagun.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_torch.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_tripmine.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_turretkit.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_u2.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_usp.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_uzi.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_uzi_akimbo.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons\wpn_WHL.cpp
# End Source File
# End Group
# Begin Group "projectiles"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\projectiles\mon_proj_guts.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_30mmgren.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_AK74grenade.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_biomass.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_blackhole.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_bolt.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_C4.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_chronoclip.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_clbaby.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_clmomma.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_dispball.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_dumbfire.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_enegrycharge.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_flakbomb.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_flame.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_flashbang.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_frostball.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_fxspawner.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_fxspawner2.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_gluon.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_gluon2.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_grenade.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_incendiary.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_m203gren.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_mmissile.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_NGgren.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_nuke.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_pbolt.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_plasma.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_plasma2.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_pulse.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_rpg.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_satchel.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_satellite.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_shielddet.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_shock.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_shrapnel.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_sunofgod.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_tankproj.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_teleenter.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_teslagren.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_tripmine.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_U2Baby.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_U2Momma.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_whl.cpp
# End Source File
# End Group
# Begin Group "gamerules"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\gamerules\ctf_gamerules.cpp
# End Source File
# Begin Source File

SOURCE=.\gamerules\game.cpp
# End Source File
# Begin Source File

SOURCE=.\gamerules\gamerules.cpp
# End Source File
# Begin Source File

SOURCE=.\gamerules\lms_gamerules.cpp
# End Source File
# Begin Source File

SOURCE=.\gamerules\maprules.cpp
# End Source File
# Begin Source File

SOURCE=.\gamerules\multiplay_gamerules.cpp
# End Source File
# Begin Source File

SOURCE=.\gamerules\singleplay_gamerules.cpp
# End Source File
# Begin Source File

SOURCE=.\gamerules\teamplay_gamerules.cpp
# End Source File
# End Group
# Begin Group "monsters"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\monsters\genericmonster.cpp
# End Source File
# Begin Source File

SOURCE=.\monsters\mon_rat.cpp
# End Source File
# Begin Source File

SOURCE=.\monsters\mon_zombie.cpp
# End Source File
# Begin Source File

SOURCE=.\monsters\monstermaker.cpp
# End Source File
# Begin Source File

SOURCE=.\monsters\monsters.cpp
# End Source File
# Begin Source File

SOURCE=.\monsters\monsterstate.cpp
# End Source File
# Begin Source File

SOURCE=.\monsters\schedule.cpp
# End Source File
# Begin Source File

SOURCE=.\monsters\scripted.cpp
# End Source File
# End Group
# Begin Group "parser"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\parser\ss_consttable.h
# End Source File
# Begin Source File

SOURCE=.\parser\ss_error.cpp
# End Source File
# Begin Source File

SOURCE=.\parser\ss_error.h
# End Source File
# Begin Source File

SOURCE=.\parser\ss_functions.h
# End Source File
# Begin Source File

SOURCE=.\parser\ss_kernel.cpp
# End Source File
# Begin Source File

SOURCE=.\parser\ss_kernel.h
# End Source File
# Begin Source File

SOURCE=.\parser\ss_keywords.cpp
# End Source File
# Begin Source File

SOURCE=.\parser\ss_keywords.h
# End Source File
# Begin Source File

SOURCE=.\parser\ss_map.h
# End Source File
# Begin Source File

SOURCE=.\parser\ss_mgr.h
# End Source File
# Begin Source File

SOURCE=.\parser\ss_mystack.h
# End Source File
# Begin Source File

SOURCE=.\parser\ss_operators.cpp
# End Source File
# Begin Source File

SOURCE=.\parser\ss_operators.h
# End Source File
# Begin Source File

SOURCE=.\parser\ss_parser.cpp
# End Source File
# Begin Source File

SOURCE=.\parser\ss_parser.h
# End Source File
# Begin Source File

SOURCE=.\parser\ss_scope.h
# End Source File
# Begin Source File

SOURCE=.\parser\ss_structures.h
# End Source File
# Begin Source File

SOURCE=.\parser\ss_types.h
# End Source File
# End Group
# Begin Group "bot"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\bot\bot.cpp
# End Source File
# Begin Source File

SOURCE=.\bot\bot_combat.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ammo.cpp
# End Source File
# Begin Source File

SOURCE=.\animating.cpp
# End Source File
# Begin Source File

SOURCE=.\animation.cpp
# End Source File
# Begin Source File

SOURCE=.\bmodels.cpp
# End Source File
# Begin Source File

SOURCE=.\buttons.cpp
# End Source File
# Begin Source File

SOURCE=.\cbase.cpp
# End Source File
# Begin Source File

SOURCE=.\client.cpp
# End Source File
# Begin Source File

SOURCE=.\combat.cpp
# End Source File
# Begin Source File

SOURCE=.\defaultai.cpp
# End Source File
# Begin Source File

SOURCE=.\doors.cpp
# End Source File
# Begin Source File

SOURCE=.\effects.cpp
# End Source File
# Begin Source File

SOURCE=.\explode.cpp
# End Source File
# Begin Source File

SOURCE=.\func_break.cpp
# End Source File
# Begin Source File

SOURCE=.\func_tank.cpp
# End Source File
# Begin Source File

SOURCE=.\func_turret.cpp
# End Source File
# Begin Source File

SOURCE=.\globals.cpp
# End Source File
# Begin Source File

SOURCE=.\h_ai.cpp
# End Source File
# Begin Source File

SOURCE=.\h_cycler.cpp
# End Source File
# Begin Source File

SOURCE=.\h_export.cpp
# End Source File
# Begin Source File

SOURCE=.\items.cpp
# End Source File
# Begin Source File

SOURCE=.\lights.cpp
# End Source File
# Begin Source File

SOURCE=.\mortar.cpp
# End Source File
# Begin Source File

SOURCE=.\msg_fx.cpp
# End Source File
# Begin Source File

SOURCE=.\nodes.cpp
# End Source File
# Begin Source File

SOURCE=.\nullents.cpp
# End Source File
# Begin Source File

SOURCE=.\observer.cpp
# End Source File
# Begin Source File

SOURCE=.\pathcorner.cpp
# End Source File
# Begin Source File

SOURCE=.\player.cpp
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_debug.c
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_math.c
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_shared.c
# End Source File
# Begin Source File

SOURCE=.\recharger.cpp
# End Source File
# Begin Source File

SOURCE=.\skill.cpp
# End Source File
# Begin Source File

SOURCE=.\sound.cpp
# End Source File
# Begin Source File

SOURCE=.\soundent.cpp
# End Source File
# Begin Source File

SOURCE=.\spectator.cpp
# End Source File
# Begin Source File

SOURCE=.\subs.cpp
# End Source File
# Begin Source File

SOURCE=.\trains.cpp
# End Source File
# Begin Source File

SOURCE=.\triggers.cpp
# End Source File
# Begin Source File

SOURCE=.\turret.cpp
# End Source File
# Begin Source File

SOURCE=.\util.cpp
# End Source File
# Begin Source File

SOURCE=..\game_shared\voice_gamemgr.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons.cpp
# End Source File
# Begin Source File

SOURCE=.\world.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\monsters\activity.h
# End Source File
# Begin Source File

SOURCE=.\monsters\activitymap.h
# End Source File
# Begin Source File

SOURCE=.\animation.h
# End Source File
# Begin Source File

SOURCE=.\monsters\basemonster.h
# End Source File
# Begin Source File

SOURCE=.\cbase.h
# End Source File
# Begin Source File

SOURCE=.\cdll_dll.h
# End Source File
# Begin Source File

SOURCE=.\client.h
# End Source File
# Begin Source File

SOURCE=.\decals.h
# End Source File
# Begin Source File

SOURCE=.\defaultai.h
# End Source File
# Begin Source File

SOURCE=.\doors.h
# End Source File
# Begin Source File

SOURCE=.\effects.h
# End Source File
# Begin Source File

SOURCE=..\engine\eiface.h
# End Source File
# Begin Source File

SOURCE=.\enginecallback.h
# End Source File
# Begin Source File

SOURCE=.\explode.h
# End Source File
# Begin Source File

SOURCE=.\extdll.h
# End Source File
# Begin Source File

SOURCE=.\func_break.h
# End Source File
# Begin Source File

SOURCE=.\gamerules\game.h
# End Source File
# Begin Source File

SOURCE=.\gamerules\gamerules.h
# End Source File
# Begin Source File

SOURCE=.\items.h
# End Source File
# Begin Source File

SOURCE=.\gamerules\maprules.h
# End Source File
# Begin Source File

SOURCE=.\projectiles\mon_projectiles.h
# End Source File
# Begin Source File

SOURCE=.\money.h
# End Source File
# Begin Source File

SOURCE=.\monsters\monsterevent.h
# End Source File
# Begin Source File

SOURCE=.\monsters\monsters.h
# End Source File
# Begin Source File

SOURCE=.\nodes.h
# End Source File
# Begin Source File

SOURCE=.\player.h
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_debug.h
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_defs.h
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_info.h
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_materials.h
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_movevars.h
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_shared.h
# End Source File
# Begin Source File

SOURCE=.\projectiles\projectiles.h
# End Source File
# Begin Source File

SOURCE=.\saverestore.h
# End Source File
# Begin Source File

SOURCE=.\monsters\schedule.h
# End Source File
# Begin Source File

SOURCE=.\monsters\scripted.h
# End Source File
# Begin Source File

SOURCE=.\monsters\scriptevent.h
# End Source File
# Begin Source File

SOURCE=.\skill.h
# End Source File
# Begin Source File

SOURCE=.\soundent.h
# End Source File
# Begin Source File

SOURCE=.\spectator.h
# End Source File
# Begin Source File

SOURCE=.\gamerules\teamplay_gamerules.h
# End Source File
# Begin Source File

SOURCE=.\trains.h
# End Source File
# Begin Source File

SOURCE=.\util.h
# End Source File
# Begin Source File

SOURCE=.\vector.h
# End Source File
# Begin Source File

SOURCE=.\weapons.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
