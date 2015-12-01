#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"

class CGlockAkimbo : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	BOOL Deploy( void );
	BOOL ShouldWeaponIdle( void ) {return TRUE; };
	void Reload( void );
	void WeaponIdle( void );
	void Holster( void );

	void BuyPrimaryAmmo( void );
	void SellWeapon( void );

	void UpdateSpot( void );
	CLaserSpot *m_pSpot;
	bool	righthand;	// TRUE if shooting right gun, FALSE in other vice.
};

void CGlockAkimbo::BuyPrimaryAmmo( void )
{
	BuyAmmo(34, (char*)pszAmmo1(), COST_ASSAULT_AMMO);
}

void CGlockAkimbo::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_GLOCK;
}

enum glockA_e
{
	GLOCK_IDLE_SINGLE1,
	GLOCK_IDLE_SINGLE2,
	GLOCK_IDLE_SINGLE3,
	GLOCK_SHOOT_SINGLE,
	GLOCK_SHOOT_SOLID,
	GLOCK_RELOAD_SINGLE,
	GLOCK_DRAW_SINGLE,
	GLOCK_HOLSTER_SINGLE,
	GLOCK_IDLE1,
	GLOCK_IDLE2,
	GLOCK_IDLE3,
	GLOCK_SHOOT_RIGHT,
	GLOCK_SHOOT_LEFT,
	GLOCK_SHOOT_RIGHT_SOLID,
	GLOCK_SHOOT_LEFT_SOLID,
	GLOCK_RELOAD,
	GLOCK_DRAW,
	GLOCK_HOLSTER
};
LINK_ENTITY_TO_CLASS(weapon_glock_akimbo, CGlockAkimbo );

void CGlockAkimbo::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_glock_akimbo.mdl");
	m_iId = WEAPON_GLOCK_AKIMBO;
	m_iDefaultAmmo = 34;
	pev->weapons = CLIP_GLOCK_DUAL;
	righthand = TRUE;
	FallInit();
}

void CGlockAkimbo::Precache( void )
{
	PRECACHE_MODEL("models/p_glock_akimbo.mdl");
}

int CGlockAkimbo::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "9mmP";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 34;
	p->iSlot = 1;
	p->iPosition = 1;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iId = WEAPON_GLOCK_AKIMBO;
	p->iWeight = GLOCK_AKIMBO_WEIGHT;
	p->iMaxspeed = GLOCK_AKIMBO_MAXSPEED;
	return 1;
}

BOOL CGlockAkimbo::Deploy( )
{
	return DefaultDeploy( "models/v_glock.mdl", "models/p_glock_akimbo.mdl", GLOCK_DRAW, "twohanded", 0.7 );
}

void CGlockAkimbo::Holster( )
{
	m_fInReload = FALSE;
	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.5;

	SendWeaponAnim(GLOCK_HOLSTER);

	if (m_pSpot)
	{
		m_pSpot->Killed( NULL, GIB_NEVER );
		m_pSpot = NULL;
		m_fSpotActive = FALSE;
	}
}

void CGlockAkimbo::PrimaryAttack( void )
{
	if (m_iClip <= 0)
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		return;
	}

	if(! ( m_pPlayer->m_afButtonPressed & IN_ATTACK ) )
	return;

	m_iClip--;
	m_iFiredAmmo++;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	m_pPlayer->FireMagnumBullets(m_pPlayer->GetGunPosition(), gpGlobals->v_forward, (m_pPlayer->pev->flags & FL_DUCKING)?VECTOR_CONE_9DEGREES:VECTOR_CONE_10DEGREES, 16384, BULLET_9mmP, m_pPlayer->pev);

	byte MODE = 0;
	if (righthand)
	{
		MODE = 1;
		FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?GLOCK_SHOOT_RIGHT_SOLID:GLOCK_SHOOT_RIGHT, MODE, FIREGUN_GLOCKAKIMBO );
	}
	else
	{
		MODE = 2;
		FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?GLOCK_SHOOT_LEFT_SOLID:GLOCK_SHOOT_LEFT, MODE, FIREGUN_GLOCKAKIMBO );
	}
	righthand = !righthand;

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + 0.06;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (8, 12);
	if (!m_pPlayer->m_fHeavyArmor)
	m_pPlayer->pev->punchangle.x -= 2.5;
	UpdateSpot( );
}

void CGlockAkimbo::SecondaryAttack( void )
{
	m_fSpotActive = !m_fSpotActive;

	if (!m_fSpotActive && m_pSpot)
	{
		m_pSpot->Killed( NULL, GIB_NORMAL );
		m_pSpot = NULL;
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/spot_off.wav", 1, ATTN_NORM);
	}
	m_flNextSecondaryAttack = gpGlobals->time + 0.4;
}

void CGlockAkimbo::Reload( void )
{
	if ( m_pSpot && m_fSpotActive )
	{
		m_pSpot->Suspend( 3.25 );
		m_flNextSecondaryAttack = gpGlobals->time  + 3.25;
	}

	DefaultReload(34, GLOCK_RELOAD, 3.15, 1);
}

void CGlockAkimbo::WeaponIdle( void )
{
	UpdateSpot( );

	if ( m_flTimeWeaponIdle > gpGlobals->time )
		return;

	switch (RANDOM_LONG(0,2))
	{
		case 0:	SendWeaponAnim(GLOCK_IDLE1); break;
		case 1:	SendWeaponAnim(GLOCK_IDLE2); break;
		case 2:	SendWeaponAnim(GLOCK_IDLE3); break;
	}

	m_flTimeWeaponIdle = gpGlobals->time + 10;
}

void CGlockAkimbo::UpdateSpot( void )
{
if (m_fSpotActive)
	{
	if (!m_pSpot)
	{
		m_pSpot = CLaserSpot::CreateSpotDGlock();
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/spot_on.wav", 1, ATTN_NORM);
	}

		UTIL_MakeVectors( m_pPlayer->pev->v_angle );
		Vector vecSrc = m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 24;
		Vector vecAiming = gpGlobals->v_forward;
                    Vector vecDest = vecSrc + vecAiming * 16384;
                    
		TraceResult tr;
		UTIL_TraceLine ( vecSrc, vecDest, dont_ignore_monsters, ENT(m_pPlayer->pev), &tr );
		float flLength = (tr.vecEndPos - vecSrc).Length();

		m_pSpot->pev->scale = flLength / (1000 + RANDOM_FLOAT(1, 100));
		int m_iSpotBright = (1 / log(flLength / 0.3))*2000;
		if (m_iSpotBright > 255 ) m_iSpotBright = 255;

		m_iSpotBright = m_iSpotBright + RANDOM_LONG (1, flLength / 200);
		m_pSpot->pev->renderamt = m_iSpotBright;	
  		UTIL_SetOrigin( m_pSpot->pev, tr.vecEndPos + tr.vecPlaneNormal * 0.1);
	}
}