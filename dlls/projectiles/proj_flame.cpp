#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "soundent.h"
#include "decals.h"
#include "game.h"
#include "projectiles.h"

LINK_ENTITY_TO_CLASS( flame, CFlame );

void CFlame:: Killed(entvars_t *pevAttacker, int iGib)
{
	FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
	UTIL_Remove (this);
}

void CFlame::ExplodeTouch( CBaseEntity *pOther )
{
	if ( UTIL_PointContents(pev->origin) == CONTENT_SKY )
	{
		FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
		UTIL_Remove( this );
		return;
	}
	if ( UTIL_PointContents(pev->origin) == CONTENT_WATER )
	{
		FX_Trail( pev->origin, entindex(), PROJ_FLAME_DETONATE_WATER );
		UTIL_Remove( this );
		return;
	}
	if (pOther->edict() == pev->owner)
	return;

	TraceResult tr;
	Vector vecSpot = pev->origin - pev->velocity.Normalize() * 32;
	UTIL_TraceLine( vecSpot, vecSpot + pev->velocity.Normalize() * 64, ignore_monsters, ENT(pev), &tr );

	FireStayTime = 8;

	entvars_t *pevOwner = VARS( pev->owner );
	::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg, pev->dmg*2.5, CLASS_NONE, DMG_IGNITE | DMG_NEVERGIB);
	UTIL_DecalTrace(&tr, DECAL_SMALLSCORCH1 + RANDOM_LONG(0,2));
	FX_Trail( tr.vecEndPos + (tr.vecPlaneNormal * 10), entindex(), PROJ_FLAME_DETONATE );

	pev->velocity = g_vecZero;
	SetTouch(NULL);
	SetThink( Burn );
	pev->nextthink = gpGlobals->time + 0.2;
}

void CFlame::Burn( void )
{
	if(FireStayTime <=0 )
		UTIL_Remove( this );
	else
		FireStayTime--;

	entvars_t *pevOwner = VARS( pev->owner );
	::RadiusDamage( pev->origin, pev, pevOwner, FireStayTime*3, pev->dmg*2.5, CLASS_NONE, DMG_BURN | DMG_NEVERGIB);
	pev->nextthink = gpGlobals->time + 0.2;
}

void CFlame:: Spawn( void )
{
	pev->movetype = MOVETYPE_FLY;
	pev->classname = MAKE_STRING( "flame" );
	SET_MODEL(ENT(pev), "sprites/anim_spr9.spr"); 
	pev->rendermode = kRenderTransAdd;
	pev->renderamt = 1;
	pev->frags = 7;

	pev->solid = SOLID_BBOX;
	UTIL_SetSize(pev, g_vecZero, g_vecZero );
}

CFlame *CFlame::ShootFlame( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity )
{
	CFlame *pFlame = GetClassPtr( (CFlame *)NULL );
	pFlame->Spawn();

	UTIL_SetOrigin( pFlame->pev, vecStart );
	pFlame->pev->velocity = vecVelocity + gpGlobals->v_right * RANDOM_FLOAT(-40,40) + gpGlobals->v_up * RANDOM_FLOAT(-40,40);
	pFlame->pev->angles = UTIL_VecToAngles(pFlame->pev->velocity);
	pFlame->pev->owner = ENT(pevOwner);
	pFlame->SetTouch( ExplodeTouch );
	pFlame->pev->dmg = dmg_flamethrower.value * (mp_wpn_power.value/100);
	pFlame->SetThink ( Fly );
	pFlame->pev->nextthink = 0.1;
	FX_Trail(pFlame->pev->origin, pFlame->entindex(), PROJ_FLAME );
	return pFlame;
}

void CFlame::Fly( void )
{
	if ( pev->frags <= 0 || UTIL_PointContents(pev->origin) == CONTENT_WATER )
	{
		FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
		UTIL_Remove( this );
		return;
	}
	pev->frags--;

	entvars_t *pevOwner = VARS(pev->owner);
	CBaseEntity *pOther = NULL;

	while ((pOther = UTIL_FindEntityInSphere( pOther, pev->origin, 50 )) != NULL)
	{
		if (pOther->edict() != pev->owner && pOther->pev->takedamage && pOther->Classify() != CLASS_MACHINE )
		{
			TraceResult tr;
			UTIL_TraceLine( pev->origin, pOther->pev->origin, dont_ignore_monsters, ENT(pev), &tr );
			CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);

			ClearMultiDamage( );
			pEntity->TraceAttack( pevOwner, pev->dmg/3, pev->velocity, &tr, DMG_IGNITE | DMG_NEVERGIB);
			ApplyMultiDamage( pev, pevOwner );
		}
	}
	pev->nextthink = gpGlobals->time + 0.1;
}