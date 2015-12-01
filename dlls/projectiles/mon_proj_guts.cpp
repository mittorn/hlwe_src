#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "game.h"
#include "mon_projectiles.h"

LINK_ENTITY_TO_CLASS( Guts, CGuts );

void CGuts::Killed (entvars_t *pevAttacker, int iGib)
{
	FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
	UTIL_Remove( this );
}

void CGuts::ExplodeTouch( CBaseEntity *pOther )
{
	if (pOther->Classify() == CLASS_PROJECTILE || pOther->Classify() == CLASS_DEAD)
		return;

	if ( UTIL_PointContents(pev->origin) == CONTENT_SKY )
	{
		FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
		UTIL_Remove( this );
		return;
	}
	TraceResult tr;
	Vector vecSpot = pev->origin - pev->velocity.Normalize() * 32;
	UTIL_TraceLine( vecSpot, vecSpot + pev->velocity.Normalize() * 64, ignore_monsters, ENT(pev), &tr );
	entvars_t *pevOwner = VARS( pev->owner );

	if (pOther->pev->takedamage)
	{
		ClearMultiDamage( );
		pOther->TraceAttack( pevOwner, pev->dmg, (pOther->pev->origin - pev->origin).Normalize(), &tr, DMG_SLASH );
		ApplyMultiDamage( pev, pevOwner );
	}

	//AND SMALL RADIUS...
	while ((pOther = UTIL_FindEntityInSphere( pOther, pev->origin, 50 )) != NULL)
	{
		if ( pOther->pev->takedamage)
		{

			ClearMultiDamage( );
			pOther->TraceAttack( pevOwner, pev->dmg, (pOther->pev->origin - pev->origin).Normalize(), &tr, DMG_SLASH );
			ApplyMultiDamage( pev, pevOwner );
		}
	}

	FX_Trail( tr.vecEndPos + (tr.vecPlaneNormal * 10), entindex(), PROJ_GUTS_DETONATE );
	UTIL_Remove( this );
}

void CGuts:: Spawn( void )
{
	pev->movetype = MOVETYPE_BOUNCE;
	pev->classname = MAKE_STRING( "Guts" );
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "models/w_gibs_all.mdl");
	pev->body = RANDOM_LONG(39,48);
	UTIL_SetSize(pev, g_vecZero, g_vecZero );
}

CGuts *CGuts::ShootGuts( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity )
{
	CGuts *pGuts = GetClassPtr( (CGuts *)NULL );
	pGuts->Spawn();

	pGuts->pev->gravity = 1;
	UTIL_SetOrigin( pGuts->pev, vecStart );
	pGuts->pev->velocity = vecVelocity;
	pGuts->pev->angles = UTIL_VecToAngles (pGuts->pev->velocity);
	pGuts->pev->owner = ENT(pevOwner);
	pGuts->SetTouch( ExplodeTouch );
	pGuts->pev->dmg = zombie_dmg_range.value;
	pGuts->pev->avelocity.y = RANDOM_FLOAT (-200, -500);
	FX_Trail(pGuts->pev->origin, pGuts->entindex(), PROJ_GUTS );
	return pGuts;
}
