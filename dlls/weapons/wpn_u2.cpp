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

class Cu2 : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	void SwitchFireMode( void );
	BOOL Deploy( void );
	void Holster( void );
	void Reload( void );
	void WeaponIdle( void );

	void BuyPrimaryAmmo( void );
	void BuySecondaryAmmo( void );

	void ZoomMode( void );
	void GrenadeLauncher( void );
	void FireBurst( void );
	void ZoomReset( void );
	void SellWeapon( void );
	int SecondaryAmmoIndex( void );
};

int Cu2::SecondaryAmmoIndex( void )
{
	return m_iSecondaryAmmoType;
}

void Cu2::BuyPrimaryAmmo( void )
{
	BuyAmmo(30, (char*)pszAmmo1(), COST_SNIPER_AMMO);
}

void Cu2::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_U2/2;
}

void Cu2::BuySecondaryAmmo( void )
{
	BuyAmmo(2, (char*)pszAmmo2(), COST_ASSAULT_AMMO_SEC);
}

enum u2_e
{
	U2_IDLE,
	U2_IDLE2,
	U2_FIRE,
	U2_FIRE_SOLID,
	U2_FIRE_GRENADE,
	U2_FIRE_GRENADE_SOLID,
	U2_RELOAD,
	U2_DRAW,
	U2_HOLSTER
};
LINK_ENTITY_TO_CLASS( weapon_u2, Cu2 );

void Cu2::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_u2.mdl");
	m_iId = WEAPON_U2;
	firemode=FIREMODE_SCOPE;
	m_iDefaultAmmo = 60;
	m_iDefaultAmmo2 = 2;
	pev->weapons = CLIP_U2;
	FallInit();
}

void Cu2::Precache( void )
{
	PRECACHE_MODEL("models/v_u2.mdl");
	PRECACHE_MODEL("models/p_u2.mdl");

	PRECACHE_SOUND ("weapons/u2_fire.wav");
	PRECACHE_SOUND ("weapons/u2_glauncher.wav");
}

int Cu2::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "556";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = "U2grenades";
	p->iMaxAmmo2 = 999;
	p->iMaxClip = 30;
	p->iSlot = 3;
	p->iPosition = 1;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iId = WEAPON_U2;
	p->iWeight = U2_WEIGHT;
	p->iMaxspeed = U2_MAXSPEED;
	return 1;
}

BOOL Cu2::Deploy( )
{
	m_iChargeLevel = 2;
	m_iLastFiremode = -1;
	return DefaultDeploy( "models/v_u2.mdl", "models/p_u2.mdl", U2_DRAW, "rifle" );
}

void Cu2::PrimaryAttack()
{
	switch (m_iChargeLevel)
	{
	case 0:
	case 1:
		FireBurst();
		m_iChargeLevel++;
		break;
	case 2:
		FireBurst();
		m_iChargeLevel = 0;
		break;
	}
}

void Cu2::FireBurst()
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
	m_pPlayer->FireMagnumBullets(m_pPlayer->GetGunPosition(), gpGlobals->v_forward, (m_pPlayer->pev->flags & FL_DUCKING)?VECTOR_CONE_1DEGREES:VECTOR_CONE_2DEGREES, 16384, BULLET_556, m_pPlayer->pev);
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?U2_FIRE_SOLID:U2_FIRE, 0, FIREGUN_U2 );

	if (m_iChargeLevel == 1)
		m_flTimeWeaponIdle = m_flNextPrimaryAttack = gpGlobals->time + 0.3;
	else 
		m_flTimeWeaponIdle = m_flNextPrimaryAttack = gpGlobals->time + 0.095;

	if (!m_pPlayer->m_fHeavyArmor)
	{
		m_pPlayer->pev->punchangle.x -= 1.1;
		m_pPlayer->pev->punchangle.y -= 0.3;
	}
}

void Cu2::SecondaryAttack()
{
	if(firemode==FIREMODE_SCOPE)
		ZoomMode();
	if(firemode==FIREMODE_GLAUNCHER)
		GrenadeLauncher();
}

void Cu2::ZoomMode( void )
{
	if (m_fInZoom == 0) 
	{
		m_pPlayer->m_iFOV = 35;
		m_fInZoom = 1;

		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
		WRITE_BYTE(MSG_SCOPE);
		WRITE_BYTE(4);
		WRITE_ELSE(1+1);
		MESSAGE_END();
	}
	else if (m_fInZoom == 1) 
	{
		m_pPlayer->m_iFOV = 90;
		m_fInZoom = 0;

		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
		WRITE_BYTE(MSG_SCOPE);
		WRITE_BYTE(0);
		WRITE_ELSE(1+1);
		MESSAGE_END();
	}

	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/zoom3.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
	m_flNextSecondaryAttack = gpGlobals->time+0.5;
}

void Cu2::GrenadeLauncher( void )
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
	CU2Momma::ShootU2Momma( m_pPlayer->pev, m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 24 + gpGlobals->v_right * 10 + gpGlobals->v_up * -12, gpGlobals->v_forward * 900, 1 );
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?U2_FIRE_GRENADE_SOLID:U2_FIRE_GRENADE, 1, FIREGUN_U2 );

	m_flNextPrimaryAttack = gpGlobals->time + 1;
	m_flNextSecondaryAttack = gpGlobals->time + 2;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (5, 8);

	if (!m_pPlayer->m_fHeavyArmor)
	{
		m_pPlayer->pev->punchangle.x -= 15;
		m_pPlayer->pev->punchangle.y -= 5;
	}
}

void Cu2::SwitchFireMode( void )
{
	if(firemode==FIREMODE_GLAUNCHER)
		firemode=FIREMODE_SCOPE;
	else
		firemode=FIREMODE_GLAUNCHER;
	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.1;
}

void Cu2::Holster( )
{
	m_fInReload = FALSE;
	ZoomReset();

	MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
	WRITE_BYTE(MSG_FIREMODE);
	WRITE_BYTE(0);
	WRITE_ELSE(1+1);
	MESSAGE_END();

	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.5;
	SendWeaponAnim( U2_HOLSTER );
}

void Cu2::Reload( void )
{
	ZoomReset();
	DefaultReload( 30, U2_RELOAD, 2.9, 1.1 );
	m_iChargeLevel = 2;
}

void Cu2::ZoomReset(void)
{
	if (m_pPlayer->m_iFOV != 90)
	{
		m_pPlayer->m_iFOV = 90;
		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
		WRITE_BYTE(MSG_SCOPE);
		WRITE_BYTE(0);
		WRITE_ELSE(1+1);
		MESSAGE_END();
	}
}

void Cu2::WeaponIdle( void )
{
	if( m_iLastFiremode != firemode)
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
		WRITE_BYTE(MSG_FIREMODE);
		WRITE_BYTE(firemode);
		WRITE_ELSE(1+1);
		MESSAGE_END();
		m_iLastFiremode = firemode;
	}

	if ( m_flTimeWeaponIdle > gpGlobals->time )
		return;

	if (m_iChargeLevel != 2)
	{
		switch (m_iChargeLevel)
		{
		case 0:
		case 1:
			FireBurst();
			m_iChargeLevel++;
			break;
		}
		return;
	}

	if (m_pPlayer->m_iFOV != 90)
		return;

	int iAnim;
	switch ( RANDOM_LONG( 0, 1 ) )
	{
	case 0:	
		iAnim = U2_IDLE;	
		break;
	
	default:
	case 1:
		iAnim = U2_IDLE2;
		break;
	}

	SendWeaponAnim( iAnim );

	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (10, 15);
}
