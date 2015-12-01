#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "decals.h"
#include "player.h"
#include "gamerules.h"
#include "projectiles.h"

class CCrossbow : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	void SwitchFireMode( void );
	BOOL Deploy( );
	void Holster( void );
	void Reload( void );
	void WeaponIdle( void );
	void ZoomReset( void );
	int SecondaryAmmoIndex( void );

	void BuyPrimaryAmmo( void );
	void BuySecondaryAmmo( void );
	void SellWeapon( void );
};

enum crossbow_e
{
	CROSSBOW_IDLE,
	CROSSBOW_IDLE_EMPTY,
	CROSSBOW_FIDGET,
	CROSSBOW_FIRE,
	CROSSBOW_FIRE_LAST,
	CROSSBOW_FIRE_SOLID,
	CROSSBOW_FIRE_SOLID_LAST,
	CROSSBOW_RELOAD,
	CROSSBOW_DRAW_FULL,
	CROSSBOW_DRAW_EMPTY,
	CROSSBOW_HOLSTER_FULL,
	CROSSBOW_HOLSTER_EMPTY
};

LINK_ENTITY_TO_CLASS( weapon_crossbow, CCrossbow );

int CCrossbow::SecondaryAmmoIndex( void )
{
	return m_iSecondaryAmmoType;
}

void CCrossbow::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_crossbow.mdl");
	m_iId = WEAPON_CROSSBOW;
	m_iDefaultAmmo = 10;
	m_iDefaultAmmo2 = 10;
	pev->weapons = CLIP_CROSSBOW;
	firemode=FIREMODE_BOLT;
	FallInit();
}

void CCrossbow::BuyPrimaryAmmo( void )
{
	BuyAmmo(5, (char*)pszAmmo1(), COST_ASSAULT_AMMO);
}

void CCrossbow::BuySecondaryAmmo( void )
{
	BuyAmmo(10, (char*)pszAmmo2(), COST_ENERGY_AMMO);
}

void CCrossbow::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_CROSSBOW/2;
}

void CCrossbow::Precache( void )
{
	PRECACHE_MODEL("models/v_crossbow.mdl");
	PRECACHE_MODEL("models/p_crossbow.mdl");

	PRECACHE_SOUND("weapons/xbow_fire_norm.wav");
	PRECACHE_SOUND("weapons/xbow_fire_accel.wav");
	PRECACHE_SOUND("weapons/xbow_fire_exp.wav");
	PRECACHE_SOUND("weapons/zoom3.wav");
	PRECACHE_SOUND("weapons/xbow_hit1.wav");
}

int CCrossbow::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "bolts";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = "accelpower";
	p->iMaxAmmo2 = 999;
	p->iMaxClip = 5;
	p->iSlot = 3;
	p->iPosition = 2;
	p->iId = WEAPON_CROSSBOW;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iWeight = CROSSBOW_WEIGHT;
	p->iMaxspeed = CROSSBOW_MAXSPEED;
	return 1;
}

BOOL CCrossbow::Deploy( )
{
	m_iLastFiremode = -1;
	return DefaultDeploy( "models/v_crossbow.mdl", "models/p_crossbow.mdl", (m_iClip)?CROSSBOW_DRAW_FULL:CROSSBOW_DRAW_EMPTY, "gauss" );
}

void CCrossbow::Holster( )
{
	MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
	WRITE_BYTE(MSG_FIREMODE);
	WRITE_BYTE(0);
	WRITE_ELSE(1+1);
	MESSAGE_END();

	m_fInReload = FALSE;
	ZoomReset();

	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.5;
	SendWeaponAnim( (m_iClip)?CROSSBOW_HOLSTER_FULL:CROSSBOW_HOLSTER_EMPTY );
}

void CCrossbow::ZoomReset(void)
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

void CCrossbow::PrimaryAttack( void )
{
	if (m_iClip <= 0)
	{
		m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		PlayEmptySound( );
		return;
	}

	if(firemode == FIREMODE_EXPBOLT && m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] <= 1)
	{
		PlayEmptySound(3);
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("Accelerator power cell has been depleted"));
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
		firemode = FIREMODE_ACCELBOLT;
	return;
	}

	if(firemode == FIREMODE_ACCELBOLT && m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] <= 0)
	{
		PlayEmptySound(4);
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("Accelerator power cell has been depleted"));
		firemode = FIREMODE_BOLT;
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	return;
	}
	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	if(firemode==FIREMODE_BOLT)
	{
		m_iClip--;
		CBolt::ShootBolt( m_pPlayer->pev, m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 16, gpGlobals->v_forward * 5000, 0 );
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/xbow_fire_norm.wav", 1, ATTN_LOW);
	}
	else if(firemode==FIREMODE_ACCELBOLT)
	{
		m_iClip--;
		CBolt::ShootBolt( m_pPlayer->pev, m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 16, gpGlobals->v_forward * 10000, 1 );
		m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType]--;
		FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), 200, 0, FIREGUN_CROSSBOW );
	}
	else if(firemode==FIREMODE_EXPBOLT)
	{
		m_iClip--;
		CBolt::ShootBolt( m_pPlayer->pev, m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 16, gpGlobals->v_forward * 7500, 2 );
		m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType]-=2;
		FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), 200, 1, FIREGUN_CROSSBOW );
	}

	if (m_iClip) 
	{
		SendWeaponAnim((m_pPlayer->m_fHeavyArmor)?CROSSBOW_FIRE_SOLID:CROSSBOW_FIRE);
		pev->body++;
	}
	else 
		SendWeaponAnim((m_pPlayer->m_fHeavyArmor)?CROSSBOW_FIRE_SOLID_LAST:CROSSBOW_FIRE_LAST);

	m_iFiredAmmo++;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	m_flNextPrimaryAttack = gpGlobals->time + 1.5;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (5, 8);
}

void CCrossbow::SecondaryAttack()
{
	if (m_fInZoom == 0) 
	{
		m_pPlayer->m_iFOV = 20;
		m_fInZoom = 1;
	}
	else if (m_fInZoom == 1) 
	{
		m_pPlayer->m_iFOV = 90;
		m_fInZoom = 0;
	}

	MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
	WRITE_BYTE(MSG_SCOPE);
	WRITE_BYTE(m_fInZoom);
	WRITE_ELSE(1+1);
	MESSAGE_END();

	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/zoom3.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
	m_flNextSecondaryAttack = gpGlobals->time + 0.5;
}

void CCrossbow::SwitchFireMode( void )
{
	if(m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] <= 0)
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("Not enough power to activate Accelerator!"));
		return;
	}

	if(firemode==FIREMODE_BOLT)
		firemode = FIREMODE_ACCELBOLT;
	else if(firemode==FIREMODE_ACCELBOLT)
		firemode = FIREMODE_EXPBOLT;
	else if(firemode==FIREMODE_EXPBOLT)
		firemode = FIREMODE_BOLT;

	MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
	WRITE_BYTE(MSG_FIREMODE);
	WRITE_BYTE(firemode);
	WRITE_ELSE(1+1);
	MESSAGE_END();
	m_iLastFiremode = firemode;
	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.25;
}

void CCrossbow::Reload( void )
{
	if (m_iClip)
	return;

	pev->body = 0;
	ZoomReset();
	DefaultReload( 5, CROSSBOW_RELOAD, 4.5, 1 );
}

void CCrossbow::WeaponIdle( void )
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

	if (m_pPlayer->m_iFOV != 90)
		return;

	if ( m_flTimeWeaponIdle > gpGlobals->time )
		return;

	if (m_iClip)
	{
		switch (RANDOM_LONG(0,1))
		{
			case 0:	SendWeaponAnim( CROSSBOW_IDLE ); break;
			case 1:	SendWeaponAnim( CROSSBOW_FIDGET ); break;
		}
	}
	else
		SendWeaponAnim( CROSSBOW_IDLE_EMPTY );
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (10, 15);
}