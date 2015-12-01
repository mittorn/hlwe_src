#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "game.h"
#include "projectiles.h"
#include "gamerules.h"

LINK_ENTITY_TO_CLASS( hellfire, Chellfire );

void Chellfire::Killed (entvars_t *pevAttacker, int iGib)
{
	FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
	UTIL_Remove( this );
}

void Chellfire::ExplodeTouch( CBaseEntity *pOther )
{
	STOP_SOUND( edict(), CHAN_VOICE, "weapons/incendiaryrocket.wav" );

	if ( UTIL_PointContents(pev->origin) == CONTENT_SKY || UTIL_PointContents(pev->origin) == CONTENT_WATER)
	{
		FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
		UTIL_Remove( this );
		return;
	}

	TraceResult tr;
	Vector vecSpot = pev->origin - pev->velocity.Normalize() * 32;
	Vector vecEnd = pev->origin + pev->velocity.Normalize() * 64;
	UTIL_TraceLine( vecSpot, vecEnd, ignore_monsters, ENT(pev), &tr );

	int iContents = UTIL_PointContents ( pev->origin );
	entvars_t *pevOwner = VARS( pev->owner );
	RadiusDamage ( pev, pevOwner, pev->dmg, CLASS_NONE, DMG_BLAST | DMG_IGNITE | DMG_NEVERGIB);
	FX_Trail( tr.vecEndPos + (tr.vecPlaneNormal * 15), entindex(), (UTIL_PointContents(pev->origin) == CONTENT_WATER)?PROJ_INCENDIARY_DETONATE_WATER:PROJ_INCENDIARY_DETONATE);

	int tex = (int)TEXTURETYPE_Trace(&tr, vecSpot, vecEnd);
	CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
	FX_ImpRocket( tr.vecEndPos, tr.vecPlaneNormal, pEntity->IsBSPModel()?1:0, BULLET_NORMEXP, (float)tex );

	if (pOther->pev->takedamage)
	{
		ClearMultiDamage( );
		pOther->TraceAttack(pevOwner, pev->dmg/4, gpGlobals->v_forward, &tr, DMG_BULLETMAGNUM ); 
		ApplyMultiDamage( pev, pevOwner);
	}
		pev->velocity = g_vecZero;
		SetTouch(NULL);
		FX_Trail( pev->origin, entindex(), (UTIL_PointContents(pev->origin) == CONTENT_WATER)?PROJ_INCENDIARY_DETONATE_WATER:PROJ_INCENDIARY_DETONATE);
		FireStayTime = 40;
		SetThink( Burn );
		pev->nextthink = gpGlobals->time + 0.2;
}

void Chellfire::Burn( void )
{
	if(FireStayTime <=0 )
	{
		STOP_SOUND( ENT(pev), CHAN_STATIC, "player/pl_burn.wav" );
		UTIL_Remove( this );
	}
	else
		FireStayTime--;

	if ( gpGlobals->time >= m_flSoundTime )
	{
		EMIT_SOUND_DYN(ENT(pev), CHAN_STATIC, "player/pl_burn.wav", 0.8, ATTN_NORM, 0, 100 );
		m_flSoundTime = gpGlobals->time + 1.8;
	}
	entvars_t *pevOwner = VARS( pev->owner );
	::RadiusDamage( pev->origin, pev, pevOwner, FireStayTime*2, pev->dmg*2, CLASS_NONE, DMG_BURN | DMG_NEVERGIB);
	pev->nextthink = gpGlobals->time + 0.2;
}

void Chellfire:: Spawn( void )
{
	pev->movetype = MOVETYPE_FLY;
	pev->classname = MAKE_STRING( "hellfire" );
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = 1;
	UTIL_SetSize(pev, g_vecZero, g_vecZero );
}

Chellfire *Chellfire::ShootHellfire( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, BOOL Homing )
{
	Chellfire *phellfire = GetClassPtr( (Chellfire *)NULL );
	phellfire->Spawn();

	UTIL_SetOrigin( phellfire->pev, vecStart );
	phellfire->pev->velocity = vecVelocity;
	phellfire->pev->angles = UTIL_VecToAngles (phellfire->pev->velocity);
	phellfire->pev->owner = ENT(pevOwner);
	FX_Trail(phellfire->pev->origin, phellfire->entindex(), Homing?PROJ_INCENDIARY2:PROJ_INCENDIARY );

	if (Homing)
		phellfire->SetThink ( IgniteFollow );
	else
		phellfire->SetThink ( Ignite );

	phellfire->pev->nextthink = 0.1;
	phellfire->SetTouch( ExplodeTouch );
	phellfire->pev->dmg = dmg_incendiary.value * (mp_wpn_power.value/100);
	return phellfire;
}

void Chellfire :: Ignite( void  )
{
	EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/incendiaryrocket.wav", 1, ATTN_LOW_HIGH);
}

void Chellfire :: IgniteFollow( void  )
{
	EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/incendiaryrocket.wav", 1, ATTN_LOW_HIGH);
	SetThink ( Follow );
	pev->nextthink = gpGlobals->time + 0.1;
}

void Chellfire :: Follow( void )
{
	CBaseEntity *pOther = NULL;
	CBaseEntity *pPlayer = CBaseEntity::Instance(pev->owner);
	Vector vecTarget, vecDir;
	float flDist, flMax, flDot;
	TraceResult tr;

	UTIL_MakeAimVectors( pev->angles );
	vecTarget = gpGlobals->v_forward;
	flMax = 4000;

	while ((pOther = UTIL_FindEntityInSphere( pOther, pev->origin, 1000 )) != NULL)
	{
		if ( pOther->pev->takedamage == DAMAGE_AIM && pOther->pev->deadflag != DEAD_DEAD && pOther->pev->health > 1 && g_pGameRules->PlayerRelationship(pPlayer, pOther) != GR_TEAMMATE && pOther->Classify() != CLASS_MACHINE && pOther != pPlayer)
		{
			UTIL_TraceLine ( pev->origin, pOther->pev->origin, dont_ignore_monsters, ENT(pev), &tr );
			if (tr.flFraction >= 0.90)
			{
				vecDir = pOther->pev->origin - pev->origin;
				flDist = vecDir.Length( );
				vecDir = vecDir.Normalize( );
				flDot = DotProduct( gpGlobals->v_forward, vecDir );
				if ((flDot > 0) && (flDist * (1 - flDot) < flMax))
				{
					flMax = flDist * (1 - flDot);
					vecTarget = vecDir;
				}
			}
		}
	}
	pev->angles = UTIL_VecToAngles( vecTarget );
	pev->velocity = gpGlobals->v_forward * 1800;
	pev->nextthink = gpGlobals->time + 0.1;
}