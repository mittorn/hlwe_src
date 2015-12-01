#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "effects.h"
#include "gamerules.h"
#include "projectiles.h"

enum tripmine_e 
{
	TRIPMINE_IDLE1,
	TRIPMINE_IDLE2,
	TRIPMINE_ARM1,
	TRIPMINE_ARM2,
	TRIPMINE_FIDGET,
	TRIPMINE_HOLSTER,
	TRIPMINE_DRAW
};

class CTripmineGrenade : public CTrip
{
	void Spawn( void );

	int RealTakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType );
	
	void EXPORT WarningThink( void );
	void EXPORT PowerupThink( void );
	void EXPORT BeamBreakThink( void );
	void EXPORT DelayDeathThink( void );
	void Killed( entvars_t *pevAttacker, int iGib );

	void MakeBeam( void );
	void KillBeam( void );

	float		m_flPowerUp;
	Vector		m_vecDir;
	Vector		m_vecEnd;
	float		m_flBeamLength;

	EHANDLE		m_hOwner;
	CBeam		*m_pBeam;
	Vector		m_posOwner;
	Vector		m_angleOwner;
	edict_t		*m_pRealOwner;// tracelines don't hit PEV->OWNER, which means a player couldn't detonate his own trip mine, so we store the owner here.
};
LINK_ENTITY_TO_CLASS( monster_tripmine, CTripmineGrenade );

void CTripmineGrenade :: Spawn( void )
{
	Precache( );
	// motor
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_NOT;

	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = 10;
	
	UTIL_SetSize(pev, Vector( -8, -8, -8), Vector(8, 8, 8));
	UTIL_SetOrigin( pev, pev->origin );

	if (pev->spawnflags & 1)
	{
		// power up quickly
		m_flPowerUp = gpGlobals->time + 1.0;
	}
	else
	{
		// power up in 2.5 seconds
		m_flPowerUp = gpGlobals->time + 2.5;
	}

	SetThink( PowerupThink );
	pev->nextthink = gpGlobals->time + 0.2;

	pev->takedamage = DAMAGE_YES;
	pev->dmg = dmg_tripmine.value * (mp_wpn_power.value/100);
	pev->health = 1; // don't let die normally

	if (pev->owner != NULL)
	{
		// play deploy sound
		EMIT_SOUND( ENT(pev), CHAN_VOICE, "weapons/mine_deploy.wav", 1.0, ATTN_NORM );
		EMIT_SOUND( ENT(pev), CHAN_BODY, "weapons/mine_charge.wav", 0.2, ATTN_NORM ); // chargeup

		m_pRealOwner = pev->owner;// see CTripmineGrenade for why.
	}

	UTIL_MakeAimVectors( pev->angles );

	m_vecDir = gpGlobals->v_forward;
	m_vecEnd = pev->origin + m_vecDir * 8192;
}

void CTripmineGrenade :: WarningThink( void  )
{
	// set to power up
	SetThink( PowerupThink );
	pev->nextthink = gpGlobals->time + 1.0;
}


void CTripmineGrenade :: PowerupThink( void  )
{
	TraceResult tr;

	if (m_hOwner == NULL)
	{
		// find an owner
		edict_t *oldowner = pev->owner;
		pev->owner = NULL;
		UTIL_TraceLine( pev->origin + m_vecDir * 8, pev->origin - m_vecDir * 32, dont_ignore_monsters, ENT( pev ), &tr );
		if (tr.fStartSolid || (oldowner && tr.pHit == oldowner))
		{
			pev->owner = oldowner;
			m_flPowerUp += 0.1;
			pev->nextthink = gpGlobals->time + 0.1;
			return;
		}
		if (tr.flFraction < 1.0)
		{
			pev->owner = tr.pHit;
			m_hOwner = CBaseEntity::Instance( pev->owner );
			m_posOwner = m_hOwner->pev->origin;
			m_angleOwner = m_hOwner->pev->angles;
		}
		else
		{
			STOP_SOUND( ENT(pev), CHAN_VOICE, "weapons/mine_deploy.wav" );
			STOP_SOUND( ENT(pev), CHAN_BODY, "weapons/mine_charge.wav" );
			SetThink( SUB_Remove );
			pev->nextthink = gpGlobals->time + 0.1;
			ALERT( at_console, "WARNING:Tripmine at %.0f, %.0f, %.0f removed\n", pev->origin.x, pev->origin.y, pev->origin.z );
			KillBeam();
			return;
		}
	}
	else if (m_posOwner != m_hOwner->pev->origin || m_angleOwner != m_hOwner->pev->angles)
	{
		// disable
		STOP_SOUND( ENT(pev), CHAN_VOICE, "weapons/mine_deploy.wav" );
		STOP_SOUND( ENT(pev), CHAN_BODY, "weapons/mine_charge.wav" );
		CBaseEntity *pMine = Create( "weapon_tripmine", pev->origin + m_vecDir * 24, pev->angles );
		pMine->pev->spawnflags |= SF_NORESPAWN;

		SetThink( SUB_Remove );
		KillBeam();
		pev->nextthink = gpGlobals->time + 0.1;
		return;
	}
	if (gpGlobals->time > m_flPowerUp)
	{
		// make solid
		pev->solid = SOLID_BBOX;
		UTIL_SetOrigin( pev, pev->origin );

		MakeBeam( );

		// play enabled sound
        EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "weapons/mine_activate.wav", 0.5, ATTN_NORM, 1.0, 75 );
	}
	pev->nextthink = gpGlobals->time + 0.1;
}


void CTripmineGrenade :: KillBeam( void )
{
	if ( m_pBeam )
	{
		UTIL_Remove( m_pBeam );
		m_pBeam = NULL;
	}
}


void CTripmineGrenade :: MakeBeam( void )
{
	TraceResult tr;

	UTIL_TraceLine( pev->origin, m_vecEnd, dont_ignore_monsters, ENT( pev ), &tr );

	m_flBeamLength = tr.flFraction;

	// set to follow laser spot
	SetThink( BeamBreakThink );
	pev->nextthink = gpGlobals->time + 0.1;

	Vector vecTmpEnd = pev->origin + m_vecDir * 8192 * m_flBeamLength;

	m_pBeam = CBeam::BeamCreate( g_pModelNameLaser, 10 );
	m_pBeam->PointEntInit( vecTmpEnd, entindex() );
	m_pBeam->SetColor( 200, 0, 0);
	m_pBeam->SetScrollRate(255);
	m_pBeam->SetBrightness(20);
}


void CTripmineGrenade :: BeamBreakThink( void  )
{
	BOOL bBlowup = 0;

	if (m_pBeam)
	{
		m_pBeam->SetColor(200, 0, 0);
		m_pBeam->SetBrightness(20);
	}

	TraceResult tr;

	// HACKHACK Set simple box using this really nice global!
	gpGlobals->trace_flags = FTRACE_SIMPLEBOX;
	UTIL_TraceLine( pev->origin, m_vecEnd, dont_ignore_monsters, ENT( pev ), &tr );

	// respawn detect. 
	if ( !m_pBeam )
	{
		MakeBeam( );
		if ( tr.pHit )
			m_hOwner = CBaseEntity::Instance( tr.pHit );	// reset owner too
	}

	if (fabs( m_flBeamLength - tr.flFraction ) > 0.001)
	{
		bBlowup = 1;
	}
	else
	{
		if (m_hOwner == NULL)
			bBlowup = 1;
		else if (m_posOwner != m_hOwner->pev->origin)
			bBlowup = 1;
		else if (m_angleOwner != m_hOwner->pev->angles)
			bBlowup = 1;
	}

	CBaseEntity *pTarget = NULL,
				*pPlayer = NULL;

	if (!FNullEnt(m_pRealOwner) && !FNullEnt(tr.pHit))
	{
		pPlayer = CBaseEntity::Instance(m_pRealOwner);
		pTarget = CBaseEntity::Instance(tr.pHit);

		if (g_pGameRules->PlayerRelationship(pPlayer, pTarget) == GR_TEAMMATE)
		{
			if (m_pBeam)
			{
				m_pBeam->SetColor(0, 255, 0);
				m_pBeam->SetBrightness(50);
			}

			pev->nextthink = gpGlobals->time + 0.1;
			return;
		}
	}

	if (bBlowup)
	{
		pev->owner = m_pRealOwner;
		pev->health = 0;
		Killed( VARS( pev->owner ), GIB_NORMAL );
		return;
	}

	pev->nextthink = gpGlobals->time + 0.1;
}

int CTripmineGrenade :: RealTakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType )
{
	if ( bitsDamageType & (DMG_RADIATION | DMG_BLIND | DMG_NERVEGAS | DMG_DROWN) )
		flDamage = 0;

	if (gpGlobals->time < m_flPowerUp && flDamage < pev->health)
	{
		// disable
		SetThink( SUB_Remove );
		pev->nextthink = gpGlobals->time + 0.1;
		KillBeam();
		return FALSE;
	}
	return CTrip::RealTakeDamage( pevInflictor, pevAttacker, flDamage, bitsDamageType );
}

void CTripmineGrenade::Killed( entvars_t *pevAttacker, int iGib )
{
	pev->takedamage = DAMAGE_NO;
	
	if ( pevAttacker && ( pevAttacker->flags & FL_CLIENT ) )
	{
		// some client has destroyed this mine, he'll get credit for any kills
		pev->owner = ENT( pevAttacker );
	}

	SetThink( DelayDeathThink );
	pev->nextthink = gpGlobals->time + RANDOM_FLOAT( 0.1, 0.3 );
	EMIT_SOUND( ENT(pev), CHAN_BODY, "common/null.wav", 0.5, ATTN_NORM ); // shut off chargeup
}


void CTripmineGrenade::DelayDeathThink( void )
{
	KillBeam();
	TraceResult tr;
	Vector vecSpot = pev->origin + m_vecDir * 8;
	Vector vecEnd = pev->origin - m_vecDir * 64;
	UTIL_TraceLine (vecSpot, vecEnd, dont_ignore_monsters, ENT(pev), &tr);

	int tex = (int)TEXTURETYPE_Trace(&tr, vecSpot, vecEnd);
	int pc = UTIL_PointContents(tr.vecEndPos);
	CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
	FX_ImpRocket( tr.vecEndPos, tr.vecPlaneNormal, pEntity->IsBSPModel()?1:0, BULLET_SMALEXP, (float)tex );
	Explode( &tr, DMG_BLAST );
}

class CTripmine : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);
	void SetObjectCollisionBox( void )
	{
		//!!!BUGBUG - fix the model!
		pev->absmin = pev->origin + Vector(-16, -16, -5);
		pev->absmax = pev->origin + Vector(16, 16, 28); 
	}

	void PrimaryAttack( void );
	BOOL Deploy( void );
	void Holster( void );
	void WeaponIdle( void );
	void SellWeapon( void );
};
LINK_ENTITY_TO_CLASS( weapon_tripmine, CTripmine );

void CTripmine::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] * COST_TRIPMINE/2);
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] = 0;
}

void CTripmine::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_tripmine.mdl");
	m_iId = WEAPON_TRIPMINE;
	FallInit();
	m_iDefaultAmmo = 1;
}

void CTripmine::Precache( void )
{
	PRECACHE_SOUND("weapons/mine_deploy.wav");
	PRECACHE_SOUND("weapons/mine_activate.wav");
	PRECACHE_SOUND("weapons/mine_charge.wav");

	PRECACHE_MODEL("models/v_tripmine.mdl");
	PRECACHE_MODEL("models/p_tripmine.mdl");
	UTIL_PrecacheOther( "monster_tripmine" );
}

int CTripmine::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "Trip Mine";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 7;
	p->iPosition = 2;
	p->iId = m_iId = WEAPON_TRIPMINE;
	p->iWeight = TRIPMINE_WEIGHT;
	p->iFlags = ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE;
	p->iMaxspeed = TRIPMINE_MAXSPEED;
	return 1;
}

BOOL CTripmine::Deploy( )
{
	return DefaultDeploy( "models/v_tripmine.mdl", "models/p_tripmine.mdl", TRIPMINE_DRAW, "trip" );
}

void CTripmine::Holster( )
{
	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.5;

	if (!m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
	{
		// out of mines
		m_pPlayer->m_iWeapons2 &= ~(1<<(WEAPON_TRIPMINE - 32));
		SetThink( DestroyItem );
		pev->nextthink = gpGlobals->time + 0.1;
	}

	SendWeaponAnim( TRIPMINE_HOLSTER );
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "common/null.wav", 1.0, ATTN_NORM);
}

void CTripmine::PrimaryAttack( void )
{
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		return;

	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );
	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming = gpGlobals->v_forward;

	TraceResult tr;

	UTIL_TraceLine( vecSrc, vecSrc + vecAiming * 128, dont_ignore_monsters, ENT( m_pPlayer->pev ), &tr );

	if (tr.flFraction < 1.0)
	{

		CBaseEntity *pEntity = CBaseEntity::Instance( tr.pHit );
		if (pEntity && !(pEntity->pev->flags & FL_CONVEYOR))
		{
			Vector angles = UTIL_VecToAngles( tr.vecPlaneNormal );

			CBaseEntity *pEnt = CBaseEntity::Create( "monster_tripmine", tr.vecEndPos + tr.vecPlaneNormal * 8, angles, m_pPlayer->edict() );

			CTripmineGrenade *pMine = (CTripmineGrenade *)pEnt;

			m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;

			// player "shoot" animation
			m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

			if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0)
			{
				SendWeaponAnim( TRIPMINE_DRAW );
			}
			else
			{
				// no more mines! 
				RetireWeapon();
				return;
			}
		}
		else
		{
		}
	}
	else
	{

	}

	m_flNextPrimaryAttack = gpGlobals->time + 0.3;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 10, 15 );
}

void CTripmine::WeaponIdle( void )
{
	if (m_flTimeWeaponIdle > gpGlobals->time)
		return;

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0)
	{
		SendWeaponAnim( TRIPMINE_DRAW );
	}
	else
	{
		RetireWeapon(); 
		return;
	}

	int iAnim;
	float flRand = RANDOM_FLOAT(0, 1);
	if (flRand <= 0.25)
	{
		iAnim = TRIPMINE_IDLE1;
		m_flTimeWeaponIdle = gpGlobals->time + 90.0 / 30.0;
	}
	else if (flRand <= 0.75)
	{
		iAnim = TRIPMINE_IDLE2;
		m_flTimeWeaponIdle = gpGlobals->time + 60.0 / 30.0;
	}
	else
	{
		iAnim = TRIPMINE_FIDGET;
		m_flTimeWeaponIdle = gpGlobals->time + 100.0 / 30.0;
	}

	SendWeaponAnim( iAnim );
	
}