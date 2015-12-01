#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "game.h"
#include "projectiles.h"

LINK_ENTITY_TO_CLASS( flak_bomb, CFlakBomb );

void CFlakBomb::Killed (entvars_t *pevAttacker, int iGib)
{
	FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
	UTIL_Remove( this );
}

void CFlakBomb::ExplodeTouch( CBaseEntity *pOther )
{
	if ( UTIL_PointContents(pev->origin) == CONTENT_SKY )
	{
		FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
		UTIL_Remove( this );
		return;
	}
	TraceResult tr;
	Vector vecSpot = pev->origin - pev->velocity.Normalize() * 64;
	Vector vecEnd = pev->origin + pev->velocity.Normalize() * 64;
	UTIL_TraceLine( vecSpot, vecEnd, ignore_monsters, ENT(pev), &tr );

	int tex = (int)TEXTURETYPE_Trace(&tr, vecSpot, vecEnd);
	CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
	FX_ImpRocket( tr.vecEndPos, tr.vecPlaneNormal, pEntity->IsBSPModel()?1:0, BULLET_SMALEXP, (float)tex );

	entvars_t *pevOwner = VARS(pev->owner);
	RadiusDamage ( pev, pevOwner, pev->dmg, CLASS_NONE, DMG_BLAST );

	for ( int i = 0; i < 10; i++ )
		CShrapnel::ShootBombShrap(pevOwner, pev->origin+Vector(0,0,15));
	FX_Trail( tr.vecEndPos + (tr.vecPlaneNormal * 15), entindex(), (UTIL_PointContents(pev->origin) == CONTENT_WATER)?PROJ_FLAKBOMB_DETONATE_WATER:PROJ_FLAKBOMB_DETONATE);

	if (pOther->pev->takedamage)
	{
		ClearMultiDamage( );
		pOther->TraceAttack(pevOwner, pev->dmg/4, gpGlobals->v_forward, &tr, DMG_BULLETMAGNUM ); 
		ApplyMultiDamage( pev, pevOwner);
	}
	UTIL_Remove( this );
}

void CFlakBomb:: Spawn( void )
{
	pev->movetype = MOVETYPE_BOUNCE;
	pev->classname = MAKE_STRING( "flak_bomb" );
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = 17;
	UTIL_SetSize(pev, g_vecZero, g_vecZero );
}

CFlakBomb *CFlakBomb::ShootFlakBomb( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity)
{
	CFlakBomb *pFlakBomb = GetClassPtr( (CFlakBomb *)NULL );
	pFlakBomb->Spawn();

	pFlakBomb->pev->gravity = 1.2;
	UTIL_SetOrigin( pFlakBomb->pev, vecStart );
	pFlakBomb->pev->velocity = vecVelocity + gpGlobals->v_right * RANDOM_FLOAT(-30,30) + gpGlobals->v_up * RANDOM_FLOAT(-30,30);
	pFlakBomb->pev->angles = UTIL_VecToAngles (pFlakBomb->pev->velocity);
	pFlakBomb->pev->owner = ENT(pevOwner);
	pFlakBomb->SetTouch( ExplodeTouch );
	pFlakBomb->pev->dmg = dmg_flak_bomb.value * (mp_wpn_power.value/100);
	FX_Trail(pFlakBomb->pev->origin, pFlakBomb->entindex(), PROJ_FLAKBOMB );
	return pFlakBomb;
}