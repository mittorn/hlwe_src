#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "soundent.h"
#include "game.h"
#include "projectiles.h"

LINK_ENTITY_TO_CLASS( grenade, CGrenade );

void CGrenade::Detonate( void )
{
	TraceResult tr;
	Vector vecSpot = pev->origin + Vector (0 ,0 ,8);
	Vector vecEnd =  pev->origin + Vector (0 ,0 ,-40);
	UTIL_TraceLine(vecSpot, vecEnd, ignore_monsters, ENT(pev), &tr);

	int tex = (int)TEXTURETYPE_Trace(&tr, vecSpot, vecEnd);
	CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
	FX_ImpRocket( tr.vecEndPos, tr.vecPlaneNormal, pEntity->IsBSPModel()?1:0, BULLET_NORMEXP, (float)tex );

	entvars_t *pevOwner = VARS( pev->owner );
	::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg, pev->dmg*2.2, CLASS_NONE, DMG_BLAST);
	FX_Explosion( tr.vecEndPos + (tr.vecPlaneNormal * 25), EXPLOSION_GRENADE );
	UTIL_Remove( this );
}

void CGrenade::BounceTouch( CBaseEntity *pOther )
{
	// don't hit the guy that launched this grenade
	if ( pOther->edict() == pev->owner )
		return;

	// only do damage if we're moving fairly fast
	if (m_flNextAttack < gpGlobals->time && pev->velocity.Length() > 100)
	{
		entvars_t *pevOwner = VARS( pev->owner );
		if (pevOwner)
		{
			TraceResult tr = UTIL_GetGlobalTrace( );
			ClearMultiDamage( );
			UTIL_MakeVectors (pev->angles);
			pOther->TraceAttack(pevOwner, 1, gpGlobals->v_forward, &tr, DMG_SLASH); 
			ApplyMultiDamage( pev, pevOwner);
		}
		m_flNextAttack = gpGlobals->time + 1.0; // debounce
	}

	Vector vecTestVelocity;
	vecTestVelocity = pev->velocity; 
	vecTestVelocity.z *= 0.45;

	if (pev->flags & FL_ONGROUND)
	{
		pev->sequence = RANDOM_LONG(2,4);
		pev->velocity = pev->velocity * 0.8;
	}
	else
	{
		switch ( RANDOM_LONG( 0, 1 ) )
		{
			case 0:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/grenade_hit1.wav", 0.5, ATTN_NORM);	break;
			case 1:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/grenade_hit2.wav", 0.5, ATTN_NORM);	break;
		}
	}
	pev->framerate = pev->velocity.Length() / 200.0;
	if (pev->framerate > 1.0)
		pev->framerate = 1;
	else if (pev->framerate < 0.5)
		pev->framerate = 0;
}

void CGrenade :: TumbleThink( void )
{
	StudioFrameAdvance( );
	pev->nextthink = gpGlobals->time + 0.1;

	if (pev->dmgtime <= gpGlobals->time)
	{
		SetThink( Detonate );
	}
	if (pev->waterlevel != 0)
	{
		pev->velocity = pev->velocity * 0.5;
		pev->framerate = 0.2;
	}
}

void CGrenade:: Spawn( void )
{
	pev->movetype = MOVETYPE_BOUNCE;
	pev->classname = MAKE_STRING( "grenade" );
	pev->solid = SOLID_BBOX;
	pev->body = 1;
	pev->framerate = 1;
	pev->dmg = dmg_handgrenade.value * (mp_wpn_power.value/100);
	pev->sequence = RANDOM_LONG(2,4);
	pev->scale = 1.5;
	pev->friction = 0.8;
	SET_MODEL(ENT(pev), "models/p_grenade.mdl");
	UTIL_SetSize(pev, g_vecZero, g_vecZero );
}

CGrenade * CGrenade:: ShootGrenade( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time )
{
	CGrenade *pGrenade = GetClassPtr( (CGrenade *)NULL );
	pGrenade->Spawn();
	UTIL_SetOrigin( pGrenade->pev, vecStart );
	pGrenade->pev->velocity = vecVelocity;
	pGrenade->pev->angles = UTIL_VecToAngles(pGrenade->pev->velocity);
	pGrenade->pev->owner = ENT(pevOwner);
	pGrenade->SetTouch( BounceTouch );
	pGrenade->pev->dmgtime = gpGlobals->time + time;
	pGrenade->SetThink( TumbleThink );
	pGrenade->pev->nextthink = gpGlobals->time + 0.1;
return pGrenade;
}