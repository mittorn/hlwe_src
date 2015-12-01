#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "explode.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "shake.h"
#include "gamerules.h"
#include "decals.h"

enum Photongun_e
{
	PHOTONGUN_IDLE1,
	PHOTONGUN_FIRE_ALL,
	PHOTONGUN_FIRE_ALL_SOLID,
	PHOTONGUN_FIRE_BARREL1,
	PHOTONGUN_FIRE_BARREL2,
	PHOTONGUN_FIRE_BARREL3,
	PHOTONGUN_FIRE_BARREL4,
	PHOTONGUN_FIRE_BARREL1_SOLID,
	PHOTONGUN_FIRE_BARREL2_SOLID,
	PHOTONGUN_FIRE_BARREL3_SOLID,
	PHOTONGUN_FIRE_BARREL4_SOLID,
	PHOTONGUN_RELOAD,
	PHOTONGUN_DRAW,
	PHOTONGUN_HOLSTER
};

class CPhotongun : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	void Reload( void );
	int GetItemInfo(ItemInfo *p);

	BOOL Deploy( void );
	void PrimaryAttack( void );
	void SecondaryAttack( void );
	void WeaponIdle( void );
	void Holster( void );

	void StartFire( void );
	void SellWeapon( void );
	void BuyPrimaryAmmo( void );
	BOOL m_fPrimaryFire;
};
LINK_ENTITY_TO_CLASS( weapon_photongun, CPhotongun );

void CPhotongun::BuyPrimaryAmmo( void )
{
	BuyAmmo(60, (char*)pszAmmo1(), COST_ENERGY2_AMMO);
}

void CPhotongun::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_PHOTONGUN/2;
}

void CPhotongun::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_photongun.mdl");
	m_iId = WEAPON_PHOTONGUN;
	m_iDefaultAmmo = 120;
	m_iChargeLevel = 0;
	pev->weapons = CLIP_PHOTONGUN;
	FallInit();
}

void CPhotongun::Precache( void )
{
	PRECACHE_MODEL("models/v_photongun.mdl");
	PRECACHE_MODEL("models/p_photongun.mdl");

	PRECACHE_SOUND("weapons/photongun_fire.wav");
	PRECACHE_SOUND("weapons/photongun_fire2.wav");
	PRECACHE_SOUND("weapons/photon_hitwall.wav");
}

int CPhotongun::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "uranium235";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 60;
	p->iSlot = 6;
	p->iPosition = 4;
	p->iId = WEAPON_PHOTONGUN;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iWeight = PHOTONGUN_WEIGHT;
	p->iMaxspeed = PHOTONGUN_MAXSPEED;
	return 1;
}

BOOL CPhotongun::Deploy( )
{
	return DefaultDeploy( "models/v_photongun.mdl", "models/p_photongun.mdl", PHOTONGUN_DRAW, "minigun", 1.2 );
}

void CPhotongun::Holster( )
{
	m_fInReload = FALSE;
	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.8;
	SendWeaponAnim( PHOTONGUN_HOLSTER );
}

void CPhotongun::StartFire( void )
{
	m_iClip--;
	m_iFiredAmmo++;
	Vector vecSrc = m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 16;

	m_iChargeLevel++;
	switch (m_iChargeLevel)
	{
	case 1:
		m_pPlayer->FireBeam(vecSrc + gpGlobals->v_up * -5 + gpGlobals->v_right * 3.7, gpGlobals->v_forward, m_fPrimaryFire?BEAM_PHOTONGUN:BEAM_PHOTONGUN_EXP, 0, m_pPlayer->pev);
		FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?PHOTONGUN_FIRE_BARREL1_SOLID:PHOTONGUN_FIRE_BARREL1, 0, m_fPrimaryFire?FIREGUN_PHOTONGUN:FIREGUN_PHOTONGUN_EXP);
	break;

	case 2:
		m_pPlayer->FireBeam(vecSrc + gpGlobals->v_up * -5 + gpGlobals->v_right * 2, gpGlobals->v_forward, m_fPrimaryFire?BEAM_PHOTONGUN:BEAM_PHOTONGUN_EXP, 0, m_pPlayer->pev);
		FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?PHOTONGUN_FIRE_BARREL2_SOLID:PHOTONGUN_FIRE_BARREL2, 1, m_fPrimaryFire?FIREGUN_PHOTONGUN:FIREGUN_PHOTONGUN_EXP);
	break;

	case 3:
		m_pPlayer->FireBeam(vecSrc + gpGlobals->v_up * -2.5 + gpGlobals->v_right * 1.8, gpGlobals->v_forward, m_fPrimaryFire?BEAM_PHOTONGUN:BEAM_PHOTONGUN_EXP, 0, m_pPlayer->pev);
		FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?PHOTONGUN_FIRE_BARREL3_SOLID:PHOTONGUN_FIRE_BARREL3, 2, m_fPrimaryFire?FIREGUN_PHOTONGUN:FIREGUN_PHOTONGUN_EXP );
	break;

	case 4:
		m_pPlayer->FireBeam(vecSrc + gpGlobals->v_up * -2.7 + gpGlobals->v_right * 3.7, gpGlobals->v_forward, m_fPrimaryFire?BEAM_PHOTONGUN:BEAM_PHOTONGUN_EXP, 0, m_pPlayer->pev);
		FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?PHOTONGUN_FIRE_BARREL4_SOLID:PHOTONGUN_FIRE_BARREL4, 3, m_fPrimaryFire?FIREGUN_PHOTONGUN:FIREGUN_PHOTONGUN_EXP);
		m_iChargeLevel = 0;
	break;
		m_iChargeLevel = 0;
		break;
	}
}

void CPhotongun::PrimaryAttack()
{
	if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 0)
 	{
		PlayEmptySound(5);
		m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		return;
	}
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	m_fPrimaryFire = TRUE;

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	StartFire();

	if (!m_pPlayer->m_fHeavyArmor)
	m_pPlayer->pev->punchangle.x -= RANDOM_FLOAT(0.5,1.5);
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.09;
	m_flTimeWeaponIdle = gpGlobals->time + 5;
}

void CPhotongun::SecondaryAttack()
{
	if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 0)
 	{
		PlayEmptySound(5);
		m_flNextSecondaryAttack = gpGlobals->time + 0.5;
		return;
	}
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	m_fPrimaryFire = FALSE;

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	StartFire();

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.1;
	m_flTimeWeaponIdle = gpGlobals->time + 2;
	if (!m_pPlayer->m_fHeavyArmor)
	m_pPlayer->pev->punchangle.x -= RANDOM_FLOAT(0.5,1.8);
}

void CPhotongun::WeaponIdle( void )
{
	if (m_flTimeWeaponIdle > gpGlobals->time)
		return;

	SendWeaponAnim( PHOTONGUN_IDLE1 );
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (8, 12);
}

void CPhotongun::Reload( void )
{
	if (m_iClip)
	return;

	DefaultReload( 60, PHOTONGUN_RELOAD, 4.7, 2.1 );
}
