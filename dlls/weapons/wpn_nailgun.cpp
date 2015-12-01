#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"
#include "decals.h"

enum nailgun_e
{
	NAILGUN_IDLE,
	NAILGUN_FIRE_RIGHT,
	NAILGUN_FIRE_RIGHT_SOLID,
	NAILGUN_FIRE_LEFT,
	NAILGUN_FIRE_LEFT_SOLID,
	NAILGUN_DRAW,
	NAILGUN_HOLSTER,
	NAILGUN_RELOAD
};

class CNailgun : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);
	void FireNail( void );

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	BOOL Deploy( );
	BOOL ShouldWeaponIdle( void ) {return TRUE; };
	void Holster( void );
	void Reload( void );
	void WeaponIdle( void );

	CLaserSpot *m_pSpot;
	void UpdateSpot ( void );

	void BuyPrimaryAmmo( void );
	void SellWeapon( void );
private:
	unsigned short m_usCrossbow;
};
LINK_ENTITY_TO_CLASS( weapon_nailgun, CNailgun );

void CNailgun::BuyPrimaryAmmo( void )
{
	BuyAmmo(60, (char*)pszAmmo1(), COST_HEAVY_AMMO);
}

void CNailgun::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_NAILGUN/2;
}

void CNailgun::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_nailgun.mdl");
	m_iId = WEAPON_NAILGUN;
	m_fSpotActive = 1;
	m_iDefaultAmmo = 120;
	m_iChargeLevel = 0;
	pev->weapons = CLIP_NAILGUN;
	FallInit();
}

void CNailgun::Precache( void )
{
	PRECACHE_MODEL("models/v_nailgun.mdl");
	PRECACHE_MODEL("models/p_nailgun.mdl");

	PRECACHE_SOUND("weapons/nailgun_fire.wav");
	PRECACHE_SOUND("weapons/nail_hitwall.wav");
}

int CNailgun::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "nails";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 60;
	p->iSlot = 4;
	p->iPosition = 2;
	p->iId = WEAPON_NAILGUN;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iWeight = NAILGUN_WEIGHT;
	p->iMaxspeed = NAILGUN_MAXSPEED;
	return 1;
}


BOOL CNailgun::Deploy( )
{
	return DefaultDeploy( "models/v_nailgun.mdl", "models/p_nailgun.mdl", NAILGUN_DRAW, "saw", 0.8 );
}

void CNailgun::Holster( )
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.6;
	m_fInReload = FALSE;

	SendWeaponAnim( NAILGUN_HOLSTER );

	if (m_pSpot)
	{
		m_pSpot->Killed( NULL, GIB_NEVER );
		m_pSpot = NULL;
		m_fSpotActive = FALSE;
	}
}

void CNailgun::FireNail( void )
{
	UpdateSpot( );
	if (m_iClip == 0)
	{
		PlayEmptySound( );
		return;
	}
	m_iClip--;
	m_iFiredAmmo++;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	m_pPlayer->FireMagnumBullets(m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 16 + gpGlobals->v_right * 3 + gpGlobals->v_up * -3, gpGlobals->v_forward, (m_pPlayer->pev->flags & FL_DUCKING)?VECTOR_CONE_1DEGREES:VECTOR_CONE_2DEGREES, 16384, BULLET_NAILGUN, m_pPlayer->pev);

	m_iChargeLevel++;
	switch (m_iChargeLevel)
	{
	case 1:
		FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?NAILGUN_FIRE_LEFT_SOLID:NAILGUN_FIRE_LEFT, 0, FIREGUN_NAILGUN );
	break;

	case 2:
		FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?NAILGUN_FIRE_RIGHT_SOLID:NAILGUN_FIRE_RIGHT, 1, FIREGUN_NAILGUN );
		m_iChargeLevel= 0;
	break;
		m_iChargeLevel= 0;
		break;
	}

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + 0.1;
	m_flTimeWeaponIdle = gpGlobals->time + 3;
}

void CNailgun::PrimaryAttack()
{
	FireNail();
}

void CNailgun::SecondaryAttack()
{
	FireNail();
}

void CNailgun::Reload( void )
{
	DefaultReload( 60, NAILGUN_RELOAD, 6.3, 2.5 );

	if ( m_pSpot && m_fSpotActive )
		m_pSpot->Suspend( 6.4 );
}

void CNailgun::WeaponIdle( void )
{
	UpdateSpot( );	
	if (m_flTimeWeaponIdle > gpGlobals->time)
		return;

	SendWeaponAnim( NAILGUN_IDLE );

	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (10, 15);
}

void CNailgun::UpdateSpot( void )
{
	if (m_fSpotActive)
	{
		if (!m_pSpot)
		{
			m_pSpot = CLaserSpot::CreateSpotNailgun();
		}
		UTIL_MakeVectors( m_pPlayer->pev->v_angle );
		Vector vecSrc = m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 24;
		Vector vecAiming = gpGlobals->v_forward;

		TraceResult tr;
		UTIL_TraceLine ( vecSrc, vecSrc + vecAiming * 16384, dont_ignore_monsters, ENT(m_pPlayer->pev), &tr );
		m_pSpot->pev->renderamt = RANDOM_FLOAT (210, 255);	
		UTIL_SetOrigin( m_pSpot->pev, tr.vecEndPos );
	}
}