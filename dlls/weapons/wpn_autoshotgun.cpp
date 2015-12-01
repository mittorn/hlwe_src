#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"

class Cautoshotgun : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	BOOL Deploy( );
	void Reload( void );
	void Holster( void );
	void WeaponIdle( void );

	void BuyPrimaryAmmo( void );
	void SellWeapon( void );
};

void Cautoshotgun::BuyPrimaryAmmo( void )
{
	BuyAmmo(10, (char*)pszAmmo1(), COST_ASSAULT_AMMO);
}

void Cautoshotgun::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_AUTOSHOTGUN/2;
}

enum autoshotgun_e
{
	AUTOSHOTGUN_IDLE,
	AUTOSHOTGUN_IDLE2,
	AUTOSHOTGUN_IDLE3,
	AUTOSHOTGUN_SHOOT,
	AUTOSHOTGUN_SHOOT_SOLID,
	AUTOSHOTGUN_RELOAD,
	AUTOSHOTGUN_START_RELOAD,
	AUTOSHOTGUN_END_RELOAD,
	AUTOSHOTGUN_DRAW,
	AUTOSHOTGUN_HOLSTER
};

LINK_ENTITY_TO_CLASS( weapon_autoshotgun, Cautoshotgun );

void Cautoshotgun::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_autoshotgun.mdl");
	m_iId = WEAPON_AUTOSHOTGUN;
	m_iDefaultAmmo = 20;
	FallInit();
}

void Cautoshotgun::Precache( void )
{
	PRECACHE_MODEL("models/v_autoshotgun.mdl");
	PRECACHE_MODEL("models/p_autoshotgun.mdl");

	PRECACHE_SOUND ("weapons/autoshotgun_fire.wav");
}

int Cautoshotgun::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "10mmbuckshot";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 10;
	p->iSlot = 2;
	p->iPosition = 1;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iId = WEAPON_AUTOSHOTGUN;
	p->iWeight = AUTOSHOTGUN_WEIGHT;
	p->iMaxspeed = AUTOSHOTGUN_MAXSPEED;
	return 1;
}

BOOL Cautoshotgun::Deploy( )
{
	return DefaultDeploy( "models/v_autoshotgun.mdl", "models/p_autoshotgun.mdl", AUTOSHOTGUN_DRAW, "shotgun", 0.7 );
}

void Cautoshotgun::PrimaryAttack()
{
	if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 0)
 	{
		PlayEmptySound( );
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
		return;
	}

	//LLAPb begin
	if (m_fInSpecialReload)
	{
		SendWeaponAnim( AUTOSHOTGUN_END_RELOAD );
		m_flNextPrimaryAttack = m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.7;

		if(m_iClip<10 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] && m_fInSpecialReload == 2)
		{
			m_iClip += 1;
			m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 1;
		}

		m_fInSpecialReload = 0;
		return;
	}
	//LLAPb end

	m_iClip--;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	for (int i=0; i<6; i++)
		m_pPlayer->FireMagnumBullets(m_pPlayer->GetGunPosition(), gpGlobals->v_forward, (m_pPlayer->pev->flags & FL_DUCKING)?VECTOR_CONE_13DEGREES:VECTOR_CONE_14DEGREES, 16384, BULLET_10MMBUCKSHOT, m_pPlayer->pev);
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?AUTOSHOTGUN_SHOOT_SOLID:AUTOSHOTGUN_SHOOT, m_pPlayer->m_fHeavyArmor?1:0, FIREGUN_AUTOSHOTGUN );

	if (m_iClip != 0)
		m_flPumpTime = gpGlobals->time + 0.5;

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.25;
	if (m_iClip != 0)
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 5.0;
	else
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.75;
	m_fInSpecialReload = 0;
}

void Cautoshotgun::SecondaryAttack( void )
{
	if (m_fInZoom == 0) 
	{
		m_pPlayer->m_iFOV = 70;
		m_fInZoom = 1;
	}
	else if (m_fInZoom == 1) 
	{
		m_pPlayer->m_iFOV = 90;
		m_fInZoom = 0;
	}
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
}

void Cautoshotgun::Holster( )
{
	m_fInReload = FALSE;

	if (m_pPlayer->m_iFOV != 90)
	m_pPlayer->m_iFOV = 90;

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.6;
	SendWeaponAnim( AUTOSHOTGUN_HOLSTER );
}

void Cautoshotgun::Reload( void )
{
	if (m_pPlayer->m_iFOV != 90)
	m_pPlayer->m_iFOV = 90;

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip == 10)
		return;

	// don't reload until recoil is done
	if (m_flNextPrimaryAttack > UTIL_WeaponTimeBase())
		return;

	// check to see if we're ready to reload
	if (m_fInSpecialReload == 0)
	{
		SendWeaponAnim( AUTOSHOTGUN_START_RELOAD );
		m_fInSpecialReload = 1;
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.6;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.6;
		//LLAPb begin
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.0;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0;
		//LLAPb end
		return;
	}
	else if (m_fInSpecialReload == 1)
	{
		if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
			return;
		// was waiting for gun to move to side
		m_fInSpecialReload = 2;

		if (RANDOM_LONG(0,1))
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/shotgun_reload1.wav", 1, ATTN_NORM, 0, 85 + RANDOM_LONG(0,0x1f));
		else
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/shotgun_reload2.wav", 1, ATTN_NORM, 0, 85 + RANDOM_LONG(0,0x1f));

		SendWeaponAnim( AUTOSHOTGUN_RELOAD );

		m_flTimeUpdate = UTIL_WeaponTimeBase() + 0.7;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
		//LLAPb begin
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
		//LLAPb end
	}
	else
	{
		// Add them to the clip
		m_iClip += 1;
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 1;
		m_fInSpecialReload = 1;
	}
}


void Cautoshotgun::WeaponIdle( void )
{
	if (m_flTimeWeaponIdle <  UTIL_WeaponTimeBase() )
	{
		if (m_iClip == 0 && m_fInSpecialReload == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
		{
			Reload( );
		}
		else if (m_fInSpecialReload != 0)
		{
			if (m_iClip != 10 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
			{
				Reload( );
			}
			else
			{
				// reload debounce has timed out
				SendWeaponAnim( AUTOSHOTGUN_END_RELOAD );
				
				m_fInSpecialReload = 0;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.5;
				//LLAPb begin
				m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.1;
				//LLAPb end
			}
		}
		else
		{
	if (m_pPlayer->m_iFOV != 90)
		return;

			int iAnim;
			float flRand = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 0, 1 );
			if (flRand <= 0.8)
			{
				iAnim = AUTOSHOTGUN_IDLE2;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (60.0/12.0);// * RANDOM_LONG(2, 5);
			}
			else if (flRand <= 0.95)
			{
				iAnim = AUTOSHOTGUN_IDLE3;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (20.0/9.0);
			}
			else
			{
				iAnim = AUTOSHOTGUN_IDLE;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (20.0/9.0);
			}
			SendWeaponAnim( iAnim );
		}
	}
}