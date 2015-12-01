#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "soundent.h"
#include "decals.h"
#include "animation.h"
#include "weapons.h"
#include "func_break.h"
#include "game.h"

#include "pm_materials.h"

extern DLL_GLOBAL Vector		g_vecAttackDir;
extern Vector VecBModelOrigin( entvars_t* pevBModel );
extern entvars_t *g_pevLastInflictor;

//=========================================================
// GetDeathActivity - determines the best type of death
// anim to play.
//=========================================================
Activity CBaseMonster :: GetDeathActivity ( void )
{
	Activity	deathActivity;
	BOOL		fTriedDirection;
	float		flDot;
	TraceResult	tr;
	Vector		vecSrc;

	if ( pev->deadflag != DEAD_NO )
	{
		// don't run this while dying.
		return m_IdealActivity;
	}

	vecSrc = Center();

	fTriedDirection = FALSE;
	deathActivity = ACT_DIESIMPLE;// in case we can't find any special deaths to do.

	UTIL_MakeVectors ( pev->angles );
	flDot = DotProduct ( gpGlobals->v_forward, g_vecAttackDir * -1 );

	switch ( m_LastHitGroup )
	{
		// try to pick a region-specific death.
	case HITGROUP_HEAD:
		deathActivity = ACT_DIE_HEADSHOT;
		break;

	case HITGROUP_STOMACH:
		deathActivity = ACT_DIE_GUTSHOT;
		break;

	case HITGROUP_GENERIC:
		// try to pick a death based on attack direction
		fTriedDirection = TRUE;

		if ( flDot > 0.3 )
		{
			deathActivity = ACT_DIEFORWARD;
		}
		else if ( flDot <= -0.3 )
		{
			deathActivity = ACT_DIEBACKWARD;
		}
		break;

	default:
		// try to pick a death based on attack direction
		fTriedDirection = TRUE;

		if ( flDot > 0.3 )
		{
			deathActivity = ACT_DIEFORWARD;
		}
		else if ( flDot <= -0.3 )
		{
			deathActivity = ACT_DIEBACKWARD;
		}
		break;
	}


	// can we perform the prescribed death?
	if ( LookupActivity ( deathActivity ) == ACTIVITY_NOT_AVAILABLE )
	{
		// no! did we fail to perform a directional death? 
		if ( fTriedDirection )
		{
			// if yes, we're out of options. Go simple.
			deathActivity = ACT_DIESIMPLE;
		}
		else
		{
			// cannot perform the ideal region-specific death, so try a direction.
			if ( flDot > 0.3 )
			{
				deathActivity = ACT_DIEFORWARD;
			}
			else if ( flDot <= -0.3 )
			{
				deathActivity = ACT_DIEBACKWARD;
			}
		}
	}

	if ( LookupActivity ( deathActivity ) == ACTIVITY_NOT_AVAILABLE )
	{
		// if we're still invalid, simple is our only option.
		deathActivity = ACT_DIESIMPLE;
	}

	if ( deathActivity == ACT_DIEFORWARD )
	{
			// make sure there's room to fall forward
			UTIL_TraceHull ( vecSrc, vecSrc + gpGlobals->v_forward * 64, dont_ignore_monsters, head_hull, edict(), &tr );

			if ( tr.flFraction != 1.0 )
			{
				deathActivity = ACT_DIESIMPLE;
			}
	}

	if ( deathActivity == ACT_DIEBACKWARD )
	{
			// make sure there's room to fall backward
			UTIL_TraceHull ( vecSrc, vecSrc - gpGlobals->v_forward * 64, dont_ignore_monsters, head_hull, edict(), &tr );

			if ( tr.flFraction != 1.0 )
			{
				deathActivity = ACT_DIESIMPLE;
			}
	}

	return deathActivity;
}

//=========================================================
// GetSmallFlinchActivity - determines the best type of flinch
// anim to play.
//=========================================================
Activity CBaseMonster :: GetSmallFlinchActivity ( void )
{
	Activity	flinchActivity;
	BOOL		fTriedDirection;
	float		flDot;

	fTriedDirection = FALSE;
	UTIL_MakeVectors ( pev->angles );
	flDot = DotProduct ( gpGlobals->v_forward, g_vecAttackDir * -1 );
	
	switch ( m_LastHitGroup )
	{
		// pick a region-specific flinch
	case HITGROUP_HEAD:
		flinchActivity = ACT_FLINCH_HEAD;
		break;
	case HITGROUP_STOMACH:
		flinchActivity = ACT_FLINCH_STOMACH;
		break;
	case HITGROUP_LEFTARM:
		flinchActivity = ACT_FLINCH_LEFTARM;
		break;
	case HITGROUP_RIGHTARM:
		flinchActivity = ACT_FLINCH_RIGHTARM;
		break;
	case HITGROUP_LEFTLEG:
		flinchActivity = ACT_FLINCH_LEFTLEG;
		break;
	case HITGROUP_RIGHTLEG:
		flinchActivity = ACT_FLINCH_RIGHTLEG;
		break;
	case HITGROUP_GENERIC:
	default:
		// just get a generic flinch.
		flinchActivity = ACT_SMALL_FLINCH;
		break;
	}


	// do we have a sequence for the ideal activity?
	if ( LookupActivity ( flinchActivity ) == ACTIVITY_NOT_AVAILABLE )
	{
		flinchActivity = ACT_SMALL_FLINCH;
	}

	return flinchActivity;
}


void CBaseMonster::BecomeDead( void )
{
	pev->takedamage = DAMAGE_YES;
	
	// give the corpse half of the monster's original maximum health. 
	pev->health = pev->max_health / 2;
	pev->max_health = 5; // max_health now becomes a counter for how many blood decals the corpse can place.

	// make the corpse fly away from the attack vector
	pev->movetype = MOVETYPE_TOSS;
}

/*
============
Killed
============
*/
void CBaseMonster :: Killed( entvars_t *pevAttacker, int iGib )
{
	unsigned int	cCount = 0;
	BOOL		fDone = FALSE;

	if ( HasMemory( bits_MEMORY_KILLED ) )
		return;

	Remember( bits_MEMORY_KILLED );

	// clear the deceased's sound channels.(may have been firing or reloading when killed)
	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "common/null.wav", 1, ATTN_NORM);
	m_IdealMonsterState = MONSTERSTATE_DEAD;
	// Make sure this condition is fired too (TakeDamage breaks out before this happens on death)
	SetConditions( bits_COND_LIGHT_DAMAGE );

	// tell owner ( if any ) that we're dead.This is mostly for MonsterMaker functionality.
	CBaseEntity *pOwner = CBaseEntity::Instance(pev->owner);

	if (pOwner)
		pOwner->DeathNotice( pev );

	else if ( pev->flags & FL_MONSTER )
	{
		SetTouch( NULL );
		BecomeDead();
	}
	
	// don't let the status bar glitch for players.with <0 health.
	if (pev->health < -99)
	{
		pev->health = 0;
	}
	m_IdealMonsterState = MONSTERSTATE_DEAD;
}

//
// fade out - slowly fades a entity out, then removes it.
//
// DON'T USE ME FOR GIBS AND STUFF IN MULTIPLAYER! 
// SET A FUTURE THINK AND A RENDERMODE!!
void CBaseEntity :: SUB_StartFadeOut ( void )
{
	if (!strcmp( STRING(pev->classname), "monster_zombie" ))
	return;

	if (pev->rendermode == kRenderNormal)
	{
		pev->renderamt = 255;
		pev->rendermode = kRenderTransTexture;
	}

	pev->solid = SOLID_NOT;
	pev->avelocity = g_vecZero;

	pev->nextthink = gpGlobals->time + 0.1;
	SetThink ( SUB_FadeOut );
}

void CBaseEntity :: SUB_FadeOut ( void  )
{
	if ( pev->renderamt > 7 )
	{
		pev->renderamt -= 7;
		pev->nextthink = gpGlobals->time + 0.1;
	}
	else 
	{
		pev->renderamt = 0;
		pev->nextthink = gpGlobals->time + 0.2;
		SetThink ( SUB_Remove );
	}
}

// take health
int CBaseMonster :: TakeHealth (float flHealth, int bitsDamageType)
{
	if (!pev->takedamage)
		return 0;

	m_bitsDamageType &= ~(bitsDamageType & ~DMG_TIMEBASED);
	
	return CBaseEntity::TakeHealth(flHealth, bitsDamageType);
}

int CBaseMonster :: RealTakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType )
{
	float	flTake;
	Vector	vecDir;

	if (!pev->takedamage)
		return 0;

	if ( bitsDamageType & DMG_ANNIHILATION && !IsPlayer() && !IsBot() )
	{
		UTIL_Remove (this);//LLAPb: monster must just dissapear in black hole, don't allow it to respawn
		return 0;
	}

	if ( !IsAlive() )
	{
		return DeadTakeDamage( pevInflictor, pevAttacker, flDamage, bitsDamageType );
	}

	if ( pev->deadflag == DEAD_NO )
	{
		// no pain sound during death animation.
		PainSound();// "Ouch!"
	}

	//!!!LATER - make armor consideration here!
	flTake = flDamage;

	// set damage type sustained
	m_bitsDamageType |= bitsDamageType;

	// grab the vector of the incoming attack. ( pretend that the inflictor is a little lower than it really is, so the body will tend to fly upward a bit).
	vecDir = Vector( 0, 0, 0 );
	if (!FNullEnt( pevInflictor ))
	{
		CBaseEntity *pInflictor = CBaseEntity :: Instance( pevInflictor );
		if (pInflictor)
		{
			vecDir = ( pInflictor->Center() - Vector ( 0, 0, 10 ) - Center() ).Normalize();
			vecDir = g_vecAttackDir = vecDir.Normalize();
		}
	}

	// add to the damage total for clients, which will be sent as a single
	// message at the end of the frame
	// todo: remove after combining shotgun blasts?
	if ( IsPlayer() )
	{
		if ( pevInflictor )
			pev->dmg_inflictor = ENT(pevInflictor);

		pev->dmg_take += flTake;

		// check for godmode or invincibility
		if ( pev->flags & FL_GODMODE )
		{
			return 0;
		}
	}

	// if this is a player, move him around!
	if ( ( !FNullEnt( pevInflictor ) ) && (pev->movetype == MOVETYPE_WALK) && (!pevAttacker || pevAttacker->solid != SOLID_TRIGGER) )
	{
		pev->velocity = pev->velocity + vecDir * -DamageForce( flDamage );
	}

	// do the damage
	pev->health -= flTake;

	// HACKHACK Don't kill monsters in a script.  Let them break their scripts first
	if ( m_MonsterState == MONSTERSTATE_SCRIPT )
	{
		SetConditions( bits_COND_LIGHT_DAMAGE );
		return 0;
	}
	
	if ( pev->health <= 0 )
	{
		g_pevLastInflictor = pevInflictor;

		if ( bitsDamageType & DMG_ALWAYSGIB )
		{
			Killed( pevAttacker, GIB_ALWAYS );
		}
		else if ( bitsDamageType & DMG_NEVERGIB )
		{
			Killed( pevAttacker, GIB_NEVER );
		}
		else
		{
			Killed( pevAttacker, GIB_NORMAL );
		}

		g_pevLastInflictor = NULL;

		return 0;
	}

	// react to the damage (get mad)
	if ( (pev->flags & FL_MONSTER) && !FNullEnt(pevAttacker) )
	{
		if ( pevAttacker->flags & (FL_MONSTER | FL_CLIENT) )
		{// only if the attack was a monster or client!
			
			// enemy's last known position is somewhere down the vector that the attack came from.
			if (pevInflictor)
			{
				if (m_hEnemy == NULL || pevInflictor == m_hEnemy->pev || !HasConditions(bits_COND_SEE_ENEMY))
				{
					m_vecEnemyLKP = pevInflictor->origin;
				}
			}
			else
			{
				m_vecEnemyLKP = pev->origin + ( g_vecAttackDir * 64 ); 
			}

			MakeIdealYaw( m_vecEnemyLKP );

			// add pain to the conditions 
			// !!!HACKHACK - fudged for now. Do we want to have a virtual function to determine what is light and 
			// heavy damage per monster class?
			if ( flDamage > 0 )
			{
				SetConditions(bits_COND_LIGHT_DAMAGE);
			}

			if ( flDamage >= 40 )
			{
				SetConditions(bits_COND_HEAVY_DAMAGE);
			}
		}
	}
	return 1;
}

//=========================================================
// DeadTakeDamage - takedamage function called when a monster's
// corpse is damaged.
//=========================================================
int CBaseMonster :: DeadTakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType )
{
	Vector			vecDir;

	// grab the vector of the incoming attack. ( pretend that the inflictor is a little lower than it really is, so the body will tend to fly upward a bit).
	vecDir = Vector( 0, 0, 0 );
	if (!FNullEnt( pevInflictor ))
	{
		CBaseEntity *pInflictor = CBaseEntity :: Instance( pevInflictor );
		if (pInflictor)
		{
			vecDir = ( pInflictor->Center() - Vector ( 0, 0, 10 ) - Center() ).Normalize();
			vecDir = g_vecAttackDir = vecDir.Normalize();
		}
	}

	// kill the corpse if enough damage was done to destroy the corpse and the damage is of a type that is allowed to destroy the corpse.

	if ( bitsDamageType & DMG_GIB_CORPSE )
	{
		if ( pev->health <= flDamage )
		{
			pev->health = -80;
			Killed( pevAttacker, GIB_ALWAYS );
			return 0;
		}
		// Accumulate corpse gibbing damage, so you can gib with multiple hits
		pev->health -= flDamage * 0.1;
	}
	return 1;
}


float CBaseMonster :: DamageForce( float damage )
{ 
	float force = damage * ((32 * 32 * 72.0) / (pev->size.x * pev->size.y * pev->size.z)) * 5;
	return force;
}

//
// RadiusDamage - this entity is exploding, or otherwise needs to inflict damage upon entities within a certain range.
// 
// only damage ents that can clearly be seen by the explosion!
void RadiusDamage( Vector vecSrc, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, float flRadius, int iClassIgnore, int bitsDamageType )
{
	CBaseEntity *pEntity = NULL;
	TraceResult	tr;
	float		flAdjustedDamage, falloff;
	Vector		vecSpot;

	if ( flRadius )
		falloff = flDamage / flRadius;
	else
		falloff = 1.0;

	vecSrc.z += 1;// in case grenade is lying on the ground

	if ( !pevAttacker )
		pevAttacker = pevInflictor;

	// iterate on all entities in the vicinity.
	while ((pEntity = UTIL_FindEntityInSphere( pEntity, vecSrc, flRadius )) != NULL)
	{
		if ( pEntity->pev->takedamage != DAMAGE_NO || FClassnameIs(pEntity->pev, "func_pushable"))
		{
			// UNDONE: this should check a damage mask, not an ignore
			if ( iClassIgnore != CLASS_NONE && pEntity->Classify() == iClassIgnore )
			{// houndeyes don't hurt other houndeyes with their attack
				continue;
			}

			vecSpot = pEntity->BodyTarget( vecSrc );
			
			UTIL_TraceLine ( vecSrc, vecSpot, ignore_monsters, ENT(pevInflictor), &tr );

			if (tr.flFraction == 1.0 || tr.pHit == pEntity->edict() || (bitsDamageType & DMG_SONIC))
			{// the explosion can 'see' this entity, so hurt them!
				if (tr.fStartSolid)
				{
					// if we're stuck inside them, fixup the position and distance
					tr.vecEndPos = vecSrc;
					tr.flFraction = 0.0;
				}
				
				// decrease damage for an ent that's farther from the bomb.
				flAdjustedDamage = ( vecSrc - tr.vecEndPos ).Length() * falloff;
				flAdjustedDamage = flDamage - flAdjustedDamage;
			
				if ( flAdjustedDamage < 0 )
					flAdjustedDamage = 0;
			
				ClearMultiDamage( );
				pEntity->TraceAttack( pevInflictor, flAdjustedDamage, (tr.vecEndPos - vecSrc).Normalize( ), &tr, bitsDamageType );
				ApplyMultiDamage( pevInflictor, pevAttacker );
			}
		}
	}
}

void CBaseMonster :: RadiusDamage(entvars_t* pevInflictor, entvars_t*	pevAttacker, float flDamage, int iClassIgnore, int bitsDamageType )
{
	::RadiusDamage( pev->origin, pevInflictor, pevAttacker, flDamage, flDamage * 2.5, iClassIgnore, bitsDamageType );
}


void CBaseMonster :: RadiusDamage( Vector vecSrc, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int iClassIgnore, int bitsDamageType )
{
	::RadiusDamage( vecSrc, pevInflictor, pevAttacker, flDamage, flDamage * 2.5, iClassIgnore, bitsDamageType );
}


//Only in water RadiusDamage!! By Ghoul, to fix 'electrical' water-explosions
void WaterRadiusDamage( Vector vecSrc, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, float flRadius, int iClassIgnore, int bitsDamageType )
{
	CBaseEntity *pEntity = NULL;
	TraceResult	tr;
	float		flAdjustedDamage, falloff;
	Vector		vecSpot;

	if ( flRadius )
		falloff = flDamage / flRadius;
	else
		falloff = 1.0;

	int bInWater = (UTIL_PointContents ( vecSrc ) == CONTENTS_WATER);

	vecSrc.z += 1;// in case grenade is lying on the ground

	if ( !pevAttacker )
		pevAttacker = pevInflictor;

	// iterate on all entities in the vicinity.
	while ((pEntity = UTIL_FindEntityInSphere( pEntity, vecSrc, flRadius )) != NULL)
	{
		if ( pEntity->pev->takedamage != DAMAGE_NO || FClassnameIs(pEntity->pev, "func_pushable"))
		{
			// UNDONE: this should check a damage mask, not an ignore
			if ( iClassIgnore != CLASS_NONE && pEntity->Classify() == iClassIgnore )
			{// houndeyes don't hurt other houndeyes with their attack
				continue;
			}

			// blast's don't tavel into or out of water
			if (bInWater && pEntity->pev->waterlevel == 0)
				continue;
			if (!bInWater && pEntity->pev->waterlevel == 3)
				continue;

			vecSpot = pEntity->BodyTarget( vecSrc );
			
			UTIL_TraceLine ( vecSrc, vecSpot, ignore_monsters, ENT(pevInflictor), &tr );

			if ( tr.flFraction == 1.0 || tr.pHit == pEntity->edict() )
			{// the explosion can 'see' this entity, so hurt them!
				if (tr.fStartSolid)
				{
					// if we're stuck inside them, fixup the position and distance
					tr.vecEndPos = vecSrc;
					tr.flFraction = 0.0;
				}
				
				// decrease damage for an ent that's farther from the bomb.
				flAdjustedDamage = ( vecSrc - tr.vecEndPos ).Length() * falloff;
				flAdjustedDamage = flDamage - flAdjustedDamage;
			
				if ( flAdjustedDamage < 0 )
					flAdjustedDamage = 0;

				ClearMultiDamage( );
				pEntity->TraceAttack( pevInflictor, flAdjustedDamage, (tr.vecEndPos - vecSrc).Normalize( ), &tr, bitsDamageType );
				ApplyMultiDamage( pevInflictor, pevAttacker );
			}
		}
	}
}

void CBaseMonster :: WaterRadiusDamage(entvars_t* pevInflictor, entvars_t*	pevAttacker, float flDamage, int iClassIgnore, int bitsDamageType )
{
	::WaterRadiusDamage( pev->origin, pevInflictor, pevAttacker, flDamage, flDamage * 2.5, iClassIgnore, bitsDamageType );
}


void CBaseMonster :: WaterRadiusDamage( Vector vecSrc, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int iClassIgnore, int bitsDamageType )
{
	::WaterRadiusDamage( vecSrc, pevInflictor, pevAttacker, flDamage, flDamage * 2.5, iClassIgnore, bitsDamageType );
}

// Damage do not decrease with radius
void FullRadiusDamage( Vector vecSrc, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, float flRadius, int iClassIgnore, int bitsDamageType )
{
	CBaseEntity *pEntity = NULL;
	TraceResult	tr;
	Vector		vecSpot;

	vecSrc.z += 1;

	if ( !pevAttacker )
		pevAttacker = pevInflictor;

	while ((pEntity = UTIL_FindEntityInSphere( pEntity, vecSrc, flRadius )) != NULL)
	{
		if ( pEntity->pev->takedamage != DAMAGE_NO || FClassnameIs(pEntity->pev, "func_pushable"))
		{
			if ( iClassIgnore != CLASS_NONE && pEntity->Classify() == iClassIgnore )
				continue;

				vecSpot = pEntity->BodyTarget( vecSrc );
				UTIL_TraceLine ( vecSrc, vecSpot, ignore_monsters, ENT(pevInflictor), &tr );

			if (tr.flFraction == 1.0 || tr.pHit == pEntity->edict() )
			{
				if (tr.fStartSolid)
				{
					tr.vecEndPos = vecSrc;
					tr.flFraction = 0.0;
				}
				ClearMultiDamage( );
				pEntity->TraceAttack( pevInflictor, flDamage, (tr.vecEndPos - vecSrc).Normalize( ), &tr, bitsDamageType );
				ApplyMultiDamage( pevInflictor, pevAttacker );
			}
		}
	}
}

void CBaseMonster :: FullRadiusDamage(entvars_t* pevInflictor, entvars_t*	pevAttacker, float flDamage, int iClassIgnore, int bitsDamageType )
{
	::RadiusDamage( pev->origin, pevInflictor, pevAttacker, flDamage, flDamage * 2.5, iClassIgnore, bitsDamageType );
}

void CBaseMonster :: FullRadiusDamage( Vector vecSrc, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int iClassIgnore, int bitsDamageType )
{
	::RadiusDamage( vecSrc, pevInflictor, pevAttacker, flDamage, flDamage * 2.5, iClassIgnore, bitsDamageType );
}

//=========================================================
// CheckTraceHullAttack - expects a length to trace, amount 
// of damage to do, and damage type. Returns a pointer to
// the damaged entity in case the monster wishes to do
// other stuff to the victim (punchangle, etc)
//
// Used for many contact-range melee attacks. Bites, claws, etc.
//=========================================================
CBaseEntity* CBaseMonster :: CheckTraceHullAttack( float flDist, int iDamage, int iDmgType )
{
	TraceResult tr;

	if (IsPlayer())
		UTIL_MakeVectors( pev->angles );
	else
		UTIL_MakeAimVectors( pev->angles );

	Vector vecStart = pev->origin;
	vecStart.z += pev->size.z * 0.5;
	Vector vecEnd = vecStart + (gpGlobals->v_forward * flDist );

	UTIL_TraceHull( vecStart, vecEnd, dont_ignore_monsters, head_hull, ENT(pev), &tr );
	
	if ( tr.pHit )
	{
		CBaseEntity *pEntity = CBaseEntity::Instance( tr.pHit );

		if ( iDamage > 0 )
		{
			ClearMultiDamage( );//				LLAPb : old value (vecEnd) was critical in case of shield gun
			pEntity->TraceAttack( pev, iDamage, gpGlobals->v_forward, &tr, iDmgType );
			ApplyMultiDamage( pev, pev );
		}

		return pEntity;
	}

	return NULL;
}


//=========================================================
// FInViewCone - returns true is the passed ent is in
// the caller's forward view cone. The dot product is performed
// in 2d, making the view cone infinitely tall. 
//=========================================================
BOOL CBaseMonster :: FInViewCone ( CBaseEntity *pEntity )
{
	Vector2D	vec2LOS;
	float	flDot;

	UTIL_MakeVectors ( pev->angles );
	
	vec2LOS = ( pEntity->pev->origin - pev->origin ).Make2D();
	vec2LOS = vec2LOS.Normalize();

	flDot = DotProduct (vec2LOS , gpGlobals->v_forward.Make2D() );

	if ( flDot > m_flFieldOfView )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//=========================================================
// FInViewCone - returns true is the passed vector is in
// the caller's forward view cone. The dot product is performed
// in 2d, making the view cone infinitely tall. 
//=========================================================
BOOL CBaseMonster :: FInViewCone ( Vector *pOrigin )
{
	Vector2D	vec2LOS;
	float		flDot;

	UTIL_MakeVectors ( pev->angles );
	
	vec2LOS = ( *pOrigin - pev->origin ).Make2D();
	vec2LOS = vec2LOS.Normalize();

	flDot = DotProduct (vec2LOS , gpGlobals->v_forward.Make2D() );

	if ( flDot > m_flFieldOfView )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//=========================================================
// FVisible - returns true if a line can be traced from
// the caller's eyes to the target
//=========================================================
BOOL CBaseEntity :: FVisible ( CBaseEntity *pEntity )
{
	TraceResult tr;
	Vector		vecLookerOrigin;
	Vector		vecTargetOrigin;
	
	if (FBitSet( pEntity->pev->flags, FL_NOTARGET ))
		return FALSE;

	// don't look through water
	if ((pev->waterlevel != 3 && pEntity->pev->waterlevel == 3) 
		|| (pev->waterlevel == 3 && pEntity->pev->waterlevel == 0))
		return FALSE;

	vecLookerOrigin = pev->origin + pev->view_ofs;//look through the caller's 'eyes'
	vecTargetOrigin = pEntity->EyePosition();

	UTIL_TraceLine(vecLookerOrigin, vecTargetOrigin, ignore_monsters, ignore_glass, ENT(pev)/*pentIgnore*/, &tr);
	
	if (tr.flFraction != 1.0)
	{
		return FALSE;// Line of sight is not established
	}
	else
	{
		return TRUE;// line of sight is valid.
	}
}

//=========================================================
// FVisible - returns true if a line can be traced from
// the caller's eyes to the target vector
//=========================================================
BOOL CBaseEntity :: FVisible ( const Vector &vecOrigin )
{
	TraceResult tr;
	Vector		vecLookerOrigin;
	
	vecLookerOrigin = EyePosition();//look through the caller's 'eyes'

	UTIL_TraceLine(vecLookerOrigin, vecOrigin, ignore_monsters, ignore_glass, ENT(pev)/*pentIgnore*/, &tr);
	
	if (tr.flFraction != 1.0)
	{
		return FALSE;// Line of sight is not established
	}
	else
	{
		return TRUE;// line of sight is valid.
	}
}

/*
================
TraceAttack
================
*/
void CBaseEntity::TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType)
{
	if ( pev->takedamage )
		AddMultiDamage( pevAttacker, this, flDamage, bitsDamageType );
}

void CBaseMonster :: TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType)
{
	if ( pev->takedamage )
	{
		AddMultiDamage( pevAttacker, this, flDamage, bitsDamageType );
		TraceBleed( flDamage, vecDir, ptr, bitsDamageType );
	}
}
/*
	{
		m_LastHitGroup = ptr->iHitgroup;

		switch ( ptr->iHitgroup )
		{
			case HITGROUP_GENERIC:
			break;

			case HITGROUP_HEAD:
				flDamage *= dmg_mon_head.value;
			break;

			case HITGROUP_CHEST:
				flDamage *= dmg_mon_chest.value;
			break;

			case HITGROUP_STOMACH:
				flDamage *= dmg_mon_stomach.value;
			break;

			case HITGROUP_LEFTARM:
			case HITGROUP_RIGHTARM:
				flDamage *= dmg_mon_arm.value;
			break;

			case HITGROUP_LEFTLEG:
			case HITGROUP_RIGHTLEG:
				flDamage *= dmg_mon_leg.value;
			break;

			default:
			break;
		}
		AddMultiDamage( pevAttacker, this, flDamage, bitsDamageType );
		TraceBleed( flDamage, vecDir, ptr, bitsDamageType );
	}
}

*/

/*
================
FireMagnumBullets
Penetrate wall bullets
================
*/
Vector CBaseEntity::FireMagnumBullets(Vector vecSrc, Vector vecDirShooting, Vector vecSpread, float flDistance, int iBulletType, entvars_t *pevAttacker)
{
	TraceResult tr, beam_tr, beam_tr_end;
	float x = 0, y = 0, z = 0;
	int iPiercePower = 0; 
	int iDmgType = 0;
	float iDamage = 0;

	if ( pevAttacker == NULL )
		pevAttacker = pev;  // the default attacker is ourselves

	x = RANDOM_FLOAT(-0.5,0.5) + RANDOM_FLOAT(-0.5,0.5);
	y = RANDOM_FLOAT(-0.5,0.5) + RANDOM_FLOAT(-0.5,0.5);

	Vector vecDir = vecDirShooting + x*vecSpread.x*gpGlobals->v_right + y*vecSpread.y*gpGlobals->v_up;
	Vector vecEnd = vecSrc + vecDir * flDistance;
	UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, ENT(pev), &tr);

	CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
	int tex = (int)TEXTURETYPE_Trace(&tr, vecSrc, vecEnd);
	int surface = (int)SURFACETYPE_Trace(&tr, vecSrc, vecEnd);

	FX_ImpBullet( tr.vecEndPos, (iBulletType == BULLET_NAILGUN)?vecDirShooting:tr.vecPlaneNormal, vecSrc, surface, iBulletType, (float)tex );

	switch(iBulletType)
	{
		default:
		break;

		case BULLET_CROWBAR:
			iPiercePower = 0;
			iDmgType = DMG_SLASH;
			iDamage = dmg_crowbar.value * (mp_wpn_power.value/100); 
		break;

		case BULLET_KNIFE:
			iPiercePower = 0;
			iDmgType = DMG_SLASH;
			iDamage = dmg_knife.value * (mp_wpn_power.value/100);
		break;

		case BULLET_CROWBARQUAD:
			iPiercePower = 0;
			iDmgType = DMG_SLASH | DMG_KNOCKBACK;
			iDamage = (dmg_crowbar.value*4) * (mp_wpn_power.value/100); 
		break;

		case BULLET_KNIFEQUAD:
			iPiercePower = 0;
			iDmgType = DMG_SLASH | DMG_KNOCKBACK;
			iDamage = (dmg_knife.value*4) * (mp_wpn_power.value/100);
		break;

		case BULLET_BANDSAW:
			iPiercePower = 0;
			iDmgType = DMG_SLASH;
			iDamage = dmg_bandsaw.value * (mp_wpn_power.value/100); 
		break;

		case BULLET_NAILGUN:
			iPiercePower = 0;
			iDmgType = DMG_BULLETMAGNUM | DMG_KNOCKBACK;
			iDamage = dmg_nailgun.value * (mp_wpn_power.value/100);
		break;

		case BULLET_2MM:	
			iDamage = dmg_m72.value * (mp_wpn_power.value/100);
			iDmgType = DMG_BULLETMAGNUM | DMG_KNOCKBACK;
			iPiercePower = iDamage/2;
		break;

		case BULLET_2MM_QUAD:	
			iDamage = (dmg_m72.value*4) * (mp_wpn_power.value/100);
			iDmgType = DMG_BULLETMAGNUM | DMG_KNOCKBACK;
			iPiercePower = iDamage/2;
		break;

		case BULLET_338Magnum:	
			iPiercePower = 90;
			iDmgType = DMG_BULLETMAGNUM;
			iDamage = dmg_awp.value * (mp_wpn_power.value/100);
		break;

		case BULLET_357:		
			iPiercePower = 30;
			iDmgType = DMG_BULLETMAGNUM;
			iDamage = dmg_python.value * (mp_wpn_power.value/100);
		break;
				
		case BULLET_762Nato:	
			iPiercePower = 55;
			iDmgType = DMG_BULLETMAGNUM;
			iDamage = dmg_m16.value * (mp_wpn_power.value/100);
		break;

		case BULLET_556Nato:	
			iPiercePower = 48;
			iDmgType = DMG_BULLETMAGNUM;
			iDamage = dmg_m249.value * (mp_wpn_power.value/100);
		break;

		case BULLET_556:	
			iPiercePower = 40;
			iDmgType = DMG_BULLETMAGNUM;
			iDamage = dmg_u2.value * (mp_wpn_power.value/100);
		break;

		case BULLET_762:	
			iPiercePower = 62;
			iDmgType = DMG_BULLETMAGNUM;
			iDamage = dmg_ak74.value * (mp_wpn_power.value/100);
		break;

		case BULLET_762x54:	
			iPiercePower = 70;
			iDmgType = DMG_BULLETMAGNUM;
			iDamage = dmg_svd.value * (mp_wpn_power.value/100);
		break;

		case BULLET_86mm:	
			iPiercePower = 75;
			iDmgType = DMG_BULLETMAGNUM;
			iDamage = dmg_minigun.value * (mp_wpn_power.value/100);
		break;

		case BULLET_32mm:	
			iPiercePower = 125;
			iDmgType = DMG_BULLETMAGNUM | DMG_KNOCKBACK;
			iDamage = dmg_machinegun.value * (mp_wpn_power.value/100);
		break;
			
		case BULLET_50AE:		
			iPiercePower = 25;
			iDmgType = DMG_BULLETMAGNUM;
			iDamage = dmg_deagle.value * (mp_wpn_power.value/100);
		break;

		case BULLET_BUCKSHOT:
			iPiercePower = 8;
			iDmgType = DMG_BULLETBUCKSHOT;
			iDamage = dmg_shotgun.value * (mp_wpn_power.value/100);
		break;

		case BULLET_MP5:
			iPiercePower = 18;
			iDmgType = DMG_BULLET;
			iDamage = dmg_mp5.value * (mp_wpn_power.value/100);
		break;

		case BULLET_12G:	
			iPiercePower = 8;
			iDmgType = DMG_BULLETBUCKSHOT;
			iDamage = dmg_shieldgun.value * (mp_wpn_power.value/100);
		break;
			
		case BULLET_10MMBUCKSHOT:	
			iPiercePower = 8;
			iDmgType = DMG_BULLETBUCKSHOT;
			iDamage = dmg_autoshotgun.value * (mp_wpn_power.value/100);
		break;

		case BULLET_30mm:	
			iPiercePower = 15;
			iDmgType = DMG_BULLETBUCKSHOT;
			iDamage = dmg_30mmsg.value * (mp_wpn_power.value/100);
		break;
			
		case BULLET_9mmP:	
			iPiercePower = 15;
			iDmgType = DMG_BULLET;
			iDamage = dmg_glock.value * (mp_wpn_power.value/100);
		break;

		case BULLET_8mm:	
			iPiercePower = 14;
			iDmgType = DMG_BULLET;
			iDamage = dmg_uzi.value * (mp_wpn_power.value/100);
		break;
			
		case BULLET_57mm:	
			iPiercePower = 18;
			iDmgType = DMG_BULLET;
			iDamage = dmg_g11.value * (mp_wpn_power.value/100);
		break;
			
		case BULLET_45ACP:		
			iPiercePower = 16;
			iDmgType = DMG_BULLET;
			iDamage = dmg_usp.value * (mp_wpn_power.value/100);
		break;

		case BULLET_BFG:	
			iPiercePower = 0;
			iDmgType = DMG_BULLET;
			iDamage = (dmg_bfg.value/10) * (mp_wpn_power.value/100);
			RadiusDamage(tr.vecEndPos, pevAttacker, pevAttacker, dmg_bfg.value * (mp_wpn_power.value/100), (dmg_bfg.value*10) * (mp_wpn_power.value/100), CLASS_NONE, DMG_NUKE);
			RadiusDamage(tr.vecEndPos, pevAttacker, pevAttacker, 5, (dmg_bfg.value*10) * (mp_wpn_power.value/100), CLASS_NONE, DMG_RADIATION);
		break;

		case BULLET_127MM:	
			iPiercePower = 0;
			iDmgType = DMG_BULLET;
			iDamage = (dmg_barett.value/10) * (mp_wpn_power.value/100);
			RadiusDamage(tr.vecEndPos, pevAttacker, pevAttacker, dmg_barett.value * (mp_wpn_power.value/100), (dmg_barett.value*2) * (mp_wpn_power.value/100), CLASS_NONE, DMG_BLAST);
		break;

		case BULLET_9MM:
			iPiercePower = 25;
			iDmgType = DMG_BULLET;
			iDamage = dmg_9mm_bullet.value * (mp_wpn_power.value/100);
		break;

		case BULLET_12MM:		
			iPiercePower = 40;
			iDmgType = DMG_BULLETMAGNUM;
			iDamage = dmg_12mm_bullet.value * (mp_wpn_power.value/100);
		break;

		case BULLET_14MM:
			iPiercePower = 50;
			iDmgType = DMG_BULLETMAGNUM;
			iDamage = dmg_14mm_bullet.value * (mp_wpn_power.value/100);
		break;
	}

		if (pEntity->pev->takedamage || FClassnameIs(pEntity->pev, "func_pushable"))
		{
			ClearMultiDamage();
			pEntity->TraceAttack(pevAttacker, iDamage, vecDir, &tr, iDmgType | DMG_NEVERGIB);
			ApplyMultiDamage(pev, pevAttacker);
		}

		UTIL_TraceLine( tr.vecEndPos + vecDir * 8, vecEnd, dont_ignore_monsters, ENT( pev ), &beam_tr);

		if (!beam_tr.fAllSolid)
		{
			if (tex == CHAR_TEX_ENERGYSHIELD || tex == CHAR_TEX_BP_METAL || tex == CHAR_TEX_BP_GLASS || tex == CHAR_TEX_BP_CONCRETE)
				return Vector( x * vecSpread.x, y * vecSpread.y, 0.0 );

			// trace backwards to find exit point
			UTIL_TraceLine( beam_tr.vecEndPos, tr.vecEndPos, dont_ignore_monsters, ENT( pev ), &beam_tr);

			if((beam_tr.vecEndPos - tr.vecEndPos).Length( ) > (iPiercePower)) //if walls are ticker than bullet-pierce power (units)
				return Vector( x * vecSpread.x, y * vecSpread.y, 0.0 );

			FX_ImpBullet( beam_tr.vecEndPos, -tr.vecPlaneNormal, vecSrc, surface, iBulletType, (float)tex );
			UTIL_TraceLine( beam_tr.vecEndPos, vecEnd, dont_ignore_monsters, ENT( pev ), &beam_tr_end);
	
			int tex_exit_end = (int)TEXTURETYPE_Trace(&beam_tr_end, beam_tr.vecEndPos, vecEnd);
			int surf_exit_end = (int)SURFACETYPE_Trace(&beam_tr_end, beam_tr.vecEndPos, vecEnd);
			CBaseEntity *pExitEntity = CBaseEntity::Instance(beam_tr_end.pHit);

			FX_ImpBullet( beam_tr_end.vecEndPos, tr.vecPlaneNormal, vecSrc, surf_exit_end, iBulletType, (float)tex_exit_end );

			if (pExitEntity->pev->takedamage || FClassnameIs(pExitEntity->pev, "func_pushable"))
			{
				ClearMultiDamage();
				pExitEntity->TraceAttack(pevAttacker, iDamage/2, vecDir, &beam_tr_end, iDmgType | DMG_NEVERGIB );
				ApplyMultiDamage(pev, pevAttacker);
			} 
		}
	return Vector( x * vecSpread.x, y * vecSpread.y, 0.0 );
}


/*
================
FireBeam
Penetrating wall beams
================
*/
Vector CBaseEntity::FireBeam (Vector vecSrc, Vector vecDirShooting, int iBeamType, float flDamage, entvars_t *pevAttacker)
{
	TraceResult tr, beam_tr, beam_tr_end;
	Vector vecEnd = vecSrc + vecDirShooting * 16384;
	UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, ENT(pev), &tr);
	int iPiercePower = 0; 
	int iDmgType = 0;
	float iDamage = 0;
	float iRadiusDamage = 0;
	int iImpactEffect = 0;

	if (pevAttacker == NULL)
		pevAttacker = pev;

	CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
	int tex = (int)TEXTURETYPE_Trace(&tr, vecSrc, vecEnd);
	int surface = (int)SURFACETYPE_Trace(&tr, vecSrc, vecEnd);

	switch(iBeamType)
	{
		case BEAM_BLASTER:		
			iDamage = dmg_blaster.value * (mp_wpn_power.value/100);
			iDmgType = DMG_ENERGYBEAM | DMG_NEVERGIB;
			iPiercePower = iDamage;
			iImpactEffect = IMPBEAM_BLASTER;
		break;

		case BEAM_GAUSS:		
			iDamage = flDamage;
			iDmgType = DMG_ENERGYBEAM | DMG_SHOCK | DMG_NEVERGIB;
			iPiercePower = iDamage;
			iImpactEffect = IMPBEAM_GAUSS;
		break;

		case BEAM_GAUSSCHARGED:		
			iDamage = flDamage;
			iDmgType = DMG_ENERGYBEAM | DMG_SHOCK | DMG_NEVERGIB;
			iPiercePower = flDamage/2;
			iImpactEffect = IMPBEAM_GAUSSCHARGED;
			::RadiusDamage( tr.vecEndPos, pev, pevAttacker, iDamage/6, iDamage/2, CLASS_NONE, iDmgType);
		break;

		case BEAM_PHOTONGUN:		
			iDamage = dmg_photon_beam.value * (mp_wpn_power.value/100);
			iDmgType = DMG_ENERGYBEAM | DMG_NEVERGIB;
			iPiercePower = iDamage;
			iImpactEffect = IMPBEAM_PHOTONGUN;
		break;

		case BEAM_PHOTONGUN_EXP:		
			iDamage = (dmg_photon_ball.value/10) * (mp_wpn_power.value/100);
			iDmgType = DMG_ENERGYBEAM | DMG_NEVERGIB;
			::FullRadiusDamage( tr.vecEndPos, pev, pevAttacker, dmg_photon_ball.value * (mp_wpn_power.value/100), dmg_photon_ball.value * (mp_wpn_power.value/100), CLASS_NONE, iDmgType);
		break;

		case BEAM_TESLAGUN:		
			iDamage = (dmg_teslagun.value/10) * (mp_wpn_power.value/100);
			iDmgType = DMG_SHOCK | DMG_NEVERGIB;

			if (UTIL_PointContents(tr.vecEndPos) == CONTENT_WATER)
				::WaterRadiusDamage( tr.vecEndPos, pev, pevAttacker, (dmg_teslagun.value*3) * (mp_wpn_power.value/100), (dmg_teslagun.value*6) * (mp_wpn_power.value/100), CLASS_NONE, iDmgType);
			else
				::FullRadiusDamage( tr.vecEndPos, pev, pevAttacker, dmg_teslagun.value * (mp_wpn_power.value/100), dmg_teslagun.value * (mp_wpn_power.value/100), CLASS_NONE, iDmgType);
		break;

		case BEAM_PULSERIFLE:		
			iDamage = dmg_pulserifle.value * (mp_wpn_power.value/100);
			iDmgType = DMG_SHOCK | DMG_KNOCKBACK | DMG_NEVERGIB;

			if (UTIL_PointContents(tr.vecEndPos) == CONTENT_WATER)
				::WaterRadiusDamage( tr.vecEndPos, pev, pevAttacker, iDamage*2, iDamage*3, CLASS_NONE, iDmgType);
			else
				::RadiusDamage( tr.vecEndPos, pev, pevAttacker, iDamage/6, iDamage, CLASS_NONE, iDmgType);
		break;

		case BEAM_TAUCANNON:		
			iDamage = dmg_taucannon.value * (mp_wpn_power.value/100);
			iDmgType = DMG_ENERGYBEAM | DMG_NEVERGIB | DMG_CONCUSSION;
			iPiercePower = iDamage;
			iImpactEffect = IMPBEAM_TAUCANNON;
			::RadiusDamage( tr.vecEndPos, pev, pevAttacker, iDamage/4, iDamage/2, CLASS_NONE, iDmgType);
		break;

		case BEAM_IONTURRET:		
			iDamage = dmg_ionturret.value * (mp_wpn_power.value/100);
			iDmgType = DMG_ENERGYBEAM | DMG_NEVERGIB;
			iPiercePower = iDamage/2;
			iImpactEffect = IMPBEAM_IONTURRET;
			::RadiusDamage( tr.vecEndPos, pev, pevAttacker, iDamage/4, iDamage/2, CLASS_NONE, iDmgType);
		break;
	}
	if (iImpactEffect > 0 && UTIL_PointContents(tr.vecEndPos) != CONTENTS_SKY)
		FX_ImpBeam( tr.vecEndPos, tr.vecPlaneNormal, surface, iImpactEffect );

	FX_FireBeam( vecSrc, tr.vecEndPos, tr.vecPlaneNormal, iBeamType );

	if (pEntity->pev->takedamage)
	{
		ClearMultiDamage();
		pEntity->TraceAttack( pevAttacker, iDamage, vecEnd, &tr, iDmgType);
		ApplyMultiDamage(pev, pevAttacker);
	}

	if (tex == CHAR_TEX_ENERGYSHIELD || iPiercePower < 1) 
		return Vector(0,0,0);

	UTIL_TraceLine( tr.vecEndPos + vecDirShooting*8, vecEnd, dont_ignore_monsters, ENT( pev ), &beam_tr);
	if (!beam_tr.fAllSolid)
	{
		UTIL_TraceLine( beam_tr.vecEndPos, tr.vecEndPos, dont_ignore_monsters, ENT( pev ), &beam_tr);

		if((beam_tr.vecEndPos - tr.vecEndPos).Length( ) > iPiercePower) //if walls are ticker than gun-DMG units
			return Vector(0,0,0);

		if (iImpactEffect > 0)
			FX_ImpBeam( beam_tr.vecEndPos, tr.vecPlaneNormal, surface, iImpactEffect );

		UTIL_TraceLine( beam_tr.vecEndPos, vecEnd, dont_ignore_monsters, ENT(pev), &beam_tr_end);
		CBaseEntity *pExitEntity = CBaseEntity::Instance(beam_tr_end.pHit);
		int surf_exit_end = (int)SURFACETYPE_Trace(&beam_tr_end, beam_tr.vecEndPos, vecEnd);

		if (iImpactEffect > 0 && UTIL_PointContents(beam_tr_end.vecEndPos) != CONTENTS_SKY)
			FX_ImpBeam( beam_tr_end.vecEndPos, tr.vecPlaneNormal, surf_exit_end, iImpactEffect );

		FX_FireBeam( beam_tr.vecEndPos, beam_tr_end.vecEndPos, beam_tr_end.vecPlaneNormal, iBeamType+1 );

		if (pExitEntity->pev->takedamage)
		{
			ClearMultiDamage();
			pExitEntity->TraceAttack(pevAttacker, iDamage/1.5, vecEnd, &beam_tr_end, iDmgType);
			ApplyMultiDamage(pev, pevAttacker);
		} 
	}
	return Vector(0,0,0);
}

void CBaseEntity :: TraceBleed( float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType )
{
	if (flDamage == 0)
		return;

	if (bitsDamageType & DMG_NOBLOOD)
		return;
	
	// make blood decal on the wall! 
	TraceResult Bloodtr;
	Vector vecTraceDir; 
	float flNoise;
	int cCount, i;

	if (flDamage < 10)
	{
		flNoise = 0.1;
		cCount = 1;
	}
	else if (flDamage < 25)
	{
		flNoise = 0.2;
		cCount = 2;
	}
	else if (flDamage < 50)
	{
		flNoise = 0.3;
		cCount = 4;
	}
	else
	{
		flNoise = 0.5;
		cCount = 6;
	}

	for ( i = 0 ; i < cCount ; i++ )
	{
		vecTraceDir = vecDir * -1;// trace in the opposite direction the shot came from (the direction the shot is going)

		vecTraceDir.x += RANDOM_FLOAT( -flNoise, flNoise );
		vecTraceDir.y += RANDOM_FLOAT( -flNoise, flNoise );
		vecTraceDir.z += RANDOM_FLOAT( -flNoise, flNoise );
		UTIL_TraceLine( ptr->vecEndPos, ptr->vecEndPos + vecTraceDir * -172, ignore_monsters, ENT(pev), &Bloodtr);

		if ( Bloodtr.flFraction != 1.0 )
			UTIL_BloodDecalTrace(&Bloodtr);
	}
}