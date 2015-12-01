#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "decals.h"
#include "game.h"
#include "projectiles.h"

LINK_ENTITY_TO_CLASS( FlashBang, CFlashBang );

void CFlashBang::Detonate( void )
{
	TraceResult tr;
	Vector vecSpot = pev->origin + Vector ( 0 , 0 , 8 );
	UTIL_TraceLine ( vecSpot, vecSpot + Vector ( 0, 0, -40 ),  ignore_monsters, ENT(pev), & tr);

	entvars_t *pevOwner = VARS( pev->owner );
	::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg, UTIL_PointContents(pev->origin)==CONTENTS_WATER?(pev->dmg*6):(pev->dmg*13), CLASS_NONE, DMG_BLIND);
	FX_Explosion( pev->origin+Vector(0,0,30), EXPLOSION_FLASHBANG );

	UTIL_Remove( this );
}

void CFlashBang::BounceTouch( CBaseEntity *pOther )
{
	if (pev->flags & FL_ONGROUND)
	{
		pev->sequence = RANDOM_LONG(2,4);
		pev->velocity = pev->velocity * 0.8;
	}
	else
	{
		switch (RANDOM_LONG(0, 1))
		{
			case 0:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/grenade_hit1.wav", 0.5, ATTN_NORM);	break;
			case 1:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/grenade_hit2.wav", 0.5, ATTN_NORM);	break;
		}
	}
	pev->framerate = pev->velocity.Length()/200.0;
	if (pev->framerate > 1.0)
		pev->framerate = 1;
	else if (pev->framerate < 0.5)
		pev->framerate = 0;
}

void CFlashBang :: TumbleThink( void )
{
	if (!IsInWorld())
	{
		UTIL_Remove( this );
		return;
	}

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


void CFlashBang:: Spawn( void )
{
	pev->movetype = MOVETYPE_BOUNCE;
	pev->classname = MAKE_STRING( "FlashBang" );
	pev->solid = SOLID_BBOX;
	pev->body = 1;
	SET_MODEL(ENT(pev), "models/p_flashbang.mdl");
	UTIL_SetSize(pev, g_vecZero, g_vecZero );
}

CFlashBang * CFlashBang:: ShootFlashbang_projectile( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time )
{
	CFlashBang *pFlashBang = GetClassPtr( (CFlashBang *)NULL );
	pFlashBang->Spawn();
	UTIL_SetOrigin( pFlashBang->pev, vecStart );
	pFlashBang->pev->velocity = vecVelocity;
	pFlashBang->pev->angles = UTIL_VecToAngles(pFlashBang->pev->velocity);
	pFlashBang->pev->owner = ENT(pevOwner);
	
	pFlashBang->SetTouch( BounceTouch );	// Bounce if touched
	
	pFlashBang->pev->dmgtime = gpGlobals->time + time;
	pFlashBang->SetThink( TumbleThink );
	pFlashBang->pev->nextthink = gpGlobals->time + 0.1;
	pFlashBang->pev->framerate = 1.0;
	pFlashBang->pev->gravity = 1;
	pFlashBang->pev->friction = 0.8;
	pFlashBang->pev->sequence = RANDOM_LONG(2,4);
	pFlashBang->pev->scale = 1.5;
	pFlashBang->pev->dmg = dmg_flashbang.value * (mp_wpn_power.value/100);
	return pFlashBang;
}