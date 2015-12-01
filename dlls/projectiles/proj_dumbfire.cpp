#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "game.h"
#include "projectiles.h"
#include "gamerules.h"

LINK_ENTITY_TO_CLASS( dumbfire, CDumbfire );

void CDumbfire::Killed (entvars_t *pevAttacker, int iGib)
{
	FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
	UTIL_Remove( this );
}

void CDumbfire::ExplodeTouch( CBaseEntity *pOther )
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
	FX_Trail( tr.vecEndPos + (tr.vecPlaneNormal * 15), entindex(), (UTIL_PointContents(pev->origin) == CONTENT_WATER)?PROJ_DUMBFIRE_DETONATE_WATER:PROJ_DUMBFIRE_DETONATE );

	int tex = (int)TEXTURETYPE_Trace(&tr, vecSpot, vecEnd);
	CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
	FX_ImpRocket( tr.vecEndPos, tr.vecPlaneNormal, pEntity->IsBSPModel()?1:0, BULLET_BOLT, (float)tex );

	if (pOther->pev->takedamage)
	{
		ClearMultiDamage( );
		pOther->TraceAttack(pevOwner, pev->dmg/3, gpGlobals->v_forward, &tr, DMG_BULLETMAGNUM ); 
		ApplyMultiDamage( pev, pevOwner);
	}
	UTIL_Remove( this );
}

void CDumbfire:: Spawn( void )
{
	pev->movetype = MOVETYPE_FLY;
	pev->classname = MAKE_STRING( "dumbfire" );
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = 2;
	UTIL_SetSize(pev, g_vecZero, g_vecZero );
}

CDumbfire *CDumbfire::ShootDumbfire( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity )
{
	CDumbfire *pDumbfire = GetClassPtr( (CDumbfire *)NULL );
	pDumbfire->Spawn();

	UTIL_SetOrigin( pDumbfire->pev, vecStart );
	pDumbfire->pev->velocity = vecVelocity;
	pDumbfire->pev->angles = UTIL_VecToAngles (pDumbfire->pev->velocity);
	pDumbfire->pev->owner = ENT(pevOwner);
	pDumbfire->SetThink ( Follow );
	pDumbfire->pev->nextthink = 0.1;
	pDumbfire->SetTouch( ExplodeTouch );
	pDumbfire->pev->dmg = dmg_dumbfire_missile.value * (mp_wpn_power.value/100);
	pDumbfire->pev->avelocity.z = -300;

	FX_Trail(pDumbfire->pev->origin, pDumbfire->entindex(), PROJ_DUMBFIRE );
	return pDumbfire;
}

void CDumbfire :: Follow( void )
{
	CBaseEntity *pOther = NULL;
	Vector vecTarget, vecDir;
	float flDist, flMax, flDot;
	TraceResult tr;

	UTIL_MakeAimVectors( pev->angles );
	CBaseEntity *pPlayer = CBaseEntity::Instance(pev->owner);

	vecTarget = gpGlobals->v_forward;
	flMax = 8192;

	pev->velocity = gpGlobals->v_forward * 2000;

	while ((pOther = UTIL_FindEntityInSphere( pOther, pev->origin, 1000 )) != NULL)
	{
		if ( pOther->pev->takedamage == DAMAGE_AIM && pOther->pev->deadflag != DEAD_DEAD && pOther->pev->health >=1 && g_pGameRules->PlayerRelationship(pPlayer, pOther) != GR_TEAMMATE && pOther->Classify() != CLASS_MACHINE && pOther != pPlayer)
		{
			Vector origin = (pOther->IsPlayer()?(pOther->pev->origin - Vector(0, 0, 35)):(pOther->pev->origin + Vector(0, 0, 5)));

			UTIL_TraceLine ( pev->origin, origin, ignore_monsters, ENT(pev), &tr );

			if (tr.flFraction == 1)
			{
				vecDir = pOther->pev->origin - Vector(0, 0, 35) - pev->origin;
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

		Vector destination = UTIL_VecToAngles( vecTarget );

		Vector increment = Vector ( destination.x - pev->angles.x, destination.y - pev->angles.y, destination.z - pev->angles.z );

		for ( int i = 0; i < 3; i++ )
		{
			if ( increment[i] > 180.0 )
			{
				increment[i] -= 360.0;
			}
			else if ( increment[i] < -180.0 )
			{
				increment[i] += 360.0;
			}
		}

		if (increment.x > ROCKET_YAW_SPEED) increment.x = ROCKET_YAW_SPEED;
		else if (increment.x < -ROCKET_YAW_SPEED) increment.x = -ROCKET_YAW_SPEED;

		if (increment.y > ROCKET_YAW_SPEED) increment.y = ROCKET_YAW_SPEED;
		else if (increment.y < -ROCKET_YAW_SPEED) increment.y = -ROCKET_YAW_SPEED;

		if (increment.z > ROCKET_YAW_SPEED) increment.z = ROCKET_YAW_SPEED;
		else if (increment.z < -ROCKET_YAW_SPEED) increment.z = -ROCKET_YAW_SPEED;

		pev->angles = Vector ( pev->angles.x + increment.x, pev->angles.y + increment.y, pev->angles.z + increment.z);
	}
	pev->nextthink = gpGlobals->time + 0.13;
}