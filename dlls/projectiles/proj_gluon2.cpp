#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "decals.h"
#include "game.h"
#include "projectiles.h"

LINK_ENTITY_TO_CLASS( gluon2, CGluon2 );

void CGluon2::Killed (entvars_t *pevAttacker, int iGib)
{
	FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
	UTIL_Remove( this );
}

void CGluon2::ExplodeTouch( CBaseEntity *pOther )
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
	UTIL_DecalTrace( &tr, DECAL_GLUONSCORCH1 + RANDOM_LONG(0,2));

	entvars_t *pevOwner = VARS( pev->owner );

	if ( UTIL_PointContents(pev->origin) == CONTENT_WATER )
	{
		FX_Trail( pev->origin, entindex(), PROJ_GLUON_DETONATE_WATER );
		::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg/2, pev->dmg/2, CLASS_NONE, DMG_ENERGYBLAST | DMG_NEVERGIB);
		UTIL_Remove( this );
		return;
	}
	::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg, pev->dmg*2, CLASS_NONE, DMG_ENERGYBLAST | DMG_NEVERGIB);
	FX_Trail( tr.vecEndPos + (tr.vecPlaneNormal * 30), entindex(), PROJ_GLUON2_DETONATE );
	UTIL_Remove( this );
}

void CGluon2:: Spawn( void )
{
	pev->movetype = MOVETYPE_FLY;
	pev->classname = MAKE_STRING( "gluon2" );
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "sprites/anim_spr6.spr"); 
	pev->rendermode = kRenderTransAdd;
	pev->renderamt = 255;
	pev->rendercolor = Vector(170,90,250);
	UTIL_SetSize(pev, g_vecZero, g_vecZero );
	pev->frame = 0;
	m_maxFrame = (float)MODEL_FRAMES(pev->modelindex)-1;
}

CGluon2 *CGluon2::ShootGluon2( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float flWastedAmmo)
{
	CGluon2 *pGluon2 = GetClassPtr((CGluon2 *)NULL );
	pGluon2->Spawn();

	UTIL_SetOrigin( pGluon2->pev, vecStart );
	pGluon2->pev->velocity = vecVelocity;
	pGluon2->pev->angles = UTIL_VecToAngles (pGluon2->pev->velocity);
	pGluon2->pev->owner = ENT(pevOwner);
	pGluon2->SetThink ( Fly );
	pGluon2->pev->nextthink = 0.001;
	pGluon2->SetTouch( ExplodeTouch );
	pGluon2->pev->dmg = (dmg_gluon.value*flWastedAmmo) * (mp_wpn_power.value/100);
	FX_Trail(pGluon2->pev->origin, pGluon2->entindex(), PROJ_GLUON2);
	return pGluon2;
}

void CGluon2::Fly( void )
{
	if ( UTIL_PointContents(pev->origin) == CONTENT_WATER )
	{
		entvars_t *pevOwner = VARS( pev->owner );
		::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg/2, pev->dmg/2, CLASS_NONE, DMG_ENERGYBLAST | DMG_NEVERGIB);                                                                  
		FX_Trail( pev->origin, entindex(), PROJ_GLUON_DETONATE_WATER );
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