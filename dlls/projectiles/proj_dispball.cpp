#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "decals.h"
#include "game.h"
#include "projectiles.h"

LINK_ENTITY_TO_CLASS( dispball, CDispball );

void CDispball::Killed (entvars_t *pevAttacker, int iGib)
{
	FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
	UTIL_Remove( this );
}

void CDispball::ExplodeTouch( CBaseEntity *pOther )
{
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
	::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg, pev->dmg*1.8, CLASS_NONE, DMG_ENERGYBLAST | DMG_NEVERGIB );
	UTIL_DecalTrace( &tr, DECAL_GLUONSCORCH1 + RANDOM_LONG(0,2));
	FX_Trail( tr.vecEndPos + (tr.vecPlaneNormal * 30), entindex(), (pev->frags==1)?PROJ_DISPPOWER_DETONATE:PROJ_DISPLACER_DETONATE );
	UTIL_Remove( this );
}

void CDispball:: Spawn( void )
{
	pev->movetype = MOVETYPE_FLY;
	pev->classname = MAKE_STRING( "dispball" );
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "sprites/anim_spr12.spr"); 
	pev->rendermode = kRenderTransAdd;
	pev->renderamt = 255;
	UTIL_SetSize(pev, g_vecZero, g_vecZero );
	pev->frame = 0;
	m_maxFrame = (float)MODEL_FRAMES(pev->modelindex)-1;
}

CDispball *CDispball::ShootDispball(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float flWastedAmmo)
{
	CDispball *pDispball = GetClassPtr( (CDispball *)NULL );
	pDispball->Spawn();

	if (flWastedAmmo == 1)
		pDispball->pev->frags = 0;
	else if (flWastedAmmo == 2)
		pDispball->pev->frags = 1;

	UTIL_SetOrigin( pDispball->pev, vecStart );
	pDispball->pev->velocity = vecVelocity;
	pDispball->pev->angles = UTIL_VecToAngles (pDispball->pev->velocity);
	pDispball->pev->owner = ENT(pevOwner);
	pDispball->SetTouch( ExplodeTouch );
	pDispball->pev->dmg = (dmg_displacer.value*flWastedAmmo) * mp_wpn_power.value/100;
	FX_Trail(pDispball->pev->origin, pDispball->entindex(), PROJ_DISPLACER);
	pDispball->SetThink ( Fly );
	pDispball->pev->nextthink = 0.001;
	return pDispball;
}
void CDispball::Fly( void )
{
	if ( UTIL_PointContents(pev->origin) == CONTENT_WATER )
	{
		entvars_t *pevOwner = VARS( pev->owner );
 		::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg, pev->dmg, CLASS_NONE, DMG_ENERGYBLAST | DMG_NEVERGIB );
		FX_Trail( pev->origin, entindex(), PROJ_DISPLACER_DETONATE_WATER );
		UTIL_Remove( this );
		return;
	}
	if ( gpGlobals->time >= pev->framerate )
	{
		if ( pev->frame++ )
		{
			if ( pev->frame > m_maxFrame )
			{
				pev->frame = 0;
			}
		}
		pev->framerate = gpGlobals->time + 0.03;
	}
	pev->nextthink = gpGlobals->time + 0.001;
}