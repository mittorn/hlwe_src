#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "game.h"
#include "decals.h"
#include "projectiles.h"

LINK_ENTITY_TO_CLASS( u2momma, CU2Momma );

void CU2Momma::Killed (entvars_t *pevAttacker, int iGib)
{
	FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
	UTIL_Remove( this );
}

void CU2Momma:: Spawn( void )
{
	pev->movetype = MOVETYPE_FLY;
	pev->classname = MAKE_STRING( "u2momma" );
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = 12;
	UTIL_SetSize(pev, g_vecZero, g_vecZero );
}

void CU2Momma::ShootShards( void )
{
	entvars_t *pevOwner = VARS( pev->owner );

	float angle = -M_PI/2;
	Vector m_angle = pev->angles;
	m_angle.x = -m_angle.x;

	for (int k = 0; k < 4; k++)
	{
		UTIL_MakeVectors ( m_angle );
		CU2Baby::ShootU2Baby( pevOwner, pev->origin + gpGlobals->v_up*(6*sin(angle)) + gpGlobals->v_right*(6*cos(angle)), gpGlobals->v_forward*2500 + gpGlobals->v_up*(80*sin(angle))+gpGlobals->v_right*(80*cos(angle)));
		angle += 2*M_PI/4;
	}
	FX_Trail( pev->origin, entindex(), PROJ_U2_DETONATE_SHARDS);
	UTIL_Remove( this );
}

void CU2Momma::ExplodeTouch( CBaseEntity *pOther )
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

	entvars_t *pevOwner = VARS( pev->owner );
	RadiusDamage ( pev, pevOwner, pev->dmg, CLASS_NONE, DMG_BLAST );
	FX_Trail( tr.vecEndPos + (tr.vecPlaneNormal * 15), entindex(), (UTIL_PointContents(pev->origin) == CONTENT_WATER)?PROJ_U2_DETONATE_WATER:PROJ_U2_DETONATE);

	int tex = (int)TEXTURETYPE_Trace(&tr, vecSpot, vecEnd);
	CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
	FX_ImpRocket( tr.vecEndPos, tr.vecPlaneNormal, pEntity->IsBSPModel()?1:0, BULLET_NORMEXP, (float)tex );

	if (pOther->pev->takedamage)
	{
		ClearMultiDamage( );
		pOther->TraceAttack(pevOwner, pev->dmg/4, gpGlobals->v_forward, &tr, DMG_BULLETMAGNUM ); 
		ApplyMultiDamage( pev, pevOwner);
	}
	UTIL_Remove( this );
}

void CU2Momma :: TumbleThink( void )
{
	pev->nextthink = gpGlobals->time + 0.1;

	if (pev->dmgtime <= gpGlobals->time)
		SetThink( ShootShards );
}

CU2Momma * CU2Momma:: ShootU2Momma( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time )
{
	CU2Momma *pU2Momma = GetClassPtr( (CU2Momma *)NULL );
	pU2Momma->Spawn();
	UTIL_SetOrigin( pU2Momma->pev, vecStart );
	pU2Momma->pev->velocity = vecVelocity + gpGlobals->v_right * RANDOM_FLOAT(-20,20) + gpGlobals->v_up * RANDOM_FLOAT(-20,20);
	pU2Momma->pev->angles = UTIL_VecToAngles(pU2Momma->pev->velocity);
	pU2Momma->pev->owner = ENT(pevOwner); 
	pU2Momma->SetTouch( ExplodeTouch ); 
	pU2Momma->pev->dmgtime = gpGlobals->time + time;
	pU2Momma->SetThink( TumbleThink );
	pU2Momma->pev->nextthink = gpGlobals->time + 0.1;
	pU2Momma->pev->dmg = dmg_u2main.value * (mp_wpn_power.value/100);
	FX_Trail(pU2Momma->pev->origin, pU2Momma->entindex(), PROJ_U2 );
	return pU2Momma;
}