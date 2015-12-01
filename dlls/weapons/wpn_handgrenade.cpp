#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "projectiles.h"

class CHandGrenade : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);

	void PrimaryAttack( void );
	BOOL Deploy( void );
	BOOL CanHolster( void );
	void Holster( void );
	void WeaponIdle( void );
	float m_flStartThrow;
	float m_flReleaseThrow;
	void SellWeapon( void );
};

void CHandGrenade::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] * COST_HANDGRENADE/2);
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] = 0;
}

enum handgrenade_e
{
	HANDGRENADE_IDLE = 0,
	HANDGRENADE_FIDGET,
	HANDGRENADE_PINPULL,
	HANDGRENADE_THROW1,	// toss
	HANDGRENADE_THROW2,	// medium
	HANDGRENADE_THROW3,	// hard
	HANDGRENADE_HOLSTER,
	HANDGRENADE_DRAW
};
LINK_ENTITY_TO_CLASS( weapon_handgrenade, CHandGrenade );

void CHandGrenade::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_grenade.mdl");
	m_iId = WEAPON_HANDGRENADE;
	m_iDefaultAmmo = 1;
	FallInit();
}

void CHandGrenade::Precache( void )
{
	PRECACHE_MODEL("models/v_grenades_all.mdl");
	PRECACHE_MODEL("models/p_grenade.mdl");
	PRECACHE_SOUND("weapons/grenade_explode.wav");
}

int CHandGrenade::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "Hand Grenade";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 7;
	p->iPosition = 1;
	p->iId = WEAPON_HANDGRENADE;
	p->iWeight = HANDGRENADE_WEIGHT;
	p->iFlags = ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE;
	p->iMaxspeed = HANDGRENADE_MAXSPEED;
	return 1;
}

BOOL CHandGrenade::Deploy( )
{
	pev->body = 1;
	m_flReleaseThrow = -1;
	return DefaultDeploy( "models/v_grenades_all.mdl", "models/p_grenade.mdl", HANDGRENADE_DRAW, "grenade" );
}

BOOL CHandGrenade::CanHolster( void )
{
	return ( m_flStartThrow == 0 );
}

void CHandGrenade::Holster( )
{
	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.5;

	if ( m_pPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] )
	{
		SendWeaponAnim( HANDGRENADE_HOLSTER );
	}
	else
	{
		m_pPlayer->pev->weapons &= ~(1<<WEAPON_HANDGRENADE);
		SetThink( DestroyItem );
		pev->nextthink = gpGlobals->time + 0.1;
	}
}

void CHandGrenade::PrimaryAttack()
{
	if ( !m_flStartThrow && m_pPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] > 0 )
	{
		m_flStartThrow = gpGlobals->time;
		m_flReleaseThrow = 0;

		SendWeaponAnim( HANDGRENADE_PINPULL );
		m_flTimeWeaponIdle = gpGlobals->time + 0.5;
	}
}

void CHandGrenade::WeaponIdle( void )
{
	if (m_flReleaseThrow == 0)
		m_flReleaseThrow = gpGlobals->time;

	if (m_flTimeWeaponIdle > gpGlobals->time)
	return;

	if (m_flStartThrow)
	{
		Vector angThrow = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;

		if (angThrow.x < 0)
			angThrow.x = -10 + angThrow.x * ((90 - 10) / 90.0);
		else
			angThrow.x = -10 + angThrow.x * ((90 + 10) / 90.0);

			float flVel = (90 - angThrow.x) * 4;
		if (flVel > 700)
			flVel = 700;

		UTIL_MakeVectors( angThrow );
		Vector vecSrc = m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 16;
		Vector vecThrow = gpGlobals->v_forward * flVel + m_pPlayer->pev->velocity;

		float time = m_flStartThrow - gpGlobals->time + 3.5;
		if (time < 0)
			time = 0;

		CGrenade::ShootGrenade( m_pPlayer->pev, vecSrc, vecThrow, time );
		SendWeaponAnim( HANDGRENADE_THROW1 );
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

		m_flStartThrow = 0;
		m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		m_flTimeWeaponIdle = gpGlobals->time + 0.5;

		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;

		if ( !m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] )
		{
			m_flTimeWeaponIdle = m_flNextSecondaryAttack = m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		}
	return;
	}
		else if (m_flReleaseThrow > 0)
		{
			m_flStartThrow = 0;

			if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
				SendWeaponAnim( HANDGRENADE_DRAW );
			else
			{
				RetireWeapon();
				return;
			}

			m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 10, 15 );
			m_flReleaseThrow = -1;
			return;
		}

		if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
		{
			int iAnim;
			float flRand = RANDOM_FLOAT(0, 1);

			if (flRand <= 0.75)
			{
				iAnim = HANDGRENADE_IDLE;
				m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 18, 24 );
			}
			else
			{
				iAnim = HANDGRENADE_FIDGET;
				m_flTimeWeaponIdle = gpGlobals->time + 75.0 / 30.0;
			}
		SendWeaponAnim( iAnim );
	}
}