#include        "extdll.h"
#include        "util.h"
#include        "cbase.h"
#include        "player.h"
#include        "weapons.h"
#include	"pm_shared.h"
#include	"gamerules.h"

extern void respawn(entvars_t *pev, BOOL fCopyCorpse);

void CBasePlayer::StartObserver( Vector vecPosition, Vector vecViewAngle )
{
	m_iHideHUD |= (HIDEHUD_HEALTH | HIDEHUD_WEAPONS);
	m_afPhysicsFlags |= PFLAG_OBSERVER;

	m_pActiveItem = m_pLastItem = NULL;

	pev->viewmodel = pev->weaponmodel = 0;
	pev->effects = EF_NODRAW;
	pev->view_ofs = g_vecZero;
	pev->angles = pev->v_angle = vecViewAngle;
	pev->fixangle = TRUE;
	pev->solid = SOLID_NOT;
	pev->takedamage = DAMAGE_NO;
	pev->movetype = MOVETYPE_NOCLIP;
	pev->weapons = 0;
	m_iWeapons2 = 0;
	UTIL_SetOrigin( pev, vecPosition );
	IgniteEnd();
	FrozenEnd();

	ClearBits( m_afPhysicsFlags, PFLAG_DUCKING );
	ClearBits( pev->flags, FL_DUCKING );

	Observer_SetMode(OBS_ROAMING);

	m_flNextObserverInput = 0;
	Observer_FindNextPlayer();
}

void CBasePlayer::EndObserver ( void )
{
	m_iHideHUD &= ~(HIDEHUD_HEALTH | HIDEHUD_WEAPONS );
	m_afPhysicsFlags &= ~PFLAG_OBSERVER;
	pev->iuser1 = 0;
	pev->iuser2 = 0;

	RemoveAllItems(1);

	respawn( pev, false );
}

// Find the next client in the game for this player to spectate
void CBasePlayer::Observer_FindNextPlayer()
{
	// MOD AUTHORS: Modify the logic of this function if you want to restrict the observer to watching
	//				only a subset of the players. e.g. Make it check the target's team.

	CBaseEntity *client = m_hObserverTarget;
	while ( (client = (CBaseEntity*)UTIL_FindEntityByClassname( client, "player" )) != m_hObserverTarget ) 
	{
		if ( !client )
			continue;
		if ( !client->pev )
			continue;
		if ( client == this )
			continue;
		if ( !client->IsAlive() )
			continue;
		if ( g_pGameRules->IsTeamplay() && g_pGameRules->PlayerRelationship ( this, client ) != GR_TEAMMATE )
			continue;

		// Add checks on target here.

		m_hObserverTarget = client;
		break;
	}

	// Did we find a target?
	if ( m_hObserverTarget )
	{
		// Store the target in pev so the physics DLL can get to it
		if (pev->iuser1 != OBS_ROAMING)
			pev->iuser2 = ENTINDEX( m_hObserverTarget->edict() );
		else
			pev->iuser2 = ENTINDEX( edict() );
		// Move to the target
		UTIL_SetOrigin( pev, m_hObserverTarget->pev->origin );

		ALERT( at_console, "Now Tracking %s\n", STRING( m_hObserverTarget->pev->classname ) );

/*
WORKS!!!
	if ( IsBot() )
	{
		EndObserver();
		ALERT( at_console, "try to respawn bot from observer.cpp.\n" );
	}

*/
	}
	else
	{
		ALERT( at_console, "No observer targets.\n" );
	}
}

// Handle buttons in observer mode
void CBasePlayer::Observer_HandleButtons()
{
	if (!IsObserver()) return;

	UTIL_MakeVectors(pev->v_angle);

	if ( pev->button & IN_FORWARD )
		pev->origin = pev->origin + gpGlobals->v_forward * 20;
	else  if ( pev->button & IN_BACK )
		pev->origin = pev->origin - gpGlobals->v_forward * 20;

	if ( pev->button & IN_MOVERIGHT )
		pev->origin = pev->origin + gpGlobals->v_right * 20;
	else  if ( pev->button & IN_MOVELEFT )
		pev->origin = pev->origin - gpGlobals->v_right * 20;

	if (gpGlobals->time < m_flNextObserverInput)
		return;

	m_flNextObserverInput = gpGlobals->time + 0.2;

	if (pev->button & IN_DUCK)
	{
		switch (pev->iuser1)
		{
		default:
		case OBS_ROAMING:
			Observer_SetMode (OBS_CHASE_FREE);
			break;

		case OBS_CHASE_FREE:
			Observer_SetMode (OBS_ROAMING);
			break;
		}
	}
}

// Attempt to change the observer mode
void CBasePlayer::Observer_SetMode( int iMode )
{
	// Just abort if we're changing to the mode we're already in
	if ( iMode == pev->iuser1 )
		return;

	// is valid mode ?
	if ( iMode < OBS_CHASE_LOCKED || iMode > OBS_ROAMING )
		iMode = OBS_CHASE_LOCKED; // now it is

	// if we are not roaming, we need a valid target to track
	if ( iMode != OBS_ROAMING )
	{
		if (m_hObserverTarget == NULL)
			Observer_FindNextPlayer();

		// if we didn't find a valid target switch to roaming
		if (m_hObserverTarget == NULL)
		{
			ClientPrint( pev, HUD_PRINTCENTER, "#Spec_NoTarget"  );
			iMode = OBS_ROAMING;
		}
	}

	// set spectator mode
	pev->iuser1 = iMode;

	// set target if not roaming
	if (iMode == OBS_ROAMING)
		pev->iuser2 = ENTINDEX( edict() );
	else
		pev->iuser2 = ENTINDEX( m_hObserverTarget->edict() );
	
	// print spepctaor mode on client screen

	if (m_hObserverTarget != NULL)
	{
		char modemsg[16];
		sprintf(modemsg,"#Spec_Mode%i", iMode);
		ClientPrint( pev, HUD_PRINTCENTER, modemsg );
	}
}