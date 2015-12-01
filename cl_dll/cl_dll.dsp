# Microsoft Developer Studio Project File - Name="cl_dll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=cl_dll - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "cl_dll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cl_dll.mak" CFG="cl_dll - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cl_dll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "cl_dll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "cl_dll - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release\cl_dll"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "." /I "rendersystem" /I "..\hud_icons" /I "..\utils\vgui\include" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\dlls" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "CLIENT_DLL" /D "CLIENT_WEAPONS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib winmm.lib ../utils/vgui/lib/win32_vc6/vgui.lib wsock32.lib /nologo /subsystem:windows /dll /map /machine:I386 /out:".\Release\client.dll"

!ELSEIF  "$(CFG)" == "cl_dll - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MTd /W3 /Gm /GR /GX /ZI /Od /I "." /I "..\hud_icons" /I "rendersystem" /I "..\utils\vgui\include" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\dlls" /D "_DEBUG" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "CLIENT_DLL" /D "CLIENT_WEAPONS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib winmm.lib ../utils/vgui/lib/win32_vc6/vgui.lib wsock32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:".\Debug\client.dll"

!ENDIF 

# Begin Target

# Name "cl_dll - Win32 Release"
# Name "cl_dll - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Group "Rendersystem"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\rendersystem\Particle.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\ParticleSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSBeam.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSBlastCone.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSBlood.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSBubbles.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSBubbleTrail.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSDrips.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSFlatTrail.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSGravityPart.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSSparks.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSSparkShower.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSSpawnEffect.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSStreaks.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSTrail.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSWallGibs.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\rain.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RenderManager.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RenderSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RotatingSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RSBeam.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RSBeamStar.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RSCylinder.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RSDisk.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RSLight.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RSSprite.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RSTeleparts.cpp
# End Source File
# End Group
# Begin Group "hud_icons"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\hud_icons\hud_antigrav_icon.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_icons\hud_battery.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_icons\hud_buyzone.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_icons\hud_cloak_icon.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_icons\hud_firemode.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_icons\hud_flashlight.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_icons\hud_harmor_icon.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_icons\hud_kevlar_icon.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_icons\hud_longjump_icon.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_icons\hud_money_icon.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_icons\hud_PHEV_icon.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_icons\hud_PHK_icon.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_icons\hud_pshield_icon.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_icons\hud_pt_icon.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_icons\hud_scope.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_icons\hud_Timer_icon.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_icons\hud_train.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_icons\hud_turretI_icon.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_icons\hud_turretM_icon.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_icons\hud_turretS_icon.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_icons\hud_warhead.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ammo.cpp
# End Source File
# Begin Source File

SOURCE=.\ammo_secondary.cpp
# End Source File
# Begin Source File

SOURCE=.\ammohistory.cpp
# End Source File
# Begin Source File

SOURCE=.\buymenu.cpp
# End Source File
# Begin Source File

SOURCE=.\cdll_int.cpp
# End Source File
# Begin Source File

SOURCE=.\classmenu.cpp
# End Source File
# Begin Source File

SOURCE=.\death.cpp
# End Source File
# Begin Source File

SOURCE=.\demo.cpp
# End Source File
# Begin Source File

SOURCE=.\entity.cpp
# End Source File
# Begin Source File

SOURCE=.\ev_common.cpp
# End Source File
# Begin Source File

SOURCE=.\game_fx.cpp
# End Source File
# Begin Source File

SOURCE=.\GameStudioModelRenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\health.cpp
# End Source File
# Begin Source File

SOURCE=.\hud.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_msg.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_redraw.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_servers.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_spectator.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_update.cpp
# End Source File
# Begin Source File

SOURCE=.\in_camera.cpp
# End Source File
# Begin Source File

SOURCE=.\input.cpp
# End Source File
# Begin Source File

SOURCE=.\inputw32.cpp
# End Source File
# Begin Source File

SOURCE=..\common\interface.cpp
# End Source File
# Begin Source File

SOURCE=.\menu.cpp
# End Source File
# Begin Source File

SOURCE=.\message.cpp
# End Source File
# Begin Source File

SOURCE=.\msgmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\overview.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\parsemsg.cpp
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

SOURCE=.\r_particle.cpp
# End Source File
# Begin Source File

SOURCE=.\saytext.cpp
# End Source File
# Begin Source File

SOURCE=.\statusbar.cpp
# End Source File
# Begin Source File

SOURCE=.\studio_util.cpp
# End Source File
# Begin Source File

SOURCE=.\StudioModelRenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\text_message.cpp
# End Source File
# Begin Source File

SOURCE=.\tri.cpp
# End Source File
# Begin Source File

SOURCE=.\util.cpp
# End Source File
# Begin Source File

SOURCE=..\game_shared\vgui_checkbutton2.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_ClassMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_ConsolePanel.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_ControlConfigPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_CustomObjects.cpp
# End Source File
# Begin Source File

SOURCE=..\game_shared\vgui_grid.cpp
# End Source File
# Begin Source File

SOURCE=..\game_shared\vgui_helpers.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_int.cpp
# End Source File
# Begin Source File

SOURCE=..\game_shared\vgui_listbox.cpp
# End Source File
# Begin Source File

SOURCE=..\game_shared\vgui_loadtga.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_MOTDWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_SchemeManager.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_ScorePanel.cpp
# End Source File
# Begin Source File

SOURCE=..\game_shared\vgui_scrollbar2.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_ServerBrowser.cpp
# End Source File
# Begin Source File

SOURCE=..\game_shared\vgui_slider2.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_SpectatorPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_TeamFortressViewport.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_teammenu.cpp
# End Source File
# Begin Source File

SOURCE=.\view.cpp
# End Source File
# Begin Source File

SOURCE=..\game_shared\voice_banmgr.cpp
# End Source File
# Begin Source File

SOURCE=..\game_shared\voice_status.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Group "Rendersystem headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\rendersystem\Particle.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\ParticleSystem.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSBeam.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSBlastCone.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSBlood.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSBubbles.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSBubbleTrail.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSDrips.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSFlatTrail.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSGravityPart.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSSparks.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSSparkShower.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSSpawnEffect.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSStreaks.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSTrail.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSWallGibs.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\rain.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RenderManager.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RenderSystem.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RotatingSystem.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RSBeam.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RSBeamStar.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RSCylinder.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RSDisk.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RSLight.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RSSprite.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RSTeleparts.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ammo.h
# End Source File
# Begin Source File

SOURCE=.\ammohistory.h
# End Source File
# Begin Source File

SOURCE=.\buymenu.h
# End Source File
# Begin Source File

SOURCE=.\camera.h
# End Source File
# Begin Source File

SOURCE=..\common\cdll_dll.h
# End Source File
# Begin Source File

SOURCE=.\com_weapons.h
# End Source File
# Begin Source File

SOURCE=.\demo.h
# End Source File
# Begin Source File

SOURCE=.\game_fx.h
# End Source File
# Begin Source File

SOURCE=.\GameStudioModelRenderer.h
# End Source File
# Begin Source File

SOURCE=.\health.h
# End Source File
# Begin Source File

SOURCE=.\hud.h
# End Source File
# Begin Source File

SOURCE=.\hud_iface.h
# End Source File
# Begin Source File

SOURCE=.\hud_servers.h
# End Source File
# Begin Source File

SOURCE=.\hud_servers_priv.h
# End Source File
# Begin Source File

SOURCE=.\hud_spectator.h
# End Source File
# Begin Source File

SOURCE=.\in_defs.h
# End Source File
# Begin Source File

SOURCE=..\common\itrackeruser.h
# End Source File
# Begin Source File

SOURCE=.\kbutton.h
# End Source File
# Begin Source File

SOURCE=.\overview.h
# End Source File
# Begin Source File

SOURCE=.\parsemsg.h
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

SOURCE=.\r_particle.h
# End Source File
# Begin Source File

SOURCE=.\studio_util.h
# End Source File
# Begin Source File

SOURCE=.\StudioModelRenderer.h
# End Source File
# Begin Source File

SOURCE=.\util.h
# End Source File
# Begin Source File

SOURCE=.\util_vector.h
# End Source File
# Begin Source File

SOURCE=.\vgui_ConsolePanel.h
# End Source File
# Begin Source File

SOURCE=.\vgui_ControlConfigPanel.h
# End Source File
# Begin Source File

SOURCE=.\vgui_int.h
# End Source File
# Begin Source File

SOURCE=.\vgui_SchemeManager.h
# End Source File
# Begin Source File

SOURCE=.\vgui_ScorePanel.h
# End Source File
# Begin Source File

SOURCE=.\vgui_ServerBrowser.h
# End Source File
# Begin Source File

SOURCE=.\vgui_SpectatorPanel.h
# End Source File
# Begin Source File

SOURCE=.\vgui_TeamFortressViewport.h
# End Source File
# Begin Source File

SOURCE=.\view.h
# End Source File
# Begin Source File

SOURCE=..\game_shared\voice_banmgr.h
# End Source File
# Begin Source File

SOURCE=..\game_shared\voice_status.h
# End Source File
# Begin Source File

SOURCE=..\game_shared\voice_vgui_tweakdlg.h
# End Source File
# Begin Source File

SOURCE=.\wrect.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
