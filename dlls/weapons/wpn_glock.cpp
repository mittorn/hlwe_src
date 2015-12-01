#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"

class CGlock : public CBasePlayerWeapon
{
public:
	int AddDuplicate( CBasePlayerItem *pOriginal );

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
};

void CGlock::BuyPrimaryAmmo( void )
{
	BuyAmmo(17, (char*)pszAmmo1(), COST_HANDGUN_AMMO);
}

void CGlock::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_GLOCK/2;
}

enum glock_e
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
LINK_ENTITY_TO_CLASS( weapon_glock, CGlock );

int CGlock::AddDuplicate( CBasePlayerItem *pOriginal )
{
	CGlock *pGlock = (CGlock *)pOriginal;

	if (!pGlock->m_pPlayer->HasNamedPlayerItem ( "weapon_glock_akimbo" ))
	{
		pGlock->m_pPlayer->GiveNamedItem( "weapon_glock_akimbo" );
		pGlock->m_pPlayer->RemoveNamedPlayerItem("weapon_glock");
	}
	return CBasePlayerWeapon::AddDuplicate (pOriginal);
}

void CGlock::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_glock.mdl");
	m_iId = WEAPON_GLOCK;
	m_iDefaultAmmo = 34;
	pev->weapons = CLIP_GLOCK;
	FallInit();
}

void CGlock::Precache( void )
{
	PRECACHE_MODEL("models/v_glock.mdl");
	PRECACHE_MODEL("models/p_glock.mdl");

	PRECACHE_SOUND ("weapons/glock_fire.wav");
}

int CGlock::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "9mmP";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 17;
	p->iSlot = 1;
	p->iPosition = 0;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iId = WEAPON_GLOCK;
	p->iWeight = GLOCK_WEIGHT;
	p->iMaxspeed = GLOCK_MAXSPEED;
	return 1;
}

BOOL CGlock::Deploy( )
{
	return DefaultDeploy( "models/v_glock.mdl", "models/p_glock.mdl", GLOCK_DRAW_SINGLE, "onehanded", 0.8 );
}

void CGlock::Holster( )
{
	m_fInReload = FALSE;
	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.5;
	SendWeaponAnim( GLOCK_HOLSTER_SINGLE );

	if (m_pSpot)
	{
		m_pSpot->Killed( NULL, GIB_NEVER );
		m_pSpot = NULL;
		m_fSpotActive = FALSE;
	}
}

void CGlock::PrimaryAttack( void )
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
	m_pPlayer->FireMagnumBullets(m_pPlayer->GetGunPosition(), gpGlobals->v_forward, (m_pPlayer->pev->flags & FL_DUCKING)?VECTOR_CONE_8DEGREES:VECTOR_CONE_9DEGREES, 16384, BULLET_9mmP, m_pPlayer->pev);
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?GLOCK_SHOOT_SOLID:GLOCK_SHOOT_SINGLE, 0, FIREGUN_GLOCK );

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + 0.08;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (8, 12);
	if (!m_pPlayer->m_fHeavyArmor)
	m_pPlayer->pev->punchangle.x -= 2;
	UpdateSpot( );
}

void CGlock::SecondaryAttack( void )
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

void CGlock::Reload( void )
{
	if ( m_pSpot && m_fSpotActive )
	{
		m_pSpot->Suspend( 2.1 );
		m_flNextSecondaryAttack = gpGlobals->time  + 2.1;
	}

	DefaultReload( 17, GLOCK_RELOAD_SINGLE, 2, 0.8 );
}

void CGlock::WeaponIdle( void )
{
	UpdateSpot( );

	if ( m_flTimeWeaponIdle > gpGlobals->time )
		return;

	switch (RANDOM_LONG(0,2))
	{
		case 0:	SendWeaponAnim(GLOCK_IDLE_SINGLE1); break;
		case 1:	SendWeaponAnim(GLOCK_IDLE_SINGLE2); break;
		case 2:	SendWeaponAnim(GLOCK_IDLE_SINGLE3); break;
	}

	m_flTimeWeaponIdle = gpGlobals->time + 15;
}

void CGlock::UpdateSpot( void )
{
if (m_fSpotActive)
	{
	if (!m_pSpot)
	{
		m_pSpot = CLaserSpot::CreateSpotEagle();
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