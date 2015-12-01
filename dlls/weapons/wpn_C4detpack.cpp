#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "projectiles.h"

enum c4_e
{
	C4_IDLE,
	C4_LONGIDLE,
	C4_SET_DET,
	C4_SET_DET_5SEC,
	C4_HOLSTER,
	C4_DRAW
};

class Cc4 : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);

	BOOL Deploy( void );
	void Holster( void );

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	void WeaponIdle( void );

	void EXPORT ThrowDetpack( void );
	void EXPORT ThrowDetpack5Sec( void );
	void SellWeapon( void );
};
LINK_ENTITY_TO_CLASS( weapon_c4, Cc4 );

void Cc4::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] * COST_C4/2);
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] = 0;
}

void Cc4::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_c4.mdl");
	m_iId = WEAPON_C4;
	m_iDefaultAmmo = 1;
	FallInit();
}

BOOL Cc4::Deploy( )
{
	return DefaultDeploy( "models/v_c4.mdl", "models/p_c4.mdl", C4_DRAW, "trip" );
}

void Cc4::Holster( )
{
	m_fInAttack = 0;

	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.8;
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
		SendWeaponAnim( C4_HOLSTER );
	else
	{
		m_pPlayer->m_iWeapons2 &= ~(1<<(WEAPON_C4 - 32));
		SetThink( DestroyItem );
		pev->nextthink = gpGlobals->time + 0.1;
	}
}

int Cc4::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "C4";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 7;
	p->iPosition = 4;
	p->iId = WEAPON_C4;
	p->iWeight = C4_WEIGHT;
	p->iMaxspeed = C4_MAXSPEED;
	p->iFlags = 0;
	return 1;
}

void Cc4::Precache( void )
{
	PRECACHE_MODEL("models/v_c4.mdl");
	PRECACHE_MODEL("models/p_c4.mdl");

	PRECACHE_SOUND ("weapons/C4_drop.wav");
	PRECACHE_SOUND ("weapons/C4_arm.wav");
	PRECACHE_SOUND( "weapons/c4_explode.wav" );
}

void Cc4::ThrowDetpack()
{
	if ( m_fInAttack == 0 )
	{
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1;
		return;
	}

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	Vector vecSrc = m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 20 + gpGlobals->v_right * 2.5 + gpGlobals->v_up * -15;
	Vector vecThrow = gpGlobals->v_forward*5;
	CDetpack::ShootTimedDetpack( m_pPlayer->pev, vecSrc, vecThrow, 45 );

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
		SendWeaponAnim( C4_DRAW );
	else
		RetireWeapon();
}

void Cc4::ThrowDetpack5Sec()
{
	if ( m_fInAttack == 0 )
	{
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1;
		return;
	}

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	Vector vecSrc = m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 20 + gpGlobals->v_right * 2.5 + gpGlobals->v_up * -15;
	Vector vecThrow = gpGlobals->v_forward*5;
	CDetpack::ShootTimedDetpack( m_pPlayer->pev, vecSrc, vecThrow, 5 );
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
		SendWeaponAnim( C4_DRAW );
	else
		RetireWeapon();
}

void Cc4::PrimaryAttack( void )
{
	if ( m_fInAttack == 0)
	{
		if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] == 0)
		{
			Holster( );
			return;
		}

		UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

		TraceResult tr;
		UTIL_TraceLine (m_pPlayer->GetGunPosition(), m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 68, dont_ignore_monsters, ENT(m_pPlayer->pev), &tr);

		if (tr.flFraction < 1.0)
		{
			ClientPrint (m_pPlayer->pev, HUD_PRINTCENTER, "Can't plant C4 here!\n");
			return;
		}

		m_fInAttack = 1;
		SendWeaponAnim( C4_SET_DET );
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.8;
	}
	else if (m_fInAttack == 1)
	{
		if (m_flTimeWeaponIdle < UTIL_WeaponTimeBase())
		{
			WeaponIdle();
			return;
		}

	}
}

void Cc4::SecondaryAttack( void )
{
	if ( m_fInAttack == 0)
	{
		if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] == 0)
		{
			Holster( );
			return;
		}

		UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

		TraceResult tr;

		UTIL_TraceLine (m_pPlayer->GetGunPosition(), m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 68, dont_ignore_monsters, ENT(m_pPlayer->pev), &tr);

		if (tr.flFraction < 1.0)
		{
			ClientPrint (m_pPlayer->pev, HUD_PRINTCENTER, "Can't plant C4 here!\n");
			return;
		}

		m_fInAttack = 2;
		SendWeaponAnim( C4_SET_DET_5SEC );
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.8;
	}
	else if (m_fInAttack == 2)
	{
		if (m_flTimeWeaponIdle < UTIL_WeaponTimeBase())
		{
			WeaponIdle();
			return;
		}

	}
}

void Cc4::WeaponIdle( void )
{
	if ( m_flTimeWeaponIdle > gpGlobals->time )
		return;

	if (m_fInAttack==1)
	{
		ThrowDetpack();
		m_fInAttack = 0;
		return;
	}
	else if (m_fInAttack==2)
	{
		ThrowDetpack5Sec();
		m_fInAttack = 0;
		return;
	}

	int iAnim;
	float flRand = RANDOM_FLOAT(0,1);

	if ( flRand <= 0.5 )
	{
		iAnim = C4_IDLE;
	}
	else 
	{
		iAnim = C4_LONGIDLE;
	}
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT(10,15);

	SendWeaponAnim( iAnim );
}