#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "shake.h"
#include "gamerules.h"
#include "decals.h"

enum taucannon_e
{
	TAUCANNON_IDLE,
	TAUCANNON_IDLE2,
	TAUCANNON_FIRE,
	TAUCANNON_FIRE_SOLID,
	TAUCANNON_RELOAD,
	TAUCANNON_DRAW,
	TAUCANNON_HOLSTER
};

class Ctaucannon : public CBasePlayerWeapon
{
public:
	void Reload( void );
	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);

	BOOL Deploy( void );
	void Holster( void );

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	void WeaponIdle( void );

	void ZoomUpdate(void);
	void ZoomReset(void);

	void BuyPrimaryAmmo( void );
	void SellWeapon( void );
};
LINK_ENTITY_TO_CLASS( weapon_taucannon, Ctaucannon );

void Ctaucannon::BuyPrimaryAmmo( void )
{
	BuyAmmo(25, (char*)pszAmmo1(), COST_ENERGY_AMMO);
}

void Ctaucannon::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_TAUCANNON/2;
}

void Ctaucannon::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_taucannon.mdl");
	m_iId = WEAPON_TAUCANNON;
	m_iDefaultAmmo = 50;
	pev->weapons = CLIP_TAUCANNON;
	FallInit();
}

void Ctaucannon::Precache( void )
{
	PRECACHE_MODEL("models/v_taucannon.mdl");
	PRECACHE_MODEL("models/p_taucannon.mdl");

	PRECACHE_SOUND("weapons/taucannon_fire.wav");
	PRECACHE_SOUND("weapons/tau_zoom.wav");
}

int Ctaucannon::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "EnergyCells";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 25;
	p->iSlot = 6;
	p->iPosition = 6;
	p->iId = WEAPON_TAUCANNON;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iWeight = TAUCANNON_WEIGHT;
	p->iMaxspeed = TAUCANNON_MAXSPEED;
	return 1;
}

BOOL Ctaucannon::Deploy( )
{
	return DefaultDeploy( "models/v_taucannon.mdl", "models/p_taucannon.mdl", TAUCANNON_DRAW, "rifle" );
}

void Ctaucannon::Holster( )
{
	m_fInReload = FALSE;
	if (m_pPlayer->m_iFOV != 90)
	ZoomReset();

	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.8;
	SendWeaponAnim( TAUCANNON_HOLSTER );
}

void Ctaucannon::PrimaryAttack()
{
	if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 4)
 	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		return;
	}

	m_iClip -= 5;
	m_iFiredAmmo+=5;
	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	m_pPlayer->FireBeam(m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 16 + gpGlobals->v_right * 2.8 + gpGlobals->v_up * -2, gpGlobals->v_forward, BEAM_TAUCANNON, 0, m_pPlayer->pev);
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?TAUCANNON_FIRE_SOLID:TAUCANNON_FIRE, 0, FIREGUN_TAUCANNON );

	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	if (!m_pPlayer->m_fHeavyArmor)
	m_pPlayer->pev->punchangle.x -= 5;
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.5;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 10, 15 );
}

void Ctaucannon::SecondaryAttack()
{
	ZoomUpdate();
}

void Ctaucannon :: ZoomUpdate( void )
{
	if (m_pPlayer->pev->button & IN_ATTACK2)
	{
		if(m_iChargeLevel == 0)
		{
			if (m_flShockTime > UTIL_WeaponTimeBase()) return;
			m_iChargeLevel = 1;
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/tau_zoom.wav", 1, ATTN_NORM);

			MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
			WRITE_BYTE(MSG_SCOPE);
			WRITE_BYTE(1);
			WRITE_ELSE(1+1);
			MESSAGE_END();

			m_flTimeUpdate = UTIL_WeaponTimeBase() + 0.001;
		}
		if(m_iChargeLevel == 1)
		{
			m_pPlayer->m_iFOV = 80;
			m_iChargeLevel = 2;//ready to zooming, wait for 0.5 secs
		}

		if (m_flTimeUpdate > UTIL_WeaponTimeBase()) return;
		if (m_iChargeLevel == 2 && m_pPlayer->m_iFOV > 8)
		{
			m_pPlayer->m_iFOV-=3;
			m_flTimeUpdate = UTIL_WeaponTimeBase() + 0.008;
		}
		if(m_iChargeLevel == 3) ZoomReset();
	}
	else if(m_iChargeLevel > 1) m_iChargeLevel = 3;
}

void Ctaucannon::ZoomReset( void )
{
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/tau_zoom.wav", 1, ATTN_NORM);

	MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
	WRITE_BYTE(MSG_SCOPE);
	WRITE_BYTE(0);
	WRITE_ELSE(1+1);
	MESSAGE_END();

	m_flShockTime = UTIL_WeaponTimeBase() + 0.5;
	m_pPlayer->m_iFOV = 90;
	m_iChargeLevel = 0;
}

void Ctaucannon::WeaponIdle( void )
{
	ZoomUpdate();

	if (m_flTimeWeaponIdle > gpGlobals->time)
		return;

	if (m_pPlayer->m_iFOV != 90)
		return;

	switch (RANDOM_LONG(0,1))
	{
		case 0:	SendWeaponAnim( TAUCANNON_IDLE ); break;
		case 1:	SendWeaponAnim( TAUCANNON_IDLE2 ); break;
	}
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 10, 15 );
}

void Ctaucannon::Reload( void )
{
	if (m_iClip)
	return;

	if (m_iChargeLevel)
	ZoomReset();

	DefaultReload( 25, TAUCANNON_RELOAD, 4.1, 1.85 );
}
