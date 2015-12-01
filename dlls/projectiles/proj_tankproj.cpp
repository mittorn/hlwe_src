#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "explode.h"
#include "game.h"
#include "projectiles.h"

LINK_ENTITY_TO_CLASS( tankproj, CTankProj );

void CTankProj::Killed (entvars_t *pevAttacker, int iGib)
{
	UTIL_Remove( this );
}

void CTankProj::ExplodeTouch( CBaseEntity *pOther )
{
	if ( UTIL_PointContents(pev->origin) == CONTENT_SKY )
	{
		UTIL_Remove( this );
		return;
	}

	TraceResult tr;
	Vector vecSpot = pev->origin - pev->velocity.Normalize() * 32;
	Vector vecEnd = pev->origin + pev->velocity.Normalize() * 64;
	UTIL_TraceLine( vecSpot, vecEnd, ignore_monsters, ENT(pev), &tr );

	int tex = (int)TEXTURETYPE_Trace(&tr, vecSpot, vecEnd);
	CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
	FX_ImpRocket( tr.vecEndPos, tr.vecPlaneNormal, pEntity->IsBSPModel()?1:0, BULLET_HIGHEXP, (float)tex );

	entvars_t *pevOwner = VARS( pev->owner );
	::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg, pev->dmg*0.8, CLASS_NONE, DMG_BLAST);
	FX_Explosion( tr.vecEndPos + (tr.vecPlaneNormal * 25), EXPLOSION_TANKPROJ );

	if (pOther->pev->takedamage)
	{
		ClearMultiDamage( );
		pOther->TraceAttack(pevOwner, pev->dmg/4, gpGlobals->v_forward, &tr, DMG_BULLETMAGNUM ); 
		ApplyMultiDamage( pev, pevOwner);
	}
	UTIL_Remove( this );
}

void CTankProj:: Spawn( void )
{
	pev->movetype = MOVETYPE_FLY;
	pev->classname = MAKE_STRING( "tankproj" );
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	UTIL_SetSize(pev, g_vecZero, g_vecZero );
}

CTankProj *CTankProj::ShootTankProj(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity)
{
	CTankProj *pTankProj = GetClassPtr( (CTankProj *)NULL );
	pTankProj->Spawn();

	UTIL_SetOrigin(pTankProj->pev, vecStart);
	pTankProj->pev->velocity = vecVelocity;
	pTankProj->pev->angles = UTIL_VecToAngles (pTankProj->pev->velocity);
	pTankProj->pev->owner = ENT(pevOwner);
	pTankProj->SetTouch( ExplodeTouch );
	pTankProj->pev->dmg = dmg_tank_cannon.value * (mp_wpn_power.value/100);
	return pTankProj;
}