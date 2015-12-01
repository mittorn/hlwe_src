#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "decals.h"
#include "game.h"
#include "projectiles.h"
#include "player.h"

LINK_ENTITY_TO_CLASS(u2baby, CU2Baby );

void CU2Baby::Killed (entvars_t *pevAttacker, int iGib)
{
	FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
	UTIL_Remove( this );
}

void CU2Baby::Spawn( void )
{
	pev->movetype = MOVETYPE_BOUNCE;
	pev->solid = SOLID_BBOX;
	pev->classname = MAKE_STRING("u2baby");
	SET_MODEL( ENT(pev), "models/projectiles.mdl" );
	pev->body = 11;
	pev->health = 666;

	UTIL_SetSize(pev, g_vecZero, g_vecZero );
	UTIL_SetOrigin( pev, pev->origin );
}

void CU2Baby::ExplodeTouch( CBaseEntity *pOther )
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
	FX_Trail( tr.vecEndPos + (tr.vecPlaneNormal * 15), entindex(), (UTIL_PointContents(pev->origin) == CONTENT_WATER)?PROJ_U2_SHARD_DETONATE_WATER:PROJ_U2_SHARD_DETONATE);

	int tex = (int)TEXTURETYPE_Trace(&tr, vecSpot, vecEnd);
	CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
	FX_ImpRocket( tr.vecEndPos, tr.vecPlaneNormal, pEntity->IsBSPModel()?1:0, BULLET_SMALEXP, (float)tex );

	if (pOther->pev->takedamage)
	{
		ClearMultiDamage( );
		pOther->TraceAttack(pevOwner, pev->dmg/2.5, gpGlobals->v_forward, &tr, DMG_BULLETMAGNUM ); 
		ApplyMultiDamage( pev, pevOwner);
	}
	UTIL_Remove( this );
}

CU2Baby *CU2Baby::ShootU2Baby( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity )
{
	CU2Baby *pU2Baby = GetClassPtr( (CU2Baby *)NULL );
	pU2Baby->Spawn();

	pU2Baby->pev->gravity = RANDOM_FLOAT(0.5,1);
	UTIL_SetOrigin( pU2Baby->pev, vecStart );
	pU2Baby->pev->velocity = vecVelocity;
	pU2Baby->pev->angles = UTIL_VecToAngles (pU2Baby->pev->velocity);
	pU2Baby->pev->owner = ENT(pevOwner);
	pU2Baby->SetTouch( ExplodeTouch );
	pU2Baby->pev->dmg = dmg_u2shard.value * (mp_wpn_power.value/100);
	FX_Trail(pU2Baby->pev->origin, pU2Baby->entindex(), PROJ_U2_SHARD );
	return pU2Baby;
}