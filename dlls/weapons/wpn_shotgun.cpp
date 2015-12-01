#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"

class CShotgun : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);

	void PrimaryAttack( void );
	BOOL Deploy( );
	void Holster( void );
	void Reload( void );
	void WeaponIdle( void );

	void BuyPrimaryAmmo( void );
	void SellWeapon( void );
	void EXPORT shellcasing( void );
};

enum shotgun_e
{
	SHOTGUN_IDLE1,
	SHOTGUN_IDLE2,
	SHOTGUN_IDLE3,
	SHOTGUN_FIRE,
	SHOTGUN_FIRE_SOLID,
	SHOTGUN_RELOAD,
	SHOTGUN_DRAW,
	SHOTGUN_HOLSTER,
	SHOTGUN_PUMP,
	SHOTGUN_START_RELOAD
};

LINK_ENTITY_TO_CLASS( weapon_shotgun, CShotgun );

void CShotgun::BuyPrimaryAmmo( void )
{
	BuyAmmo(8, (char*)pszAmmo1(), COST_ASSAULT_AMMO);
}

void CShotgun::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_SHOTGUN/2;
}

void CShotgun::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_shotgun.mdl");
	m_iId = WEAPON_SHOTGUN;
	m_iDefaultAmmo = 16;
	FallInit();
}

void CShotgun::Precache( void )
{
	PRECACHE_MODEL("models/v_shotgun.mdl");
	PRECACHE_MODEL("models/p_shotgun.mdl");

	PRECACHE_SOUND ("weapons/shotgun_fire.wav");
	PRECACHE_SOUND ("weapons/shotgun_reload1.wav");
	PRECACHE_SOUND ("weapons/shotgun_reload2.wav");
}

int CShotgun::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "buckshot";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 8;
	p->iSlot = 2;
	p->iPosition = 0;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iId = WEAPON_SHOTGUN;
	p->iWeight = SHOTGUN_WEIGHT;
	p->iMaxspeed = SHOTGUN_MAXSPEED;
	return 1;
}

BOOL CShotgun::Deploy( )
{
	return DefaultDeploy( "models/v_shotgun.mdl", "models/p_shotgun.mdl", SHOTGUN_DRAW, "shotgun" );
}

void CShotgun::Holster( )
{
	SendWeaponAnim( SHOTGUN_HOLSTER );
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
}

void CShotgun::shellcasing() 
{
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), 200, 0, FIREGUN_SHOTGUN_BR_SHELL );
}

void CShotgun::PrimaryAttack()
{
	if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 0)
 	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		return;
	}

	//LLAPb begin
	if (m_fInSpecialReload)
	{
		SendWeaponAnim( SHOTGUN_PUMP );
		m_flNextPrimaryAttack = m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.7;

		if(m_iClip<8 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] && m_fInSpecialReload == 2)
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
	for (int i=0; i<8; i++)
		m_pPlayer->FireMagnumBullets(m_pPlayer->GetGunPosition(), gpGlobals->v_forward, (m_pPlayer->pev->flags & FL_DUCKING)?VECTOR_CONE_10DEGREES:VECTOR_CONE_11DEGREES, 16384, BULLET_BUCKSHOT, m_pPlayer->pev);
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?SHOTGUN_FIRE_SOLID:SHOTGUN_FIRE, m_pPlayer->m_fHeavyArmor?1:0, FIREGUN_SHOTGUN );

	SetThink( shellcasing ); 
	(m_pPlayer->m_fHeavyArmor)?pev->nextthink = gpGlobals->time + 0.25:pev->nextthink = gpGlobals->time + 0.5;

	if (m_iClip != 0)
		m_flPumpTime = gpGlobals->time + 0.5;

	(m_pPlayer->m_fHeavyArmor)?m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5:m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.75;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1;
	m_fInSpecialReload = 0;
}

void CShotgun::Reload( void )
{
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip == 8)
		return;

	// don't reload until recoil is done
	if (m_flNextPrimaryAttack > UTIL_WeaponTimeBase())
		return;

	// check to see if we're ready to reload
	if (m_fInSpecialReload == 0)
	{
		SendWeaponAnim( SHOTGUN_START_RELOAD );
		m_fInSpecialReload = 1;
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.8;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.8;
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

		SendWeaponAnim( SHOTGUN_RELOAD );

		m_flTimeUpdate = UTIL_WeaponTimeBase() + 0.5;
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


void CShotgun::WeaponIdle( void )
{
	if (m_flTimeWeaponIdle <  UTIL_WeaponTimeBase() )
	{
		if (m_iClip == 0 && m_fInSpecialReload == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
		{
			Reload( );
		}
		else if (m_fInSpecialReload != 0)
		{
			if (m_iClip != 8 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
			{
				Reload( );
			}
			else
			{
				// reload debounce has timed out
				SendWeaponAnim( SHOTGUN_PUMP );
				
				m_fInSpecialReload = 0;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.5;
				//LLAPb begin
				m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.1;
				//LLAPb end
			}
		}
		else
		{
			int iAnim;
			float flRand = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 0, 1 );
			if (flRand <= 0.8)
			{
				iAnim = SHOTGUN_IDLE1;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (60.0/12.0);// * RANDOM_LONG(2, 5);
			}
			else if (flRand <= 0.95)
			{
				iAnim = SHOTGUN_IDLE2;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (20.0/9.0);
			}
			else
			{
				iAnim = SHOTGUN_IDLE3;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (20.0/9.0);
			}
			SendWeaponAnim( iAnim );
		}
	}
}
