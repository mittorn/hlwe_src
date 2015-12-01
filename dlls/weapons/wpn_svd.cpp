#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"

class Csvd : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	BOOL Deploy( void );
	void Reload( void );
	void WeaponIdle( void );
	void Holster( void );
	void ZoomReset( void );

	void BuyPrimaryAmmo( void );
	void SellWeapon( void );
};

void Csvd::BuyPrimaryAmmo( void )
{
	BuyAmmo(12, (char*)pszAmmo1(), COST_SNIPER_AMMO);
}

void Csvd::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_SVD/2;
}

enum svd_e
{
	SVD_IDLE1,
	SVD_IDLE2,
	SVD_FIRE,
	SVD_FIRE_SOLID,
	SVD_RELOAD,
	SVD_DRAW,
	SVD_HOLSTER
};

LINK_ENTITY_TO_CLASS( weapon_svd, Csvd );

void Csvd::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_svd.mdl");
	m_iId = WEAPON_SVD;
	m_iDefaultAmmo = 24;
	pev->weapons = CLIP_SVD;
	FallInit();
}

void Csvd::Precache( void )
{
	PRECACHE_MODEL("models/v_svd.mdl");
	PRECACHE_MODEL("models/p_svd.mdl");

	PRECACHE_SOUND ("weapons/svd_fire.wav");
	PRECACHE_SOUND ("weapons/zoom2.wav");
}

int Csvd::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "762x54";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 12;
	p->iSlot = 3;
	p->iPosition = 3;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iId = WEAPON_SVD;
	p->iWeight = SVD_WEIGHT;
	p->iMaxspeed = SVD_MAXSPEED;
	return 1;
}

BOOL Csvd::Deploy( )
{
	return DefaultDeploy( "models/v_svd.mdl", "models/p_svd.mdl", SVD_DRAW, "rifle" );
}

void Csvd::PrimaryAttack()
{
	if (m_iClip <= 0)
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = gpGlobals->time+0.5;
		return;
	}

	m_iClip--;
	m_iFiredAmmo++;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	m_pPlayer->FireMagnumBullets(m_pPlayer->GetGunPosition(), gpGlobals->v_forward, (m_pPlayer->pev->flags & FL_DUCKING)?VECTOR_CONE_0DEGREES:VECTOR_CONE_1DEGREES, 16384, BULLET_762x54, m_pPlayer->pev);
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?SVD_FIRE_SOLID:SVD_FIRE, m_pPlayer->m_fHeavyArmor?1:0, FIREGUN_SVD );

	(m_pPlayer->m_fHeavyArmor)?m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.3:m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.45;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (8, 12);
}

void Csvd::SecondaryAttack( void )
{
	if (m_fInZoom == 0) 
	{
		m_pPlayer->m_iFOV = 50;
		m_fInZoom = 1;

		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
		WRITE_BYTE(MSG_SCOPE);
		WRITE_BYTE(3);
		WRITE_ELSE(1+1);
		MESSAGE_END();
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

		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
		WRITE_BYTE(MSG_SCOPE);
		WRITE_BYTE(0);
		WRITE_ELSE(1+1);
		MESSAGE_END();
	}

	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/zoom2.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
	m_flNextSecondaryAttack = gpGlobals->time+0.5;
}

void Csvd::Holster( )
{
	m_fInReload = FALSE;
	ZoomReset();

	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.6;
	SendWeaponAnim( SVD_HOLSTER );
}

void Csvd::ZoomReset(void)
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

void Csvd::Reload( void )
{
	ZoomReset();
	DefaultReload( 12, SVD_RELOAD, 2.0, 0.7 );
}

void Csvd::WeaponIdle( void )
{
	if ( m_flTimeWeaponIdle > gpGlobals->time )
		return;

	if (m_pPlayer->m_iFOV != 90)
		return;

	int iAnim;
	switch ( RANDOM_LONG( 0, 1 ) )
	{
	case 0:	
		iAnim = SVD_IDLE1;	
		break;
	
	default:
	case 1:
		iAnim = SVD_IDLE2;
		break;
	}

	SendWeaponAnim( iAnim );
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (10, 15);
}
