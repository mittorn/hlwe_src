#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "decals.h"
#include "game.h"
#include "projectiles.h"

LINK_ENTITY_TO_CLASS( plasma2, CPlasma2 );

void CPlasma2::ExplodeTouch( CBaseEntity *pOther )
{
	if ( UTIL_PointContents(pev->origin) == CONTENT_SKY )
	{
		UTIL_Remove( this );
		return;
	}
	TraceResult tr;
	Vector vecSpot = pev->origin - pev->velocity.Normalize() * 32;
	UTIL_TraceLine( vecSpot, vecSpot + pev->velocity.Normalize() * 64, ignore_monsters, ENT(pev), &tr );
	UTIL_DecalTrace( &tr, DECAL_OFSCORCH1 + RANDOM_FLOAT(0,5));

	entvars_t *pevOwner = VARS( pev->owner );

	if ( UTIL_PointContents(pev->origin) == CONTENT_WATER )
	{
		::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg/2, pev->dmg, CLASS_NONE, DMG_PLASMA | DMG_NEVERGIB);
		FX_Explosion( pev->origin, EXPLOSION_ENERGY_INWATER_M);
		UTIL_Remove( this );
		return;
	}
	RadiationStayTime = pev->dmg/4;
	CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
	::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg, pev->dmg*1.2, CLASS_NONE, DMG_PLASMA | DMG_NEVERGIB);
	FX_Explosion( tr.vecEndPos + (tr.vecPlaneNormal * 15), ( tr.flFraction != 1.0 )?EXPLOSION_PLASMABALL2_SPARKS:EXPLOSION_PLASMABALL2);

	pev->effects |= EF_NODRAW;
	pev->velocity = g_vecZero;
	SetTouch(NULL);
	SetThink( Irradiate );
	pev->nextthink = gpGlobals->time + 0.3;
}

void CPlasma2::Irradiate( void )
{
	if(RadiationStayTime <=0 )
		UTIL_Remove( this );
	else
		RadiationStayTime--;

	entvars_t *pevOwner = VARS( pev->owner );
	::RadiusDamage( pev->origin, pev, pevOwner, RadiationStayTime/2, pev->dmg*1.2, CLASS_MACHINE, DMG_RADIATION | DMG_NEVERGIB);
	pev->nextthink = gpGlobals->time + 0.3;
}

void CPlasma2:: Spawn( void )
{
	pev->movetype = MOVETYPE_FLY;
	pev->classname = MAKE_STRING( "plasma2" );
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "sprites/particles_green.spr"); 
	pev->rendermode = kRenderTransAdd;
	pev->renderamt = 255;
	pev->scale = 1.2;
	pev->frame = 7;
	pev->rendercolor = Vector(0,255,0);
	UTIL_SetSize(pev, g_vecZero, g_vecZero );
}

CPlasma2 *CPlasma2::ShootPlasma2( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity )
{
	CPlasma2 *pPlasma2 = GetClassPtr( (CPlasma2 *)NULL );
	pPlasma2->Spawn();

	UTIL_SetOrigin( pPlasma2->pev, vecStart );
	pPlasma2->pev->velocity = vecVelocity;
	pPlasma2->pev->angles = UTIL_VecToAngles (pPlasma2->pev->velocity);
	pPlasma2->pev->owner = ENT(pevOwner);
	pPlasma2->SetTouch( ExplodeTouch );
	pPlasma2->SetThink ( Fly );
	pPlasma2->pev->nextthink = 0.001;
	pPlasma2->pev->dmg = dmg_plasma_secondary.value * (mp_wpn_power.value/100);
	FX_Trail(pPlasma2->pev->origin, pPlasma2->entindex(), PROJ_PLASMA);
	return pPlasma2;
}

void CPlasma2::Fly( void )
{
	if ( UTIL_PointContents(pev->origin) == CONTENT_WATER )
	{
		entvars_t *pevOwner = VARS( pev->owner );
		FX_Explosion( pev->origin, EXPLOSION_ENERGY_INWATER_M);
		::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg/2, pev->dmg, CLASS_NONE, DMG_PLASMA | DMG_NEVERGIB);
		UTIL_Remove( this );
		return;
	}
	pev->nextthink = gpGlobals->time + 0.001;
}