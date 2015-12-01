#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "projectiles.h"

enum flashbang_e
{
	FLASHBANG_IDLE = 0,
	FLASHBANG_FIDGET,
	FLASHBANG_PINPULL,
	FLASHBANG_THROW1,
	FLASHBANG_THROW2,
	FLASHBANG_THROW3,
	FLASHBANG_HOLSTER,
	FLASHBANG_DRAW
};

class CFlashbang : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);

	void PrimaryAttack( void );
	BOOL Deploy( void );
	BOOL CanHolster( void );
	void Holster( void );
	void WeaponIdle( void );
	float m_flStartThrow;
	float m_flReleaseThrow;
	void SellWeapon( void );
};
LINK_ENTITY_TO_CLASS( weapon_flashbang, CFlashbang );

void CFlashbang::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] * COST_FLASHBANG/2);
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] = 0;
}

void CFlashbang::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_flashbang.mdl");
	m_iId = WEAPON_FLASHBANG;
	m_iDefaultAmmo = 1;
	FallInit();
}

void CFlashbang::Precache( void )
{
	PRECACHE_MODEL("models/p_flashbang.mdl");
	PRECACHE_SOUND ("weapons/flashbang_explode.wav");
}

int CFlashbang::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "Flashbang";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 7;
	p->iPosition = 0;
	p->iId = WEAPON_FLASHBANG;
	p->iWeight = FLASHBANG_WEIGHT;
	p->iFlags = ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE;
	p->iMaxspeed = FLASHBANG_MAXSPEED;
	return 1;
}


BOOL CFlashbang::Deploy( )
{
	m_flReleaseThrow = -1;
	return DefaultDeploy( "models/v_grenades_all.mdl", "models/p_flashbang.mdl", FLASHBANG_DRAW, "grenade" );
}

BOOL CFlashbang::CanHolster( void )
{
	return ( m_flStartThrow == 0 );
}

void CFlashbang::Holster( )
{
	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.5;
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
		SendWeaponAnim( FLASHBANG_HOLSTER );
	else
	{
		m_pPlayer->m_iWeapons2 &= ~(1<<(WEAPON_FLASHBANG - 32));
		SetThink( DestroyItem );
		pev->nextthink = gpGlobals->time + 0.1;
	}
}

void CFlashbang::PrimaryAttack()
{
	if (!m_flStartThrow && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0)
	{
		m_flStartThrow = gpGlobals->time;
		m_flReleaseThrow = 0;

		SendWeaponAnim( FLASHBANG_PINPULL );
		m_flTimeWeaponIdle = gpGlobals->time + 0.5;
	}
}

void CFlashbang::WeaponIdle( void )
{
	if (m_flReleaseThrow == 0)
		m_flReleaseThrow = gpGlobals->time;

	if (m_flTimeWeaponIdle > gpGlobals->time)
	return;

	if (m_flStartThrow)
	{
		Vector angThrow = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;

		if (angThrow.x < 0)
			angThrow.x = -10 + angThrow.x * ((90 - 10) / 90.0);
		else
			angThrow.x = -10 + angThrow.x * ((90 + 10) / 90.0);

			float flVel = (90 - angThrow.x) * 6;
		if (flVel > 800)
			flVel = 800;

		UTIL_MakeVectors( angThrow );
		Vector vecSrc = m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 16;
		Vector vecThrow = gpGlobals->v_forward * flVel + m_pPlayer->pev->velocity;

		float time = m_flStartThrow - gpGlobals->time + 5.0;
		if (time < 0)
			time = 0;

		CFlashBang::ShootFlashbang_projectile( m_pPlayer->pev, vecSrc, vecThrow, time );
		SendWeaponAnim( FLASHBANG_THROW1 );
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

		m_flStartThrow = 0;
		m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		m_flTimeWeaponIdle = gpGlobals->time + 0.5;

		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;

		if ( !m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] )
		{
			m_flTimeWeaponIdle = m_flNextSecondaryAttack = m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		}
	return;
	}
		else if (m_flReleaseThrow > 0)
		{
			m_flStartThrow = 0;

			if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
				SendWeaponAnim( FLASHBANG_DRAW );
			else
			{
				RetireWeapon();
				return;
			}

			m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 10, 15 );
			m_flReleaseThrow = -1;
			return;
		}

		if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
		{
			int iAnim;
			float flRand = RANDOM_FLOAT(0, 1);

			if (flRand <= 0.75)
			{
				iAnim = FLASHBANG_IDLE;
				m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 18, 24 );
			}
			else
			{
				iAnim = FLASHBANG_FIDGET;
				m_flTimeWeaponIdle = gpGlobals->time + 75.0 / 30.0;
			}

		SendWeaponAnim( iAnim );
	}
}