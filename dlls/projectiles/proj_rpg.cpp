#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "decals.h"
#include "explode.h"
#include "game.h"
#include "projectiles.h"

LINK_ENTITY_TO_CLASS( rrp, CRpgRocket );

void CRpgRocket::Killed (entvars_t *pevAttacker, int iGib)
{
	if (m_pLauncher)
		m_pLauncher->m_cActiveRockets--;

	STOP_SOUND( edict(), CHAN_ITEM, "weapons/rocket1.wav" );
	FX_Trail( pev->origin, entindex(), PROJ_REMOVE );

	UTIL_Remove (this);
}

void CRpgRocket::ExplodeTouch( CBaseEntity *pOther )
{
	if (m_pLauncher)
		m_pLauncher->m_cActiveRockets--;

	STOP_SOUND( edict(), CHAN_ITEM, "weapons/rocket1.wav" );
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
	FX_Trail( tr.vecEndPos + (tr.vecPlaneNormal * 15), entindex(), (UTIL_PointContents(pev->origin) == CONTENT_WATER)?PROJ_RPGROCKET_DETONATE_WATER:PROJ_RPGROCKET_DETONATE);

	int tex = (int)TEXTURETYPE_Trace(&tr, vecSpot, vecEnd);
	CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
	FX_ImpRocket( tr.vecEndPos, tr.vecPlaneNormal, pEntity->IsBSPModel()?1:0, BULLET_NORMEXP, (float)tex );

	//Direct hit does additional damage
	if (pOther->pev->takedamage)
	{
		entvars_t *pevOwner = VARS( pev->owner );
		ClearMultiDamage();
		pOther->TraceAttack(pevOwner, pev->dmg/3, gpGlobals->v_forward, &tr, DMG_SLASH );
		ApplyMultiDamage(pev, pevOwner);
	} 
	UTIL_Remove( this );
}

void CRpgRocket:: Spawn( void )
{
	pev->movetype = MOVETYPE_FLY;
	pev->classname = MAKE_STRING( "rrp" );
	pev->solid = SOLID_BBOX;
	pev->effects |= EF_LIGHT;

	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	UTIL_SetSize(pev, g_vecZero, g_vecZero );
	pev->body = 0;
}

CRpgRocket *CRpgRocket::ShootRpgRocket( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, BOOL LaserFollow, CRpg *pLauncher )
{
	CRpgRocket *pRpgRocket = GetClassPtr( (CRpgRocket *)NULL );
	pRpgRocket->Spawn();

	UTIL_SetOrigin( pRpgRocket->pev, vecStart );
	pRpgRocket->pev->velocity = vecVelocity;
	pRpgRocket->pev->angles = UTIL_VecToAngles (pRpgRocket->pev->velocity);
	pRpgRocket->pev->owner = ENT(pevOwner);
	pRpgRocket->m_pLauncher = pLauncher; 
	FX_Trail(pRpgRocket->pev->origin, pRpgRocket->entindex(), PROJ_RPGROCKET );

	if (pLauncher)
		pRpgRocket->m_pLauncher->m_cActiveRockets++;

	if (LaserFollow)
		pRpgRocket->SetThink ( IgniteFollow );
	else
		pRpgRocket->SetThink ( Ignite );

	pRpgRocket->pev->nextthink = 0.1;
	pRpgRocket->SetTouch( ExplodeTouch );
	pRpgRocket->pev->dmg = dmg_rpg.value * (mp_wpn_power.value/100);
	return pRpgRocket;
}

void CRpgRocket :: Ignite( void  )
{
	EMIT_SOUND( ENT(pev), CHAN_ITEM, "weapons/rocket1.wav", 1, ATTN_LOW_HIGH);
}

void CRpgRocket:: IgniteFollow( void  )
{
	m_flIgniteTime = gpGlobals->time;
	EMIT_SOUND( ENT(pev), CHAN_ITEM, "weapons/rocket1.wav", 1, ATTN_LOW_HIGH);
	SetThink ( Follow );
	pev->nextthink = gpGlobals->time + 0.1;
}

void CRpgRocket :: Follow( void )
{
	CBaseEntity *pOther = NULL;
	Vector vecTarget, vecDir;
	float flDist, flMax, flDot;
	TraceResult tr;

	UTIL_MakeAimVectors( pev->angles );

	vecTarget = gpGlobals->v_forward;
	flMax = 8192;
	
	while ((pOther = UTIL_FindEntityByClassname( pOther, "laser_spot" )) != NULL)
	{
		UTIL_TraceLine ( pev->origin, pOther->pev->origin, dont_ignore_monsters, ENT(pev), &tr );
		if (tr.flFraction >= 0.90)
		{
			vecDir = pOther->pev->origin - pev->origin;
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

	pev->angles = UTIL_VecToAngles( vecTarget );
	pev->velocity = gpGlobals->v_forward * 1500;
	pev->nextthink = gpGlobals->time + 0.1;
}