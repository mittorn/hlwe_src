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

class CGauss : public CBasePlayerWeapon
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
	float GetFullChargeTime( void );
	int m_iSoundState;

	void BuyPrimaryAmmo( void );
	void SellWeapon( void );
	BOOL m_fPrimaryFire;
};

void CGauss::BuyPrimaryAmmo( void )
{
	BuyAmmo(40, (char*)pszAmmo1(), COST_ENERGY_AMMO);
}

void CGauss::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_GAUSS/2;
}

enum gauss_e
{
	GAUSS_IDLE,
	GAUSS_IDLE2,
	GAUSS_FIDGET,
	GAUSS_SPINUP,
	GAUSS_SPIN,
	GAUSS_FIRE,
	GAUSS_FIRE_SOLID,
	GAUSS_FIRE_SEC,
	GAUSS_FIRE_SEC_SOLID,
	GAUSS_RELOAD,
	GAUSS_DRAW,
	GAUSS_HOLSTER
};

LINK_ENTITY_TO_CLASS( weapon_gauss, CGauss );

float CGauss::GetFullChargeTime( void )
{
	return 3.2;
}

void CGauss::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_gauss.mdl");
	m_iId = WEAPON_GAUSS;
	m_iDefaultAmmo = 80;
	pev->weapons = CLIP_GAUSS;
	FallInit();
}

void CGauss::Precache( void )
{
	PRECACHE_MODEL("models/v_gauss.mdl");
	PRECACHE_MODEL("models/p_gauss.mdl");

	PRECACHE_MODEL( "sprites/smoke.spr" );

	PRECACHE_SOUND("weapons/gauss_fire.wav");
	PRECACHE_SOUND("weapons/gauss_fire2.wav");
	PRECACHE_SOUND("weapons/electro4.wav");
	PRECACHE_SOUND("weapons/electro5.wav");
	PRECACHE_SOUND("weapons/electro6.wav");
	PRECACHE_SOUND("weapons/gauss_charge.wav");
}

int CGauss::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "uranium";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 40;
	p->iSlot = 6;
	p->iPosition = 5;
	p->iId = WEAPON_GAUSS;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iWeight = GAUSS_WEIGHT;
	p->iMaxspeed = GAUSS_MAXSPEED;
	return 1;
}

BOOL CGauss::Deploy( )
{
	return DefaultDeploy( "models/v_gauss.mdl", "models/p_gauss.mdl", GAUSS_DRAW, "gauss" );
}

void CGauss::Holster( )
{
	if (m_fInAttack > 0)
	{
		STOP_SOUND( ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/gauss_charge.wav" );
		StartFire();
	}

	m_fInReload = FALSE;
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.6;
	SendWeaponAnim( GAUSS_HOLSTER );
	m_fInAttack = 0;
}

void CGauss::PrimaryAttack()
{
	if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 0)
 	{
		PlayEmptySound(3);
		m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		return;
	}
	m_iClip -= 2;
	m_iFiredAmmo+=2;
	m_fPrimaryFire = TRUE;

	StartFire();
	m_fInAttack = 0;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.25;
}

void CGauss::SecondaryAttack()
{
	entvars_t *pevOwner = VARS( pev->owner );

	if ( m_pPlayer->pev->waterlevel == 3 )
	{
		if ( m_fInAttack != 0 )
		{
			::WaterRadiusDamage( pev->origin, pev, pevOwner, (dmg_gauss_secondary.value*2) * (mp_wpn_power.value/100), (dmg_gauss_secondary.value*4) * (mp_wpn_power.value/100), CLASS_NONE, DMG_SHOCK | DMG_NEVERGIB);
			EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/electro4.wav", 0.55, ATTN_NORM);
			m_iClip = 0;
			SendWeaponAnim( GAUSS_IDLE );
			m_fInAttack = 0;
		}
		else
		{
			PlayEmptySound(3);
		}
		m_flNextSecondaryAttack = m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
		return;
	}

	if ( m_fInAttack == 0 )
	{
		if ( m_iClip <= 0 )
		{
			PlayEmptySound(3);
			m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
			return;
		}

		m_fPrimaryFire = FALSE;

		m_iClip-=2;
		m_iFiredAmmo++;
		m_pPlayer->m_flNextAmmoBurn = UTIL_WeaponTimeBase();

		SendWeaponAnim( GAUSS_SPINUP );
		m_fInAttack = 1;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
		m_pPlayer->m_flStartCharge = gpGlobals->time;
		m_pPlayer->m_flAmmoStartCharge = UTIL_WeaponTimeBase() + GetFullChargeTime();
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/gauss_charge.wav",1.0 , ATTN_NORM, 0, 110 );
		m_iSoundState = SND_CHANGE_PITCH;
		if ( m_iClip <= 0 )
		{
			StartFire();
			m_fInAttack = 0;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
			m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1;
			m_iClip = 0;
			return;
		}
	}
	else if (m_fInAttack == 1)
	{
		if (m_flTimeWeaponIdle < UTIL_WeaponTimeBase())
		{
			SendWeaponAnim( GAUSS_SPIN );
			m_fInAttack = 2;
		}
	}
	else
	{
		// during the charging process, eat one bit of ammo every once in a while
		if ( UTIL_WeaponTimeBase() >= m_pPlayer->m_flNextAmmoBurn && m_pPlayer->m_flNextAmmoBurn != 1000 )
		{
			m_iClip-=2;
			m_iFiredAmmo++;
			m_pPlayer->m_flNextAmmoBurn = UTIL_WeaponTimeBase() + 0.7;
		}

		if ( m_iClip <= 0 )
		{
			// out of ammo! force the gun to fire
			StartFire();
			m_fInAttack = 0;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
			m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1;
			m_iClip = 0;
			return;
		}
		
		if ( UTIL_WeaponTimeBase() >= m_pPlayer->m_flAmmoStartCharge )
		{
			// don't eat any more ammo after gun is fully charged.
			m_pPlayer->m_flNextAmmoBurn = 1000;
		}

		int pitch = ( gpGlobals->time - m_pPlayer->m_flStartCharge ) * ( 150 / GetFullChargeTime() ) + 100;
		if ( pitch > 250 ) 
			 pitch = 250;
		
		if ( m_iSoundState == 0 )
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/gauss_charge.wav", 1.0, ATTN_NORM, m_iSoundState, pitch);

		m_iSoundState = SND_CHANGE_PITCH;

		if ( m_pPlayer->m_flStartCharge < gpGlobals->time - 20 )
		{
			// Player charged up too long. Zap him.
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/electro4.wav", 1.0, ATTN_NORM, 0, 80 + RANDOM_LONG(0,0x3f));
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM,   "weapons/electro6.wav", 1.0, ATTN_NORM, 0, 75 + RANDOM_LONG(0,0x3f));
			STOP_SOUND( ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/gauss_charge.wav" );
			
			m_fInAttack = 0;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
			m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
				
			m_pPlayer->TakeDamage( VARS(eoNullEntity), VARS(eoNullEntity), 105, DMG_SHOCK | DMG_NEVERGIB );
			UTIL_ScreenFade( m_pPlayer, Vector(255,128,0), 2, 0.5, 128, FFADE_IN );

			SendWeaponAnim( GAUSS_IDLE );
			return;
		}
	}
}

void CGauss::StartFire( void )
{
	float flDamage;

	if ( gpGlobals->time - m_pPlayer->m_flStartCharge > GetFullChargeTime() )
	flDamage = dmg_gauss_secondary.value * (mp_wpn_power.value/100);
	else
	flDamage = (dmg_gauss_secondary.value * (mp_wpn_power.value/100)) * (( gpGlobals->time - m_pPlayer->m_flStartCharge) / GetFullChargeTime() );

	if (m_fPrimaryFire)
		flDamage = dmg_gauss.value * (mp_wpn_power.value/100);

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	if(!m_fPrimaryFire)
	{
		m_pPlayer->FireBeam(m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 12 + gpGlobals->v_right * 2.5 + gpGlobals->v_up * -2, gpGlobals->v_forward, BEAM_GAUSSCHARGED, flDamage, m_pPlayer->pev);
		FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?GAUSS_FIRE_SEC_SOLID:GAUSS_FIRE_SEC, 1, FIREGUN_GAUSS );

		if (!m_pPlayer->m_fHeavyArmor)
		{
			m_pPlayer->pev->punchangle.x -= 9;
	 		if (m_fInAttack != 3)
			{
				UTIL_MakeVectors (m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
				m_pPlayer->pev->velocity = m_pPlayer->pev->velocity - gpGlobals->v_forward * flDamage * 5;
			}
		}
	}
	else
	{
		m_pPlayer->FireBeam(m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 10 + gpGlobals->v_right * 2.5 + gpGlobals->v_up * -2, gpGlobals->v_forward, BEAM_GAUSS, flDamage, m_pPlayer->pev);
		FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?GAUSS_FIRE_SOLID:GAUSS_FIRE, 0, FIREGUN_GAUSS );

		if (!m_pPlayer->m_fHeavyArmor)
			m_pPlayer->pev->punchangle.x -= 2.5;
	}
}

void CGauss::WeaponIdle( void )
{
	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	if (m_fInAttack != 0)
	{
		StartFire();
		m_fInAttack = 0;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0;
	}
	else
	{
	int iAnim;
	switch ( RANDOM_LONG( 0, 2 ) )
	{
	case 0:	
		iAnim = GAUSS_IDLE;	
		break;
	
	case 1:	
		iAnim = GAUSS_IDLE2;	
		break;
	
	default:
	case 2:	
		iAnim = GAUSS_FIDGET;	
		break;
	}

	SendWeaponAnim( iAnim );

	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 10, 15 );
		
	}
}

void CGauss::Reload( void )
{
	if (m_iClip)
	return;

	if ( m_fInAttack != 0 )
	return;

	DefaultReload( 40, GAUSS_RELOAD, 4.5, 1.2 );
}
                                                       