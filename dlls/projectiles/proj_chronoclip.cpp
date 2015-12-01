#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "decals.h"
#include "shake.h"
#include "game.h"
#include "projectiles.h"

LINK_ENTITY_TO_CLASS( chronoclip, CChronoClip );

void CChronoClip::Explode( TraceResult *pTrace, int bitsDamageType )
{
	entvars_t *pevOwner = VARS( pev->owner );
	RadiusDamage ( pev, pevOwner, pev->dmg, CLASS_NONE, bitsDamageType );
	UTIL_ScreenShake( pev->origin, 16, 1, 2, pev->dmg*3 );
	IrradiateStayTime = pev->dmg/5;

	FX_Explosion( pev->origin + Vector(0,0,40), EXPLOSION_CHRONOCLIP );

	pev->solid = SOLID_NOT;
	pev->takedamage = DAMAGE_NO;
	pev->effects |= EF_NODRAW;
	pev->velocity = g_vecZero;
	SetThink( Irradiate );
	pev->nextthink = gpGlobals->time + 0.3;
}

void CChronoClip::Irradiate( void )
{
	if(IrradiateStayTime <=0 )
		UTIL_Remove( this );
	else
		IrradiateStayTime--;

	entvars_t *pevOwner = VARS( pev->owner );
	::RadiusDamage( pev->origin, pev, pevOwner, IrradiateStayTime/2, pev->dmg/1.5, CLASS_NONE, DMG_RADIATION | DMG_NEVERGIB);
	pev->nextthink = gpGlobals->time + 0.3;
}

void CChronoClip::Detonate( void )
{
	TraceResult tr;
	Vector vecSpot = pev->origin + Vector (0 ,0 ,8);
	Vector vecEnd =  pev->origin + Vector (0 ,0 ,-40);
	UTIL_TraceLine(vecSpot, vecEnd, ignore_monsters, ENT(pev), &tr);
	Explode( &tr, DMG_ENERGYBLAST | DMG_NEVERGIB);

	int tex = (int)TEXTURETYPE_Trace(&tr, vecSpot, vecEnd);
	CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
	FX_ImpRocket( tr.vecEndPos, tr.vecPlaneNormal, pEntity->IsBSPModel()?1:0, BULLET_MEGAEXP, (float)tex );
}

void CChronoClip::BounceTouch( CBaseEntity *pOther )
{
	EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/Chronoclip_bounce.wav", 1, ATTN_NORM);
	Vector vecTestVelocity;
	vecTestVelocity = pev->velocity; 
	vecTestVelocity.z *= 0.45;
	pev->velocity = pev->velocity * 0.8;
}

void CChronoClip :: TumbleThink( void )
{
	if (!IsInWorld())
	{
		UTIL_Remove( this );
		return;
	}

	pev->nextthink = gpGlobals->time + 0.1;

	if (pev->dmgtime <= gpGlobals->time)
	{
		SetThink( Detonate );
	}
	if (pev->waterlevel != 0)
	{
		pev->velocity = pev->velocity * 0.5;
	}
}

void CChronoClip:: Spawn( void )
{
	pev->movetype = MOVETYPE_BOUNCE;
	pev->classname = MAKE_STRING( "chronoclip" );
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	UTIL_SetSize(pev, g_vecZero, g_vecZero );
	pev->body = 9;
}

CChronoClip * CChronoClip:: ShootChronoClip( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time )
{
	CChronoClip *pChronoClip = GetClassPtr( (CChronoClip *)NULL );
	pChronoClip->Spawn();
	UTIL_SetOrigin( pChronoClip->pev, vecStart );
	pChronoClip->pev->velocity = vecVelocity;
	pChronoClip->pev->owner = ENT(pevOwner);
	pChronoClip->SetTouch( BounceTouch );
	pChronoClip->pev->dmgtime = gpGlobals->time + time;
	pChronoClip->SetThink( TumbleThink );
	pChronoClip->pev->nextthink = gpGlobals->time + 0.1;
	pChronoClip->pev->gravity = 1;
	pChronoClip->pev->friction = 0.8;
	int DamageAmount = RANDOM_LONG(dmg_chrono_clip_min.value, dmg_chrono_clip_max.value);
	pChronoClip->pev->dmg = DamageAmount * (mp_wpn_power.value/100);
	return pChronoClip;
}