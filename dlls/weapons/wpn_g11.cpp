#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"

class Cg11 : public CBasePlayerWeapon
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
	void ZoomReset( void );
	void SellWeapon( void );
	void BuyPrimaryAmmo( void );
};

void Cg11::BuyPrimaryAmmo( void )
{
	BuyAmmo(50, (char*)pszAmmo1(), COST_SNIPER_AMMO);
}

void Cg11::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_G11/2;
}

enum g11_e
{
	G11_IDLE,
	G11_FIRE,
	G11_FIRE_SOLID,
	G11_RELOAD,
	G11_DRAW,
	G11_HOLSTER
};

LINK_ENTITY_TO_CLASS( weapon_g11, Cg11 );

void Cg11::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_g11.mdl");
	m_iId = WEAPON_G11;
	m_iDefaultAmmo = 100;
	pev->weapons = CLIP_G11;
	FallInit();
}

void Cg11::Precache( void )
{
	PRECACHE_MODEL("models/v_g11.mdl");
	PRECACHE_MODEL("models/p_g11.mdl");
	PRECACHE_SOUND ("weapons/g11_fire.wav");
}

int Cg11::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "57mm";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 50;
	p->iSlot = 3;
	p->iPosition = 0;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iId = WEAPON_G11;
	p->iWeight = G11_WEIGHT;
	p->iMaxspeed = G11_MAXSPEED;
	return 1;
}

BOOL Cg11::Deploy( )
{
	return DefaultDeploy( "models/v_g11.mdl", "models/p_g11.mdl", G11_DRAW, "rifle", 0.7 );
}

void Cg11::PrimaryAttack()
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
	m_pPlayer->FireMagnumBullets(m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 16 + gpGlobals->v_right * 2 + gpGlobals->v_up * -3, gpGlobals->v_forward, (m_pPlayer->pev->flags & FL_DUCKING)?VECTOR_CONE_2DEGREES:VECTOR_CONE_3DEGREES, 16384, BULLET_57mm, m_pPlayer->pev);
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?G11_FIRE_SOLID:G11_FIRE, 0, FIREGUN_G11 );

	m_flNextPrimaryAttack = gpGlobals->time + 0.03;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (8, 12);
	if (!m_pPlayer->m_fHeavyArmor)
	{
		m_pPlayer->pev->punchangle.x -= RANDOM_FLOAT(-1.5,2);
		m_pPlayer->pev->punchangle.y -= 0.2;
	}
}

void Cg11::SecondaryAttack( void )
{
	if (m_fInZoom == 0) 
	{
		m_pPlayer->m_iFOV = 50;
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

	m_flNextSecondaryAttack = gpGlobals->time + 0.5;
}

void Cg11::Holster( )
{
	m_fInReload = FALSE;
	ZoomReset();

	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.6;
	SendWeaponAnim( G11_HOLSTER );
}

void Cg11::ZoomReset(void)
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

void Cg11::Reload( void )
{
	ZoomReset();
	DefaultReload( 50, G11_RELOAD, 5.1, 1.9 );
}

void Cg11::WeaponIdle( void )
{
	if ( m_flTimeWeaponIdle > gpGlobals->time )
		return;

	if (m_pPlayer->m_iFOV != 90)
		return;

	SendWeaponAnim(G11_IDLE);
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (10, 15);
}
