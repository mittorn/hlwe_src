#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "game.h"
#include "projectiles.h"

LINK_ENTITY_TO_CLASS( bolt, CBolt );

void CBolt::ExplodeTouch( CBaseEntity *pOther )
{
	if ( UTIL_PointContents(pev->origin) == CONTENT_SKY )
	{
		UTIL_Remove( this );
		return;
	}
	if (pOther->pev->health == 666)
	{
		pev->movetype = MOVETYPE_FLY;
		pev->velocity = g_vecZero;
		SetThink(SUB_Remove);
		pev->nextthink = gpGlobals->time + 15;
		return;
	}
	TraceResult tr;
	Vector vecSpot = pev->origin - pev->velocity.Normalize() * 32;
	Vector vecEnd = pev->origin + pev->velocity.Normalize() * 64;
	UTIL_TraceLine( vecSpot, vecEnd, ignore_monsters, ENT(pev), &tr );

	entvars_t *pevOwner = VARS( pev->owner );

	if (pev->frags == 2)//Explosive bolt
	{
		int tex = (int)TEXTURETYPE_Trace(&tr, vecSpot, vecEnd);
		CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
		FX_ImpRocket( tr.vecEndPos, tr.vecPlaneNormal, pEntity->IsBSPModel()?1:0, BULLET_BOLT, (float)tex );
		::RadiusDamage( pev->origin, pev, pevOwner, dmg_crossbow_exp.value * (mp_wpn_power.value/100), (dmg_crossbow_exp.value * (mp_wpn_power.value/100))*2.5, CLASS_NONE, DMG_BLAST);
		FX_Explosion( tr.vecEndPos + (tr.vecPlaneNormal * 15), EXPLOSION_BOLT );
		UTIL_Remove( this );
	}
	else//Normal bolt
	{
		int tex = (int)TEXTURETYPE_Trace(&tr, vecSpot, vecEnd);
		CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
		FX_ImpBullet( tr.vecEndPos, tr.vecPlaneNormal, pev->origin, pEntity->IsBSPModel()?1:0, BULLET_BANDSAW, (float)tex );

		if (pOther->pev->takedamage == DAMAGE_AIM || pOther->Classify() == CLASS_MACHINE)
		{
			ClearMultiDamage();
			{
				if (pev->frags == 0)//Normal bolt
					pOther->TraceAttack(pevOwner, dmg_crossbow.value * (mp_wpn_power.value/100), gpGlobals->v_forward, &tr, DMG_SLASH | DMG_PARALYZE | DMG_KNOCKBACK );
				else if (pev->frags == 1)//Accelerated bolt
					pOther->TraceAttack(pevOwner, (dmg_crossbow.value*2) * (mp_wpn_power.value/100), gpGlobals->v_forward, &tr, DMG_SLASH | DMG_PARALYZE | DMG_KNOCKBACK);
			}
			ApplyMultiDamage(pev, pevOwner);
			UTIL_Remove( this );
			return;
		} 
		else if (FClassnameIs(pOther->pev, "func_pushable") || FClassnameIs(pOther->pev, "func_breakable"))
		{
			ClearMultiDamage();
			{
				if (pev->frags == 0)//Normal bolt
					pOther->TraceAttack(pevOwner, dmg_crossbow.value * (mp_wpn_power.value/100), gpGlobals->v_forward, &tr, DMG_SLASH | DMG_PARALYZE );
				else if (pev->frags == 1)//Accelerated bolt
					pOther->TraceAttack(pevOwner, (dmg_crossbow.value*2) * (mp_wpn_power.value/100), gpGlobals->v_forward, &tr, DMG_SLASH | DMG_PARALYZE );
			}
			ApplyMultiDamage(pev, pevOwner);

			EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/xbow_hit1.wav", 1, ATTN_NORM);
			pev->movetype = MOVETYPE_FLY;
			pev->velocity = g_vecZero;
			SetThink(SUB_Remove);
			pev->nextthink = gpGlobals->time + 15;
		}
		else if ( pOther->pev->solid == SOLID_BSP)
		{
			EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/xbow_hit1.wav", 1, ATTN_NORM);
			pev->movetype = MOVETYPE_FLY;
			pev->velocity = g_vecZero;
			SetThink(SUB_Remove);
			pev->nextthink = gpGlobals->time + 15;
		}
	}
}

void CBolt:: Spawn( void )
{
	pev->movetype = MOVETYPE_BOUNCE;
	pev->classname = MAKE_STRING( "bolt" );
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	UTIL_SetSize(pev, g_vecZero, g_vecZero );
}

CBolt *CBolt::ShootBolt( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, int type )
{
	CBolt *pBolt = GetClassPtr( (CBolt *)NULL );
	pBolt->Spawn();

	UTIL_SetOrigin( pBolt->pev, vecStart );
	pBolt->pev->velocity = vecVelocity + gpGlobals->v_right * RANDOM_FLOAT(-30,30) + gpGlobals->v_up * RANDOM_FLOAT(-30,30);
	pBolt->pev->angles = UTIL_VecToAngles (pBolt->pev->velocity);
	pBolt->pev->owner = ENT(pevOwner);
	pBolt->pev->nextthink = gpGlobals->time;
	pBolt->SetTouch( ExplodeTouch );
	pBolt->pev->frags = type;

	if (type == 2)
	{
		pBolt->pev->gravity = 1;
		pBolt->pev->body = 13;
		pBolt->pev->health = 1;
	}
	else
	{
		pBolt->pev->gravity = 0.5;
		pBolt->pev->body = 5;
		pBolt->pev->health = 666;
	}
	return pBolt;
}