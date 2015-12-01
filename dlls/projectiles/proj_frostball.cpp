#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "soundent.h"
#include "decals.h"
#include "game.h"
#include "projectiles.h"

LINK_ENTITY_TO_CLASS( frostball, CFrostball );

void CFrostball::Killed (entvars_t *pevAttacker, int iGib)
{
	FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
	UTIL_Remove( this );
}

void CFrostball::ExplodeTouch( CBaseEntity *pOther )
{
	if ( UTIL_PointContents(pev->origin) == CONTENT_SKY )
	{
		FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
		UTIL_Remove( this );
		return;
	}
	if ( UTIL_PointContents(pev->origin) == CONTENT_WATER )
	{
		entvars_t *pevOwner = VARS( pev->owner );
		FX_Trail( pev->origin, entindex(), PROJ_ICE_DETONATE_WATER );
		::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg/2, pev->dmg, CLASS_NONE, DMG_FREEZE );
		UTIL_Remove( this );
		return;
	}
	TraceResult tr;
	Vector vecSpot = pev->origin - pev->velocity.Normalize() * 32;
	UTIL_TraceLine( vecSpot, vecSpot + pev->velocity.Normalize() * 64, ignore_monsters, ENT(pev), &tr );

	entvars_t *pevOwner = VARS( pev->owner );
	::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg, pev->dmg*2.5, CLASS_NONE, DMG_FREEZE);
	UTIL_DecalTrace(&tr, DECAL_FROST_SCORCH1 + RANDOM_LONG(0,1));
	FX_Trail( tr.vecEndPos + (tr.vecPlaneNormal * 12), entindex(), PROJ_ICE_DETONATE );
	UTIL_Remove( this );
}

void CFrostball:: Spawn( void )
{
	pev->movetype = MOVETYPE_FLY;
	pev->classname = MAKE_STRING( "frostball" );
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "sprites/anim_spr11.spr"); 
	pev->rendermode = kRenderTransAdd;
	pev->renderamt = 255;
	UTIL_SetSize(pev, g_vecZero, g_vecZero );
	pev->frame = 0;
	m_maxFrame = (float)MODEL_FRAMES(pev->modelindex)-1;
}

CFrostball *CFrostball::ShootFrostball( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity )
{
	CFrostball *pFrostball = GetClassPtr( (CFrostball *)NULL );
	pFrostball->Spawn();

	UTIL_SetOrigin( pFrostball->pev, vecStart );
	pFrostball->pev->velocity = vecVelocity + gpGlobals->v_right * RANDOM_FLOAT(-50,50) + gpGlobals->v_up * RANDOM_FLOAT(-50,50);
	pFrostball->pev->angles = UTIL_VecToAngles(pFrostball->pev->velocity);
	pFrostball->pev->owner = ENT(pevOwner);
	pFrostball->SetTouch( ExplodeTouch );
	pFrostball->pev->dmg = dmg_froster.value * (mp_wpn_power.value/100);
	pFrostball->SetThink ( Fly );
	pFrostball->pev->nextthink = 0.1;
	FX_Trail(pFrostball->pev->origin, pFrostball->entindex(), PROJ_ICE );
	return pFrostball;
}

void CFrostball::Fly( void )
{
	if ( UTIL_PointContents(pev->origin) == CONTENT_WATER )
	{
		entvars_t *pevOwner = VARS( pev->owner );
		FX_Trail( pev->origin, entindex(), PROJ_ICE_DETONATE_WATER );
		::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg/2, pev->dmg, CLASS_NONE, DMG_FREEZE );
		UTIL_Remove( this );
		return;
	}
	if ( gpGlobals->time >= pev->framerate )
	{
		if ( pev->frame++ )
		{
			if ( pev->frame > m_maxFrame )
			{
				pev->frame = 0;
			}
		}
		pev->framerate = gpGlobals->time + 0.03;
	}
	pev->nextthink = gpGlobals->time + 0.001;
}