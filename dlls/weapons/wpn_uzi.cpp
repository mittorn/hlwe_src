#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"

class Cuzi : public CBasePlayerWeapon
{
public:
	int AddDuplicate( CBasePlayerItem *pOriginal );
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

void Cuzi::BuyPrimaryAmmo( void )
{
	BuyAmmo(32, (char*)pszAmmo1(), COST_HANDGUN_AMMO);
}

void Cuzi::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_UZI/2;
}

int Cuzi::AddDuplicate( CBasePlayerItem *pOriginal )
{
	Cuzi *puzi = (Cuzi *)pOriginal;

	if (!puzi->m_pPlayer->HasNamedPlayerItem ("weapon_uzi_akimbo"))
	{
		puzi->m_pPlayer->GiveNamedItem("weapon_uzi_akimbo");
		puzi->m_pPlayer->RemoveNamedPlayerItem("weapon_uzi");
	}
	return CBasePlayerWeapon::AddDuplicate (pOriginal);
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
LINK_ENTITY_TO_CLASS( weapon_uzi, Cuzi );

void Cuzi::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_uzi.mdl");
	m_iId = WEAPON_UZI;
	pev->weapons = CLIP_UZI;
	m_iDefaultAmmo = 64;
	FallInit();
}

void Cuzi::Precache( void )
{
	PRECACHE_MODEL("models/v_uzi.mdl");
	PRECACHE_MODEL("models/p_uzi.mdl");

	PRECACHE_SOUND ("weapons/uzi_fire.wav");
}

int Cuzi::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "8mm";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 32;
	p->iSlot = 1;
	p->iPosition = 5;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iId = WEAPON_UZI;
	p->iWeight = UZI_WEIGHT;
	p->iMaxspeed = UZI_MAXSPEED;
	return 1;
}

BOOL Cuzi::Deploy( )
{
	return DefaultDeploy( "models/v_uzi.mdl", "models/p_uzi.mdl", UZI_DEPLOY, "onehanded", 1 );
}

void Cuzi::Holster( )
{
	m_fInReload = FALSE;
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1;
	SendWeaponAnim( UZI_HOLSTER );
}

void Cuzi::PrimaryAttack()
{
	if (m_pPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = gpGlobals->time+0.5;
		return;
	}

	if (m_iClip <= 0)
	{
		m_flNextPrimaryAttack = gpGlobals->time+0.5;
		PlayEmptySound( );
		return;
	}
	m_iClip --;
	m_iFiredAmmo++;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	m_pPlayer->FireMagnumBullets(m_pPlayer->GetGunPosition(), gpGlobals->v_forward, (m_pPlayer->pev->flags & FL_DUCKING)?VECTOR_CONE_8DEGREES:VECTOR_CONE_9DEGREES, 16384, BULLET_8mm, m_pPlayer->pev);
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?UZI_SHOOT_SOLID:UZI_SHOOT, 0, FIREGUN_UZI );

	if (!m_pPlayer->m_fHeavyArmor)
	m_pPlayer->pev->punchangle.x -= RANDOM_FLOAT(-2,2);

	m_flNextPrimaryAttack = gpGlobals->time + 0.086;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (5, 8);
}

void Cuzi::Reload( void )
{
	DefaultReload(32, UZI_RELOAD, 2.8, 0.8);
}

void Cuzi::WeaponIdle( void )
{
	if ( m_flTimeWeaponIdle > gpGlobals->time )
		return;

	switch (RANDOM_LONG(0,2))
	{
		case 0:	SendWeaponAnim( UZI_IDLE ); break;
		case 1:	SendWeaponAnim( UZI_IDLE2 ); break;
		case 2:	SendWeaponAnim( UZI_IDLE3 ); break;
	}
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (8, 12);
}
