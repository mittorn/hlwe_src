#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"

class Cusp : public CBasePlayerWeapon
{
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
	void SellWeapon( void );
	int silencer;
};


enum usp_e
{
	USP_IDLE,
	USP_IDLE_SIL,
	USP_IDLE2,
	USP_IDLE_SIL2,
	USP_SHOOT,
	USP_SHOOT_SOLID,
	USP_SHOOT_SIL,
	USP_SHOOT_SOLID_SIL,
	USP_RELOAD,
	USP_RELOAD_SIL,
	USP_DRAW,
	USP_DRAW_SIL,
	USP_ADD_SIL,
	USP_REMOVE_SIL,
	USP_HOLSTER,
	USP_HOLSTER_SIL
};

LINK_ENTITY_TO_CLASS( weapon_usp, Cusp );

void Cusp::BuyPrimaryAmmo( void )
{
	BuyAmmo(12, (char*)pszAmmo1(), COST_HANDGUN_AMMO);
}

void Cusp::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_USP/2;
}

void Cusp::Spawn( )
{
	Precache( );
	silencer=0;
	SET_MODEL(ENT(pev), "models/p_usp_sil.mdl");
	m_iId = WEAPON_USP;
	m_iDefaultAmmo = 24;
	pev->weapons = CLIP_USP;
	FallInit();
}

void Cusp::Precache( void )
{
	PRECACHE_MODEL("models/v_usp.mdl");
	PRECACHE_MODEL("models/p_usp.mdl");
	PRECACHE_MODEL("models/p_usp_sil.mdl");

	PRECACHE_SOUND ("weapons/usp_fire.wav");
	PRECACHE_SOUND ("weapons/usp_fire_sil.wav");
}

int Cusp::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "45ACP";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 12;
	p->iSlot = 1;
	p->iPosition = 2;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iId = WEAPON_USP;
	p->iWeight = USP_WEIGHT;
	p->iMaxspeed = USP_MAXSPEED;
	return 1;
}

BOOL Cusp::Deploy( )
{
	return DefaultDeploy( "models/v_usp.mdl", "models/p_usp.mdl", (silencer)?USP_DRAW_SIL:USP_DRAW, "onehanded", 0.7 );
}

void Cusp::Holster( )
{
	m_fInReload = FALSE;
	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.6;
	SendWeaponAnim( (silencer)?USP_HOLSTER_SIL:USP_HOLSTER );
}

void Cusp::PrimaryAttack( void )
{
	if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 0)
 	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		return;
	}

	if(! ( m_pPlayer->m_afButtonPressed & IN_ATTACK ) )
	return;

	m_iClip--;
	m_iFiredAmmo++;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	if(silencer==0)
	{
		m_pPlayer->FireMagnumBullets(m_pPlayer->GetGunPosition(), gpGlobals->v_forward, (m_pPlayer->pev->flags & FL_DUCKING)?VECTOR_CONE_5DEGREES:VECTOR_CONE_6DEGREES, 16384, BULLET_45ACP, m_pPlayer->pev);
		FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?USP_SHOOT_SOLID:USP_SHOOT, 0, FIREGUN_USP );
		m_flNextPrimaryAttack = gpGlobals->time + 0.1;
	}
	else
	{
		m_pPlayer->FireMagnumBullets(m_pPlayer->GetGunPosition(), gpGlobals->v_forward, (m_pPlayer->pev->flags & FL_DUCKING)?VECTOR_CONE_2DEGREES:VECTOR_CONE_3DEGREES, 16384, BULLET_45ACP, m_pPlayer->pev);
		FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?USP_SHOOT_SOLID_SIL:USP_SHOOT_SIL, 1, FIREGUN_USP );
		m_flNextPrimaryAttack = gpGlobals->time + 0.15;
	}

	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (5, 8);
	if (!m_pPlayer->m_fHeavyArmor)
	m_pPlayer->pev->punchangle.x -= 3.2;
}

void Cusp::SecondaryAttack( void )
{
	if(silencer==1)
	{
		silencer=0;
		m_pPlayer->pev->weaponmodel = MAKE_STRING("models/p_usp.mdl");
		DefaultDeploy( "models/v_usp.mdl", "models/p_usp.mdl", USP_REMOVE_SIL, "onehanded", 0 );
	}
	else
	{
		silencer=1;
		m_pPlayer->pev->weaponmodel = MAKE_STRING("models/p_usp_sil.mdl");
		DefaultDeploy( "models/v_usp.mdl", "models/p_usp_sil.mdl", USP_ADD_SIL, "onehanded", 0);
	}
	m_flNextSecondaryAttack = m_flNextPrimaryAttack = gpGlobals->time + 2;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (5, 8);
}

void Cusp::Reload( void )
{
	DefaultReload( 12, (silencer)?USP_RELOAD_SIL:USP_RELOAD, 2.15, 0.8 );
}

void Cusp::WeaponIdle( void )
{
	if ( m_flTimeWeaponIdle > gpGlobals->time )
		return;

	int iAnim;
	switch ( RANDOM_LONG( 0, 1 ) )
	{
	case 0: 
		if(silencer==0)
			iAnim = USP_IDLE; 
		else
			iAnim = USP_IDLE_SIL; 
	break;

	default:
	case 1:
		if(silencer==0)
			iAnim = USP_IDLE2; 
		else
			iAnim = USP_IDLE_SIL2; 
	break;
	}
	SendWeaponAnim( iAnim );
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (5, 8);
}