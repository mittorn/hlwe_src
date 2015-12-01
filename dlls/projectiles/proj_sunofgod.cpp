#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "decals.h"
#include "game.h"
#include "projectiles.h"

LINK_ENTITY_TO_CLASS( sunofgod, CSunOfGod );

void CSunOfGod::Explode( TraceResult *pTrace, int bitsDamageType )
{
	RadiationStayTime = pev->dmg/50;
	FX_Trail( pev->origin, entindex(), PROJ_SUNOFGOD_DETONATE );

	entvars_t *pevOwner = VARS( pev->owner );
	::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg/10, pev->dmg*2, CLASS_NONE, DMG_BLIND );
	::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg, pev->dmg*2, CLASS_NONE, DMG_NUKE | DMG_NEVERGIB);

	pev->velocity = g_vecZero;
	SetTouch(NULL);
	SetThink( Irradiate );
	pev->nextthink = gpGlobals->time + 0.3;
}

void CSunOfGod::Irradiate( void )
{
	if(RadiationStayTime <=0 )
		UTIL_Remove( this );
	else
		RadiationStayTime--;

	entvars_t *pevOwner = VARS( pev->owner );
	::RadiusDamage( pev->origin, pev, pevOwner, RadiationStayTime/2, pev->dmg/2.5, CLASS_MACHINE, DMG_RADIATION | DMG_NEVERGIB);
	pev->nextthink = gpGlobals->time + 0.3;
}

void CSunOfGod::Detonate( void )
{
	TraceResult tr;
	Vector vecSpot = pev->origin + Vector ( 0 , 0 , 8 );
	UTIL_TraceLine ( vecSpot, vecSpot + Vector ( 0, 0, -40 ),  ignore_monsters, ENT(pev), & tr);
	Explode(&tr, DMG_NUKE);
}

void CSunOfGod::ExplodeTouch( CBaseEntity *pOther )
{
	TraceResult tr;
	Vector vecSpot = pev->origin - pev->velocity.Normalize() * 32;
	UTIL_TraceLine( vecSpot, vecSpot + pev->velocity.Normalize() * 64, ignore_monsters, ENT(pev), &tr );
	Explode(&tr, DMG_NUKE);
}

void CSunOfGod:: Spawn( void )
{
	pev->movetype = MOVETYPE_FLY;
	pev->classname = MAKE_STRING( "sunofgod" );
	SET_MODEL(ENT(pev), "sprites/anim_spr10.spr"); 
	pev->solid = SOLID_BBOX;
	pev->rendermode = kRenderTransAdd;
	pev->renderamt = 1;
	pev->frags = 60;
	UTIL_SetSize(pev, g_vecZero, g_vecZero );
}

CSunOfGod *CSunOfGod::ShootSunOfGod( entvars_t *pevOwner, Vector vecStart)
{
	CSunOfGod *pSunOfGod = GetClassPtr( (CSunOfGod *)NULL );
	pSunOfGod->Spawn();

	UTIL_SetOrigin( pSunOfGod->pev, vecStart );
	pSunOfGod->pev->velocity = gpGlobals->v_forward * 0;
	pSunOfGod->pev->angles = UTIL_VecToAngles (pSunOfGod->pev->velocity);
	pSunOfGod->pev->owner = ENT(pevOwner);
	pSunOfGod->SetThink ( Animate );
	pSunOfGod->pev->nextthink = gpGlobals->time + 0.1;
	pSunOfGod->SetTouch( ExplodeTouch );
	pSunOfGod->pev->dmg = dmg_sunofgod.value * (mp_wpn_power.value/100);
	FX_Trail(pSunOfGod->pev->origin, pSunOfGod->entindex(), PROJ_SUNOFGOD);
	return pSunOfGod;
}

void CSunOfGod::Animate( void )
{
	entvars_t *pevOwner = VARS( pev->owner );

	if ( UTIL_PointContents(pev->origin) == CONTENT_WATER )
	{
		FX_Trail( pev->origin, entindex(), PROJ_SUNOFGOD_DETONATE_WATER );
		::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg/3, pev->dmg/3, CLASS_NONE, DMG_NUKE | DMG_NEVERGIB);
		UTIL_Remove( this );
		return;
	}

	::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg/50, 180, CLASS_NONE, DMG_NUKE | DMG_NEVERGIB);
	pev->frags--;

	if (pev->frags <= 0)
	Detonate( );
	pev->nextthink = gpGlobals->time + 0.1;
}