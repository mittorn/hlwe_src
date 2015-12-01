#include "hud.h"
#include "cl_util.h"
#include "netadr.h"
//#include "vgui_schememanager.h"
#include "RenderManager.h"
#include "r_particle.h"
extern "C"
{
#include "pm_shared.h"
}

#include <string.h>
#include "hud_servers.h"
//#include "vgui_int.h"
#include "interface.h"

#define DLLEXPORT 
//__declspec( dllexport )


cl_enginefunc_t gEngfuncs;
CHud gHUD;
void initMsgManager();
//TeamFortressViewport *gViewPort = NULL;

void InitInput (void);
void IN_Commands( void );

vec3_t g_vecViewOrigin;// XDM: real view point
vec3_t g_vecViewAngles;
vec3_t g_vecViewForward;// can be calculated from angles

int g_iSprIndError = 0;

void PrecacheModels(void)
{
	// XDM: These must have been precached on server! CL_LoadModel()?
	g_iSprIndError = LoadModel("sprites/error.spr");
}


/*
========================== 
    Initialize

Called when the DLL is first loaded.
==========================
*/
extern "C" 
{
int		DLLEXPORT Initialize( cl_enginefunc_t *pEnginefuncs, int iVersion );
int		DLLEXPORT HUD_VidInit( void );
void	DLLEXPORT HUD_Init( void );
int		DLLEXPORT HUD_Redraw( float flTime, int intermission );
int		DLLEXPORT HUD_UpdateClientData( client_data_t *cdata, float flTime );
void	DLLEXPORT HUD_Reset ( void );
void	DLLEXPORT HUD_PlayerMove( struct playermove_s *ppmove, int server );
void	DLLEXPORT HUD_PlayerMoveInit( struct playermove_s *ppmove );
char	DLLEXPORT HUD_PlayerMoveTexture( char *name );
int		DLLEXPORT HUD_ConnectionlessPacket( const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size );
int		DLLEXPORT HUD_GetHullBounds( int hullnumber, float *mins, float *maxs );
void	DLLEXPORT HUD_Frame( double time );
void	DLLEXPORT HUD_VoiceStatus(int entindex, qboolean bTalking);
void	DLLEXPORT HUD_DirectorMessage( int iSize, void *pbuf );
}

/*
================================
HUD_GetHullBounds

  Engine calls this to enumerate player collision hulls, for prediction.  Return 0 if the hullnumber doesn't exist.
================================
*/
int DLLEXPORT HUD_GetHullBounds( int hullnumber, float *mins, float *maxs )
{
	int iret = 0;

	switch ( hullnumber )
	{
	case 0:				// Normal player
		mins = Vector(-16, -16, -36);
		maxs = Vector(16, 16, 36);
		iret = 1;
		break;
	case 1:				// Crouched player
		mins = Vector(-16, -16, -18 );
		maxs = Vector(16, 16, 18 );
		iret = 1;
		break;
	case 2:				// Point based hull
		mins = Vector( 0, 0, 0 );
		maxs = Vector( 0, 0, 0 );
		iret = 1;
		break;
	}

	return iret;
}

/*
================================
HUD_ConnectionlessPacket

 Return 1 if the packet is valid.  Set response_buffer_size if you want to send a response packet.  Incoming, it holds the max
  size of the response_buffer, so you must zero it out if you choose not to respond.
================================
*/
int	DLLEXPORT HUD_ConnectionlessPacket( const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size )
{
	// Parse stuff from args
	int max_buffer_size = *response_buffer_size;

	// Zero it out since we aren't going to respond.
	// If we wanted to response, we'd write data into response_buffer
	*response_buffer_size = 0;

	// Since we don't listen for anything here, just respond that it's a bogus message
	// If we didn't reject the message, we'd return 1 for success instead.
	return 0;
}

void DLLEXPORT HUD_PlayerMoveInit( struct playermove_s *ppmove )
{
	PM_Init( ppmove );
}

char DLLEXPORT HUD_PlayerMoveTexture( char *name )
{
	return PM_FindTextureType( name );
}

void DLLEXPORT HUD_PlayerMove( struct playermove_s *ppmove, int server )
{
	PM_Move( ppmove, server );
}

int DLLEXPORT Initialize( cl_enginefunc_t *pEnginefuncs, int iVersion )
{
	gEngfuncs = *pEnginefuncs;

	if (iVersion != CLDLL_INTERFACE_VERSION)
		return 0;

	memcpy(&gEngfuncs, pEnginefuncs, sizeof(cl_enginefunc_t));

	return 1;
}


/*
==========================
	HUD_VidInit

Called when the game initializes
and whenever the vid_mode is changed
so the HUD can reinitialize itself.
==========================
*/

int DLLEXPORT HUD_VidInit( void )
{
	g_iSprIndError = 0;

	if (g_pRenderManager != NULL)// XDM
		g_pRenderManager->DeleteAllSystems();

	g_pParticleSystems->ClearSystems();

	gHUD.VidInit();
//	VGui_Startup();
	return 1;
}

/*
==========================
	HUD_Init

Called whenever the client connects
to a server.  Reinitializes all 
the hud variables.
==========================
*/

void DLLEXPORT HUD_Init( void )
{
	if (g_pRenderManager == NULL)
		g_pRenderManager = new CRenderManager();// XDM

	if (g_pParticleSystems)//init partsystem
	{
		delete g_pParticleSystems;
		g_pParticleSystems = NULL;
	}
	g_pParticleSystems = new ParticleSystemManager();

	InitInput();
	gHUD.Init();
	initMsgManager();
//	Scheme_Init();
}

/*
==========================
	HUD_Redraw

called every screen frame to
redraw the HUD.
===========================
*/

int DLLEXPORT HUD_Redraw( float time, int intermission )
{
	gHUD.Redraw( time, intermission );
	return 1;
}


/*
==========================
	HUD_UpdateClientData

called every time shared client
dll/engine data gets changed,
and gives the cdll a chance
to modify the data.

returns 1 if anything has been changed, 0 otherwise.
==========================
*/

int DLLEXPORT HUD_UpdateClientData(client_data_t *pcldata, float flTime )
{
	IN_Commands();
	return gHUD.UpdateClientData(pcldata, flTime );
}

/*
==========================
	HUD_Reset

Called at start and end of demos to restore to "non"HUD state.
==========================
*/

void DLLEXPORT HUD_Reset( void )
{
	gHUD.VidInit();
	g_pParticleSystems->ClearSystems();
}

/*
==========================
HUD_Frame

Called by engine every frame that client .dll is loaded
==========================
*/

void DLLEXPORT HUD_Frame( double time )
{
	if (g_iSprIndError <= 0)// XDM
		PrecacheModels();

	// XDM3035: this works well even in steam versions
	if (g_pRenderManager && gHUD.m_iIntermission == 0)// XDM: call this AFTER gHUD.Redraw()!
		g_pRenderManager->Update(gEngfuncs.GetClientTime(), time);

//	ServersThink( time );
//	GetClientVoiceMgr()->Frame(time);
}


/*
==========================
HUD_VoiceStatus

Called when a player starts or stops talking.
==========================
*/

void DLLEXPORT HUD_VoiceStatus(int entindex, qboolean bTalking)
{
//	GetClientVoiceMgr()->UpdateSpeakerStatus(entindex, bTalking);
}

/*
==========================
HUD_DirectorEvent

Called when a director event message was received
==========================
*/

void DLLEXPORT HUD_DirectorMessage( int iSize, void *pbuf )
{
	 gHUD.m_Spectator.DirectorMessage( iSize, pbuf );
}

/*
==========================
HUD_Shutdown

Called whenever the client disconnects from a server.
==========================
*/

void DLLEXPORT HUD_Shutdown(void)
{
	if (g_pRenderManager)
	{
		g_pRenderManager->DeleteAllSystems();
		delete g_pRenderManager;
		g_pRenderManager = NULL;
	}
	g_iSprIndError = 0;
}
