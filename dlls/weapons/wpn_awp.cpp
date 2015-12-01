#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"

class Cawp : public CBasePlayerWeapon
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

	void EXPORT shellcasing( void );
	void ZoomReset( void );

	void BuyPrimaryAmmo( void );
	void SellWeapon( void );
};

void Cawp::BuyPrimaryAmmo( void )
{
	BuyAmmo(10, (char*)pszAmmo1(), COST_SNIPER_AMMO);
}

void Cawp::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_AWP/2;
}

enum awp_e
{
	AWP_IDLE,
	AWP_FIRE,
	AWP_FIRE_SOLID,
	AWP_RELOAD,
	AWP_DRAW,
	AWP_HOLSTER
};

LINK_ENTITY_TO_CLASS( weapon_awp, Cawp );

void Cawp::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_awp.mdl");
	m_iId = WEAPON_AWP;
	m_iDefaultAmmo = 20;
	pev->weapons = CLIP_AWP;
	FallInit();
}

void Cawp::Precache( void )
{
	PRECACHE_MODEL("models/v_awp.mdl");
	PRECACHE_MODEL("models/p_awp.mdl");

	PRECACHE_SOUND ("weapons/awp_fire.wav");
	PRECACHE_SOUND ("weapons/zoom.wav");
}

int Cawp::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "338Magnum";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 10;
	p->iSlot = 3;
	p->iPosition = 4;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iId = WEAPON_AWP;
	p->iWeight = AWP_WEIGHT;
	p->iMaxspeed = AWP_MAXSPEED;
	return 1;
}

BOOL Cawp::Deploy( )
{
	return DefaultDeploy( "models/v_awp.mdl", "models/p_awp.mdl", AWP_DRAW, "rifle", 1 );
}

void Cawp::shellcasing() 
{ 
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), 200, 2, FIREGUN_AWP );
} 

void Cawp::PrimaryAttack()
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
	m_pPlayer->FireMagnumBullets(m_pPlayer->GetGunPosition(), gpGlobals->v_forward, VECTOR_CONE_0DEGREES, 16384, BULLET_338Magnum, m_pPlayer->pev);
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?AWP_FIRE_SOLID:AWP_FIRE, m_pPlayer->m_fHeavyArmor?1:0, FIREGUN_AWP );

	SetThink( shellcasing ); 
		(m_pPlayer->m_fHeavyArmor)?pev->nextthink = gpGlobals->time + 0.6:pev->nextthink = gpGlobals->time + 1.15;

	(m_pPlayer->m_fHeavyArmor)?m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.2:m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.8;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (8, 12);
}

void Cawp::SecondaryAttack( void )
{
	if (m_fInZoom == 0) 
	{
		m_pPlayer->m_iFOV = 40;
		m_fInZoom = 1;
	}
	else if (m_fInZoom == 1) 
	{
		m_pPlayer->m_iFOV = 8;
		m_fInZoom = 2;
	}
	else if (m_fInZoom == 2) 
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
	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/zoom.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
}

void Cawp::Holster( )
{
	m_fInReload = FALSE;
	ZoomReset();

	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.85;
	SendWeaponAnim( AWP_HOLSTER );
}

void Cawp::Reload( void )
{
	ZoomReset();
	DefaultReload( 10, AWP_RELOAD, 3.7, 1.1 );
}

void Cawp::ZoomReset(void)
{
	if (m_pPlayer->m_iFOV != 90)
	{
		m_fInZoom = 0;
		m_pPlayer->m_iFOV = 90;
		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
		WRITE_BYTE(MSG_SCOPE);
		WRITE_BYTE(0);
		WRITE_ELSE(1+1);
		MESSAGE_END();
	}
}

void Cawp::WeaponIdle( void )
{
	if ( m_flTimeWeaponIdle > gpGlobals->time )
		return;

	if (m_pPlayer->m_iFOV != 90)
		return;

	SendWeaponAnim( AWP_IDLE );
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (8, 12);
}