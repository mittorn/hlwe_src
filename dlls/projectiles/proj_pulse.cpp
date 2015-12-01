#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "projectiles.h"
#include "game.h"

LINK_ENTITY_TO_CLASS( pulse, CPulse );

void CPulse :: Spawn( void )
{
	pev->solid = SOLID_NOT;
	SET_MODEL( edict(), "models/projectiles.mdl");	// Need a model, just use the grenade, we don't draw it anyway

	UTIL_SetSize(pev, g_vecZero, g_vecZero );
	pev->effects |= EF_NODRAW;

	SetThink ( DamageThink );
	pev->nextthink = gpGlobals->time + 0.1;
	pev->movetype = MOVETYPE_NONE;
	PulseStayTime = 20; //2 seconds
}

void CPulse :: DamageThink( void )
{
	if(PulseStayTime <=0 )
		UTIL_Remove( this );
	else
		PulseStayTime--;

	entvars_t *pevOwner = VARS( pev->owner );
	::RadiusDamage( pev->origin, pev, pevOwner, PulseStayTime, (dmg_pulserifle.value/2) * (mp_wpn_power.value/100), CLASS_NONE, DMG_SHOCK | DMG_NEVERGIB);
	pev->nextthink = gpGlobals->time + 0.1;
}