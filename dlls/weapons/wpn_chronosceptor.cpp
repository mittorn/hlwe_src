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
#include "projectiles.h"

enum Chronosceptor_e
{
	CHRONO_IDLE,
	CHRONO_IDLE2,
	CHRONO_SPINUP,
	CHRONO_SPIN,
	CHRONO_FIRE,
	CHRONO_FIRE_SOLID,
	CHRONO_DISARM_CELL,
	CHRONO_RELOAD,
	CHRONO_DRAW,
	CHRONO_HOLSTER
};

class CChronosceptor : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	void UpdateScreen ( void );
	void EXPORT DisarmClip( void );

	BOOL Deploy( void );
	void Reload( void );
	void Holster( void );
	void WeaponIdle( void );
	void StartFire( void );
	BOOL ShouldWeaponIdle( void ) {return TRUE; };

	void BuyPrimaryAmmo( void );
	void SellWeapon( void );
};
LINK_ENTITY_TO_CLASS( weapon_chronosceptor, CChronosceptor );

void CChronosceptor::BuyPrimaryAmmo( void )
{
	BuyAmmo(4, (char*)pszAmmo1(), COST_EXPERIMENTAL_AMMO);
}

void CChronosceptor::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_CHRONOSCEPTOR/2;
}

void CChronosceptor::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_chronosceptor.mdl");
	m_iId = WEAPON_CHRONOSCEPTOR;
	m_iDefaultAmmo = 8;
	pev->weapons = CLIP_CHRONOSCEPTOR;
	FallInit();
}

void CChronosceptor::Precache( void )
{
	PRECACHE_MODEL("models/v_chronosceptor.mdl");
	PRECACHE_MODEL("models/p_chronosceptor.mdl");

	PRECACHE_SOUND("weapons/chronoclip_bounce.wav");
	PRECACHE_SOUND("weapons/chronoclip_explode.wav");
	PRECACHE_SOUND("weapons/chronosceptor_fire.wav");
	PRECACHE_SOUND("weapons/chronosceptor_blackhole.wav");
	PRECACHE_SOUND("weapons/chronosceptor_idle.wav");
}

int CChronosceptor::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "quark";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 4;
	p->iSlot = 8;
	p->iPosition = 6;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iId = WEAPON_CHRONOSCEPTOR;
	p->iWeight = CHRONOSCEPTOR_WEIGHT;
	p->iMaxspeed = CHRONOSCEPTOR_MAXSPEED;
	return 1;
}

BOOL CChronosceptor::Deploy( )
{
	return DefaultDeploy( "models/v_chronosceptor.mdl", "models/p_chronosceptor.mdl", CHRONO_DRAW, "minigun", 0.9 );
}

void CChronosceptor::Holster( )
{
	if (m_fInAttack > 0)
	{
		FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), 200, 2, FIREGUN_CHRONOSCEPTOR );
		m_fInAttack = 0;
	}
	STOP_SOUND( ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/chronosceptor_idle.wav" );
	m_fInReload = FALSE;
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.7;
	SendWeaponAnim( CHRONO_HOLSTER );
}

void CChronosceptor::StartFire()
{
	if (m_pPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound(2);
		m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		return;
	}

	if ( m_fInAttack == 0 )
	{
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1;
		return;
	}
	m_iClip--;
	m_iFiredAmmo++;
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?CHRONO_FIRE_SOLID:CHRONO_FIRE, 0, FIREGUN_CHRONOSCEPTOR );
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	Vector vecSrc = m_pPlayer->GetGunPosition( );
	Vector vecDir = gpGlobals->v_forward;

	TraceResult tr;
	UTIL_TraceLine(vecSrc, vecSrc + vecDir * 16384, dont_ignore_monsters, m_pPlayer->edict(), &tr);
	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	if (UTIL_PointContents(tr.vecEndPos) != CONTENTS_SKY)
	{
		CBlackHole::ShootBlackHole( m_pPlayer->pev, tr.vecEndPos + (tr.vecPlaneNormal * 100));
		EMIT_SOUND(ENT(pev), CHAN_AUTO, "weapons/chronosceptor_blackhole.wav", 1, ATTN_LOW);
	}
	if (!m_pPlayer->m_fHeavyArmor)
	m_pPlayer->pev->punchangle.x -= 3;
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1;
}

void CChronosceptor::DisarmClip()
{
	if ( m_fInAttack == 0)
	{
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1;
		return;
	}

	if (m_iClip <= 1)
	{
		PlayEmptySound( );
		return;
	}
	m_iClip -= 2;

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	Vector vecSrc = m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 16 + gpGlobals->v_up * -16;
	Vector vecThrow = gpGlobals->v_forward;

	CChronoClip::ShootChronoClip( m_pPlayer->pev, vecSrc, vecThrow, RANDOM_LONG(1, 20) );
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1;
}

void CChronosceptor::PrimaryAttack()
{
	if ( m_fInAttack == 0)
	{
		if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 0)
	 	{
			PlayEmptySound(1);
			m_flNextPrimaryAttack = gpGlobals->time + 0.5;
			return;
		}
		FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), CHRONO_SPINUP, 1, FIREGUN_CHRONOSCEPTOR );

		m_fInAttack = 1;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3;
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

void CChronosceptor::SecondaryAttack( void )
{
	if ( m_fInAttack == 0)
	{
		if (m_iClip <= 1)
	 	{
			PlayEmptySound( );
			m_flNextSecondaryAttack = gpGlobals->time + 0.5;
			return;
		}

		m_fInAttack = 2;
		SendWeaponAnim( CHRONO_DISARM_CELL );
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3.5;
	}
	else if (m_fInAttack == 2)
	{
		if (m_flTimeWeaponIdle < UTIL_WeaponTimeBase())
		{
			WeaponIdle();
			return;
		}
	}
}

void CChronosceptor::Reload( void )
{
	if (m_iClip)
	return;

	DefaultReload( 4, CHRONO_RELOAD, 4.5, 1.4 );
}

void CChronosceptor::UpdateScreen ( void )
{
	if ( m_flTimeUpdate > UTIL_WeaponTimeBase() )
	return;

	if( pev->skin >= 12 ) pev->skin = 0;
	pev->skin++;
	m_flTimeUpdate = UTIL_WeaponTimeBase() + 0.3;
}

void CChronosceptor::WeaponIdle( void )
{
	UpdateScreen();

	if ( gpGlobals->time >= m_flShockTime )
	{
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/chronosceptor_idle.wav", 0.9, ATTN_NORM );
		m_flShockTime = gpGlobals->time +1.6;
	}

	if (m_flTimeWeaponIdle > gpGlobals->time)
		return;

	if (m_fInAttack==1)
	{
		StartFire();
		m_fInAttack = 0;
		return;
	}
	else if (m_fInAttack==2)
	{
		DisarmClip();
		m_fInAttack = 0;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.6;
		return;
	}

	switch (RANDOM_LONG(0,1))
	{
		case 0:	SendWeaponAnim( CHRONO_IDLE ); break;
		case 1:	SendWeaponAnim( CHRONO_IDLE2 ); break;
	}
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 10, 15 );
}