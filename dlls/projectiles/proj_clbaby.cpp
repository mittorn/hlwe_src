#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "game.h"
#include "decals.h"
#include "projectiles.h"

LINK_ENTITY_TO_CLASS( clbaby, CClBaby );

void CClBaby::ExplodeTouch( CBaseEntity *pOther )
{
	if (pOther->pev->health == 666)
		return;

	TraceResult tr;
	Vector vecSpot = pev->origin - pev->velocity.Normalize() * 32;
	Vector vecEnd = pev->origin + pev->velocity.Normalize() * 64;
	UTIL_TraceLine( vecSpot, vecEnd, ignore_monsters, ENT(pev), &tr );

	entvars_t *pevOwner = VARS( pev->owner );
	::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg, pev->dmg*3, CLASS_NONE, DMG_FREEZE );
	UTIL_DecalTrace(&tr, DECAL_FROST_SCORCH1 + RANDOM_LONG(0,1));
	FX_Explosion( tr.vecEndPos + (tr.vecPlaneNormal * 15), EXPLOSION_CLUSTERBABY );
	UTIL_Remove( this );
}

void CClBaby:: Spawn( void )
{
	pev->movetype = MOVETYPE_BOUNCE;
	pev->classname = MAKE_STRING( "clbaby" );
	pev->solid = SOLID_BBOX;
	pev->health = 666;

	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	UTIL_SetSize(pev, g_vecZero, g_vecZero );

	// Safety removal
	pev->nextthink = gpGlobals->time + 4.0;
	SetThink( RemoveMe );
}

void CClBaby::RemoveMe( void )
{
	pev->owner = NULL;
	SetThink (SUB_Remove);
}

CClBaby *CClBaby::ShootClBaby( entvars_t *pevOwner, Vector vecStart)
{
	CClBaby *pClBaby = GetClassPtr( (CClBaby *)NULL );
	pClBaby->Spawn();

	pClBaby->pev->gravity = RANDOM_FLOAT(0.5,1);
	UTIL_SetOrigin( pClBaby->pev, vecStart );
	pClBaby->pev->velocity = gpGlobals->v_up * RANDOM_FLOAT(150,500) + gpGlobals->v_right * RANDOM_FLOAT(-350,350) + gpGlobals->v_forward * RANDOM_FLOAT(-350,350);
	pClBaby->pev->angles = UTIL_VecToAngles (pClBaby->pev->velocity);
	pClBaby->pev->owner = ENT(pevOwner);
	pClBaby->SetTouch( ExplodeTouch );
	pClBaby->pev->dmg = dmg_cluster_baby.value * (mp_wpn_power.value/100);
	pClBaby->pev->body = 11;
	pClBaby->pev->scale = 2;
	return pClBaby;
}