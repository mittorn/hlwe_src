#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"
#include "projectiles.h"

enum plasmarifle_e 
{
	PLASMARIFLE_IDLE,
	PLASMARIFLE_IDLE2,
	PLASMARIFLE_FIDGET,
	PLASMARIFLE_SPINUP,
	PLASMARIFLE_SPIN,
	PLASMARIFLE_FIRE,
	PLASMARIFLE_FIRE_SOLID,
	PLASMARIFLE_FIRE_SEC,
	PLASMARIFLE_FIRE_SEC_SOLID,
	PLASMARIFLE_RELOAD,
	PLASMARIFLE_DRAW,
	PLASMARIFLE_HOLSTER
};

class CPlasmarifle : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);
	void Reload( void );

	BOOL Deploy( void );
	void Holster( void );

	void EXPORT FireBall( void );

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	void WeaponIdle( void );

	void BuyPrimaryAmmo( void );
	void SellWeapon( void );
};
LINK_ENTITY_TO_CLASS( weapon_plasmarifle, CPlasmarifle );

void CPlasmarifle::BuyPrimaryAmmo( void )
{
	BuyAmmo(30, (char*)pszAmmo1(), COST_ENERGY_AMMO);
}

void CPlasmarifle::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_PLASMARIFLE/2;
}

void CPlasmarifle::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_plasmarifle.mdl");
	m_iId = WEAPON_PLASMARIFLE;
	m_iDefaultAmmo = 60;
	pev->weapons = CLIP_PLASMARIFLE;
	FallInit();
}

void CPlasmarifle::Precache( void )
{
	PRECACHE_MODEL("models/v_plasmarifle.mdl");
	PRECACHE_MODEL("models/p_plasmarifle.mdl");

	PRECACHE_SOUND("weapons/plasmarifle_fire2.wav");
	PRECACHE_SOUND("weapons/plasmarifle_fire.wav");
	PRECACHE_SOUND("weapons/plasmarifle_spin.wav");
	PRECACHE_SOUND("weapons/plasma_hitwall.wav");
}

BOOL CPlasmarifle::Deploy( void )
{
	return DefaultDeploy( "models/v_plasmarifle.mdl", "models/p_plasmarifle.mdl", PLASMARIFLE_DRAW, "egon", 0.6 );
}

void CPlasmarifle::Holster( )
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	m_fInAttack = 0;
	m_fInReload = FALSE;
	SendWeaponAnim( PLASMARIFLE_HOLSTER );
}

int CPlasmarifle::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "MicroFusionCells";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 30;
	p->iSlot = 6;
	p->iPosition = 3;
	p->iId = WEAPON_PLASMARIFLE;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iWeight = PLASMARIFLE_WEIGHT;
	p->iMaxspeed = PLASMARIFLE_MAXSPEED;
	return 1;
}

void CPlasmarifle::PrimaryAttack()
{
	if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 0)
 	{
		PlayEmptySound(4);
		m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		return;
	}
	if (m_fInAttack!=0)
	return;

	m_iClip--;
	m_iFiredAmmo++;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	CPlasma::ShootPlasma( m_pPlayer->pev, m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 16 + gpGlobals->v_right * 2 + gpGlobals->v_up * -5, gpGlobals->v_forward * 20000 );
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?PLASMARIFLE_FIRE_SOLID:PLASMARIFLE_FIRE, 0, FIREGUN_PLASMARIFLE );

	m_flNextPrimaryAttack = gpGlobals->time + 0.12;
	m_flTimeWeaponIdle = gpGlobals->time + 7;
}

void CPlasmarifle::FireBall()
{
	if ( m_fInAttack == 0 )
	{
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1;
		return;
	}
	if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 2)
	{
		PlayEmptySound(4);
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
		return;
	}
	m_iClip -= 3;
	m_iFiredAmmo+=3;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	CPlasma2::ShootPlasma2( m_pPlayer->pev, m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 16 + gpGlobals->v_right * 4 + gpGlobals->v_up * -6, gpGlobals->v_forward * 20000 );
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?PLASMARIFLE_FIRE_SEC_SOLID:PLASMARIFLE_FIRE_SEC, 1, FIREGUN_PLASMARIFLE );

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.4;
	if (!m_pPlayer->m_fHeavyArmor)
	m_pPlayer->pev->punchangle.x -= 3.5;
}

void CPlasmarifle::SecondaryAttack( void )
{
	if ( m_fInAttack == 0)
	{
		if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 2)
	 	{
			PlayEmptySound(4);
			m_flNextSecondaryAttack = gpGlobals->time + 0.5;
			return;
		}
		m_fInAttack = 1;
		SendWeaponAnim( PLASMARIFLE_SPIN );
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/plasmarifle_spin.wav", 0.9, ATTN_NORM );
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.7;
	}
	else if (m_fInAttack == 1)
	{
		if (m_flTimeWeaponIdle < UTIL_WeaponTimeBase())
		{
			WeaponIdle();
			return;
		}

	}
}

void CPlasmarifle::WeaponIdle( void )
{
	if (m_flTimeWeaponIdle > gpGlobals->time)
		return;

	if (m_fInAttack==1)
	{
		FireBall();
		m_fInAttack = 0;
		return;
	}

	int iAnim;
	switch ( RANDOM_LONG( 0, 1 ) )
	{
	case 0:	
		iAnim = PLASMARIFLE_IDLE;	
		break;
	
	default:
	case 1:
		iAnim = PLASMARIFLE_IDLE2;
		break;
	}

	SendWeaponAnim( iAnim );

	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 10, 15 );
}

void CPlasmarifle::Reload( void )
{
	if (m_iClip)
	return;

	DefaultReload( 30, PLASMARIFLE_RELOAD, 4.9, 1.4 );
}
