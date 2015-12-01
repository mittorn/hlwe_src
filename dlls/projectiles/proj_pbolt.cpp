#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "decals.h"
#include "explode.h"
#include "game.h"
#include "projectiles.h"

LINK_ENTITY_TO_CLASS( pbolt, CPBolt );

void CPBolt::ExplodeTouch( CBaseEntity *pOther )
{
	if ( UTIL_PointContents(pev->origin) == CONTENT_SKY )
	{
		UTIL_Remove( this );
		return;
	}
	entvars_t *pevOwner = VARS( pev->owner );

	if ( UTIL_PointContents(pev->origin) == CONTENT_WATER )
	{
		::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg, pev->dmg, CLASS_NONE, DMG_PLASMA | DMG_NEVERGIB);
		FX_Explosion( pev->origin, EXPLOSION_ENERGY_INWATER_S);
		UTIL_Remove( this );
		return;
	}
	TraceResult tr;
	Vector vecSpot = pev->origin - pev->velocity.Normalize() * 32;
	UTIL_TraceLine( vecSpot, vecSpot + pev->velocity.Normalize() * 64, ignore_monsters, ENT(pev), &tr );
	CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);

	::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg, pev->dmg*2.5, CLASS_NONE, DMG_PLASMA | DMG_NEVERGIB);
	FX_ImpBeam( tr.vecEndPos, tr.vecPlaneNormal, pEntity->IsBSPModel()?1:0, IMPBEAM_PBOLT );

	pev->effects |= EF_NODRAW;
	pev->velocity = g_vecZero;
	SetTouch(NULL);

	SetThink( Nova );
	pev->nextthink = gpGlobals->time + 0.9;
}

void CPBolt::Nova( void )
{
	entvars_t *pevOwner = VARS( pev->owner );
	::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg, pev->dmg*3, CLASS_NONE, DMG_SONIC);
	FX_Explosion( pev->origin, EXPLOSION_PBOLT);
	SetThink(SUB_Remove);
}

void CPBolt:: Spawn( void )
{
	pev->movetype = MOVETYPE_FLY;
	pev->classname = MAKE_STRING( "pbolt" );
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "sprites/anim_spr10.spr"); 
	pev->rendermode = kRenderTransAdd;
	pev->renderamt = 255;
	pev->scale = 1;
	pev->rendercolor = Vector(220,150,0);
	UTIL_SetSize(pev, g_vecZero, g_vecZero );
}

CPBolt *CPBolt::ShootPBolt( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity )
{
	CPBolt *pPBolt = GetClassPtr( (CPBolt *)NULL );
	pPBolt->Spawn();

	UTIL_SetOrigin( pPBolt->pev, vecStart );
	pPBolt->pev->velocity = vecVelocity;
	pPBolt->pev->angles = UTIL_VecToAngles (pPBolt->pev->velocity);
	pPBolt->pev->owner = ENT(pevOwner);
	pPBolt->SetTouch( ExplodeTouch );
	pPBolt->SetThink ( Fly );
	pPBolt->pev->nextthink = 0.001;
	pPBolt->pev->dmg = dmg_lightsaber_pistol.value * (mp_wpn_power.value/100);
	return pPBolt;
}

void CPBolt::Fly( void )
{
	if ( UTIL_PointContents(pev->origin) == CONTENT_WATER )
	{
		entvars_t *pevOwner = VARS( pev->owner );
		FX_Explosion( pev->origin, EXPLOSION_ENERGY_INWATER_S);
		::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg, pev->dmg, CLASS_NONE, DMG_ENERGYBLAST | DMG_NEVERGIB);
		UTIL_Remove( this );
		return;
	}
	pev->nextthink = gpGlobals->time + 0.001;
}