#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"
#include "decals.h"
#include "projectiles.h"

enum teslagun_e
{
	TESLAGUN_IDLE,
	TESLAGUN_IDLE2,
	TESLAGUN_IDLE3,
	TESLAGUN_FIDGET,
	TESLAGUN_FIRE,
	TESLAGUN_FIRE_SOLID,
	TESLAGUN_FIRE_GREN,
	TESLAGUN_FIRE_GREN_SOLID,
	TESLAGUN_RELOAD,
	TESLAGUN_DRAW,
	TESLAGUN_HOLSTER
};

class Cteslagun : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	BOOL Deploy( void );
	void Reload( void );
	void Holster( void );
	void WeaponIdle( void );

	void BuyPrimaryAmmo( void );
	void BuySecondaryAmmo( void );
	void SellWeapon( void );
	int SecondaryAmmoIndex( void );
};
LINK_ENTITY_TO_CLASS( weapon_teslagun, Cteslagun );

void Cteslagun::BuyPrimaryAmmo( void )
{
	BuyAmmo(80, (char*)pszAmmo1(), COST_ENERGY_AMMO);
}

void Cteslagun::BuySecondaryAmmo( void )
{
	BuyAmmo(6, (char*)pszAmmo2(), COST_ASSAULT_AMMO_SEC);
}

void Cteslagun::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_TESLAGUN/2;
}
int Cteslagun::SecondaryAmmoIndex( void )
{
	return m_iSecondaryAmmoType;
}

void Cteslagun::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_teslagun.mdl");
	m_iId = WEAPON_TESLAGUN;
	m_iDefaultAmmo = 160;
	m_iDefaultAmmo2 = 6;
	pev->weapons = CLIP_TESLAGUN;
	FallInit();
}

void Cteslagun::Precache( void )
{
	PRECACHE_MODEL("models/v_teslagun.mdl");
	PRECACHE_MODEL("models/p_teslagun.mdl");

	PRECACHE_SOUND("weapons/tesla_fire.wav");
	PRECACHE_SOUND("weapons/tesla_launcher_fire.wav");
	PRECACHE_SOUND("weapons/shockgrenade_explo.wav");
}

int Cteslagun::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "ShockCore";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = "ShockGrenade";
	p->iMaxAmmo2 = 999;
	p->iMaxClip = 80;
	p->iSlot = 6;
	p->iPosition = 1;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iId = WEAPON_TESLAGUN;
	p->iWeight = TESLAGUN_WEIGHT;
	p->iMaxspeed = TESLAGUN_MAXSPEED;
	return 1;
}

BOOL Cteslagun::Deploy( )
{
	return DefaultDeploy( "models/v_teslagun.mdl", "models/p_teslagun.mdl", TESLAGUN_DRAW, "rifle", 0.8 );
}

void Cteslagun::Holster( )
{
	m_fInReload = FALSE;
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.6;
	SendWeaponAnim( TESLAGUN_HOLSTER );
}

void Cteslagun::PrimaryAttack()
{
	if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 0)
 	{
		PlayEmptySound(3);
		m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		return;
	}
	pev->skin = m_iClip;
	m_iClip--;
	m_iFiredAmmo++;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	m_pPlayer->FireBeam(m_pPlayer->GetGunPosition(), gpGlobals->v_forward, BEAM_TESLAGUN, 0, m_pPlayer->pev );
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?TESLAGUN_FIRE_SOLID:TESLAGUN_FIRE, 0, FIREGUN_TESLAGUN );

	m_flNextPrimaryAttack = gpGlobals->time + 0.075;
	m_flTimeWeaponIdle = gpGlobals->time + 3;
	if (!m_pPlayer->m_fHeavyArmor)
	m_pPlayer->pev->punchangle.x -= 1;
}

void Cteslagun::SecondaryAttack( void )
{
	if (m_pPlayer->pev->waterlevel == 3 || m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] <= 0)
 	{
		PlayEmptySound(5);
		m_flNextSecondaryAttack = gpGlobals->time + 0.5;
		return;
	}
	m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType]--;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	CTeslagren::ShootTeslagren( m_pPlayer->pev, m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 28 + gpGlobals->v_right * 6 + gpGlobals->v_up * -3, gpGlobals->v_forward * 3000 );
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?TESLAGUN_FIRE_GREN_SOLID:TESLAGUN_FIRE_GREN, 1, FIREGUN_TESLAGUN );
	
	(m_pPlayer->m_fHeavyArmor)?m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.7:m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1;
	m_flTimeWeaponIdle = gpGlobals->time + 3;
	if (!m_pPlayer->m_fHeavyArmor)
	m_pPlayer->pev->punchangle.x -= 6;
}

void Cteslagun::Reload( void )
{
	if (m_iClip)
	return;

	DefaultReload( 80, TESLAGUN_RELOAD, 2.3, 0.8 );
}

void Cteslagun::WeaponIdle( void )
{
	pev->skin = m_iClip;

	if (m_flTimeWeaponIdle > gpGlobals->time)
		return;

	switch (RANDOM_LONG(0,3))
	{
		case 0:	SendWeaponAnim( TESLAGUN_IDLE ); break;
		case 1:	SendWeaponAnim( TESLAGUN_IDLE2 ); break;
		case 2:	SendWeaponAnim( TESLAGUN_IDLE3 ); break;
		case 3:	SendWeaponAnim( TESLAGUN_FIDGET ); break;
	}
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 10, 15 );
}
