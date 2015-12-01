#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"

class Cminigun : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);

	void SecondaryAttack( void );
	void PrimaryAttack( void );
	void DoubleAttack( void );
	BOOL Deploy( void );
	void Reload( void );
	void Holster( void );
	void WeaponIdle( void );

	void BuyPrimaryAmmo( void );
	void SellWeapon( void );
};

void Cminigun::BuyPrimaryAmmo( void )
{
	BuyAmmo(120, (char*)pszAmmo1(), COST_HEAVY_AMMO);
}

void Cminigun::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_MINIGUN/2;
}

enum minigun_e
{
	MINIGUN_IDLE1,
	MINIGUN_IDLE2,
	MINIGUN_SPINUP,
	MINIGUN_SPINDOWN,
	MINIGUN_FIRE,
	MINIGUN_FIRE_SOLID,
	MINIGUN_RELOAD,
	MINIGUN_DRAW,
	MINIGUN_HOLSTER,
	MINIGUN_SPIN
};
LINK_ENTITY_TO_CLASS( weapon_minigun, Cminigun );

void Cminigun::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_minigun.mdl");
	m_iId = WEAPON_MINIGUN;
	pev->weapons = CLIP_MINIGUN;
	m_iDefaultAmmo = 240;
	FallInit();
}

void Cminigun::Precache( void )
{
	PRECACHE_MODEL("models/v_minigun.mdl");
	PRECACHE_MODEL("models/p_minigun.mdl");

	PRECACHE_SOUND ("weapons/minigun_fire.wav");
}

int Cminigun::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "86mm";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 120;
	p->iSlot = 4;
	p->iPosition = 1;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iId = WEAPON_MINIGUN;
	p->iWeight = MINIGUN_WEIGHT;
	p->iMaxspeed = MINIGUN_MAXSPEED;
	return 1;
}

BOOL Cminigun::Deploy( )
{
	return DefaultDeploy( "models/v_minigun.mdl", "models/p_minigun.mdl", MINIGUN_DRAW, "minigun", 0.9 );
}

void Cminigun::Holster( )
{
	m_fInReload = FALSE;
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.85;
	SendWeaponAnim( MINIGUN_HOLSTER );
}

void Cminigun::DoubleAttack()
{
	PrimaryAttack();
}

void Cminigun::PrimaryAttack()
{
	if ( m_fInAttack == 0)
	{
		if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 0)
	 	{
			PlayEmptySound( );
			m_flNextPrimaryAttack = gpGlobals->time + 0.5;
			return;
		}

		// spin up
		SendWeaponAnim(MINIGUN_SPINUP);
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
		m_fInAttack = 1;
	}
	else if (m_fInAttack == 1)
	{
		if ( m_iClip <= 0 )
		{
			WeaponIdle();
			return;
		}
		if (m_flTimeWeaponIdle < UTIL_WeaponTimeBase())
			m_fInAttack = 2;
	}
	else if (m_fInAttack == 2)
	{
		if ( m_iClip <= 0 )
		{
			WeaponIdle();
			return;
		}
		if ( m_flTimeUpdate < UTIL_WeaponTimeBase())
		{
			if (!m_pPlayer->m_fHeavyArmor)
			{
				UTIL_MakeVectors (m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
				float flZVel = m_pPlayer->pev->velocity.z;
				m_pPlayer->pev->velocity = m_pPlayer->pev->velocity - gpGlobals->v_forward * 35;
				m_pPlayer->pev->velocity.z = flZVel;
			}
			m_iClip --;
			m_iFiredAmmo++;
			m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

				if ( gpGlobals->time >= m_flShockTime )
				{
					SendWeaponAnim((m_pPlayer->m_fHeavyArmor)?MINIGUN_FIRE_SOLID:MINIGUN_FIRE);
					m_flShockTime = gpGlobals->time + 0.15;
				}

			UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
			m_pPlayer->FireMagnumBullets(m_pPlayer->GetGunPosition(), gpGlobals->v_forward, (m_pPlayer->pev->flags & FL_DUCKING)?VECTOR_CONE_8DEGREES:VECTOR_CONE_9DEGREES, 16384, BULLET_86mm, m_pPlayer->pev);
			FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), 200, m_pPlayer->m_fHeavyArmor?1:0, FIREGUN_MINIGUN );
			m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.05;
		}
	}
}

void Cminigun::SecondaryAttack()
{
	if ( m_fInAttack == 0)
	{
		if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 0)
	 	{
			PlayEmptySound( );
			m_flNextPrimaryAttack = gpGlobals->time + 0.5;
			return;
		}

		// spin up
		SendWeaponAnim(MINIGUN_SPINUP);
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
		m_fInAttack = 1;
	}
	else if (m_fInAttack == 1)
	{
		if ( m_iClip <= 0 )
		{
			WeaponIdle();
			return;
		}
		if (m_flTimeWeaponIdle < UTIL_WeaponTimeBase())
			m_fInAttack = 2;
	}
	if (m_fInAttack == 2)
	{
		if ( m_iClip <= 0 )
		{
			WeaponIdle();
			return;
		}
		if ( m_flTimeUpdate < UTIL_WeaponTimeBase())
		{
			if ( gpGlobals->time >= m_flShockTime )
			{
				SendWeaponAnim(MINIGUN_SPIN);
				m_flShockTime = gpGlobals->time + 0.15;
			}
		}
	}
}

void Cminigun::Reload( void )
{
	DefaultReload( 120, MINIGUN_RELOAD, 5.5, 2.0 );
}

void Cminigun::WeaponIdle( void )
{
	if ( m_flTimeWeaponIdle > gpGlobals->time )
		return;

	if (m_fInAttack!=0)
	{
		SendWeaponAnim(MINIGUN_SPINDOWN);
		m_fInAttack = 0;
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0;
		return;
	}

	int iAnim;
	switch ( RANDOM_LONG( 0, 1 ) )
	{
	case 0:	
		iAnim = MINIGUN_IDLE1;	
		break;
	
	default:
	case 1:
		iAnim = MINIGUN_IDLE2;
		break;
	}
	SendWeaponAnim( iAnim );
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (10, 15);
}
