#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "decals.h"
#include "game.h"
#include "projectiles.h"

LINK_ENTITY_TO_CLASS( gluon, CGluon );

void CGluon::Killed (entvars_t *pevAttacker, int iGib)
{
	FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
	UTIL_Remove( this );
}

void CGluon::ExplodeTouch( CBaseEntity *pOther )
{
	if ( UTIL_PointContents(pev->origin) == CONTENT_SKY )
	{
		FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
		UTIL_Remove( this );
		return;
	}
	TraceResult tr;
	Vector vecSpot = pev->origin - pev->velocity.Normalize() * 32;
	UTIL_TraceLine( vecSpot, vecSpot + pev->velocity.Normalize() * 64, ignore_monsters, ENT(pev), &tr );
	UTIL_DecalTrace( &tr, DECAL_OFSCORCH1 + RANDOM_FLOAT(0,5));

	entvars_t *pevOwner = VARS( pev->owner );

	if ( UTIL_PointContents(pev->origin) == CONTENT_WATER )
	{
		FX_Trail( pev->origin, entindex(), PROJ_GLUON_DETONATE_WATER );
		::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg/2, pev->dmg/2, CLASS_NONE, DMG_ENERGYBLAST | DMG_NEVERGIB);
		UTIL_Remove( this );
		return;
	}
	::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg, pev->dmg*4, CLASS_NONE, DMG_ENERGYBLAST | DMG_NEVERGIB);
	FX_Trail( tr.vecEndPos + (tr.vecPlaneNormal * 30), entindex(), PROJ_GLUON_DETONATE );
	UTIL_Remove( this );
}

void CGluon:: Spawn( void )
{
	pev->movetype = MOVETYPE_BOUNCE;
	pev->classname = MAKE_STRING( "gluon" );
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "sprites/anim_spr6.spr"); 
	pev->rendermode = kRenderTransAdd;
	pev->renderamt = 255;
	pev->rendercolor = Vector(170,90,250);
	UTIL_SetSize(pev, g_vecZero, g_vecZero );
	pev->frame = 0;
	m_maxFrame = (float)MODEL_FRAMES(pev->modelindex)-1;
}

CGluon *CGluon::ShootGluon( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity )
{
	CGluon *pGluon = GetClassPtr( (CGluon *)NULL );
	pGluon->Spawn();

	pGluon->pev->gravity = 4;
	UTIL_SetOrigin( pGluon->pev, vecStart );
	pGluon->pev->velocity = vecVelocity;
	pGluon->pev->angles = UTIL_VecToAngles (pGluon->pev->velocity);
	pGluon->pev->owner = ENT(pevOwner);
	pGluon->SetThink ( Fly );
	pGluon->pev->nextthink = 0.001;
	pGluon->SetTouch( ExplodeTouch );
	pGluon->pev->dmg = (dmg_gluon.value*2) * (mp_wpn_power.value/100);
	FX_Trail(pGluon->pev->origin, pGluon->entindex(), PROJ_GLUON);
	return pGluon;
}

void CGluon::Fly( void )
{
	if ( UTIL_PointContents(pev->origin) == CONTENT_WATER )
	{
		entvars_t *pevOwner = VARS( pev->owner );
		FX_Trail( pev->origin, entindex(), PROJ_GLUON_DETONATE_WATER );
		::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg/2, pev->dmg/2, CLASS_NONE, DMG_ENERGYBLAST | DMG_NEVERGIB);
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