#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"
#include "decals.h"

class Cbarett : public CBasePlayerWeapon
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

	void ZoomUpdate(void);
	void ZoomReset(void);

	void BuyPrimaryAmmo( void );
	void SellWeapon( void );
};

void Cbarett::BuyPrimaryAmmo( void )
{
	BuyAmmo(5, (char*)pszAmmo1(), COST_SNIPER_AMMO);
}

void Cbarett::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_BARETT/2;
}

enum barett_e
{
	BARETT_IDLE1,
	BARETT_IDLE2,
	BARETT_FIRE,
	BARETT_FIRE_SOLID,
	BARETT_RELOAD,
	BARETT_DRAW,
	BARETT_HOLSTER
};

LINK_ENTITY_TO_CLASS( weapon_barett, Cbarett );

void Cbarett::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_barett.mdl");
	m_iId = WEAPON_BARETT;
	m_iDefaultAmmo = 10;
	pev->weapons = CLIP_BARETT;
	FallInit();
}

void Cbarett::Precache( void )
{
	PRECACHE_MODEL("models/v_barett.mdl");
	PRECACHE_MODEL("models/p_barett.mdl");

	PRECACHE_SOUND("weapons/barett_fire.wav");
	PRECACHE_SOUND("weapons/barett_explode.wav");
}

int Cbarett::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "127mm";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 5;
	p->iSlot = 3;
	p->iPosition = 5;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iId = WEAPON_BARETT;
	p->iWeight = BARETT_WEIGHT;
	p->iMaxspeed = BARETT_MAXSPEED;
	return 1;
}

BOOL Cbarett::Deploy( )
{
	return DefaultDeploy( "models/v_barett.mdl", "models/p_barett.mdl", BARETT_DRAW, "rifle" );
}

void Cbarett::Holster( )
{
	m_fInReload = FALSE;
	if (m_pPlayer->m_iFOV != 90)
	ZoomReset();

	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.4;
	SendWeaponAnim( BARETT_HOLSTER );
}

void Cbarett::PrimaryAttack()
{
	if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 0)
 	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		return;
	}

	if (!m_pPlayer->m_fHeavyArmor)
	{
		UTIL_MakeVectors (m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
		float flZVel = m_pPlayer->pev->velocity.z;
		m_pPlayer->pev->velocity = m_pPlayer->pev->velocity - gpGlobals->v_forward * 180;
		m_pPlayer->pev->velocity.z = flZVel;
	}
	m_iClip--;
	m_iFiredAmmo++;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	m_pPlayer->FireMagnumBullets(m_pPlayer->GetGunPosition(), gpGlobals->v_forward, VECTOR_CONE_0DEGREES, 16384, BULLET_127MM, m_pPlayer->pev);
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?BARETT_FIRE_SOLID:BARETT_FIRE, m_pPlayer->m_fHeavyArmor?1:0, FIREGUN_BARETT );

	(m_pPlayer->m_fHeavyArmor)?m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.3:m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.8;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (8, 12);
}

void Cbarett::SecondaryAttack( void )
{
	ZoomUpdate();
}

void Cbarett::Reload( void )
{
	if (m_iChargeLevel)
	ZoomReset();

	DefaultReload( 5, BARETT_RELOAD, 2.5, 0.9 );
}

void Cbarett :: ZoomUpdate( void )
{
	if (m_pPlayer->pev->button & IN_ATTACK2)
	{
		if(m_iChargeLevel == 0)
		{
			if (m_flShockTime > UTIL_WeaponTimeBase()) return;
			m_iChargeLevel = 1;
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/zoom2.wav", 1, ATTN_NORM);

			MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
			WRITE_BYTE(MSG_SCOPE);
			WRITE_BYTE(3);
			WRITE_ELSE(1+1);
			MESSAGE_END();

			m_flTimeUpdate = UTIL_WeaponTimeBase() + 0.001;
		}
		if(m_iChargeLevel == 1)
		{
			m_pPlayer->m_iFOV = 50;
			m_iChargeLevel = 2;//ready to zooming, wait for 0.5 secs
		}

		if (m_flTimeUpdate > UTIL_WeaponTimeBase()) return;
		if (m_iChargeLevel == 2 && m_pPlayer->m_iFOV > 5)
		{
			m_pPlayer->m_iFOV-=3;
			m_flTimeUpdate = UTIL_WeaponTimeBase() + 0.001;
		}
		if(m_iChargeLevel == 3) ZoomReset();
	}
	else if(m_iChargeLevel > 1) m_iChargeLevel = 3;
}

void Cbarett::ZoomReset( void )
{
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/zoom2.wav", 1, ATTN_NORM);

	MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
	WRITE_BYTE(MSG_SCOPE);
	WRITE_BYTE(0);
	WRITE_ELSE(1+1);
	MESSAGE_END();

	m_flShockTime = UTIL_WeaponTimeBase() + 0.5;
	m_pPlayer->m_iFOV = 90;
	m_iChargeLevel = 0;
}

void Cbarett::WeaponIdle( void )
{
	ZoomUpdate();

	if (m_flTimeWeaponIdle > gpGlobals->time)
		return;

	if (m_pPlayer->m_iFOV != 90)
		return;

	int iAnim;
	switch ( RANDOM_LONG( 0, 1 ) )
	{
	case 0:	
		iAnim = BARETT_IDLE1;	
		break;
	
	default:
	case 1:
		iAnim = BARETT_IDLE2;
		break;
	}
	SendWeaponAnim( iAnim );
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (8, 12);
}