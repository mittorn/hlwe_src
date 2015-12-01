#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "game.h"
#include "projectiles.h"

LINK_ENTITY_TO_CLASS( ak74grenade, CAK74grenade );

void CAK74grenade::Killed (entvars_t *pevAttacker, int iGib)
{
	FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
	UTIL_Remove( this );
}

void CAK74grenade::ExplodeTouch( CBaseEntity *pOther )
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

	entvars_t *pevOwner = VARS( pev->owner );
	RadiusDamage ( pev, pevOwner, pev->dmg, CLASS_NONE, DMG_BLAST );
	FX_Trail( tr.vecEndPos + (tr.vecPlaneNormal * 15), entindex(), (UTIL_PointContents(pev->origin) == CONTENT_WATER)?PROJ_AK74_DETONATE_WATER:PROJ_AK74_DETONATE );

	int tex = (int)TEXTURETYPE_Trace(&tr, vecSpot, vecEnd);
	CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
	FX_ImpRocket( tr.vecEndPos, tr.vecPlaneNormal, pEntity->IsBSPModel()?1:0, BULLET_NORMEXP, (float)tex );

	if (pOther->pev->takedamage)
	{
		ClearMultiDamage( );
		pOther->TraceAttack(pevOwner, pev->dmg/4, gpGlobals->v_forward, &tr, DMG_BULLETMAGNUM ); 
		ApplyMultiDamage( pev, pevOwner);
	}
	UTIL_Remove( this );
}

void CAK74grenade:: Spawn( void )
{
	pev->movetype = MOVETYPE_BOUNCE;
	pev->classname = MAKE_STRING( "ak74grenade" );
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = 2;
	UTIL_SetSize(pev, g_vecZero, g_vecZero );
}

CAK74grenade *CAK74grenade::ShootAK74grenade( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity )
{
	CAK74grenade *pAK74grenade = GetClassPtr( (CAK74grenade *)NULL );
	pAK74grenade->Spawn();

	pAK74grenade->pev->gravity = 1.2;
	UTIL_SetOrigin( pAK74grenade->pev, vecStart );
	pAK74grenade->pev->velocity = vecVelocity + gpGlobals->v_right * RANDOM_FLOAT(-100,100) + gpGlobals->v_up * RANDOM_FLOAT(-100,100);
	pAK74grenade->pev->angles = UTIL_VecToAngles (pAK74grenade->pev->velocity);
	pAK74grenade->pev->owner = ENT(pevOwner);
	pAK74grenade->SetTouch( ExplodeTouch );
	pAK74grenade->pev->dmg = dmg_ak74_grenade.value * (mp_wpn_power.value/100);
	FX_Trail(pAK74grenade->pev->origin, pAK74grenade->entindex(), PROJ_AK74 );
	return pAK74grenade;
}