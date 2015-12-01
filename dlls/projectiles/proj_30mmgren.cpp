#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "game.h"
#include "projectiles.h"

LINK_ENTITY_TO_CLASS( sggren, C30mmgren );

void C30mmgren::Killed (entvars_t *pevAttacker, int iGib)
{
	FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
	UTIL_Remove( this );
}

void C30mmgren::ExplodeTouch( CBaseEntity *pOther )
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
	FX_ImpRocket( tr.vecEndPos, tr.vecPlaneNormal, pEntity->IsBSPModel()?1:0, BULLET_BOLT, (float)tex );

	entvars_t *pevOwner = VARS( pev->owner );
	RadiusDamage ( pev, pevOwner, pev->dmg, CLASS_NONE, DMG_BLAST );
	FX_Trail( tr.vecEndPos + (tr.vecPlaneNormal * 15), entindex(), (UTIL_PointContents(pev->origin) == CONTENT_WATER)?PROJ_30MMGREN_DETONATE_WATER:PROJ_30MMGREN_DETONATE );

	if (pOther->pev->takedamage)
	{
		ClearMultiDamage( );
		pOther->TraceAttack(pevOwner, pev->dmg/4, gpGlobals->v_forward, &tr, DMG_BULLETMAGNUM ); 
		ApplyMultiDamage( pev, pevOwner);
	}
	UTIL_Remove( this );
}

void C30mmgren:: Spawn( void )
{
	pev->movetype = MOVETYPE_BOUNCE;
	pev->classname = MAKE_STRING( "sggren" );
	pev->solid = SOLID_BBOX;
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = 4;
	UTIL_SetSize(pev, g_vecZero, g_vecZero );
}

C30mmgren *C30mmgren::Shoot30mmgren( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity )
{
	C30mmgren *p30mmgren = GetClassPtr( (C30mmgren *)NULL );
	p30mmgren->Spawn();

	UTIL_SetOrigin( p30mmgren->pev, vecStart );
	p30mmgren->pev->velocity = vecVelocity + gpGlobals->v_right * RANDOM_FLOAT(-100,100) + gpGlobals->v_up * RANDOM_FLOAT(-100,100);
	p30mmgren->pev->angles = UTIL_VecToAngles(p30mmgren->pev->velocity);
	p30mmgren->pev->gravity = 0.3;
	p30mmgren->pev->owner = ENT(pevOwner);
	p30mmgren->pev->nextthink = gpGlobals->time;
	p30mmgren->SetTouch( ExplodeTouch );
	p30mmgren->pev->dmg = dmg_30mmgren.value * (mp_wpn_power.value/100);
	FX_Trail(p30mmgren->pev->origin, p30mmgren->entindex(), PROJ_30MMGREN );

	return p30mmgren;
}