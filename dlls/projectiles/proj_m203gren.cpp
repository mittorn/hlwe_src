#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "game.h"
#include "projectiles.h"

LINK_ENTITY_TO_CLASS( m203grenade, CM203grenade );

void CM203grenade::Killed (entvars_t *pevAttacker, int iGib)
{
	FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
	UTIL_Remove( this );
}

void CM203grenade::ExplodeTouch( CBaseEntity *pOther )
{
	if ( UTIL_PointContents(pev->origin) == CONTENT_SKY )
	{
		FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
		UTIL_Remove( this );
		return;
	}
	TraceResult tr;
	Vector vecSpot = pev->origin - pev->velocity.Normalize() * 32;
	Vector vecEnd = pev->origin + pev->velocity.Normalize() * 64;
	UTIL_TraceLine( vecSpot, vecEnd, ignore_monsters, ENT(pev), &tr );

	int tex = (int)TEXTURETYPE_Trace(&tr, vecSpot, vecEnd);
	CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
	FX_ImpRocket( tr.vecEndPos, tr.vecPlaneNormal, pEntity->IsBSPModel()?1:0, BULLET_NORMEXP, (float)tex );

	entvars_t *pevOwner = VARS( pev->owner );
	RadiusDamage ( pev, pevOwner, pev->dmg, CLASS_NONE, DMG_BLAST );
	FX_Trail( tr.vecEndPos + (tr.vecPlaneNormal * 15), entindex(), (UTIL_PointContents(pev->origin) == CONTENT_WATER)?PROJ_M203_DETONATE_WATER:PROJ_M203_DETONATE );

	if (pOther->pev->takedamage)
	{
		ClearMultiDamage( );
		pOther->TraceAttack(pevOwner, pev->dmg/4, gpGlobals->v_forward, &tr, DMG_BULLETMAGNUM ); 
		ApplyMultiDamage( pev, pevOwner);
	}
	UTIL_Remove( this );
}

void CM203grenade:: Spawn( void )
{
	pev->movetype = MOVETYPE_BOUNCE;
	pev->classname = MAKE_STRING( "m203grenade" );
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = 3;
	UTIL_SetSize(pev, g_vecZero, g_vecZero );
}

CM203grenade *CM203grenade::ShootM203grenade( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity)
{
	CM203grenade *pM203grenade = GetClassPtr( (CM203grenade *)NULL );
	pM203grenade->Spawn();

	pM203grenade->pev->gravity = 0.5;
	UTIL_SetOrigin( pM203grenade->pev, vecStart );
	pM203grenade->pev->velocity = vecVelocity + gpGlobals->v_right * RANDOM_FLOAT(-40,40) + gpGlobals->v_up * RANDOM_FLOAT(-40,40);
	pM203grenade->pev->angles = UTIL_VecToAngles (pM203grenade->pev->velocity);
	pM203grenade->pev->owner = ENT(pevOwner);
	pM203grenade->SetTouch( ExplodeTouch );
	pM203grenade->pev->dmg = dmg_mp5_grenade.value * (mp_wpn_power.value/100);
	FX_Trail(pM203grenade->pev->origin, pM203grenade->entindex(), PROJ_M203 );
	return pM203grenade;
}