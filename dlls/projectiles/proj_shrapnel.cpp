#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "decals.h"
#include "game.h"
#include "projectiles.h"
#include "player.h"

LINK_ENTITY_TO_CLASS(flak_shard, CShrapnel );

void CShrapnel::Spawn( void )
{
	pLauncher = NULL;
	pev->movetype = MOVETYPE_BOUNCEMISSILE;
	pev->solid = SOLID_BBOX;
	SetTouch( ShrapnelTouch );

	pev->dmg = dmg_flak_shrapnel.value * (mp_wpn_power.value/100); 
	pev->classname = MAKE_STRING("flak_shard");

	SET_MODEL( ENT(pev), "models/projectiles.mdl" );
	pev->body = 16;
	pev->health = 666;

	UTIL_SetSize(pev, Vector(0,0,0), Vector(0,0,0));
	UTIL_SetOrigin( pev, pev->origin );

	// Safety removal
	pev->nextthink = gpGlobals->time + 0.1;
	SetThink( ShrapnelThink );
}

void CShrapnel::ShrapnelThink ( void )
{
	SetThink (SUB_Remove);
	pev->nextthink = gpGlobals->time + 4.9f;
	pev->owner = NULL;
}

void CShrapnel::ShrapnelTouch( CBaseEntity *pOther )
{
	if (pOther->pev->health == 666)
		return;

	if ( UTIL_PointContents(pev->origin) == CONTENT_SKY )
	{
		UTIL_Remove( this );
		return;
	}

	TraceResult tr = UTIL_GetGlobalTrace();

	if (pOther->pev->takedamage == DAMAGE_AIM || pOther->Classify() == CLASS_MACHINE)
	{
		ClearMultiDamage();
		pOther->TraceAttack(pLauncher->pev, pev->dmg, gpGlobals->v_forward, &tr, DMG_BULLETMAGNUM | DMG_KNOCKBACK | DMG_ALWAYSGIB );
		ApplyMultiDamage(pev, pLauncher->pev);

		UTIL_Remove( this );
		return;
	} 
	else if (FClassnameIs(pOther->pev, "func_pushable") || FClassnameIs(pOther->pev, "func_breakable"))
	{
		ClearMultiDamage();
		pOther->TraceAttack(pLauncher->pev, pev->dmg, gpGlobals->v_forward, &tr, DMG_BULLETMAGNUM | DMG_KNOCKBACK | DMG_ALWAYSGIB );
		ApplyMultiDamage(pev, pLauncher->pev);

		int iRand = RANDOM_LONG(0,0x7FFF);//not every shard makes sound/decal (traffic friendly)
		if (iRand < (0x7fff/3))
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/nail_hitwall.wav", 1, ATTN_NORM);
		UTIL_DecalTrace(&tr, DECAL_BIGSHOT1 + RANDOM_LONG(0,4));
	}
	else if ( pOther->pev->solid == SOLID_BSP)
	{
		int iRand = RANDOM_LONG(0,0x7FFF);//not every shard makes sound/decal (traffic friendly)
		if (iRand < (0x7fff/3))
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/nail_hitwall.wav", 1, ATTN_NORM);
		UTIL_DecalTrace(&tr, DECAL_BIGSHOT1 + RANDOM_LONG(0,4));
	}
	pev->velocity = 2*tr.vecPlaneNormal + pev->velocity;//waves reflection
}

CShrapnel *CShrapnel::ShootShrapnel(entvars_t *pevOwner, Vector vecOrigin, Vector vecVelocity)
{
	CShrapnel *pShrapnel = GetClassPtr( (CShrapnel *)NULL );
	UTIL_SetOrigin( pShrapnel->pev, vecOrigin );

	UTIL_MakeVectors (pevOwner->v_angle + pevOwner->punchangle);
	pShrapnel->pev->velocity = vecVelocity + gpGlobals->v_right * RANDOM_FLOAT(-350,350) + gpGlobals->v_up * RANDOM_FLOAT(-350,350);
	pShrapnel->pev->angles = UTIL_VecToAngles(pShrapnel->pev->velocity);
	pShrapnel->Spawn();
	pShrapnel->pLauncher = (CBasePlayer *)CBaseEntity::Instance(pevOwner);
	pShrapnel->pev->owner = ENT(pevOwner);//LLAPb - shrapnel can't touch player for 0,1 seconds
	return pShrapnel;
}

CShrapnel *CShrapnel::ShootBombShrap(entvars_t *pevOwner, Vector vecOrigin)
{
	CShrapnel *pShrapnel = GetClassPtr( (CShrapnel *)NULL );
	UTIL_SetOrigin( pShrapnel->pev, vecOrigin );

	pShrapnel->pev->angles.x = RANDOM_FLOAT(-180,180);
	pShrapnel->pev->angles.y = RANDOM_FLOAT(-180,180);
	pShrapnel->pev->angles.z = RANDOM_FLOAT(-180,180);
	pShrapnel->pev->velocity = pShrapnel->pev->angles * 100;

	pShrapnel->Spawn();
	pShrapnel->pLauncher = (CBasePlayer *)CBaseEntity::Instance(pevOwner); 
	return pShrapnel;
}