#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "game.h"
#include "projectiles.h"
#include "gamerules.h"

LINK_ENTITY_TO_CLASS( black_hole, CBlackHole );

void CBlackHole::RadiusDamage( )
{
	CBaseEntity *pEntity = NULL;
	TraceResult	tr;
	float		flAdjustedDamage;
	Vector		vecSpot;

	entvars_t *pevOwner = VARS( pev->owner );
	Vector vecSrc = pev->origin + Vector(0,0,1);

	if ( !pevOwner )
		pevOwner = pev;

	CBaseEntity *pPlayer = CBaseEntity::Instance (pevOwner);

	// iterate on all entities in the vicinity.
	while ((pEntity = UTIL_FindEntityInSphere( pEntity, vecSrc, (dmg_chrono_radius.value * (mp_wpn_power.value/100)) )) != NULL)
	{
		if ( pEntity->pev->movetype != MOVETYPE_PUSH && pEntity->pev->movetype != MOVETYPE_NONE && pEntity->pev->movetype != MOVETYPE_FOLLOW && pEntity->pev->movetype != MOVETYPE_NOCLIP )
		{
			if (pEntity == this) continue;

			if (CVAR_GET_FLOAT("mp_noselfdamage")==1 && pEntity->pev == pevOwner )
			continue;

			if (CVAR_GET_FLOAT("mp_noteamdamage")==1 && g_pGameRules->PlayerRelationship(pPlayer, pEntity) == GR_TEAMMATE && pEntity->pev != pevOwner)
			continue;

			vecSpot = pEntity->BodyTarget( vecSrc );
			
			UTIL_TraceLine ( vecSrc, vecSpot, ignore_monsters, ENT(pev), &tr );

			if ( tr.flFraction == 1.0 || tr.pHit == pEntity->edict() )
			{
				flAdjustedDamage = ( vecSrc - tr.vecEndPos ).Length();
			
				if ( flAdjustedDamage <	1 )
					flAdjustedDamage = 1;
			
				Vector m_vel = ( vecSrc - tr.vecEndPos ).Normalize() * ((500 * (dmg_chrono_radius.value * (mp_wpn_power.value/100)) / flAdjustedDamage) + 150);

				if (m_vel.Length() > 1000) m_vel = m_vel.Normalize() * 1000;

				pEntity->pev->velocity = m_vel;

				if (flAdjustedDamage < 200)
				{
					if (!(pEntity->Classify() == CLASS_MACHINE || pEntity->IsBot() || pEntity->IsPlayer() || FClassnameIs(pEntity->pev, "laser_dot") || FClassnameIs(pEntity->pev, "laser_spot")))
					{
						pEntity->Killed ( pevOwner, 0 );
						return;
					}
				}
				
			}
		}
	}
}

void CBlackHole :: Spawn( void )
{
	SET_MODEL(ENT(pev), "sprites/anim_spr1.spr");	
	pev->rendermode = kRenderTransAdd;
	pev->renderamt = 1;
	pev->frags = 60;
}

void CBlackHole :: DesintegrateThink( void )
{
	entvars_t *pevOwner = VARS( pev->owner );

	if (UTIL_PointContents ( pev->origin ) == CONTENTS_WATER)
	{
		::RadiusDamage( pev->origin, pev, pevOwner, (dmg_chrono_radius.value/2) * (mp_wpn_power.value/100), dmg_chrono_radius.value * (mp_wpn_power.value/100), CLASS_NONE, DMG_SONIC);
		FX_Trail( pev->origin, entindex(), PROJ_BLACKHOLE_DETONATE );
		UTIL_Remove( this );
		return;
	}

	::FullRadiusDamage( pev->origin, pev, pevOwner, 999, 200, CLASS_NONE, DMG_ANNIHILATION);
	RadiusDamage();

 	pev->frags--;
	if (pev->frags <= 0)
	{
		::RadiusDamage( pev->origin, pev, pevOwner, (dmg_chrono_radius.value/2) * (mp_wpn_power.value/100), dmg_chrono_radius.value* (mp_wpn_power.value/100), CLASS_NONE, DMG_SONIC);
		FX_Trail( pev->origin, entindex(), PROJ_BLACKHOLE_DETONATE );
		UTIL_Remove( this );
		return;
	}
	pev->nextthink = gpGlobals->time + 0.1;
}

CBlackHole *CBlackHole::ShootBlackHole( entvars_t *pevOwner, Vector vecStart)
{
	CBlackHole *pBlackHole = GetClassPtr( (CBlackHole *)NULL );
	pBlackHole->Spawn();

	UTIL_SetOrigin( pBlackHole->pev, vecStart );
	pBlackHole->pev->owner = ENT(pevOwner);
	pBlackHole->SetThink ( DesintegrateThink );
	pBlackHole->pev->nextthink = gpGlobals->time + 0.1;
	FX_Trail(pBlackHole->pev->origin, pBlackHole->entindex(), PROJ_BLACKHOLE);
	return pBlackHole;
}