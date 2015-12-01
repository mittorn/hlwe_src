#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "decals.h"
#include "shake.h"
#include "game.h"
#include "projectiles.h"

LINK_ENTITY_TO_CLASS( sstrike, CSatelliteStrike );

void CSatelliteStrike::Explode( TraceResult *pTrace, int bitsDamageType )
{
	TraceResult tr;
	entvars_t *pevOwner = VARS( pev->owner );

	RadiationStayTime = pev->dmg/100;
	::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg, pev->dmg/4, CLASS_NONE, DMG_PLASMA | DMG_NEVERGIB);
	UTIL_ScreenShake( pev->origin, 80, 10, 5, pev->dmg/2 );
	FX_Explosion( pev->origin + Vector(0,0,5), EXPLOSION_SATELLITE );

	pev->effects |= EF_NODRAW;
	pev->velocity = g_vecZero;
	SetTouch(NULL);
	SetThink(Irradiate);
	pev->nextthink = gpGlobals->time + 0.3;
}

void CSatelliteStrike::Irradiate( void )
{
	if(RadiationStayTime <=0 )
		UTIL_Remove( this );
	else
		RadiationStayTime--;

	entvars_t *pevOwner = VARS( pev->owner );
	::RadiusDamage( pev->origin, pev, pevOwner, RadiationStayTime/2, pev->dmg/8, CLASS_MACHINE, DMG_RADIATION);
	pev->nextthink = gpGlobals->time + 0.3;
}

void CSatelliteStrike::Detonate( void )
{
	TraceResult tr;
	Vector vecSpot = pev->origin + Vector (0 , 0, 8);
	UTIL_TraceLine ( vecSpot, vecSpot + Vector ( 0, 0, -80),  ignore_monsters, ENT(pev), & tr);
	Explode( &tr, DMG_ENERGYBLAST | DMG_NEVERGIB);
}

void CSatelliteStrike :: TumbleThink( void )
{
	if (!IsInWorld())
	{
		UTIL_Remove( this );
		return;
	}

	pev->nextthink = gpGlobals->time + 0.1;

	if (pev->dmgtime <= gpGlobals->time)
	{
		SetThink( Detonate );
	}
	if (pev->waterlevel != 0)
	{
		pev->velocity = pev->velocity * 0.5;
	}
}

void CSatelliteStrike::BounceTouch( CBaseEntity *pOther )
{
	pev->velocity = g_vecZero;
}

void CSatelliteStrike:: Spawn( void )
{
	pev->movetype = MOVETYPE_NONE;
	pev->classname = MAKE_STRING( "sstrike" );
	pev->solid = SOLID_BBOX;
	pev->effects |= EF_NODRAW;

	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = 11;
	UTIL_SetSize(pev, g_vecZero, g_vecZero );
}

CSatelliteStrike * CSatelliteStrike:: ShootSatelliteStrike( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time )
{
	CSatelliteStrike *pSatelliteStrike = GetClassPtr( (CSatelliteStrike *)NULL );
	pSatelliteStrike->Spawn();
	UTIL_SetOrigin( pSatelliteStrike->pev, vecStart );
	pSatelliteStrike->pev->velocity = vecVelocity;
	pSatelliteStrike->pev->owner = ENT(pevOwner); 

	pSatelliteStrike->pev->dmgtime = gpGlobals->time + time;
	pSatelliteStrike->SetThink( TumbleThink );
	pSatelliteStrike->pev->nextthink = gpGlobals->time + 0.1;
	pSatelliteStrike->SetTouch( BounceTouch );
	pSatelliteStrike->pev->friction = 1;
	pSatelliteStrike->pev->dmg = dmg_satellite.value * (mp_wpn_power.value/100); 
	return pSatelliteStrike;
}