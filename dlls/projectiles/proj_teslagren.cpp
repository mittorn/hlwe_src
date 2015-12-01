#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "game.h"
#include "projectiles.h"

LINK_ENTITY_TO_CLASS( teslagren, CTeslagren);

void CTeslagren::Killed (entvars_t *pevAttacker, int iGib)
{
	FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
	UTIL_Remove( this );
}

void CTeslagren::ExplodeTouch( CBaseEntity *pOther )
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
	RadiusDamage ( pev, pevOwner, pev->dmg, CLASS_NONE, DMG_BLAST | DMG_SHOCK );
	FX_Trail( tr.vecEndPos + (tr.vecPlaneNormal * 15), entindex(), (UTIL_PointContents(pev->origin) == CONTENT_WATER)?PROJ_TESLAGREN_DETONATE_WATER:PROJ_TESLAGREN_DETONATE);

	int tex = (int)TEXTURETYPE_Trace(&tr, vecSpot, vecEnd);
	CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
	FX_ImpRocket( tr.vecEndPos, tr.vecPlaneNormal, pEntity->IsBSPModel()?1:0, BULLET_SMALEXP, (float)tex );

	if (pOther->pev->takedamage)
	{
		ClearMultiDamage( );
		pOther->TraceAttack(pevOwner, pev->dmg/4, gpGlobals->v_forward, &tr, DMG_BULLETMAGNUM ); 
		ApplyMultiDamage( pev, pevOwner);
	}
	UTIL_Remove( this );
}

void CTeslagren:: Spawn( void )
{
	pev->movetype = MOVETYPE_BOUNCE;
	pev->classname = MAKE_STRING( "teslagren" );
	pev->solid = SOLID_BBOX;
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = 4;
	UTIL_SetSize(pev, g_vecZero, g_vecZero );
}

CTeslagren *CTeslagren::ShootTeslagren( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity )
{
	CTeslagren*pTeslagren= GetClassPtr( (CTeslagren*)NULL );
	pTeslagren->Spawn();

	pTeslagren->pev->gravity = 0.1;
	UTIL_SetOrigin( pTeslagren->pev, vecStart );
	pTeslagren->pev->velocity = vecVelocity;
	pTeslagren->pev->angles = UTIL_VecToAngles (pTeslagren->pev->velocity);
	pTeslagren->pev->owner = ENT(pevOwner);
	pTeslagren->pev->nextthink = gpGlobals->time;
	pTeslagren->SetTouch( ExplodeTouch );
	pTeslagren->pev->dmg = dmg_tesla_grenade.value * (mp_wpn_power.value/100);
	FX_Trail(pTeslagren->pev->origin, pTeslagren->entindex(), PROJ_TESLAGREN );
	return pTeslagren;
}