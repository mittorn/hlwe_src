#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "decals.h"
#include "game.h"
#include "projectiles.h"

LINK_ENTITY_TO_CLASS( mmissile, CMmissile );

void CMmissile::Killed (entvars_t *pevAttacker, int iGib)
{
	FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
	UTIL_Remove( this );
}

void CMmissile::ExplodeTouch( CBaseEntity *pOther )
{
	if ( UTIL_PointContents(pev->origin) == CONTENT_SKY )
	{
		FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
		UTIL_Remove( this );
		return;
	}
	if (pOther->pev->health == 666)
	{
		return;
	}
	TraceResult tr;
	Vector vecSpot = pev->origin - pev->velocity.Normalize() * 32;
	Vector vecEnd = pev->origin + pev->velocity.Normalize() * 64;
	UTIL_TraceLine( vecSpot, vecEnd, ignore_monsters, ENT(pev), &tr );

	entvars_t *pevOwner = VARS( pev->owner );
	RadiusDamage ( pev, pevOwner, pev->dmg, CLASS_NONE, DMG_BLAST );
	FX_Trail( tr.vecEndPos + (tr.vecPlaneNormal * 15), entindex(), (UTIL_PointContents(pev->origin) == CONTENT_WATER)?PROJ_MMISSILE_DETONATE_WATER:PROJ_MMISSILE_DETONATE );

	int tex = (int)TEXTURETYPE_Trace(&tr, vecSpot, vecEnd);
	CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
	FX_ImpRocket( tr.vecEndPos, tr.vecPlaneNormal, pEntity->IsBSPModel()?1:0, BULLET_SMALEXP, (float)tex );

	if (pOther->pev->takedamage)
	{
		ClearMultiDamage( );
		pOther->TraceAttack(pevOwner, pev->dmg/3, gpGlobals->v_forward, &tr, DMG_BULLETMAGNUM ); 
		ApplyMultiDamage( pev, pevOwner);
	}
	UTIL_Remove( this );
}

void CMmissile:: Spawn( void )
{
	pev->movetype = MOVETYPE_FLY;
	pev->classname = MAKE_STRING( "mmissile" );
	pev->solid = SOLID_BBOX;
	pev->health = 666;

	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = 14;
	UTIL_SetSize(pev, g_vecZero, g_vecZero );
}

CMmissile *CMmissile::ShootMmissile( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity )
{
	CMmissile *pMmissile = GetClassPtr( (CMmissile *)NULL );
	pMmissile->Spawn();

	UTIL_SetOrigin( pMmissile->pev, vecStart );
	pMmissile->pev->velocity = vecVelocity;
	pMmissile->pev->angles = UTIL_VecToAngles (pMmissile->pev->velocity);
	pMmissile->pev->owner = ENT(pevOwner);
	pMmissile->SetTouch( ExplodeTouch );
	pMmissile->pev->dmg = dmg_devastator.value * (mp_wpn_power.value/100);
	FX_Trail(pMmissile->pev->origin, pMmissile->entindex(), PROJ_MMISSILE );
	return pMmissile;
}