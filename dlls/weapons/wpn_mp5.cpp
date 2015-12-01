#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"
#include "projectiles.h"

class CMP5 : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	BOOL Deploy( void );
	void Holster( void );
	void Reload( void );
	void WeaponIdle( void );
	int SecondaryAmmoIndex( void );

	void BuyPrimaryAmmo( void );
	void BuySecondaryAmmo( void );
	void SellWeapon( void );
};

enum mp5_e
{
	MP5_IDLE1,
	MP5_IDLE2,
	MP5_SHOOT,
	MP5_SHOOT_SOLID,
	MP5_GRENADE,
	MP5_GRENADE_SOLID,
	MP5_RELOAD,
	MP5_DRAW,
	MP5_HOLSTER
};
LINK_ENTITY_TO_CLASS( weapon_mp5, CMP5 );

int CMP5::SecondaryAmmoIndex( void )
{
	return m_iSecondaryAmmoType;
}

void CMP5::BuyPrimaryAmmo( void )
{
	BuyAmmo(30, (char*)pszAmmo1(), COST_ASSAULT_AMMO);
}

void CMP5::BuySecondaryAmmo( void )
{
	BuyAmmo(2, (char*)pszAmmo2(), COST_ASSAULT_AMMO_SEC);
}

void CMP5::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_MP5/2;
}

void CMP5::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_mp5.mdl");
	m_iId = WEAPON_MP5;
	m_iDefaultAmmo = 60;
	m_iDefaultAmmo2 = 2;
	pev->weapons = CLIP_MP5;
	FallInit();
}

void CMP5::Precache( void )
{
	PRECACHE_MODEL("models/v_mp5.mdl");
	PRECACHE_MODEL("models/p_mp5.mdl");

	PRECACHE_SOUND("weapons/mp5_fire.wav");
	PRECACHE_SOUND("weapons/mp5_glauncher.wav");
	PRECACHE_SOUND("weapons/m203_explode.wav");
}

int CMP5::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "9mm";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = "ARgrenades";
	p->iMaxAmmo2 = 999;
	p->iMaxClip = 30;
	p->iSlot = 2;
	p->iPosition = 3;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iId = WEAPON_MP5;
	p->iWeight = MP5_WEIGHT;
	p->iMaxspeed = MP5_MAXSPEED;
	return 1;
}

BOOL CMP5::Deploy( )
{
	return DefaultDeploy( "models/v_mp5.mdl", "models/p_mp5.mdl", MP5_DRAW, "mp5" );
}

void CMP5::Holster( )
{
	m_fInReload = FALSE;
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	SendWeaponAnim( MP5_HOLSTER );
}

void CMP5::PrimaryAttack()
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
	m_pPlayer->FireMagnumBullets(m_pPlayer->GetGunPosition(), gpGlobals->v_forward, (m_pPlayer->pev->flags & FL_DUCKING)?VECTOR_CONE_5DEGREES:VECTOR_CONE_6DEGREES, 16384, BULLET_MP5, m_pPlayer->pev);
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?MP5_SHOOT_SOLID:MP5_SHOOT, 0, FIREGUN_MP5 );

	m_flNextPrimaryAttack = gpGlobals->time + 0.1;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (10, 15);
	if (!m_pPlayer->m_fHeavyArmor)
	{
		m_pPlayer->pev->punchangle.x -= RANDOM_FLOAT(-1.2,2);
		m_pPlayer->pev->punchangle.y -= RANDOM_FLOAT(0.1,0.15);
	}
}

void CMP5::SecondaryAttack( void )
{
	if (m_pPlayer->pev->waterlevel == 3 || m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] <= 0)
 	{
		PlayEmptySound( );
		m_flNextSecondaryAttack = gpGlobals->time + 0.5;
		return;
	}

	m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType]--;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	CM203grenade::ShootM203grenade( m_pPlayer->pev,	m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 24 + gpGlobals->v_right * 6 + gpGlobals->v_up * -7, gpGlobals->v_forward * 1800);
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?MP5_GRENADE_SOLID:MP5_GRENADE, 1, FIREGUN_MP5 );
	
	m_flNextPrimaryAttack = gpGlobals->time + 1;
	m_flNextSecondaryAttack = gpGlobals->time + 2;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (5, 8);

	if (!m_pPlayer->m_fHeavyArmor)
	{
		m_pPlayer->pev->punchangle.x -= 12;
		m_pPlayer->pev->punchangle.y -= 7;
	}
}

void CMP5::Reload( void )
{
	DefaultReload( 30, MP5_RELOAD, 1.5, 0.65 );
}


void CMP5::WeaponIdle( void )
{
	if ( m_flTimeWeaponIdle > gpGlobals->time )
		return;

	int iAnim;
	switch ( RANDOM_LONG( 0, 1 ) )
	{
	case 0:	
		iAnim = MP5_IDLE1;	
		break;
	
	default:
	case 1:
		iAnim = MP5_IDLE2;
		break;
	}
	SendWeaponAnim( iAnim );
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (10, 15);
}
