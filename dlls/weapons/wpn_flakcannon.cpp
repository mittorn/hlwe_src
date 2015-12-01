#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"
#include "projectiles.h"

enum flakcannon_e
{
	FLAKCANNON_IDLE,
	FLAKCANNON_IDLE_EMPTY,
	FLAKCANNON_FIRE,
	FLAKCANNON_FIRE_LAST,
	FLAKCANNON_FIRE_SOLID,
	FLAKCANNON_FIRE_SOLID_LAST,
	FLAKCANNON_RELOAD,
	FLAKCANNON_RELOAD_EMPTY,
	FLAKCANNON_DRAW,
	FLAKCANNON_DRAW_EMPTY,
	FLAKCANNON_HOLSTER,
	FLAKCANNON_HOLSTER_EMPTY
};

class CFlakcannon : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	void Reload( void );
	int GetItemInfo(ItemInfo *p);

	BOOL Deploy( void );
	void Holster( void );

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	void WeaponIdle( void );

	void BuyPrimaryAmmo( void );
	void SellWeapon( void );
};
LINK_ENTITY_TO_CLASS(weapon_flakcannon, CFlakcannon);

void CFlakcannon::BuyPrimaryAmmo( void )
{
	BuyAmmo(5, (char*)pszAmmo1(), COST_HEAVY_AMMO);
}

void CFlakcannon::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_FLAKCANNON/2;
}

void CFlakcannon::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_flakcannon.mdl");
	m_iId = WEAPON_FLAKCANNON;
	m_iDefaultAmmo = 10;
	pev->weapons = CLIP_FLAKCANNON;
	FallInit();
}

void CFlakcannon::Precache( void )
{
	PRECACHE_MODEL("models/v_flakcannon.mdl");
	PRECACHE_MODEL("models/p_flakcannon.mdl");

	PRECACHE_SOUND("weapons/flakcannon_fire.wav");
	PRECACHE_SOUND("weapons/flakcannon_fire2.wav");
	PRECACHE_SOUND("weapons/explosion_flakbomb.wav");
}

int CFlakcannon::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "shrapnel";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 5;
	p->iSlot = 4;
	p->iPosition = 5;
	p->iId = WEAPON_FLAKCANNON;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iWeight = FLAKCANNON_WEIGHT;
	p->iMaxspeed = FLAKCANNON_MAXSPEED;
	return 1;
}

BOOL CFlakcannon::Deploy( )
{
	return DefaultDeploy( "models/v_flakcannon.mdl", "models/p_flakcannon.mdl", (m_iClip)?FLAKCANNON_DRAW:FLAKCANNON_DRAW_EMPTY, "gauss", 0.9 );
}

void CFlakcannon::Holster( )
{
	m_fInReload = FALSE;
	SendWeaponAnim( (m_iClip)?FLAKCANNON_HOLSTER:FLAKCANNON_HOLSTER_EMPTY );
	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.7;
}

void CFlakcannon::PrimaryAttack()
{
	if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 0)
 	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		return;
	}
	m_iClip--;
	m_iFiredAmmo++;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	for ( int i = 0; i < 10; i++ )
	{
		Vector vecSrc = m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 24 + gpGlobals->v_right * 4 + gpGlobals->v_right * 2 * cos (M_PI * i * 2.0f / 10) + gpGlobals->v_up * -6 + gpGlobals->v_up * 2 * sin (M_PI * i * 2.0f / 10);
		CShrapnel::ShootShrapnel(m_pPlayer->pev, vecSrc, gpGlobals->v_forward * 10000);
	}

	if (m_iClip) 
		FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?FLAKCANNON_FIRE_SOLID:FLAKCANNON_FIRE, 0, FIREGUN_FLAKCANNON );
	else 
		FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?FLAKCANNON_FIRE_SOLID_LAST:FLAKCANNON_FIRE_LAST, 0, FIREGUN_FLAKCANNON );

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 2;
	m_flTimeWeaponIdle = gpGlobals->time + 5;
	if (!m_pPlayer->m_fHeavyArmor)
	{
		m_pPlayer->pev->punchangle.x -= 6;
		m_pPlayer->pev->punchangle.y -= 3;
	}
}

void CFlakcannon::SecondaryAttack()
{
	if (m_iClip <= 0)
	{
		PlayEmptySound();
		m_flNextSecondaryAttack = gpGlobals->time + 0.5;
		return;
	}

	m_iClip--;
	m_iFiredAmmo++;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	CFlakBomb::ShootFlakBomb( m_pPlayer->pev, m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 24 + gpGlobals->v_right * 4 + gpGlobals->v_up * -6, gpGlobals->v_forward * 1800);

	if (m_iClip) 
		FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?FLAKCANNON_FIRE_SOLID:FLAKCANNON_FIRE, 1, FIREGUN_FLAKCANNON );
	else 
		FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?FLAKCANNON_FIRE_SOLID_LAST:FLAKCANNON_FIRE_LAST, 1, FIREGUN_FLAKCANNON );

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 2;
	m_flTimeWeaponIdle = gpGlobals->time + 5;
	if (!m_pPlayer->m_fHeavyArmor)
	{
		m_pPlayer->pev->punchangle.x -= 6;
		m_pPlayer->pev->punchangle.y -= 3;
	}
}

void CFlakcannon::WeaponIdle( void )
{
	if (m_flTimeWeaponIdle > gpGlobals->time)
		return;

	SendWeaponAnim( (m_iClip)?FLAKCANNON_IDLE:FLAKCANNON_IDLE_EMPTY );
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 10, 15 );
}

void CFlakcannon::Reload( void )
{
	if (m_iClip)
		DefaultReload( 5, FLAKCANNON_RELOAD, 4.2, 1.4);
	else
		DefaultReload( 5, FLAKCANNON_RELOAD_EMPTY, 5.6, 1.8);
}