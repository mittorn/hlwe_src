#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"

class Cm249 : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);

	void PrimaryAttack( void );
	BOOL Deploy( void );
	void Reload( void );
	void Holster( void );
	void WeaponIdle( void );

	void BuyPrimaryAmmo( void );
	void SellWeapon( void );
};

void Cm249::BuyPrimaryAmmo( void )
{
	BuyAmmo(100, (char*)pszAmmo1(), COST_HEAVY_AMMO);
}

void Cm249::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_M249/2;
}

enum m249_e
{
	M249_IDLE,
	M249_FIRE,
	M249_FIRE_SOLID,
	M249_START_RELOAD,
	M249_RELOAD,
	M249_DRAW,
	M249_HOLSTER
};

LINK_ENTITY_TO_CLASS( weapon_m249, Cm249 );

void Cm249::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_m249.mdl");
	m_iId = WEAPON_M249;
	m_iDefaultAmmo = 200;
	pev->weapons = CLIP_M249;
	FallInit();
}

void Cm249::Precache( void )
{
	PRECACHE_MODEL("models/v_m249.mdl");
	PRECACHE_MODEL("models/p_m249.mdl");
	PRECACHE_SOUND ("weapons/m249_fire.wav");
}

int Cm249::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "556Nato";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 100;
	p->iSlot = 4;
	p->iPosition = 0;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iId = WEAPON_M249;
	p->iWeight = M249_WEIGHT;
	p->iMaxspeed = M249_MAXSPEED;
	return 1;
}

BOOL Cm249::Deploy( )
{
	if(m_iClip < 10 && m_iClip)
		pev->body = m_iClip;
	else
		pev->body = 10;

	return DefaultDeploy( "models/v_m249.mdl", "models/p_m249.mdl", M249_DRAW, "saw", 0.9 );
}

void Cm249::Holster( )
{
	m_fInReload = FALSE;
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.7;
	SendWeaponAnim( M249_HOLSTER );
}

void Cm249::PrimaryAttack()
{
	if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 0)
 	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		return;
	}

	if (!m_pPlayer->m_fHeavyArmor)
	{
		UTIL_MakeVectors (m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
		float flZVel = m_pPlayer->pev->velocity.z;
		m_pPlayer->pev->velocity = m_pPlayer->pev->velocity - gpGlobals->v_forward * 35;
		m_pPlayer->pev->velocity.z = flZVel;
	}

	m_iClip--;
	m_iFiredAmmo++;

	if(m_iClip < 10 && m_iClip)
		pev->body = m_iClip;
	else
		pev->body = 10;

	if ( gpGlobals->time >= m_flShockTime )
	{
		SendWeaponAnim( (m_pPlayer->m_fHeavyArmor)?M249_FIRE_SOLID:M249_FIRE );
		m_flShockTime = gpGlobals->time + 0.2;
	}
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	m_pPlayer->FireMagnumBullets(m_pPlayer->GetGunPosition(), gpGlobals->v_forward, (m_pPlayer->pev->flags & FL_DUCKING)?VECTOR_CONE_6DEGREES:VECTOR_CONE_7DEGREES, 16384, BULLET_556Nato, m_pPlayer->pev);
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), 200, 0, FIREGUN_M249 );

	m_flNextPrimaryAttack = gpGlobals->time  + 0.07;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (5,7);
	if (!m_pPlayer->m_fHeavyArmor)
	{
		m_pPlayer->pev->punchangle.x -= RANDOM_FLOAT(-1,1.7);
		m_pPlayer->pev->punchangle.y -= 0.4;
	}
}

void Cm249 :: Reload( void )
{                      
	pev->body = 0;

	DefaultReload( 100, M249_START_RELOAD, 2.6, 2.0 );
	m_fInSpecialReload = 1;          
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 6;
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time  + 5.5;
}                                                       

void Cm249::WeaponIdle( void )
{
	if (m_fInSpecialReload)
	{
		pev->body = 10;
		SendWeaponAnim( M249_RELOAD );
	 	m_fInSpecialReload = 0;
	}   
	if ( m_flTimeWeaponIdle > gpGlobals->time )
		return;

	SendWeaponAnim( M249_IDLE );
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (8, 12);
}
