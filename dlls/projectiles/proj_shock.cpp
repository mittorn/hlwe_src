#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "decals.h"
#include "gamerules.h"
#include "game.h"
#include "projectiles.h"

LINK_ENTITY_TO_CLASS( shock, CShock );

void CShock::Killed (entvars_t *pevAttacker, int iGib)
{
	FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
	UTIL_Remove( this );
}

void CShock::ExplodeTouch( CBaseEntity *pOther )
{
	STOP_SOUND( edict(), CHAN_VOICE, "weapons/smartgun_lgtngball_fly.wav" );

	if ( UTIL_PointContents(pev->origin) == CONTENT_SKY )
	{
		STOP_SOUND( edict(), CHAN_VOICE, "weapons/smartgun_lgtngball_fly.wav" );
		FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
		UTIL_Remove( this );
		return;
	}
	TraceResult tr;
	Vector vecSpot = pev->origin - pev->velocity.Normalize() * 32;
	UTIL_TraceLine( vecSpot, vecSpot + pev->velocity.Normalize() * 64, ignore_monsters, ENT(pev), &tr );
	UTIL_DecalTrace(&tr, DECAL_OFSMSCORCH1 + RANDOM_FLOAT(0,2));

	entvars_t *pevOwner = VARS( pev->owner );

	if ( UTIL_PointContents(pev->origin) == CONTENT_WATER )
	{
		FX_Trail( pev->origin, entindex(), PROJ_SHOCK_DETONATE_WATER);
		::WaterRadiusDamage( pev->origin, pev, pevOwner, (dmg_smartgun.value*3) * (mp_wpn_power.value/100), (dmg_smartgun.value*6) * (mp_wpn_power.value/100), CLASS_NONE, DMG_SHOCK | DMG_NEVERGIB);
		UTIL_Remove( this );
	return;
	}

	::FullRadiusDamage( tr.vecEndPos, pev, pevOwner, pev->dmg, pev->dmg, CLASS_NONE, DMG_SHOCK | DMG_NEVERGIB);

	FX_Trail( tr.vecEndPos + (tr.vecPlaneNormal * 10), entindex(), (pev->frags==1)?PROJ_SHOCKPOWER_DETONATE:PROJ_SHOCK_DETONATE);
	UTIL_Remove( this );
}

void CShock:: Spawn( void )
{
	pev->movetype = MOVETYPE_FLY;
	pev->classname = MAKE_STRING( "shock" );
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "sprites/anim_spr7.spr"); 
	pev->rendermode = kRenderTransAdd;
	pev->renderamt = 255;
	UTIL_SetSize(pev, g_vecZero, g_vecZero );
	pev->frame = 0;
	pev->scale = 0.3;
	m_maxFrame = (float)MODEL_FRAMES(pev->modelindex)-1;
}

CShock *CShock::ShootShock( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, BOOL LightningBall )
{
	CShock *pShock = GetClassPtr( (CShock *)NULL );
	pShock->Spawn();

	UTIL_SetOrigin( pShock->pev, vecStart );
	pShock->pev->velocity = vecVelocity;
	pShock->pev->angles = UTIL_VecToAngles (pShock->pev->velocity);
	pShock->pev->owner = ENT(pevOwner);
	pShock->SetTouch( ExplodeTouch );
	FX_Trail(pShock->pev->origin, pShock->entindex(), PROJ_SHOCK);

	if (LightningBall)
	{
		pShock->pev->dmg = dmg_lgtng_ball.value * (mp_wpn_power.value/100);
		pShock->SetThink ( Lightning );
		pShock->pev->frags = 1;
	}
	else
	{
		pShock->pev->dmg = dmg_smartgun.value * (mp_wpn_power.value/100);
		pShock->SetThink ( Follow );
		pShock->pev->frags = 0;

	}
	pShock->pev->nextthink = 0.1;
	return pShock;
}

void CShock :: Follow( void )
{
	CBaseEntity *pOther = NULL;
	CBaseEntity *pPlayer = CBaseEntity::Instance(pev->owner);
	Vector vecTarget, vecDir;
	float flDist, flMax, flDot;
	TraceResult tr;

	UTIL_MakeAimVectors( pev->angles );
	vecTarget = gpGlobals->v_forward;
	flMax = 4000;

	if ( pev->frame++ )
	{
		if ( pev->frame > m_maxFrame )
		pev->frame = 0;
	}

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
	pev->velocity = gpGlobals->v_forward * 4000;
	pev->nextthink = gpGlobals->time + 0.03;
}

void CShock :: Lightning( void )
{
	entvars_t *pevOwner = VARS(pev->owner);
	CBaseEntity *pOther = NULL;
	CBaseEntity *pPlayer = CBaseEntity::Instance(pev->owner);

	if ( UTIL_PointContents(pev->origin) == CONTENT_WATER )
	{
		STOP_SOUND( edict(), CHAN_VOICE, "weapons/smartgun_lgtngball_fly.wav" );
		FX_Trail( pev->origin, entindex(), PROJ_SHOCK_DETONATE_WATER);
		::WaterRadiusDamage( pev->origin, pev, pevOwner, (dmg_lgtng_ball.value*3) * (mp_wpn_power.value/100), (dmg_lgtng_ball.value*6) * (mp_wpn_power.value/100), CLASS_NONE, DMG_SHOCK | DMG_NEVERGIB );
		UTIL_Remove( this );
	return;
	}

	if ( pev->frame++ )
	{
		if ( pev->frame > m_maxFrame )
		pev->frame = 0;
	}

	while ((pOther = UTIL_FindEntityInSphere( pOther, pev->origin, 800 )) != NULL)
	{
		if ( pOther->edict() != pev->owner && pOther->pev->takedamage == DAMAGE_AIM && pOther->pev->deadflag != DEAD_DEAD && pOther->pev->health >=1 && g_pGameRules->PlayerRelationship(pPlayer, pOther) != GR_TEAMMATE && pOther->Classify() != CLASS_MACHINE )
		{
			TraceResult tr;

			if ( gpGlobals->time >= m_flStrikeTime )
			{
				UTIL_TraceLine( pev->origin, pOther->pev->origin, dont_ignore_monsters, ENT(pev), &tr );

				if (tr.pHit != pOther->edict()) continue;

				CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);

				ClearMultiDamage( );
				pEntity->TraceAttack( pevOwner, dmg_smartgun.value * (mp_wpn_power.value/100), pev->velocity, &tr, DMG_SHOCK | DMG_NEVERGIB);
				ApplyMultiDamage( pev, pevOwner );
				FX_FireBeam(pev->origin, pOther->pev->origin, g_vecZero, BEAM_LGTNGBALL );

				UTIL_EmitAmbientSound( ENT(pev), tr.vecEndPos, "weapons/smartgun_lgtngball_strike.wav", 1, ATTN_LOW_HIGH, 0, RANDOM_LONG( 140, 160 ) );
				m_flStrikeTime = gpGlobals->time + 0.4;
			}
		}
	}

	if ( gpGlobals->time >= m_flBuzzTime )
	{
		EMIT_SOUND( ENT(pev), CHAN_VOICE, "weapons/smartgun_lgtngball_fly.wav", 0.9, ATTN_LOW_HIGH );
		m_flBuzzTime = gpGlobals->time + 2.7;
	}
	pev->nextthink = gpGlobals->time + 0.1;
}