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
#include "projectiles.h"

class CGluongun : public CBasePlayerWeapon
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
	void WeaponIdle( void );
	
	void StartFire( void );

	void BuyPrimaryAmmo( void );
	void SellWeapon( void );
};

void CGluongun::BuyPrimaryAmmo( void )
{
	BuyAmmo(20, (char*)pszAmmo1(), COST_ENERGY_AMMO);
}

void CGluongun::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_GLUONGUN/2;
}

enum gluongun_e 
{
	GLUONGUN_IDLE,
	GLUONGUN_IDLE2,
	GLUONGUN_FIRE,
	GLUONGUN_FIRE_SOLID,
	GLUONGUN_CHARGE,
	GLUONGUN_RELOAD,
	GLUONGUN_DRAW,
	GLUONGUN_HOLSTER
};
LINK_ENTITY_TO_CLASS( weapon_gluongun, CGluongun );

void CGluongun::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_gluongun.mdl");
	m_iId = WEAPON_GLUONGUN;
	m_iDefaultAmmo = 40;
	pev->weapons = CLIP_GLUONGUN;
	FallInit();
}

void CGluongun::Precache( void )
{
	PRECACHE_MODEL("models/v_gluongun.mdl");
	PRECACHE_MODEL("models/p_gluongun.mdl");

	PRECACHE_SOUND("weapons/gluon_hitwall.wav");
	PRECACHE_SOUND("weapons/gluon_hitwall2.wav");
	PRECACHE_SOUND("weapons/gluongun_fire.wav");
	PRECACHE_SOUND("weapons/gluongun_zap.wav");
	PRECACHE_SOUND("weapons/gluongun_charge.wav");
}

int CGluongun::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "GluonCells";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 20;
	p->iSlot = 6;
	p->iPosition = 7;
	p->iId = WEAPON_GLUONGUN;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iWeight = GLUONGUN_WEIGHT;
	p->iMaxspeed = GLUONGUN_MAXSPEED;
	return 1;
}

BOOL CGluongun::Deploy( void )
{
	return DefaultDeploy( "models/v_gluongun.mdl", "models/p_gluongun.mdl", GLUONGUN_DRAW, "gauss" );
}

void CGluongun::Holster( )
{
	if (m_fInAttack > 0)
		StartFire();

	m_fInReload = FALSE;
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.6;
	SendWeaponAnim( GLUONGUN_HOLSTER );
	m_fInAttack = 0;
}

void CGluongun::PrimaryAttack()
{
	if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 1)
 	{
		PlayEmptySound(3);
		m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		return;
	}

	m_iClip -= 2;
	m_iFiredAmmo+=2;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	pev->skin = (m_iClip/2);

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	CGluon::ShootGluon( m_pPlayer->pev, m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 28 + gpGlobals->v_right * 5 + gpGlobals->v_up * -6, gpGlobals->v_forward * 2500 );
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?GLUONGUN_FIRE_SOLID:GLUONGUN_FIRE, 0, FIREGUN_GLUONGUN);

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0;
	m_flTimeWeaponIdle = gpGlobals->time;
	if (!m_pPlayer->m_fHeavyArmor)
	m_pPlayer->pev->punchangle.x -= 4;
}

void CGluongun::SecondaryAttack()
{
	entvars_t *pevOwner = VARS( pev->owner );

	if ( m_pPlayer->pev->waterlevel == 3 )
	{
		if ( m_fInAttack != 0 )
		{
			::WaterRadiusDamage( pev->origin, pev, pevOwner, (dmg_gluon.value*WastedAmmo*2) * (mp_wpn_power.value/100), (dmg_gluon.value*WastedAmmo*4) * (mp_wpn_power.value/100), CLASS_NONE, DMG_ENERGYBLAST | DMG_NEVERGIB );
			EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/electro4.wav", 0.55, ATTN_NORM);
			m_iClip = 0;
			SendWeaponAnim(GLUONGUN_IDLE);
			m_fInAttack = 0;
		}
		else
		{
			PlayEmptySound(3);
		}
			m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
		return;
	}

	if ( m_fInAttack == 0 )
	{
		if ( m_iClip <= 0 )
		{
			PlayEmptySound( );
			m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
			return;
		}

		m_iClip-=2;
		WastedAmmo+=2;
		m_iFiredAmmo+=2;
		m_pPlayer->m_flNextAmmoBurn = UTIL_WeaponTimeBase();
		pev->skin = (m_iClip/2);

		m_fInAttack = 1;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
		m_pPlayer->m_flStartCharge = gpGlobals->time;
		m_pPlayer->m_flAmmoStartCharge = UTIL_WeaponTimeBase() + 3.2;

		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/gluongun_charge.wav",1.0 , ATTN_NORM, 0, 110 );
		FX_FireGun(m_pPlayer->pev->angles, m_pPlayer->entindex(), GLUONGUN_CHARGE, 1, FIREGUN_GLUONGUN );

		if ( m_iClip <= 0 )
		{
			StartFire();
			m_fInAttack = 0;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
			m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1;
			m_iClip = 0;
			return;
		}
	}
	else if (m_fInAttack == 1)
	{
		if (m_flTimeWeaponIdle < UTIL_WeaponTimeBase())
		{
			m_fInAttack = 2;
		}
	}
	else
	{
		// during the charging process, eat one bit of ammo every once in a while
		if ( UTIL_WeaponTimeBase() >= m_pPlayer->m_flNextAmmoBurn && m_pPlayer->m_flNextAmmoBurn != 1000 )
		{
			m_iClip-=2;
			WastedAmmo+=2;
			m_iFiredAmmo+=2;
			m_pPlayer->m_flNextAmmoBurn = UTIL_WeaponTimeBase() + 0.8;
			pev->skin = (m_iClip/2);
		}

		if ( m_iClip <= 0 )
		{
			StartFire();
			m_fInAttack = 0;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
			m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1;
			m_iClip = 0;
			return;
		}
		
		if ( UTIL_WeaponTimeBase() >= m_pPlayer->m_flAmmoStartCharge )
			m_pPlayer->m_flNextAmmoBurn = 1000;

		if ( m_pPlayer->m_flStartCharge < gpGlobals->time - 3.4)
		{
			StartFire();
			m_fInAttack = 0;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1;
			m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1;
			return;
		}
	}
}

void CGluongun::StartFire( void )
{
	FX_FireGun(m_pPlayer->pev->angles, m_pPlayer->entindex(), m_pPlayer->m_fHeavyArmor?GLUONGUN_FIRE_SOLID:GLUONGUN_FIRE, 2, FIREGUN_GLUONGUN );
	STOP_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/gluongun_charge.wav" );
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	CGluon2::ShootGluon2( m_pPlayer->pev, m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 28 + gpGlobals->v_right * 5 + gpGlobals->v_up * -6, gpGlobals->v_forward * (WastedAmmo*700), WastedAmmo );
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1;

	if (!m_pPlayer->m_fHeavyArmor)
	m_pPlayer->pev->punchangle.x -= WastedAmmo;
	WastedAmmo = 0;
	m_fInAttack = 0;
}

void CGluongun::WeaponIdle( void )
{
	pev->skin = (m_iClip/2);

	if (m_fInAttack != 0)
	{
		StartFire();
		m_fInAttack = 0;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0;
	}

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	SendWeaponAnim(RANDOM_LONG(GLUONGUN_IDLE, GLUONGUN_IDLE2));
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (15, 25);
}

void CGluongun::Reload( void )
{
	if (m_iClip)
	return;

	if ( m_fInAttack != 0 )
	return;
	DefaultReload( 20, GLUONGUN_RELOAD, 4.8, 1.1 );
}
