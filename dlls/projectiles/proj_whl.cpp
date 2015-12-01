#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "explode.h"
#include "game.h"
#include "projectiles.h"

LINK_ENTITY_TO_CLASS( wh, CWhlProjectile );

void CWhlProjectile::Killed (entvars_t *pevAttacker, int iGib)
{
	entvars_t *pevOwner = VARS( pev->owner );
	SET_VIEW( pev->owner, pev->owner );
	STOP_SOUND( edict(), CHAN_VOICE, "weapons/whl_rocket_fly.wav" );
	FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
	UTIL_Remove( this );
}

void CWhlProjectile::Explode( TraceResult *pTrace, int bitsDamageType )
{
	pev->solid = SOLID_NOT;
	pev->takedamage = DAMAGE_NO;

	entvars_t *pevOwner = VARS( pev->owner );
	::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg, pev->dmg*2, CLASS_NONE, DMG_BLAST);
	FX_Trail( pTrace->vecEndPos + (pTrace->vecPlaneNormal * 25), entindex(), (UTIL_PointContents(pev->origin) == CONTENT_WATER)?PROJ_WARHEAD_DETONATE_WATER:PROJ_WARHEAD_DETONATE );

	if ( pTrace->flFraction != 1.0 )
		pev->origin = pTrace->vecEndPos + (pTrace->vecPlaneNormal * 210);

	pev->effects |= EF_NODRAW;
	pev->velocity = g_vecZero;
	SetTouch(NULL);
	SetThink(ShootShards);
	pev->nextthink = gpGlobals->time + 0.3;
}

void CWhlProjectile::ShootShards( void )
{
	if(ExplodeStayTime <=0 )
		UTIL_Remove( this );
	else
		ExplodeStayTime--;

	entvars_t *pevOwner = VARS( pev->owner );
	UTIL_MakeVectors (pev->angles);
	Vector ExplodeOrig = pev->origin + gpGlobals->v_forward * RANDOM_FLOAT(-200,200) + gpGlobals->v_right * RANDOM_FLOAT(-200,200) + gpGlobals->v_up * RANDOM_FLOAT(-200,200);

	FX_Explosion( ExplodeOrig, EXPLOSION_WHL_SHARD );

	::RadiusDamage( ExplodeOrig, pev, pevOwner, pev->dmg/3, pev->dmg/2, CLASS_NONE, DMG_BLAST );
	pev->nextthink = gpGlobals->time + 0.3;
}

void CWhlProjectile::ExplodeTouch( CBaseEntity *pOther )
{
	entvars_t *pevOwner = VARS( pev->owner );
	SET_VIEW( pev->owner, pev->owner );
	STOP_SOUND( edict(), CHAN_VOICE, "weapons/whl_rocket_fly.wav" );

	if ( UTIL_PointContents(pev->origin) == CONTENT_SKY )
	{
		FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
		UTIL_Remove( this );
		return;
	}
	ExplodeStayTime = 10;

	TraceResult tr;
	Vector vecSpot = pev->origin - pev->velocity.Normalize() * 32;
	Vector vecEnd = pev->origin + pev->velocity.Normalize() * 64;
	UTIL_TraceLine( vecSpot, vecEnd, ignore_monsters, ENT(pev), &tr );

	Explode( &tr, DMG_BLAST );

	int tex = (int)TEXTURETYPE_Trace(&tr, vecSpot, vecEnd);
	CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
	FX_ImpRocket( tr.vecEndPos, tr.vecPlaneNormal, pEntity->IsBSPModel()?1:0, BULLET_HIGHEXP, (float)tex );

	if (pOther->pev->takedamage)
	{
		ClearMultiDamage( );
		pOther->TraceAttack(pevOwner, pev->dmg/3, gpGlobals->v_forward, &tr, DMG_BULLETMAGNUM ); 
		ApplyMultiDamage( pev, pevOwner);
	}
}

void CWhlProjectile:: Spawn( void )
{
	pev->movetype = MOVETYPE_FLY;
	pev->classname = MAKE_STRING( "wh" );
	pev->solid = SOLID_BBOX;
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = 7;

	UTIL_SetSize(pev, g_vecZero, g_vecZero );
}

CWhlProjectile *CWhlProjectile::ShootWhlProjectile( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, BOOL Camera )
{
	CWhlProjectile *pWhlProjectile = GetClassPtr( (CWhlProjectile *)NULL );
	pWhlProjectile->Spawn();

	UTIL_SetOrigin( pWhlProjectile->pev, vecStart );
	pWhlProjectile->pev->velocity = vecVelocity;
	pWhlProjectile->pev->angles = UTIL_VecToAngles (pWhlProjectile->pev->velocity);
	pWhlProjectile->pev->owner = ENT(pevOwner);
	pWhlProjectile->SetTouch( ExplodeTouch );
	pWhlProjectile->pev->dmg = dmg_whl.value * (mp_wpn_power.value/100);
	pWhlProjectile->SetThink ( Ignite );
	pWhlProjectile->pev->nextthink = 0.1;
	FX_Trail(pWhlProjectile->pev->origin, pWhlProjectile->entindex(), PROJ_WARHEAD );

	if(Camera)
	{
		SET_VIEW( ENT(pevOwner), pWhlProjectile->edict() );
		pWhlProjectile->pev->angles.x = -pWhlProjectile->pev->angles.x;
		pWhlProjectile->pev->avelocity.z = -100;
	}
	return pWhlProjectile;
}

void CWhlProjectile :: Ignite( void  )
{
	EMIT_SOUND( ENT(pev), CHAN_VOICE, "weapons/whl_rocket_fly.wav", 1, ATTN_LOW_HIGH);
}