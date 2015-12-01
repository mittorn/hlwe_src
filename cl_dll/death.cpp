#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"

#include <string.h>
#include <stdio.h>

//#include "vgui_TeamFortressViewport.h"

DECLARE_MESSAGE( m_DeathNotice, DeathMsg );

struct DeathNoticeItem {
	char szKiller[MAX_PLAYER_NAME_LENGTH*2];
	char szVictim[MAX_PLAYER_NAME_LENGTH*2];
	int iId;	// the index number of the associated sprite
	int iSuicide;
	int iTeamKill;
	int iNonPlayerKill;
	float flDisplayTime;
	float *KillerColor;
	float *VictimColor;
};

#define MAX_DEATHNOTICES	4
static int DEATHNOTICE_DISPLAY_TIME = 6;

#define DEATHNOTICE_TOP		32

DeathNoticeItem rgDeathNoticeList[ MAX_DEATHNOTICES + 1 ];

float g_ColorBlue[3]	= { 0.6, 0.8, 1.0 };
float g_ColorRed[3]		= { 1.0, 0.25, 0.25 };
float g_ColorGreen[3]	= { 0.6, 1.0, 0.6 };
float g_ColorYellow[3]	= { 1.0, 0.7, 0.0 };
float g_ColorGrey[3]	= { 0.8, 0.8, 0.8 };

float *GetClientColor( int clientIndex )
{
	switch ( g_PlayerExtraInfo[clientIndex].teamnumber )
	{
	case 1:	return g_ColorBlue;
	case 2: return g_ColorRed;
	case 3: return g_ColorYellow;
	case 4: return g_ColorGreen;
	case 0: return g_ColorYellow;

		default	: return g_ColorGrey;
	}

	return NULL;
}

int CHudDeathNotice :: Init( void )
{
	gHUD.AddHudElem( this );

	HOOK_MESSAGE( DeathMsg );

	CVAR_CREATE( "hud_deathnotice_time", "6", 0 );

	return 1;
}


void CHudDeathNotice :: InitHUDData( void )
{
	memset( rgDeathNoticeList, 0, sizeof(rgDeathNoticeList) );
}


int CHudDeathNotice :: VidInit( void )
{
	m_HUD_d_skull = gHUD.GetSpriteIndex( "d_skull" );

	return 1;
}

int CHudDeathNotice :: Draw( float flTime )
{
	int x, y, r, g, b;

	for ( int i = 0; i < MAX_DEATHNOTICES; i++ )
	{
		if ( rgDeathNoticeList[i].iId == 0 )
			break;  // we've gone through them all

		if ( rgDeathNoticeList[i].flDisplayTime < flTime )
		{ // display time has expired
			// remove the current item from the list
			memmove( &rgDeathNoticeList[i], &rgDeathNoticeList[i+1], sizeof(DeathNoticeItem) * (MAX_DEATHNOTICES - i) );
			i--;  // continue on the next item;  stop the counter getting incremented
			continue;
		}

		rgDeathNoticeList[i].flDisplayTime = min( rgDeathNoticeList[i].flDisplayTime, gHUD.m_flTime + DEATHNOTICE_DISPLAY_TIME );

		// Only draw if the viewport will let me
		//if ( gViewPort && gViewPort->AllowedToPrintText() )
		{
			// Draw the death notice
			y = YRES(DEATHNOTICE_TOP) + (36 * i);  //!!!

			int id = (rgDeathNoticeList[i].iId == -1) ? m_HUD_d_skull : rgDeathNoticeList[i].iId;
			x = ScreenWidth - ConsoleStringLen(rgDeathNoticeList[i].szVictim) - (gHUD.GetSpriteRect(id).right - gHUD.GetSpriteRect(id).left);

			if ( !rgDeathNoticeList[i].iSuicide )
			{
				x -= (5 + ConsoleStringLen( rgDeathNoticeList[i].szKiller ) );

				// Draw killers name
				if ( rgDeathNoticeList[i].KillerColor )
					gEngfuncs.pfnDrawSetTextColor( rgDeathNoticeList[i].KillerColor[0], rgDeathNoticeList[i].KillerColor[1], rgDeathNoticeList[i].KillerColor[2] );
				x = 5 + DrawConsoleString( x, y, rgDeathNoticeList[i].szKiller );
			}

			r = 255;  g = 0;	b = 0;
			if ( rgDeathNoticeList[i].iTeamKill )
			{
				r = 10;	g = 240; b = 10;  // display it in sickly green
			}

			// Draw death weapon
			SPR_Set( gHUD.GetSprite(id), r, g, b );
			SPR_DrawAdditive( 0, x, y, &gHUD.GetSpriteRect(id) );

			x += (gHUD.GetSpriteRect(id).right - gHUD.GetSpriteRect(id).left);

			// Draw victims name (if it was a player that was killed)
			if (rgDeathNoticeList[i].iNonPlayerKill == FALSE)
			{
				if ( rgDeathNoticeList[i].VictimColor )
					gEngfuncs.pfnDrawSetTextColor( rgDeathNoticeList[i].VictimColor[0], rgDeathNoticeList[i].VictimColor[1], rgDeathNoticeList[i].VictimColor[2] );
				x = DrawConsoleString( x, y, rgDeathNoticeList[i].szVictim );
			}
		}
	}

	return 1;
}

// This message handler may be better off elsewhere
int CHudDeathNotice :: MsgFunc_DeathMsg( const char *pszName, int iSize, void *pbuf )
{
	m_iFlags |= HUD_ACTIVE;

	BEGIN_READ( pbuf, iSize );

	int killer = READ_BYTE();
	int victim = READ_BYTE();

	char killedwith[32];
	strcpy( killedwith, "d_" );
	strncat( killedwith, READ_STRING(), 32 );

//	if (gViewPort)
//		gViewPort->DeathMsg( killer, victim );

	gHUD.m_Spectator.DeathMessage(victim);

	for ( int i = 0; i < MAX_DEATHNOTICES; i++ )
	{
		if ( rgDeathNoticeList[i].iId == 0 )
			break;
	}
	if ( i == MAX_DEATHNOTICES )
	{ // move the rest of the list forward to make room for this item
		memmove( rgDeathNoticeList, rgDeathNoticeList+1, sizeof(DeathNoticeItem) * MAX_DEATHNOTICES );
		i = MAX_DEATHNOTICES - 1;
	}

//	if (gViewPort)
//		gViewPort->GetAllPlayersInfo();

	// Get the Killer's name
	char *killer_name = g_PlayerInfoList[ killer ].name;
	if ( !killer_name )
	{
		killer_name = "";
		rgDeathNoticeList[i].szKiller[0] = 0;
	}
	else
	{
		rgDeathNoticeList[i].KillerColor = GetClientColor( killer );
		strncpy( rgDeathNoticeList[i].szKiller, killer_name, MAX_PLAYER_NAME_LENGTH );
		rgDeathNoticeList[i].szKiller[MAX_PLAYER_NAME_LENGTH-1] = 0;
	}

	// Get the Victim's name
	char *victim_name = NULL;
	// If victim is -1, the killer killed a specific, non-player object (like a sentrygun)
	if ( ((char)victim) != -1 )
		victim_name = g_PlayerInfoList[ victim ].name;
	if ( !victim_name )
	{
		victim_name = "";
		rgDeathNoticeList[i].szVictim[0] = 0;
	}
	else
	{
		rgDeathNoticeList[i].VictimColor = GetClientColor( victim );
		strncpy( rgDeathNoticeList[i].szVictim, victim_name, MAX_PLAYER_NAME_LENGTH );
		rgDeathNoticeList[i].szVictim[MAX_PLAYER_NAME_LENGTH-1] = 0;
	}

	// Is it a non-player object kill?
	if ( ((char)victim) == -1 )
	{
		rgDeathNoticeList[i].iNonPlayerKill = TRUE;

		// Store the object's name in the Victim slot (skip the d_ bit)
		strcpy( rgDeathNoticeList[i].szVictim, killedwith+2 );
	}
	else
	{
		if ( killer == victim || killer == 0 )
			rgDeathNoticeList[i].iSuicide = TRUE;

		if ( !strcmp( killedwith, "d_teammate" ) )
			rgDeathNoticeList[i].iTeamKill = TRUE;
	}

	// Find the sprite in the list
	int spr = gHUD.GetSpriteIndex( killedwith );

	rgDeathNoticeList[i].iId = spr;

	DEATHNOTICE_DISPLAY_TIME = CVAR_GET_FLOAT( "hud_deathnotice_time" );
	rgDeathNoticeList[i].flDisplayTime = gHUD.m_flTime + DEATHNOTICE_DISPLAY_TIME;

	if (rgDeathNoticeList[i].iNonPlayerKill)
	{
		ConsolePrint( rgDeathNoticeList[i].szKiller );
		ConsolePrint( " killed a " );
		ConsolePrint( rgDeathNoticeList[i].szVictim );
		ConsolePrint( "\n" );
	}
	else
	{
		// record the death notice in the console
		if ( rgDeathNoticeList[i].iSuicide )
		{
			ConsolePrint( rgDeathNoticeList[i].szVictim );

			if ( !strcmp( killedwith, "d_world" ) )
			{
				ConsolePrint( " died" );
			}
			else
			{
				ConsolePrint( " killed self" );
			}
		}
		else if ( rgDeathNoticeList[i].iTeamKill )
		{
			ConsolePrint( rgDeathNoticeList[i].szKiller );
			ConsolePrint( " killed his teammate " );
			ConsolePrint( rgDeathNoticeList[i].szVictim );
		}
		else
		{
			ConsolePrint( rgDeathNoticeList[i].szKiller );
			ConsolePrint( " killed " );
			ConsolePrint( rgDeathNoticeList[i].szVictim );
		}

		if ( killedwith && *killedwith && (*killedwith > 13 ) && strcmp( killedwith, "d_world" ) && !rgDeathNoticeList[i].iTeamKill )
		{
			ConsolePrint( " with " );

			// replace the code names with the 'real' names
			if ( !strcmp( killedwith+2, "blaster" ) )
				strcpy( killedwith, "d_E11 Blaster Rifle" );
			if ( !strcmp( killedwith+2, "wh" ) )
				strcpy( killedwith, "d_War Head Launcher" );
			if ( !strcmp( killedwith+2, "egon" ) )
				strcpy( killedwith, "d_Egon Gun" );
			if ( !strcmp( killedwith+2, "nuke" ) )
				strcpy( killedwith, "d_Redeemer" );
			if ( !strcmp( killedwith+2, "flame" ) )
				strcpy( killedwith, "d_Flamethrower" );
			if ( !strcmp( killedwith+2, "gauss" ) )
				strcpy( killedwith, "d_Gauss Gun" );
			if ( !strcmp( killedwith+2, "frostball" ) )
				strcpy( killedwith, "d_Froster" );
			if ( !strcmp( killedwith+2, "glock" ) )
				strcpy( killedwith, "d_Glock 18" );
			if ( !strcmp( killedwith+2, "30mmsg" ) )
				strcpy( killedwith, "d_30mm Assault Shotgun" );
			if ( !strcmp( killedwith+2, "sggren" ) )
				strcpy( killedwith, "d_30mm Assault Shotgun" );
			if ( !strcmp( killedwith+2, "glock_akimbo" ) )
				strcpy( killedwith, "d_Dual Glock 18" );
			if ( !strcmp( killedwith+2, "9mmAR" ) )
				strcpy( killedwith, "d_MP5" );
			if ( !strcmp( killedwith+2, "turretkit" ) )
				strcpy( killedwith, "d_Personal Defencive System" );
			if ( !strcmp( killedwith+2, "shieldgun" ) )
				strcpy( killedwith, "d_Chendler pistol" );
			if ( !strcmp( killedwith+2, "ak74grenade" ) )
				strcpy( killedwith, "d_AK74 grenade" );
			if ( !strcmp( killedwith+2, "mmissile" ) )
				strcpy( killedwith, "d_Devastator" );
			if ( !strcmp( killedwith+2, "pbolt" ) )
				strcpy( killedwith, "d_Energy Pistol" );
			if ( !strcmp( killedwith+2, "flak_shard" ) )
				strcpy( killedwith, "d_Flak Cannon" );
			if ( !strcmp( killedwith+2, "flak_bomb" ) )
				strcpy( killedwith, "d_Flak Cannon" );
			if ( !strcmp( killedwith+2, "clmomma" ) )
				strcpy( killedwith, "d_Cluster Grenade" );
			if ( !strcmp( killedwith+2, "clbaby" ) )
				strcpy( killedwith, "d_Cluster Baby" );
			if ( !strcmp( killedwith+2, "chronoclip" ) )
				strcpy( killedwith, "d_Chronosceptor Power Cell" );
			if ( !strcmp( killedwith+2, "black_hole" ) )
				strcpy( killedwith, "d_Chronosceptor" );
			if ( !strcmp( killedwith+2, "pshield_det" ) )
				strcpy( killedwith, "d_Power Shield Explosion" );
			if ( !strcmp( killedwith+2, "sunofgod" ) )
				strcpy( killedwith, "d_Charge Radiance Emitter" );
			if ( !strcmp( killedwith+2, "nggrenade" ) )
				strcpy( killedwith, "d_Nerve Gas Grenade" );
			if ( !strcmp( killedwith+2, "bolt" ) )
				strcpy( killedwith, "d_Crossbow" );
			if ( !strcmp( killedwith+2, "gluon" ) )
				strcpy( killedwith, "d_Gluon Gun" );
			if ( !strcmp( killedwith+2, "gluon2" ) )
				strcpy( killedwith, "d_Gluon Gun" );
			if ( !strcmp( killedwith+2, "rrp" ) )
				strcpy( killedwith, "d_RPG" );
			if ( !strcmp( killedwith+2, "tankproj" ) )
				strcpy( killedwith, "d_Tank Heavy Cannon" );
			if ( !strcmp( killedwith+2, "tank_model" ) )
				strcpy( killedwith, "d_Machine gun turret" );
			if ( !strcmp( killedwith+2, "mortar" ) )
				strcpy( killedwith, "d_Air Strike" );
			if ( !strcmp( killedwith+2, "tank_gauss" ) )
				strcpy( killedwith, "d_Energy Turret" );
			if ( !strcmp( killedwith+2, "turret_sentry" ) )
				strcpy( killedwith, "d_Sentry Turret" );
			if ( !strcmp( killedwith+2, "turret_ion" ) )
				strcpy( killedwith, "d_Ion Turret" );
			if ( !strcmp( killedwith+2, "dumbfire" ) )
				strcpy( killedwith, "d_Missile Turret" );
			if ( !strcmp( killedwith+2, "satchel" ) )
				strcpy( killedwith, "d_Satchel Charge" );
			if ( !strcmp( killedwith+2, "biomass" ) )
				strcpy( killedwith, "d_Bio Rifle" );
			if ( !strcmp( killedwith+2, "u2" ) )
				strcpy( killedwith, "d_U2 Assault Rifle" );
			if ( !strcmp( killedwith+2, "u2momma" ) )
				strcpy( killedwith, "d_U2 Grenade" );
			if ( !strcmp( killedwith+2, "u2baby" ) )
				strcpy( killedwith, "d_U2 Grenade" );
			if ( !strcmp( killedwith+2, "deagle" ) )
				strcpy( killedwith, "d_Desert Eagle" );
			if ( !strcmp( killedwith+2, "usp" ) )
				strcpy( killedwith, "d_Silenced USP Match" );
			if ( !strcmp( killedwith+2, "357" ) )
				strcpy( killedwith, "d_Python 357" );
			if ( !strcmp( killedwith+2, "shotgun" ) )
				strcpy( killedwith, "d_SPAS 14" );
			if ( !strcmp( killedwith+2, "autoshotgun" ) )
				strcpy( killedwith, "d_Combat SPAS-12" );
			if ( !strcmp( killedwith+2, "g11" ) )
				strcpy( killedwith, "d_H&K G11" );
			if ( !strcmp( killedwith+2, "plasma" ) )
				strcpy( killedwith, "d_Plasma Rifle" );
			if ( !strcmp( killedwith+2, "plasma2" ) )
				strcpy( killedwith, "d_Plasma Rifle" );
			if ( !strcmp( killedwith+2, "dispball" ) )
				strcpy( killedwith, "d_Displacer" );
			if ( !strcmp( killedwith+2, "teleenter" ) )
				strcpy( killedwith, "d_Displacer" );
			if ( !strcmp( killedwith+2, "pulserifle" ) )
				strcpy( killedwith, "d_Pulse Rifle" );
			if ( !strcmp( killedwith+2, "pulse" ) )
				strcpy( killedwith, "d_Pulse Rifle" );
			if ( !strcmp( killedwith+2, "teslagun" ) )
				strcpy( killedwith, "d_M41A Tesla Gun" );
			if ( !strcmp( killedwith+2, "teslagren" ) )
				strcpy( killedwith, "d_Shock Grenade" );
			if ( !strcmp( killedwith+2, "barett" ) )
				strcpy( killedwith, "d_M82 Barett" );
			if ( !strcmp( killedwith+2, "hellfire" ) )
				strcpy( killedwith, "d_Incendiary Cannon" );
			if ( !strcmp( killedwith+2, "nail" ) )
				strcpy( killedwith, "d_Nailgun" );
			if ( !strcmp( killedwith+2, "photongun" ) )
				strcpy( killedwith, "d_Photon Gun" );
			if ( !strcmp( killedwith+2, "taucannon" ) )
				strcpy( killedwith, "d_Tau Particle Weapon" );
			if ( !strcmp( killedwith+2, "shock" ) )
				strcpy( killedwith, "d_Smart Shock Pistol" );
			if ( !strcmp( killedwith+2, "detpack" ) )
				strcpy( killedwith, "d_C4 Explosive" );
			if ( !strcmp( killedwith+2, "sstrike" ) )
				strcpy( killedwith, "d_Satellite Dispersion Cannon" );
			if ( !strcmp( killedwith+2, "uzi" ) )
				strcpy( killedwith, "d_Mac 10" );
			if ( !strcmp( killedwith+2, "uzi_akimbo" ) )
				strcpy( killedwith, "d_Dual Mac 10" );
			if ( !strcmp( killedwith+2, "akimbogun" ) )
				strcpy( killedwith, "d_Steyr Aug + SG552" );
			if ( !strcmp( killedwith+2, "m72" ) )
				strcpy( killedwith, "d_M72 Gauss Rifle" );

			if ( !strcmp( killedwith+2, "x_flame" ) )
				strcpy( killedwith, "d_FIRE" );
			if ( !strcmp( killedwith+2, "x_radiation" ) )
				strcpy( killedwith, "d_RADIATION" );
			if ( !strcmp( killedwith+2, "x_nervegas" ) )
				strcpy( killedwith, "d_NERVEGAS" );
			if ( !strcmp( killedwith+2, "x_poison" ) )
				strcpy( killedwith, "d_POISON" );
			if ( !strcmp( killedwith+2, "x_acid" ) )
				strcpy( killedwith, "d_ACID" );
			if ( !strcmp( killedwith+2, "x_freeze" ) )
				strcpy( killedwith, "d_COLD" );
			if ( !strcmp( killedwith+2, "x_bleeding" ) )
				strcpy( killedwith, "d_BLOOD LOST" );

			ConsolePrint( killedwith+2 ); // skip over the "d_" part
		}

		ConsolePrint( "\n" );
	}

	return 1;
}




