#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"

enum machinegun_e 
{
	MACHINEGUN_IDLE,
	MACHINEGUN_IDLE2,
	MACHINEGUN_FIRE_LEFT,
	MACHINEGUN_FIRE_RIGHT,
	MACHINEGUN_RELOAD,
	MACHINEGUN_DRAW,
	MACHINEGUN_HOLSTER
};

class CMachinegun : public CBasePlayerWeapon
{
public:
	int AddToPlayer( CBasePlayer *pPlayer );

	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);
	void Reload( void );

	BOOL Deploy( void );
	void Holster( void );

	void PrimaryAttack( void );
	void DoubleAttack( void );
	void WeaponIdle( void );

	void BuyPrimaryAmmo( void );
	void SellWeapon( void );

	void FireLeft( void );
	void FireRight( void );
};
LINK_ENTITY_TO_CLASS( weapon_machinegun, CMachinegun );

int CMachinegun::AddToPlayer ( CBasePlayer *pPlayer )
{
	if ( !pPlayer->m_fHeavyArmor )
		return FALSE;

	return CBasePlayerWeapon::AddToPlayer( pPlayer );
}

void CMachinegun::BuyPrimaryAmmo( void )
{
	BuyAmmo(90, (char*)pszAmmo1(), COST_ENERGY_AMMO);
}

void CMachinegun::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_MACHINEGUN/2;
}

void CMachinegun::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_machinegun.mdl");
	m_iId = WEAPON_MACHINEGUN;
	m_iDefaultAmmo = 180;
	pev->weapons = CLIP_MACHINEGUN;
	FallInit();
}

void CMachinegun::Precache( void )
{
	PRECACHE_MODEL("models/v_machinegun.mdl");
	PRECACHE_MODEL("models/p_machinegun.mdl");
	PRECACHE_SOUND("weapons/machinegun_fire.wav");
}

BOOL CMachinegun::Deploy( void )
{
	m_iChargeLevel = 5;
	return DefaultDeploy( "models/v_machinegun.mdl", "models/p_machinegun.mdl", MACHINEGUN_DRAW, "twohanded", 1.2 );
}

void CMachinegun::Holster( )
{
	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.6;
	m_fInReload = FALSE;
	SendWeaponAnim( MACHINEGUN_HOLSTER );
}

int CMachinegun::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "32mm";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 90;
	p->iSlot = 4;
	p->iPosition = 6;
	p->iId = WEAPON_MACHINEGUN;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iWeight = MACHINEGUN_WEIGHT;
	p->iMaxspeed = MACHINEGUN_MAXSPEED;
	return 1;
}

void CMachinegun::FireLeft()
{
	if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 0)
	{
		PlayEmptySound();
		m_flTimeWeaponIdle = m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + 0.4;
		return;
	}
	m_iClip--;
	m_iFiredAmmo++;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	m_pPlayer->FireMagnumBullets(m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 16 + gpGlobals->v_right * -3 + gpGlobals->v_up * -3, gpGlobals->v_forward, (m_pPlayer->pev->flags & FL_DUCKING)?VECTOR_CONE_4DEGREES:VECTOR_CONE_5DEGREES, 16384, BULLET_32mm, m_pPlayer->pev);

	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), MACHINEGUN_FIRE_LEFT, m_iChargeLevel, FIREGUN_MACHINEGUN );
	m_pPlayer->pev->punchangle.x -= 1.2;

	if (m_iChargeLevel == 1)
		m_flTimeWeaponIdle = m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + 0.4;
	else 
		m_flTimeWeaponIdle = m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + 0.07;
}

void CMachinegun::FireRight()
{
	if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 0)
	{
		PlayEmptySound();
		m_flTimeWeaponIdle = m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + 0.4;
		return;
	}
	m_iClip--;
	m_iFiredAmmo++;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	m_pPlayer->FireMagnumBullets(m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 16 + gpGlobals->v_right * 3 + gpGlobals->v_up * -3, gpGlobals->v_forward, (m_pPlayer->pev->flags & FL_DUCKING)?VECTOR_CONE_4DEGREES:VECTOR_CONE_5DEGREES, 16384, BULLET_32mm, m_pPlayer->pev);
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), MACHINEGUN_FIRE_RIGHT, m_iChargeLevel, FIREGUN_MACHINEGUN );

	m_pPlayer->pev->punchangle.x -= 1.2;
	if (m_iChargeLevel == 4) 
		m_flTimeWeaponIdle = m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + 0.4;
	else 
		m_flTimeWeaponIdle = m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + 0.07;
}

void CMachinegun::DoubleAttack( void )
{
	WeaponIdle();
}

void CMachinegun::PrimaryAttack( void )
{
	switch (m_iChargeLevel)
	{
	case 0:
	case 1:
		FireLeft();
		m_iChargeLevel++;
		break;
	case 2:
	case 3:
	case 4:
		FireRight();
		m_iChargeLevel++;
		break;
	case 5:
		FireLeft();
		m_iChargeLevel= 0;
		break;
	}
}

void CMachinegun::WeaponIdle( void )
{
	if (m_flTimeWeaponIdle > gpGlobals->time)
		return;

	if (m_iChargeLevel != 2 && m_iChargeLevel != 5)
	{
		switch (m_iChargeLevel)
		{
		case 0:
		case 1:
			FireLeft();
			m_iChargeLevel++;
			break;
		case 3:
		case 4:
			FireRight();
			m_iChargeLevel++;
			break;
		}

		return;
	}

	int iAnim;
	switch ( RANDOM_LONG( 0, 1 ) )
	{
	case 0:	
		iAnim = MACHINEGUN_IDLE;	
		break;
	
	default:
	case 1:
		iAnim = MACHINEGUN_IDLE2;
		break;
	}
	SendWeaponAnim( iAnim );
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 10, 15 );
}

void CMachinegun::Reload( void )
{
	DefaultReload( 90, MACHINEGUN_RELOAD, 6.6, 1.2 );
	m_iChargeLevel= 5;
}
