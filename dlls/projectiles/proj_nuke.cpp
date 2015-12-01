#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "explode.h"
#include "shake.h"
#include "game.h"
#include "projectiles.h"
#include "player.h"

LINK_ENTITY_TO_CLASS( nuke, CNuke );

void CNuke::Killed (entvars_t *pevAttacker, int iGib)
{
	entvars_t *pevOwner = VARS( pev->owner );
	SET_VIEW( pev->owner, pev->owner );
	STOP_SOUND( edict(), CHAN_VOICE, "weapons/redeemer_WH_fly.wav" );

	MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, ENT(pevOwner) );
	WRITE_BYTE( MSG_WARHEAD );
	WRITE_BYTE( 0 );
	WRITE_ELSE(1+1);
	MESSAGE_END();

	FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
	UTIL_Remove( this );
}

void CNuke::Explode( TraceResult *pTrace, int bitsDamageType )
{
	UTIL_ScreenShake( pev->origin, 80, 8, 5, pev->dmg*2.5 );
	entvars_t *pevOwner = VARS( pev->owner );
	::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg, pev->dmg*1.2, CLASS_NONE, DMG_NUKE | DMG_NEVERGIB);
	::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg/20, pev->dmg, CLASS_NONE, DMG_BLIND );
	FX_Trail( pTrace->vecEndPos + (pTrace->vecPlaneNormal * 30), entindex(), (UTIL_PointContents(pev->origin) == CONTENT_WATER)?PROJ_NUKE_DETONATE_WATER:PROJ_NUKE_DETONATE );

	pev->effects |= EF_NODRAW;
	pev->velocity = g_vecZero;
	SetTouch(NULL);
	SetThink( Irradiate );
	pev->nextthink = gpGlobals->time + 0.3;
}

void CNuke::Irradiate( void )
{
	if(RadiationStayTime <=0 )
		UTIL_Remove( this );
	else
		RadiationStayTime--;

	entvars_t *pevOwner = VARS( pev->owner );
	::RadiusDamage( pev->origin, pev, pevOwner, RadiationStayTime/2, pev->dmg, CLASS_MACHINE, DMG_RADIATION | DMG_NEVERGIB);
	pev->nextthink = gpGlobals->time + 0.3;
}

void CNuke::ExplodeTouch( CBaseEntity *pOther )
{
	entvars_t *pevOwner = VARS( pev->owner );
	SET_VIEW( pev->owner, pev->owner );
	STOP_SOUND( edict(), CHAN_VOICE, "weapons/redeemer_WH_fly.wav" );

	MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, ENT(pevOwner) );
	WRITE_BYTE( MSG_WARHEAD );
	WRITE_BYTE( 0 );
	WRITE_ELSE(1+1);
	MESSAGE_END();

	if ( UTIL_PointContents(pev->origin) == CONTENT_SKY )
	{
		FX_Trail( pev->origin, entindex(), PROJ_REMOVE );
		UTIL_Remove( this );
		return;
	}
	RadiationStayTime = pev->dmg/15;

	TraceResult tr;
	Vector vecSpot = pev->origin - pev->velocity.Normalize() * 32;
	Vector vecEnd = pev->origin + pev->velocity.Normalize() * 64;
	UTIL_TraceLine( vecSpot, vecEnd, ignore_monsters, ENT(pev), &tr );

	Explode( &tr, DMG_NUKE );

	int tex = (int)TEXTURETYPE_Trace(&tr, vecSpot, vecEnd);
	CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
	FX_ImpRocket( tr.vecEndPos, tr.vecPlaneNormal, pEntity->IsBSPModel()?1:0, BULLET_MEGAEXP, (float)tex );

	if (pOther->pev->takedamage)
	{
		ClearMultiDamage( );
		pOther->TraceAttack(pevOwner, pev->dmg/8, gpGlobals->v_forward, &tr, DMG_BULLETMAGNUM ); 
		ApplyMultiDamage( pev, pevOwner);
	}
}

void CNuke:: Spawn( void )
{
	pev->movetype = MOVETYPE_FLY;
	pev->classname = MAKE_STRING( "nuke" );
	pev->solid = SOLID_BBOX;
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = 15;

	m_yawCenter = pev->angles.y;
	m_pitchCenter = pev->angles.x;

	UTIL_SetSize(pev, g_vecZero, g_vecZero );
}

CNuke *CNuke::ShootNuke( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, BOOL Camera )
{
	CNuke *pNuke = GetClassPtr( (CNuke *)NULL );
	pNuke->Spawn();

	UTIL_SetOrigin( pNuke->pev, vecStart );
	pNuke->pev->velocity = vecVelocity;
	pNuke->pev->angles = UTIL_VecToAngles (pNuke->pev->velocity);
	pNuke->pev->owner = ENT(pevOwner);
	pNuke->SetTouch( ExplodeTouch );
	pNuke->pev->dmg = dmg_redeemer.value * (mp_wpn_power.value/100);
	FX_Trail(pNuke->pev->origin, pNuke->entindex(), PROJ_NUKE );

	if(Camera)
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, ENT(pevOwner));
		WRITE_BYTE( MSG_WARHEAD );
		WRITE_BYTE( 1 );
		WRITE_ELSE(1+1);
		MESSAGE_END();

		SET_VIEW( ENT(pevOwner), pNuke->edict() );
		pNuke->pev->angles.x = -pNuke->pev->angles.x;

		pNuke->SetThink ( IgniteFollow );
		pNuke->pev->nextthink = 0.1;
	}
	else
	{
		pNuke->SetThink ( Ignite );
		pNuke->pev->nextthink = 0.1;
	}
	return pNuke;
}

void CNuke :: Ignite( void  )
{
	EMIT_SOUND( ENT(pev), CHAN_VOICE, "weapons/redeemer_WH_fly.wav", 1, ATTN_LOW_HIGH);
}

void CNuke :: IgniteFollow( void  )
{
	EMIT_SOUND( ENT(pev), CHAN_VOICE, "weapons/redeemer_WH_fly.wav", 1, ATTN_LOW_HIGH);
	SetThink ( Follow );
	pev->nextthink = gpGlobals->time + 0.1;
}

void CNuke :: Follow( void )
{
	entvars_t *pevOwner = VARS( pev->owner );
	Vector angles;
	Vector velocity;

	angles = pevOwner->v_angle;
	angles[0] = 0 - angles[0];
	angles.x = -angles.x;
	angles.y = m_yawCenter + UTIL_AngleDistance( angles.y, m_yawCenter );
	angles.x = m_pitchCenter + UTIL_AngleDistance( angles.x, m_pitchCenter );

	float distY = UTIL_AngleDistance( angles.y, pev->angles.y );
	pev->avelocity.y = distY * 3;
	float distX = UTIL_AngleDistance( angles.x, pev->angles.x );
	pev->avelocity.x = distX  * 3;

	UTIL_MakeVectors(pev->angles);
	pev->velocity = gpGlobals->v_forward * 800;
	pev->nextthink = gpGlobals->time + 0.1;
}