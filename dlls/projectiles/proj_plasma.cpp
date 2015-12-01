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

LINK_ENTITY_TO_CLASS( plasma, CPlasma );

void CPlasma::ExplodeTouch( CBaseEntity *pOther )
{
	if ( UTIL_PointContents(pev->origin) == CONTENT_SKY )
	{
		UTIL_Remove( this );
		return;
	}
	TraceResult tr;
	UTIL_TraceLine( pev->origin, pev->origin + pev->velocity.Normalize() * 64, dont_ignore_monsters, ENT(pev), &tr );
	entvars_t *pevOwner = VARS( pev->owner );

	if ( UTIL_PointContents(pev->origin) == CONTENT_WATER )
	{
		FX_Explosion( pev->origin, EXPLOSION_ENERGY_INWATER_S);
		::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg/2, pev->dmg, CLASS_NONE, DMG_PLASMA | DMG_NEVERGIB);
		UTIL_Remove( this );
		return;
	}
	CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
	RadiationStayTime = pev->dmg/2.5;
	::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg, pev->dmg*2.5, CLASS_NONE, DMG_PLASMA | DMG_NEVERGIB);
	FX_ImpBeam( tr.vecEndPos, tr.vecPlaneNormal, pEntity->IsBSPModel()?1:0, IMPBEAM_PLASMABALL );

	pev->effects |= EF_NODRAW;
	pev->velocity = g_vecZero;
	SetTouch(NULL);
	SetThink( Irradiate );
	pev->nextthink = gpGlobals->time + 0.3;
}

void CPlasma::Irradiate( void )
{
	if(RadiationStayTime <=0 )
		UTIL_Remove( this );
	else
		RadiationStayTime--;

	entvars_t *pevOwner = VARS( pev->owner );
	::RadiusDamage( pev->origin, pev, pevOwner, RadiationStayTime/2, pev->dmg*1.5, CLASS_MACHINE, DMG_RADIATION | DMG_NEVERGIB);
	pev->nextthink = gpGlobals->time + 0.3;
}

void CPlasma:: Spawn( void )
{
	pev->movetype = MOVETYPE_FLY;
	pev->classname = MAKE_STRING( "plasma" );
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "sprites/particles_green.spr"); 
	pev->rendermode = kRenderTransAdd;
	pev->renderamt = 255;
	pev->scale = 1;
	pev->frame = 8;
	UTIL_SetSize(pev, g_vecZero, g_vecZero );
}

CPlasma *CPlasma::ShootPlasma( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity )
{
	CPlasma *pPlasma = GetClassPtr( (CPlasma *)NULL );
	pPlasma->Spawn();

	UTIL_SetOrigin( pPlasma->pev, vecStart );
	pPlasma->pev->velocity = vecVelocity;
	pPlasma->pev->angles = UTIL_VecToAngles (pPlasma->pev->velocity);
	pPlasma->pev->owner = ENT(pevOwner);
	pPlasma->SetTouch( ExplodeTouch );
	pPlasma->SetThink ( Fly );
	pPlasma->pev->nextthink = 0.001;
	pPlasma->pev->dmg = dmg_plasma.value * (mp_wpn_power.value/100);
	FX_Trail(pPlasma->pev->origin, pPlasma->entindex(), PROJ_PLASMA);
	return pPlasma;
}

void CPlasma::Fly( void )
{
	if ( UTIL_PointContents(pev->origin) == CONTENT_WATER )
	{
		entvars_t *pevOwner = VARS( pev->owner );
		FX_Explosion( pev->origin, EXPLOSION_ENERGY_INWATER_S);
		::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg/2, pev->dmg, CLASS_NONE, DMG_PLASMA | DMG_NEVERGIB);
		UTIL_Remove( this );
		return;
	}
	pev->nextthink = gpGlobals->time + 0.001;
}