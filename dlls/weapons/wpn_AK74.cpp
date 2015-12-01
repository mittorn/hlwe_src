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

class CAK74 : public CBasePlayerWeapon
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

	void BuyPrimaryAmmo( void );
	void BuySecondaryAmmo( void );
	void SellWeapon( void );
	int SecondaryAmmoIndex( void );
};

enum AK74_e
{
	AK74_IDLE1,
	AK74_IDLE2,
	AK74_SHOOT,
	AK74_SHOOT_SOLID,
	AK74_GRENADE,
	AK74_GRENADE_SOLID,
	AK74_RELOAD,
	AK74_DRAW,
	AK74_HOLSTER
};

LINK_ENTITY_TO_CLASS( weapon_ak74, CAK74 );

int CAK74::SecondaryAmmoIndex( void )
{
	return m_iSecondaryAmmoType;
}

void CAK74::BuyPrimaryAmmo( void )
{
	BuyAmmo(30, (char*)pszAmmo1(), COST_ASSAULT_AMMO);
}

void CAK74::BuySecondaryAmmo( void )
{
	BuyAmmo(3, (char*)pszAmmo2(), COST_ASSAULT_AMMO_SEC);
}

void CAK74::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_AK74/2;
}

void CAK74::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_AK74.mdl");
	m_iId = WEAPON_AK74;
	m_iDefaultAmmo = 60;
	m_iDefaultAmmo2 = 3;
	pev->weapons = CLIP_AK74;
	FallInit();
}

void CAK74::Precache( void )
{
	PRECACHE_MODEL("models/v_AK74.mdl");
	PRECACHE_MODEL("models/p_AK74.mdl");

	PRECACHE_SOUND("weapons/AK74_fire.wav");
	PRECACHE_SOUND("weapons/AK74_glauncher.wav");
	PRECACHE_SOUND("weapons/ak74gren_explode.wav");
}

int CAK74::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "762";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = "AK74grenades";
	p->iMaxAmmo2 = 999;
	p->iMaxClip = 30;
	p->iSlot = 2;
	p->iPosition = 6;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iId = WEAPON_AK74;
	p->iWeight = AK74_WEIGHT;
	p->iMaxspeed = AK74_MAXSPEED;
	return 1;
}

BOOL CAK74::Deploy( )
{
	return DefaultDeploy( "models/v_AK74.mdl", "models/p_AK74.mdl", AK74_DRAW, "rifle", 1.3 );
}

void CAK74::Holster( )
{
	m_fInReload = FALSE;
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.6;
	SendWeaponAnim( AK74_HOLSTER );
}

void CAK74::PrimaryAttack()
{
	if (m_iClip <= 0)
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		return;
	}

	m_iClip--;
	m_iFiredAmmo++;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	m_pPlayer->FireMagnumBullets(m_pPlayer->GetGunPosition(), gpGlobals->v_forward, (m_pPlayer->pev->flags & FL_DUCKING)?VECTOR_CONE_4DEGREES:VECTOR_CONE_5DEGREES, 16384, BULLET_762, m_pPlayer->pev);
	FX_FireGun( m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?AK74_SHOOT_SOLID:AK74_SHOOT, 0, FIREGUN_AK74 );

	m_flNextPrimaryAttack = gpGlobals->time  + 0.1;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (8, 12);

	if (!m_pPlayer->m_fHeavyArmor)
	{
		m_pPlayer->pev->punchangle.x -= RANDOM_FLOAT(-2,3);
		m_pPlayer->pev->punchangle.y -= 0.5;
	}
}

void CAK74::SecondaryAttack()
{
	if (m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] == 0)
	{
		PlayEmptySound( );
		return;
	}

	m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType]--;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	CAK74grenade::ShootAK74grenade( m_pPlayer->pev, m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 24 + gpGlobals->v_right * 6 + gpGlobals->v_up * -7, gpGlobals->v_forward * 4000 );
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?AK74_GRENADE_SOLID:AK74_GRENADE, 1, FIREGUN_AK74 );
	
	if (!m_pPlayer->m_fHeavyArmor)
	{
		m_pPlayer->pev->punchangle.x -= 14;
		m_pPlayer->pev->punchangle.y -= 6;
	}

	m_flNextPrimaryAttack = gpGlobals->time + 1;
	m_flNextSecondaryAttack = gpGlobals->time + 2;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (5, 8);
}

void CAK74::Reload( void )
{
	DefaultReload( 30, AK74_RELOAD, 2, 0.8 );
}

void CAK74::WeaponIdle( void )
{
	if ( m_flTimeWeaponIdle > gpGlobals->time )
		return;

	int iAnim;
	switch ( RANDOM_LONG( 0, 1 ) )
	{
	case 0:	
		iAnim = AK74_IDLE1;	
		break;
	default:
	case 1:
		iAnim = AK74_IDLE2;
		break;
	}
	SendWeaponAnim( iAnim );
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (8, 12);
}