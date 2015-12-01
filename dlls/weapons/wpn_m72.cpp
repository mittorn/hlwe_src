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

enum M72_e
{
	M72_IDLE1,
	M72_IDLE2,
	M72_FIRE,
	M72_FIRE_QUAD,
	M72_FIRE_SOLID,
	M72_FIRE_QUAD_SOLID,
	M72_RELOAD,
	M72_DRAW,
	M72_HOLSTER
};

class CM72 : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	void Reload( void );
	int GetItemInfo(ItemInfo *p);

	BOOL Deploy( void );
	void Holster( void );

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	void SwitchFireMode( void );
	void WeaponIdle( void );

	void BuyPrimaryAmmo( void );
	void BuySecondaryAmmo( void );
	int SecondaryAmmoIndex( void );

	void FireNormal( void );
	void FireQuad( void );
	void SellWeapon( void );
	void ZoomUpdate(void);
	void ZoomReset(void);
};
LINK_ENTITY_TO_CLASS( weapon_m72, CM72 );

int CM72::SecondaryAmmoIndex( void )
{
	return m_iSecondaryAmmoType;
}

void CM72::BuyPrimaryAmmo( void )
{
	BuyAmmo(20, (char*)pszAmmo1(), COST_ENERGY2_AMMO);
}

void CM72::BuySecondaryAmmo( void )
{
	BuyAmmo(5, (char*)pszAmmo2(), COST_EXPERIMENTAL_AMMO);
}

void CM72::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_M72/2;
}

void CM72::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_m72.mdl");
	m_iId = WEAPON_M72;
	firemode=FIREMODE_SHOOT;
	m_iDefaultAmmo = 40;
	m_iDefaultAmmo2 = 5;
	pev->weapons = CLIP_M72;
	FallInit();
}

void CM72::Precache( void )
{
	PRECACHE_MODEL("models/v_m72.mdl");
	PRECACHE_MODEL("models/p_m72.mdl");

	PRECACHE_SOUND("weapons/m72_fire.wav");
	PRECACHE_SOUND("weapons/m72_fire_quad.wav");
	PRECACHE_SOUND("items/quaddamage.wav");
	PRECACHE_SOUND("weapons/m72_quad_finished.wav");
}

int CM72::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "2mm";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = "quad";
	p->iMaxAmmo2 = 999;
	p->iMaxClip = 20;
	p->iSlot = 8;
	p->iPosition = 3;
	p->iId = WEAPON_M72;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iWeight = M72_WEIGHT;
	p->iMaxspeed = M72_MAXSPEED;
	return 1;
}

BOOL CM72::Deploy( )
{
	m_iLastFiremode = -1;
	return DefaultDeploy( "models/v_m72.mdl", "models/p_m72.mdl", M72_DRAW, "rifle", 1.1);
}

void CM72::Holster( )
{
	if (m_pPlayer->m_iFOV != 90)
	ZoomReset();

	MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
	WRITE_BYTE(MSG_FIREMODE);
	WRITE_ELSE(1);
	MESSAGE_END();

	m_fInReload = FALSE;
	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.7;
	SendWeaponAnim( M72_HOLSTER );
}

void CM72 :: ZoomUpdate( void )
{
	if (m_pPlayer->pev->button & IN_ATTACK2)
	{
		if(m_iChargeLevel == 0)
		{
			if (m_flShockTime > UTIL_WeaponTimeBase()) return;
			m_iChargeLevel = 1;
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/zoom.wav", 1, ATTN_NORM);

			MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
			WRITE_BYTE(MSG_SCOPE);
			WRITE_BYTE(4);
			WRITE_ELSE(1+1);
			MESSAGE_END();

			m_flTimeUpdate = UTIL_WeaponTimeBase() + 0.001;
		}
		if(m_iChargeLevel == 1)
		{
			m_pPlayer->m_iFOV = 40;
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

void CM72::ZoomReset( void )
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

void CM72::FireNormal( void )
{
	m_iClip--;
	m_iFiredAmmo++;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	UTIL_MakeVectors (m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	m_pPlayer->FireMagnumBullets(m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 18 + gpGlobals->v_right * 1.5 + gpGlobals->v_up * -1.2, gpGlobals->v_forward, VECTOR_CONE_0DEGREES, 16384, BULLET_2MM, m_pPlayer->pev);
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?M72_FIRE_SOLID:M72_FIRE, 0, FIREGUN_M72 );

	if (!m_pPlayer->m_fHeavyArmor)
		m_pPlayer->pev->punchangle.x -= RANDOM_FLOAT(2.5, 4.2);
}

void CM72::FireQuad( void )
{
	if(m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] <= 0)
	{
		PlayEmptySound(4);
		firemode = FIREMODE_SHOOT;
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/m72_quad_finished.wav", 1, ATTN_LOW_HIGH );
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("Quad Damage power cell has been depleted"));
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 2;
	return;
	}

	m_iClip--;
	m_iFiredAmmo++;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType]--;

	UTIL_MakeVectors (m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	m_pPlayer->FireMagnumBullets(m_pPlayer->GetGunPosition(), gpGlobals->v_forward, VECTOR_CONE_0DEGREES, 16384, BULLET_2MM_QUAD, m_pPlayer->pev);
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?M72_FIRE_QUAD_SOLID:M72_FIRE_QUAD, 1, FIREGUN_M72 );

	if (!m_pPlayer->m_fHeavyArmor)
	{
		UTIL_MakeVectors (m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
		m_pPlayer->pev->velocity = m_pPlayer->pev->velocity - gpGlobals->v_forward * 300;
		m_pPlayer->pev->punchangle.x -= RANDOM_FLOAT(12, 16);
	}
}

void CM72::SwitchFireMode( void )
{
	if(firemode==FIREMODE_SHOOT)
	{
		if(m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] <= 0)
		{
			firemode = FIREMODE_SHOOT;
			ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("Not enough power to activate Quad Damage"));
		}
		else
		{
			firemode = FIREMODE_QUAD;
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "items/quaddamage.wav", 1, ATTN_NORM );
			m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1;
		}
	}
	else if(firemode==FIREMODE_QUAD)
	{
		firemode = FIREMODE_SHOOT;
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/m72_quad_finished.wav", 1, ATTN_NORM );
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1;
	}

	MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
	WRITE_BYTE(MSG_FIREMODE);
	WRITE_BYTE(firemode);
	WRITE_ELSE(1+1);
	MESSAGE_END();
	m_iLastFiremode = firemode;
}

void CM72::PrimaryAttack()
{
	if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 0)
 	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		return;
	}
	if(firemode==FIREMODE_SHOOT)
		FireNormal();
	else if(firemode==FIREMODE_QUAD)
		FireQuad();

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1;
	m_flTimeWeaponIdle = gpGlobals->time + 5;
}

void CM72::SecondaryAttack()
{
	ZoomUpdate();
}

void CM72::Reload( void )
{
	if (m_iChargeLevel)
		ZoomUpdate();
	DefaultReload( 20, M72_RELOAD, 4.8, 1.4 );
}

void CM72::WeaponIdle( void )
{
	ZoomUpdate();

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

	if (m_flTimeWeaponIdle > gpGlobals->time)
		return;

	switch (RANDOM_LONG(0,1))
	{
		case 0:	SendWeaponAnim( M72_IDLE1 ); break;
		case 1:	SendWeaponAnim( M72_IDLE2 ); break;
	}
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 10, 15 );
}