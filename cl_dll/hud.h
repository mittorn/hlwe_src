#include <stdio.h>
#include "wrect.h"
#include "cl_dll.h"
#include "ammo.h"
#include "r_efx.h"

#define DHN_DRAWZERO	1
#define DHN_2DIGITS	2
#define DHN_3DIGITS	4
#define MIN_ALPHA	100	
#define	HUDELEM_ACTIVE	1

#define RGB_YELLOWISH 0x00FFA000 //255,160,0
#define RGB_REDISH 0x00FF1010 //255,160,0
#define RGB_GREENISH 0x0000A000 //0,160,0
#define RGB_BLUEISH 0x0000A0FF //0,160,255
#define RGB_STRONGBLUEISH 0x000000FF //0,0,255

#define MAX_SPRITE_NAME_LENGTH	24

typedef struct
{
	int x, y;
} POSITION;


enum 
{ 
	MAX_PLAYERS = 64,
	MAX_TEAMS = 64,
	MAX_TEAM_NAME = 16,
};

typedef struct
{
	unsigned char r,g,b,a;
} RGBA;

typedef struct cvar_s cvar_t;


#define HUD_ACTIVE	1
#define HUD_INTERMISSION 2

#define MAX_PLAYER_NAME_LENGTH		32
#define	MAX_MOTD_LENGTH				1536

class CHudBase
{
public:
	POSITION  m_pos;
	int   m_type;
	int	  m_iFlags; // active, moving, 
	virtual		~CHudBase() {}
	virtual int Init( void ) {return 0;}
	virtual int VidInit( void ) {return 0;}
	virtual int Draw(float flTime) {return 0;}
	virtual void Think(void) {return;}
	virtual void Reset(void) {return;}
	virtual void InitHUDData( void ) {}		// called every time a server is connected to
};

struct HUDLIST {
	CHudBase	*p;
	HUDLIST		*pNext;
};



//
//-----------------------------------------------------
//
//#include "../game_shared/voice_status.h"
#include "hud_spectator.h"


//
//-----------------------------------------------------
//
class CHudAmmo: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	void Think(void);
	void Reset(void);
	int DrawWList(float flTime);
	int MsgFunc_CurWeapon(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_WeaponList(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_AmmoX(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_AmmoPickup( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_WeapPickup( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_ItemPickup( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_HideWeapon( const char *pszName, int iSize, void *pbuf );

	void SlotInput( int iSlot );
	void _cdecl UserCmd_Slot1( void );
	void _cdecl UserCmd_Slot2( void );
	void _cdecl UserCmd_Slot3( void );
	void _cdecl UserCmd_Slot4( void );
	void _cdecl UserCmd_Slot5( void );
	void _cdecl UserCmd_Slot6( void );
	void _cdecl UserCmd_Slot7( void );
	void _cdecl UserCmd_Slot8( void );
	void _cdecl UserCmd_Slot9( void );
	void _cdecl UserCmd_Slot10( void );
	void _cdecl UserCmd_Close( void );
	void _cdecl UserCmd_NextWeapon( void );
	void _cdecl UserCmd_PrevWeapon( void );

private:
	float m_fFade;
	RGBA  m_rgba;
	WEAPON *m_pWeapon;
	int	m_HUD_bucket0;
	int m_HUD_selection;
	int m_mach;
};

//
//-----------------------------------------------------
//

class CHudAmmoSecondary: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	void Reset( void );
	int Draw(float flTime);

	int MsgFunc_SecAmmoVal( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_SecAmmoIcon( const char *pszName, int iSize, void *pbuf );

private:
	enum {
		MAX_SEC_AMMO_VALUES = 4
	};

	int m_HUD_ammoicon; // sprite indices
	int m_iAmmoAmounts[MAX_SEC_AMMO_VALUES];
	float m_fFade;
};


#include "health.h"


#define FADE_TIME 100

class CHudTrain: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	int MsgFunc_Train(const char *pszName, int iSize, void *pbuf);

private:
	HSPRITE m_hSprite;
	int m_iPos;

};

// + New Icons
class CHudScope: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	int MsgFunc_Scope(const char *pszName,  int iSize, void *pbuf );
	int m_iHudMode;
private:
	HSPRITE m_hCrosshair;
	HSPRITE m_hLines;
};


class CHudRedeemer : public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	int MsgFunc_Warhead(const char *pszName,  int iSize, void *pbuf );
	int m_iHudMode;
private:
	HSPRITE m_hSprite;
	HSPRITE m_hCrosshair;
	HSPRITE m_hStatic;
	bool bSet;
};

class CHudCloak : public CHudBase
{
public:
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);
	int MsgFunc_Cloak(const char *pszName, int iSize, void *pbuf); 

private:
	HSPRITE m_hSprite1;
	int	Cloak;
	int	m_iWidth;
	wrect_t *m_prc1;
};

class CHudAntigrav : public CHudBase
{
public:
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);
	int MsgFunc_Antigrav(const char *pszName, int iSize, void *pbuf); 

private:
	HSPRITE m_hSprite1;
	int Antigrav;
	int	  m_iWidth;
	wrect_t *m_prc1;
};

class CHudPt : public CHudBase
{
public:
int Init(void);
int VidInit(void);
int Draw(float flTime);
int MsgFunc_Pt(const char *pszName, int iSize, void *pbuf); 

private:

HSPRITE m_hSprite1;
int Pt;
int	  m_iWidth;
wrect_t *m_prc1;
};

class CHudPshield : public CHudBase
{
public:
int Init(void);
int VidInit(void);
int Draw(float flTime);
int MsgFunc_Pshield(const char *pszName, int iSize, void *pbuf); 

private:

HSPRITE m_hSprite1;
int Pshield;
int	  m_iWidth;
wrect_t *m_prc1;
};

class CHudBuyZone : public CHudBase
{
public:
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);
	int MsgFunc_BuyZone(const char *pszName, int iSize, void *pbuf); 
	int HUD_BuyZone;

private:
	int buyzoneW, buyzoneH, ScreenX, ScreenY;
};

class CHudMoney : public CHudBase
{
public:
      int Init(void);
      int VidInit(void);
      int Draw(float flTime);
      int MsgFunc_Money(const char *pszName, int iSize, void *pbuf); 
private:
	HSPRITE m_hSprite1, m_hSprite_plus, m_hSprite_minus;
	wrect_t *m_prc_dollar;
	wrect_t *m_prc_plus;
	wrect_t *m_prc_minus;
	int	m_iMoney, m_iNewMoney, m_iPlusMoney, m_iMinusMoney;	
	float	m_fFade;
	int	m_iHeight;
	bool	m_iPlus;
};

class CHudFiremode : public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	int MsgFunc_Firemode(const char *pszName,  int iSize, void *pbuf );

	int m_FIREMODE;
	int m_FIREMODE_GLAUNCHER;
	int m_FIREMODE_QUAD;
	int m_FIREMODE_SCOPE;
	int m_FIREMODE_CHARGE;
	int m_FIREMODE_PULSE;
	int m_FIREMODE_BEAM;
	int m_FIREMODE_NARROW;
	int m_FIREMODE_WIDE;
	int m_FIREMODE_BOLT;
	int m_FIREMODE_HALF;
	int m_FIREMODE_FULL;
	int m_FIREMODE_SHOOT;
	int m_FIREMODE_KNIFE;
	int m_FIREMODE_ACCELBOLT;
	int m_FIREMODE_EXPBOLT;
	int m_FIREMODE_SENTRYTURRET;
	int m_FIREMODE_MISSILETURRET;
	int m_FIREMODE_IONTURRET;
private:
	int m_iType, m_iHeight;	
};

class CHudKevlar : public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	int MsgFunc_Kevlar(const char *pszName,  int iSize, void *pbuf );
private:
	HSPRITE m_hSprite1;
	HSPRITE m_hSprite2;
	wrect_t *m_prc1;
	wrect_t *m_prc2;
	int m_iBat, m_iHeight;	
	float m_fFade;
};

class CHudHarmor : public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	int MsgFunc_Harmor(const char *pszName,  int iSize, void *pbuf );
private:
	HSPRITE m_hSprite1;
	HSPRITE m_hSprite2;
	wrect_t *m_prc1;
	wrect_t *m_prc2;
	int m_iBat, m_iHeight;	
	float 	m_fFade;
};

class CHudLongjump : public CHudBase
{
public:
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);
	int MsgFunc_Longjump(const char *pszName, int iSize, void *pbuf); 

private:
	HSPRITE m_hSprite1;
	int Longjump;
	int	  m_iWidth;
	wrect_t *m_prc1;
};

class CHudPHK : public CHudBase
{
public:
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);
	int MsgFunc_PHK(const char *pszName, int iSize, void *pbuf); 

private:
	HSPRITE m_hSprite1;
	int PHK;
	int	  m_iWidth;
	wrect_t *m_prc1;
};

class CHudTurretSentry : public CHudBase
{
public:
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);
	int MsgFunc_TurretS(const char *pszName, int iSize, void *pbuf); 
	int m_TURRET_S;
private:
	int TurretSentry;
};

class CHudTurretMissile : public CHudBase
{
public:
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);
	int MsgFunc_TurretM(const char *pszName, int iSize, void *pbuf); 
	int m_TURRET_M;

private:
	int TurretMissile;
};

class CHudTurretIon : public CHudBase
{
public:
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);
	int MsgFunc_TurretI(const char *pszName, int iSize, void *pbuf); 
	int m_TURRET_I;

private:
	int TurretIon;
};

class CHudPHEV : public CHudBase
{
public:
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);
	int MsgFunc_PHEV(const char *pszName, int iSize, void *pbuf); 

private:
	HSPRITE m_hSprite1;
	int PHEV;
	int	  m_iWidth;
	wrect_t *m_prc1;
};

class CHudTimer : public CHudBase
{
public:
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);
	int MsgFunc_Timer(const char *pszName, int iSize, void *pbuf); 

	float Time;

private:
	bool Inited;
	HSPRITE m_hSprite1;
	HSPRITE m_hSprite2;
	int	  m_iWidth;
	wrect_t *m_prc1;
	wrect_t *m_prc2;
};

// - New Icons

//
//-----------------------------------------------------
//
// REMOVED: Vgui has replaced this.
//
/*
class CHudMOTD : public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw( float flTime );
	void Reset( void );

	int MsgFunc_MOTD( const char *pszName, int iSize, void *pbuf );

protected:
	static int MOTD_DISPLAY_TIME;
	char m_szMOTD[ MAX_MOTD_LENGTH ];
	float m_flActiveRemaining;
	int m_iLines;
};
*/

//
//-----------------------------------------------------
//
class CHudStatusBar : public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw( float flTime );
	void Reset( void );
	void ParseStatusString( int line_num );

	int MsgFunc_StatusText( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_StatusValue( const char *pszName, int iSize, void *pbuf );

protected:
	enum { 
		MAX_STATUSTEXT_LENGTH = 128,
		MAX_STATUSBAR_VALUES = 8,
		MAX_STATUSBAR_LINES = 2,
	};

	char m_szStatusText[MAX_STATUSBAR_LINES][MAX_STATUSTEXT_LENGTH];  // a text string describing how the status bar is to be drawn
	char m_szStatusBar[MAX_STATUSBAR_LINES][MAX_STATUSTEXT_LENGTH];	// the constructed bar that is drawn
	int m_iStatusValues[MAX_STATUSBAR_VALUES];  // an array of values for use in the status bar

	int m_bReparseString; // set to TRUE whenever the m_szStatusBar needs to be recalculated

	// an array of colors...one color for each line
	float *m_pflNameColors[MAX_STATUSBAR_LINES];
};

//
//-----------------------------------------------------
//
// REMOVED: Vgui has replaced this.
//
/*
class CHudScoreboard: public CHudBase
{
public:
	int Init( void );
	void InitHUDData( void );
	int VidInit( void );
	int Draw( float flTime );
	int DrawPlayers( int xoffset, float listslot, int nameoffset = 0, char *team = NULL ); // returns the ypos where it finishes drawing
	void UserCmd_ShowScores( void );
	void UserCmd_HideScores( void );
	int MsgFunc_ScoreInfo( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_TeamInfo( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_TeamScore( const char *pszName, int iSize, void *pbuf );
	void DeathMsg( int killer, int victim );

	int m_iNumTeams;

	int m_iLastKilledBy;
	int m_fLastKillTime;
	int m_iPlayerNum;
	int m_iShowscoresHeld;

	void GetAllPlayersInfo( void );
private:
	struct cvar_s *cl_showpacketloss;

};
*/

struct extra_player_info_t 
{
	short frags;
	short deaths;
	short playerclass;
	short teamnumber;
	char teamname[MAX_TEAM_NAME];
};

struct ctf_info_t
{
	char name[MAX_TEAM_NAME];
	short score;
};

struct team_info_t 
{
	char name[MAX_TEAM_NAME];
	short frags;
	short deaths;
	short ping;
	short packetloss;
	short ownteam;
	short players;
	int already_drawn;
	int scores_overriden;
	int teamnumber;
};

extern hud_player_info_t	g_PlayerInfoList[MAX_PLAYERS+1];	   // player info from the engine
extern extra_player_info_t  g_PlayerExtraInfo[MAX_PLAYERS+1];   // additional player info sent directly to the client dll
extern team_info_t			g_TeamInfo[MAX_TEAMS+1];
extern ctf_info_t			g_ExtraTeamInfo[MAX_TEAMS+1];
extern int					g_IsSpectator[MAX_PLAYERS+1];


//
//-----------------------------------------------------
//
class CHudDeathNotice : public CHudBase
{
public:
	int Init( void );
	void InitHUDData( void );
	int VidInit( void );
	int Draw( float flTime );
	int MsgFunc_DeathMsg( const char *pszName, int iSize, void *pbuf );

private:
	int m_HUD_d_skull;  // sprite index of skull icon
};

//
//-----------------------------------------------------
//
class CHudMenu : public CHudBase
{
public:
	int Init( void );
	void InitHUDData( void );
	int VidInit( void );
	void Reset( void );
	int Draw( float flTime );
	int MsgFunc_ShowMenu( const char *pszName, int iSize, void *pbuf );

	void SelectMenuItem( int menu_item );

	int m_fMenuDisplayed;
	int m_bitsValidSlots;
	float m_flShutoffTime;
	int m_fWaitingForMore;
};

//
//-----------------------------------------------------
//
class CHudSayText : public CHudBase
{
public:
	int Init( void );
	void InitHUDData( void );
	int VidInit( void );
	int Draw( float flTime );
	int MsgFunc_SayText( const char *pszName, int iSize, void *pbuf );
	void SayTextPrint( const char *pszBuf, int iBufSize, int clientIndex = -1 );
	void EnsureTextFitsInOneLineAndWrapIfHaveTo( int line );
friend class CHudSpectator;

private:

	struct cvar_s *	m_HUD_saytext;
	struct cvar_s *	m_HUD_saytext_time;
};

//
//-----------------------------------------------------
//
class CHudBattery: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	int MsgFunc_Battery(const char *pszName,  int iSize, void *pbuf );
	
private:
	HSPRITE m_hSprite1;
	HSPRITE m_hSprite2;
	wrect_t *m_prc1;
	wrect_t *m_prc2;
	int	  m_iBat;	
	float m_fFade;
	int	  m_iHeight;		// width of the battery innards
};


//
//-----------------------------------------------------
//
class CHudFlashlight: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	void Reset( void );
	int MsgFunc_Flashlight(const char *pszName,  int iSize, void *pbuf );
	int MsgFunc_FlashBat(const char *pszName,  int iSize, void *pbuf );
	
private:
	HSPRITE m_hSprite1;
	HSPRITE m_hSprite2;
	wrect_t *m_prc1;
	wrect_t *m_prc2;
	float m_flBat;	
	int	  m_iBat;	
	int	  m_fOn;
	float m_fFade;
	int	  m_iWidth;		// width of the battery innards
};

//
//-----------------------------------------------------
//
const int maxHUDMessages = 16;
struct message_parms_t
{
	client_textmessage_t	*pMessage;
	float	time;
	int x, y;
	int	totalWidth, totalHeight;
	int width;
	int lines;
	int lineLength;
	int length;
	int r, g, b;
	int text;
	int fadeBlend;
	float charTime;
	float fadeTime;
};

//
//-----------------------------------------------------
//

class CHudTextMessage: public CHudBase
{
public:
	int Init( void );
	static char *LocaliseTextString( const char *msg, char *dst_buffer, int buffer_size );
	static char *BufferedLocaliseTextString( const char *msg );
	char *LookupString( const char *msg_name, int *msg_dest = NULL );
	int MsgFunc_TextMsg(const char *pszName, int iSize, void *pbuf);
};

//
//-----------------------------------------------------
//

class CHudMessage: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	int MsgFunc_HudText(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_GameTitle(const char *pszName, int iSize, void *pbuf);

	float FadeBlend( float fadein, float fadeout, float hold, float localTime );
	int	XPosition( float x, int width, int lineWidth );
	int YPosition( float y, int height );

	void MessageAdd( const char *pName, float time );
	void MessageAdd(client_textmessage_t * newMessage );
	void MessageDrawScan( client_textmessage_t *pMessage, float time );
	void MessageScanStart( void );
	void MessageScanNextChar( void );
	void Reset( void );

private:
	client_textmessage_t		*m_pMessages[maxHUDMessages];
	float				m_startTime[maxHUDMessages];
	message_parms_t			m_parms;
	float				m_gameTitleTime;
	client_textmessage_t		*m_pGameTitle;

	int m_HUD_title_half;
};

class CHud
{
private:
	HUDLIST			*m_pHudList;
	client_sprite_t		*m_pSpriteList;
	int			m_iSpriteCount;
	int			m_iSpriteCountAllRes;
	float			m_flMouseSensitivity;

public:

	HSPRITE						m_hsprCursor;
	float m_flTime;	   // the current client time
	float m_fOldTime;  // the time at which the HUD was last redrawn
	double m_flTimeDelta; // the difference between flTime and fOldTime
	Vector	m_vecOrigin;
	Vector	m_vecAngles;
	int		m_iKeyBits;
	int		m_iHideHUDDisplay;
	int		m_iFOV;
	int		m_Teamplay;
	int		m_iRes;
	cvar_t  *m_pCvarStealMouse;
	cvar_t	*m_pCvarDraw;

	cvar_t	*RainInfo; // rain tutorial

	int m_iFontHeight;
	int DrawHudNumber(int x, int y, int iFlags, int iNumber, int r, int g, int b );
	int DrawHudNumberLarge(int x, int y, int iFlags, int iNumber, int r, int g, int b );
	int DrawHudString(int x, int y, int iMaxX, char *szString, int r, int g, int b );
	int DrawHudStringReverse( int xpos, int ypos, int iMinX, char *szString, int r, int g, int b );
	int DrawHudNumberString( int xpos, int ypos, int iMinX, int iNumber, int r, int g, int b );
	int GetNumWidth(int iNumber, int iFlags);
	int m_iSkin;//set skin for view weaponmodel
	int m_iBody;//set body for view weaponmodel
	int m_iConcussionEffect; 

// Client effects (wallgibs, shells, human gibs - gib life)
	cvar_t	*TempEntLifeCvar;
	cvar_t	*GibsLifeCvar;
	cvar_t	*SmokingShells;

// Crosshair color change
	cvar_t	*m_pCrossColor;
	int m_iPaused;

	// buz: spec tank variables;
	int	m_SpecTank_on;
	Vector	m_SpecTank_point;
	float	m_SpecTank_defYaw;
	float	m_SpecTank_coneHor;
	float	m_SpecTank_coneVer;
	float	m_SpecTank_distFwd;
	float	m_SpecTank_distUp;
	int	m_SpecTank_Ammo;
	int	m_SpecTank_Ammo2;
	float   m_SpecTank_CrossSize;
private:
	// the memory for these arrays are allocated in the first call to CHud::VidInit(), when the hud.txt and associated sprites are loaded.
	// freed in ~CHud()
	HSPRITE *m_rghSprites;		// the sprites loaded from hud.txt
	wrect_t *m_rgrcRects;
	char *m_rgszSpriteNames;

	struct cvar_s *default_fov;
public:
	HSPRITE GetSprite( int index ) 
	{
		return (index < 0) ? 0 : m_rghSprites[index];
	}

	wrect_t& GetSpriteRect( int index )
	{
		return m_rgrcRects[index];
	}

	
	int GetSpriteIndex( const char *SpriteName );	// gets a sprite index, for use in the m_rghSprites[] array

	CHudAmmo		m_Ammo;
	CHudHealth		m_Health;
	CHudSpectator		m_Spectator;
	CHudBattery		m_Battery;
	CHudTrain		m_Train;
	CHudFlashlight		m_Flash;
	CHudMessage		m_Message;
	CHudStatusBar   	m_StatusBar;
	CHudDeathNotice 	m_DeathNotice;
	CHudSayText		m_SayText;
	CHudMenu		m_Menu;
	CHudAmmoSecondary	m_AmmoSecondary;
	CHudTextMessage 	m_TextMessage;

	CHudCloak		m_Cloak;
	CHudPHK			m_PHK;
	CHudTurretSentry	m_TurretSentry;
	CHudTurretMissile	m_TurretMissile;
	CHudTurretIon 		m_TurretIon;
	CHudPHEV		m_PHEV;
	CHudTimer		m_Timer;
	CHudLongjump		m_Longjump;
	CHudPshield		m_Pshield;
	CHudKevlar		m_Kevlar;
	CHudHarmor		m_Harmor;
	CHudAntigrav		m_Antigrav;
	CHudPt			m_Pt;
	CHudMoney		m_Money;
	CHudFiremode		m_Firemode;
	CHudBuyZone		m_BuyZone;
	CHudRedeemer		m_Warhead;
	CHudScope		m_Scope;

	void Init( void );
	void VidInit( void );
	void Think(void);
	int Redraw( float flTime, int intermission );
	int UpdateClientData( client_data_t *cdata, float time );
	void OnGamePaused(int paused);// XDM

	CHud() : m_iSpriteCount(0), m_pHudList(NULL) {}  
	~CHud();			// destructor, frees allocated memory

	// user messages
	int _cdecl MsgFunc_Damage(const char *pszName, int iSize, void *pbuf );
	int _cdecl MsgFunc_GameMode(const char *pszName, int iSize, void *pbuf );
	int _cdecl MsgFunc_ResetHUD(const char *pszName,  int iSize, void *pbuf);
	void _cdecl MsgFunc_InitHUD( const char *pszName, int iSize, void *pbuf );
	void _cdecl MsgFunc_ViewMode( const char *pszName, int iSize, void *pbuf );
	int _cdecl MsgFunc_SetFOV(const char *pszName,  int iSize, void *pbuf);
	int  _cdecl MsgFunc_SpecTank( const char *pszName, int iSize, void *pbuf );

	// Screen information
	SCREENINFO	m_scrinfo;

	int	m_iWeaponBits;
	int	m_iWeaponBits2;
	int	m_fPlayerDead;
	int	m_iIntermission;
	int	m_iPlayerFrozen; 
	// sprite indexes
	int m_HUD_number_0;
	int m_HUD_money_number_0;


	void AddHudElem(CHudBase *p);

	float GetSensitivity();

};

//class TeamFortressViewport;

extern CHud gHUD;
//extern TeamFortressViewport *gViewPort;

extern int g_iPlayerClass;
extern int g_iTeamNumber;
extern int g_iUser1;
extern int g_iUser2;
extern int g_iUser3;

