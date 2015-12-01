#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "game.h"
#include "decals.h"
#include "projectiles.h"

LINK_ENTITY_TO_CLASS( clmomma, CClMomma );

void CClMomma::Killed (entvars_t *pevAttacker, int iGib)
{
	FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
	UTIL_Remove( this );
}

void CClMomma::Detonate( void )
{
	TraceResult tr;
	Vector vecSpot = pev->origin + Vector (0 ,0 ,8);
	Vector vecEnd =  pev->origin + Vector (0 ,0 ,-40);
	UTIL_TraceLine(vecSpot, vecEnd, ignore_monsters, ENT(pev), &tr);

	entvars_t *pevOwner = VARS( pev->owner );
	::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg, pev->dmg*3, CLASS_NONE, DMG_FREEZE | DMG_BLAST );
	UTIL_DecalTrace(&tr, DECAL_FROST_SCORCH1 + RANDOM_LONG(0,1));

	for ( int i = 0; i < 8; i++ )
		CClBaby::ShootClBaby( pevOwner, pev->origin+Vector(0,0,20));
	FX_Trail( tr.vecEndPos + (tr.vecPlaneNormal * 15), entindex(), (UTIL_PointContents(pev->origin) == CONTENT_WATER)?PROJ_CLUSTERBOMB_DETONATE_WATER:PROJ_CLUSTERBOMB_DETONATE);
	UTIL_Remove( this );
}

void CClMomma::BounceTouch( CBaseEntity *pOther )
{
	if (pOther->pev->takedamage == DAMAGE_AIM)
	{
		TraceResult tr;
		entvars_t *pevOwner = VARS( pev->owner );
		ClearMultiDamage( );
		pOther->TraceAttack(pevOwner, pev->dmg/3.5, gpGlobals->v_forward, &tr, DMG_BULLETMAGNUM ); 
		ApplyMultiDamage( pev, pevOwner);
		SetThink( Detonate );
		return;
	}

	if (pev->flags & FL_ONGROUND)
		pev->velocity = pev->velocity * 0.8;
	else
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/cluster_bounce.wav", 0.6, ATTN_NORM);
}

void CClMomma :: TumbleThink( void )
{
	if (!IsInWorld())
	{
		FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
		UTIL_Remove( this );
		return;
	}

	pev->nextthink = gpGlobals->time + 0.1;

	if (pev->dmgtime <= gpGlobals->time)
	{
		SetThink( Detonate );
	}
}

void CClMomma:: Spawn( void )
{
	pev->movetype = MOVETYPE_BOUNCE;
	pev->classname = MAKE_STRING( "clmomma" );
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = 12;
	UTIL_SetSize(pev, g_vecZero, g_vecZero );
}

CClMomma * CClMomma:: ShootClusterMomma( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time )
{
	CClMomma *pClMomma = GetClassPtr( (CClMomma *)NULL );
	pClMomma->Spawn();
	UTIL_SetOrigin( pClMomma->pev, vecStart );
	pClMomma->pev->velocity = vecVelocity;
	pClMomma->pev->angles = UTIL_VecToAngles(pClMomma->pev->velocity);
	pClMomma->pev->owner = ENT(pevOwner); 
	pClMomma->SetTouch( BounceTouch ); 
	pClMomma->pev->dmgtime = gpGlobals->time + time;
	pClMomma->SetThink( TumbleThink );
	pClMomma->pev->nextthink = gpGlobals->time + 0.1;
	pClMomma->pev->gravity = 2;
	pClMomma->pev->friction = 0.7;
	pClMomma->pev->dmg = dmg_cluster_momma.value * (mp_wpn_power.value/100); 
	FX_Trail(pClMomma->pev->origin, pClMomma->entindex(), PROJ_CLUSTERBOMB );
	return pClMomma;
}