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

class CM16 : public CBasePlayerWeapon
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

int CM16::SecondaryAmmoIndex( void )
{
	return m_iSecondaryAmmoType;
}

void CM16::BuyPrimaryAmmo( void )
{
	BuyAmmo(30, (char*)pszAmmo1(), COST_ASSAULT_AMMO);
}

void CM16::BuySecondaryAmmo( void )
{
	BuyAmmo(2, (char*)pszAmmo2(), COST_ASSAULT_AMMO_SEC);
}

void CM16::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_M16/2;
}

enum M16_e
{
	M16_IDLE1,
	M16_IDLE2,
	M16_SHOOT,
	M16_SHOOT_SOLID,
	M16_GRENADE,
	M16_GRENADE_SOLID,
	M16_RELOAD,
	M16_DRAW,
	M16_HOLSTER
};

LINK_ENTITY_TO_CLASS( weapon_m16, CM16 );

void CM16::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_M16.mdl");
	m_iId = WEAPON_M16;
	m_iDefaultAmmo = 60;
	m_iDefaultAmmo2 = 2;
	pev->weapons = CLIP_M16;
	FallInit();
}

void CM16::Precache( void )
{
	PRECACHE_MODEL("models/v_M16.mdl");
	PRECACHE_MODEL("models/p_M16.mdl");

	PRECACHE_SOUND ("weapons/m16_fire.wav");
	PRECACHE_SOUND( "weapons/m16_glauncher.wav" );
	PRECACHE_SOUND( "weapons/NGgrenade_explode.wav" );
}

int CM16::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "762Nato";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = "NGgrenades";
	p->iMaxAmmo2 = 999;
	p->iMaxClip = 30;
	p->iSlot = 2;
	p->iPosition = 4;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iId = WEAPON_M16;
	p->iWeight = M16_WEIGHT;
	p->iMaxspeed = M16_MAXSPEED;
	return 1;
}

BOOL CM16::Deploy( )
{
	return DefaultDeploy( "models/v_M16.mdl", "models/p_M16.mdl", M16_DRAW, "mp5", 0.8 );
}

void CM16::Holster( )
{
	m_fInReload = FALSE;
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	SendWeaponAnim( M16_HOLSTER );
}

void CM16::PrimaryAttack()
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
	m_pPlayer->FireMagnumBullets(m_pPlayer->GetGunPosition(), gpGlobals->v_forward, (m_pPlayer->pev->flags & FL_DUCKING)?VECTOR_CONE_3DEGREES:VECTOR_CONE_4DEGREES, 16384, BULLET_762Nato, m_pPlayer->pev);
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?M16_SHOOT_SOLID:M16_SHOOT, 0, FIREGUN_M16 );

	m_flNextPrimaryAttack = gpGlobals->time + 0.14;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (10, 15);
	if (!m_pPlayer->m_fHeavyArmor)
	{
		m_pPlayer->pev->punchangle.x -= RANDOM_FLOAT(-1,2.5);
		m_pPlayer->pev->punchangle.y -= 0.3;
	}
}

void CM16::SecondaryAttack( void )
{
	if (m_pPlayer->pev->waterlevel == 3 || m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] <= 0)
 	{
		PlayEmptySound( );
		m_flNextSecondaryAttack = gpGlobals->time + 0.5;
		return;
	}

	m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType]--;
	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	CNGgrenade::ShootNGgrenade( m_pPlayer->pev, m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 24 + gpGlobals->v_right * 6 + gpGlobals->v_up * -7, gpGlobals->v_forward * 2000, 3 );
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?M16_GRENADE_SOLID:M16_GRENADE, 1, FIREGUN_M16 );

	if (!m_pPlayer->m_fHeavyArmor)
	{
		m_pPlayer->pev->punchangle.x -= 16;
		m_pPlayer->pev->punchangle.y -= 5;
	}

	m_flNextSecondaryAttack = gpGlobals->time + 2;
	m_flTimeWeaponIdle = gpGlobals->time + 5;
}

void CM16::Reload( void )
{
	DefaultReload( 30, M16_RELOAD, 2.3, 0.8 );
}


void CM16::WeaponIdle( void )
{
	if ( m_flTimeWeaponIdle > gpGlobals->time )
		return;

	int iAnim;
	switch ( RANDOM_LONG( 0, 1 ) )
	{
	case 0:	
		iAnim = M16_IDLE1;	
		break;
	
	default:
	case 1:
		iAnim = M16_IDLE2;
		break;
	}

	SendWeaponAnim( iAnim );

	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (10, 15);
}
