#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"

class CuziA : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);

	void PrimaryAttack( void );
	BOOL Deploy( void );
	void Holster( void );
	void Reload( void );
	void WeaponIdle( void );

	void BuyPrimaryAmmo( void );
	void SellWeapon( void );
};

void CuziA::BuyPrimaryAmmo( void )
{
	BuyAmmo(64, (char*)pszAmmo1(), COST_ASSAULT_AMMO);
}

void CuziA::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_UZI;
}

enum uzi_e
{
	UZI_IDLE,
	UZI_IDLE2,
	UZI_IDLE3,
	UZI_RELOAD,
	UZI_DEPLOY,
	UZI_SHOOT,
	UZI_SHOOT_SOLID,
	UZI_AKIMBO_OFF,
	UZI_AKIMBO_ON,
	UZI_AKIMBO_IDLE,
	UZI_AKIMBO_RELOAD_BOTH,
	UZI_AKIMBO_FIRE_BOTH,
	UZI_AKIMBO_FIRE_BOTH_SOLID,
	UZI_AKIMBO_DEPLOY,
	UZI_HOLSTER,
	UZI_AKIMBO_HOLSTER
};
LINK_ENTITY_TO_CLASS( weapon_uzi_akimbo, CuziA );

void CuziA::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_uzi_akimbo.mdl");
	m_iId = WEAPON_UZI_AKIMBO;
	pev->weapons = CLIP_UZI_RIGHT;
	m_iDefaultAmmo = 128;
	FallInit();
}

void CuziA::Precache( void )
{
	PRECACHE_MODEL("models/p_uzi_akimbo.mdl");
}

int CuziA::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "8mm";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 64;
	p->iSlot = 1;
	p->iPosition = 6;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iId = WEAPON_UZI_AKIMBO;
	p->iWeight = UZI_AKIMBO_WEIGHT;
	p->iMaxspeed = UZI_AKIMBO_MAXSPEED;
	return 1;
}

BOOL CuziA::Deploy( )
{
	return DefaultDeploy( "models/v_uzi.mdl", "models/p_uzi_akimbo.mdl", UZI_AKIMBO_DEPLOY, "twohanded", 1 );
}

void CuziA::Holster( )
{
	m_fInReload = FALSE;
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1;
	SendWeaponAnim( UZI_AKIMBO_HOLSTER );
}

void CuziA::PrimaryAttack()
{
	if (m_pPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = gpGlobals->time+0.5;
		return;
	}

	if (m_iClip <= 1)
	{
		m_flNextPrimaryAttack = gpGlobals->time+0.5;
		PlayEmptySound( );
		return;
	}

	m_iClip -= 2;
	m_iFiredAmmo += 2;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	for (int i=0; i<2; i++)
		m_pPlayer->FireMagnumBullets(m_pPlayer->GetGunPosition(), gpGlobals->v_forward, (m_pPlayer->pev->flags & FL_DUCKING)?VECTOR_CONE_9DEGREES:VECTOR_CONE_10DEGREES, 16384, BULLET_8mm, m_pPlayer->pev);
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?UZI_AKIMBO_FIRE_BOTH_SOLID:UZI_AKIMBO_FIRE_BOTH, 0, FIREGUN_UZIAKIMBO );

	if (!m_pPlayer->m_fHeavyArmor)
	m_pPlayer->pev->punchangle.x -= RANDOM_FLOAT(-3,2);

	m_flNextPrimaryAttack = gpGlobals->time + 0.086;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (5, 8);
}

void CuziA::Reload( void )
{
	DefaultReload( 64, UZI_AKIMBO_RELOAD_BOTH, 6.6, 1 );
}

void CuziA::WeaponIdle( void )
{
	if ( m_flTimeWeaponIdle > gpGlobals->time )
		return;

	SendWeaponAnim( UZI_AKIMBO_IDLE );
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (8, 12);
}
