#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "decals.h"
#include "player.h"
#include "gamerules.h"

class CBandsaw : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);
	void Holster( void );

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	void WeaponIdle( void );
	BOOL Deploy( void );
	void SellWeapon( void );
	void BuyPrimaryAmmo( void );
};
LINK_ENTITY_TO_CLASS( weapon_bandsaw, CBandsaw );

void CBandsaw::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_BANDSAW/2;
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] = 0;
}

void CBandsaw::BuyPrimaryAmmo( void )
{
if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 40)
	BuyAmmo(20, (char*)pszAmmo1(), COST_HANDGUN_AMMO);
}

enum bandsaw_e
{
	BANDSAW_IDLE,
	BANDSAW_SHOOT,
	BANDSAW_DRAW,
	BANDSAW_HOLSTER
};


void CBandsaw::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_bandsaw.mdl");
	m_iId = WEAPON_BANDSAW;
	m_iDefaultAmmo = 60;
	FallInit();
}

void CBandsaw::Precache( void )
{
	PRECACHE_MODEL("models/v_bandsaw.mdl");
	PRECACHE_MODEL("models/p_bandsaw.mdl");

	PRECACHE_SOUND("weapons/bandsaw_attack.wav");
	PRECACHE_SOUND("weapons/bandsaw_idle.wav");
}

int CBandsaw::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "gasoline";
	p->iMaxAmmo1 = 60;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 0;
	p->iPosition = 3;
	p->iId = WEAPON_BANDSAW;
	p->iWeight = BANDSAW_WEIGHT;
	p->iMaxspeed = BANDSAW_MAXSPEED;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	return 1;
}

BOOL CBandsaw::Deploy( )
{
	return DefaultDeploy( "models/v_bandsaw.mdl", "models/p_bandsaw.mdl", BANDSAW_DRAW, "gauss" );
}

void CBandsaw::Holster( )
{
	STOP_SOUND( ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/bandsaw_idle.wav" );
	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.6;
	SendWeaponAnim( BANDSAW_HOLSTER );
}

void CBandsaw::PrimaryAttack()
{
	if (m_pPlayer->pev->waterlevel == 3 || m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
 	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		return;
	}
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + 0.1;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 5, 7 );

	if ( gpGlobals->time >= m_flTimeUpdate)
	{
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
		m_flTimeUpdate = gpGlobals->time + 1;
	}
	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );
	m_pPlayer->FireMagnumBullets(m_pPlayer->GetGunPosition(), gpGlobals->v_forward, VECTOR_CONE_0DEGREES, 60, BULLET_BANDSAW, m_pPlayer->pev);
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), BANDSAW_SHOOT, 0, FIREGUN_BANDSAW );
}

void CBandsaw::SecondaryAttack()
{
	PrimaryAttack();
}

void CBandsaw::WeaponIdle( void )
{
	if (m_pPlayer->pev->waterlevel != 3)
	{
		if ( gpGlobals->time >= m_flShockTime )
		{
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/bandsaw_idle.wav", 0.9, ATTN_NORM );
			m_flShockTime = gpGlobals->time + 0.7;
		}
	}

	if (m_flTimeWeaponIdle > gpGlobals->time)
		return;

	SendWeaponAnim( BANDSAW_IDLE );
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (10,15);
}