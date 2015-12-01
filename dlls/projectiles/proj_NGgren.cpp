#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "decals.h"
#include "game.h"
#include "projectiles.h"

LINK_ENTITY_TO_CLASS( nggrenade, CNGgrenade );

void CNGgrenade::Killed (entvars_t *pevAttacker, int iGib)
{
	FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
	UTIL_Remove( this );
}

void CNGgrenade::Explode( TraceResult *pTrace, int bitsDamageType )
{
	pev->velocity = g_vecZero;
	pev->origin = pev->origin + Vector(0,0,5);

	entvars_t *pevOwner = VARS( pev->owner );
	::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg, pev->dmg*40, CLASS_MACHINE, DMG_NERVEGAS | DMG_CONCUSSION);

	GasStayTime = 80;
	FX_Trail( pev->origin, entindex(), PROJ_NERVEGREN_DETONATE );

	SetTouch(NULL);
	SetThink( Smoke );
	pev->nextthink = gpGlobals->time + 0.2;
}

void CNGgrenade::Smoke( void )
{
	if(GasStayTime <=0 )
	{
		STOP_SOUND( edict(), CHAN_VOICE, "weapons/NGgrenade_explode.wav" );
		UTIL_Remove( this );
	}
	else
		GasStayTime--;

	entvars_t *pevOwner = VARS( pev->owner );
	if (UTIL_PointContents ( pev->origin ) != CONTENTS_WATER)
	{
		::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg, pev->dmg*35, CLASS_MACHINE, DMG_NERVEGAS | DMG_CONCUSSION);
		if ( gpGlobals->time >= m_flSoundTime )
		{
			EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/NGgrenade_explode.wav", 0.7, ATTN_NORM);
			m_flSoundTime = gpGlobals->time + 3;
		}
	}
	pev->nextthink = gpGlobals->time + 0.2;
}

void CNGgrenade::Detonate( void )
{
	TraceResult tr;
	Vector vecSpot = pev->origin + Vector ( 0 , 0 , 8 );
	UTIL_TraceLine ( vecSpot, vecSpot + Vector ( 0, 0, -40 ),  ignore_monsters, ENT(pev), & tr);
	Explode( &tr, DMG_BLAST );
}

void CNGgrenade::BounceTouch( CBaseEntity *pOther )
{
	if (pOther->pev->takedamage == DAMAGE_AIM)
	{
		TraceResult tr;
		entvars_t *pevOwner = VARS( pev->owner );
		ClearMultiDamage( );
		pOther->TraceAttack(pevOwner, pev->dmg*3, gpGlobals->v_forward, &tr, DMG_BULLETMAGNUM ); 
		ApplyMultiDamage( pev, pevOwner);
		SetThink( Detonate );
		return;
	}
	if ( UTIL_PointContents(pev->origin) == CONTENT_SKY )
	{
		FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
		UTIL_Remove( this );
		return;
	}

	if (pev->flags & FL_ONGROUND)
		pev->velocity = pev->velocity * 0.8;
	else
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/chronoclip_bounce.wav", 1, ATTN_NORM);
}

void CNGgrenade :: TumbleThink( void )
{
	if (!IsInWorld())
	{
		Detonate( );
		return;
	}
	pev->nextthink = gpGlobals->time + 0.1;

	if (pev->dmgtime <= gpGlobals->time)
		SetThink( Detonate );
}

void CNGgrenade:: Spawn( void )
{
	pev->movetype = MOVETYPE_BOUNCE;
	pev->classname = MAKE_STRING( "nggrenade" );
	pev->solid = SOLID_BBOX;
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = 8;
	UTIL_SetSize(pev, g_vecZero, g_vecZero );
}

CNGgrenade *CNGgrenade::ShootNGgrenade( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time )
{
	CNGgrenade *pNGgrenade = GetClassPtr( (CNGgrenade *)NULL );
	pNGgrenade->Spawn();

	UTIL_SetOrigin( pNGgrenade->pev, vecStart );
	pNGgrenade->pev->velocity = vecVelocity;
	pNGgrenade->pev->angles = UTIL_VecToAngles(pNGgrenade->pev->velocity);
	pNGgrenade->pev->owner = ENT(pevOwner); 
	pNGgrenade->SetTouch( BounceTouch ); 
	pNGgrenade->pev->dmgtime = gpGlobals->time + time;
	pNGgrenade->SetThink( TumbleThink );
	pNGgrenade->pev->nextthink = gpGlobals->time + 0.1;
	pNGgrenade->pev->gravity = 1;
	pNGgrenade->pev->friction = 0.8;
	pNGgrenade->pev->dmg = dmg_m16_grenade.value * (mp_wpn_power.value/100);
	FX_Trail(pNGgrenade->pev->origin, pNGgrenade->entindex(), PROJ_NERVEGREN );
	return pNGgrenade;
}