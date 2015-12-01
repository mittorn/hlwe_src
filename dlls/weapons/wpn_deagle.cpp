#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "monsters.h"
#include "player.h"
#include "gamerules.h"

class Cdeagle : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);
	void PrimaryAttack( void );
	void SecondaryAttack( void );
	BOOL Deploy( void );
	void Holster( void );
	void Reload( void );
	void WeaponIdle( void );
	BOOL ShouldWeaponIdle( void ) {return TRUE; };

	void BuyPrimaryAmmo( void );
	void ZoomReset( void );
	void UpdateSpot( void );
	CLaserSpot *m_pSpot;
	void SellWeapon( void );
};

enum deagle_e
{
	DEAGLE_IDLE1,
	DEAGLE_IDLE2,
	DEAGLE_IDLE3,
	DEAGLE_FIDGET,
	DEAGLE_FIRE,
	DEAGLE_FIRE_SOLID,
	DEAGLE_RELOAD,
	DEAGLE_DRAW,
	DEAGLE_HOLSTER
};
LINK_ENTITY_TO_CLASS( weapon_deagle, Cdeagle );

void Cdeagle::BuyPrimaryAmmo( void )
{
	BuyAmmo(7, (char*)pszAmmo1(), COST_HANDGUN_AMMO);
}

void Cdeagle::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_DEAGLE/2;
}

int Cdeagle::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "50AE";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 7;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iSlot = 1;
	p->iPosition = 3;
	p->iId = WEAPON_DEAGLE;
	p->iWeight = DEAGLE_WEIGHT;
	p->iMaxspeed = DEAGLE_MAXSPEED;
	return 1;
}

void Cdeagle::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_deagle.mdl");
	m_iId = WEAPON_DEAGLE;
	m_iDefaultAmmo = 14;
	pev->weapons = CLIP_DEAGLE;
	FallInit();
}

void Cdeagle::Precache( void )
{
	PRECACHE_MODEL("models/v_deagle.mdl");
	PRECACHE_MODEL("models/p_deagle.mdl");

	PRECACHE_SOUND ("weapons/deagle_fire.wav");
	PRECACHE_SOUND ("weapons/spot_on.wav");
	PRECACHE_SOUND ("weapons/spot_off.wav");
}

BOOL Cdeagle::Deploy( )
{
	return DefaultDeploy( "models/v_deagle.mdl", "models/p_deagle.mdl", DEAGLE_DRAW, "onehanded", 0.7 );
}

void Cdeagle::Holster( )
{
	ZoomReset();
	m_fInReload = FALSE;
	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.5;
	SendWeaponAnim( DEAGLE_HOLSTER );

	if (m_pSpot)
	{
		m_pSpot->Killed( NULL, GIB_NEVER );
		m_pSpot = NULL;
		m_fSpotActive = FALSE;
	}
}

void Cdeagle::ZoomReset(void)
{
	if (m_pPlayer->m_iFOV != 90)
	{
		m_pPlayer->m_iFOV = 90;
		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
		WRITE_BYTE(MSG_SCOPE);
		WRITE_BYTE(0);
		WRITE_ELSE(1+1);
		MESSAGE_END();
	}
}

void Cdeagle::PrimaryAttack( void )
{
	if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 0)
 	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		return;
	}

	if(! ( m_pPlayer->m_afButtonPressed & IN_ATTACK ) )
	return;

	m_iClip--;
	m_iFiredAmmo++;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );
	m_pPlayer->FireMagnumBullets(m_pPlayer->GetGunPosition(), gpGlobals->v_forward, (m_pPlayer->pev->flags & FL_DUCKING)?VECTOR_CONE_6DEGREES:VECTOR_CONE_7DEGREES, 16384, BULLET_50AE, m_pPlayer->pev);
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?DEAGLE_FIRE_SOLID:DEAGLE_FIRE, 0, FIREGUN_DEAGLE );

	m_flNextPrimaryAttack = gpGlobals->time + 0.2;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 5, 7 );
	if (!m_pPlayer->m_fHeavyArmor)
	m_pPlayer->pev->punchangle.x -= 5;
	UpdateSpot( );
}

void Cdeagle::SecondaryAttack()
{
	if (m_fInZoom == 0) 
	{
		m_pPlayer->m_iFOV = 40;
		m_fInZoom = 1;
		m_fSpotActive = 1;
	}
	else if (m_fInZoom == 1) 
	{
		m_pPlayer->m_iFOV = 90;
		m_fInZoom = 0;
		m_fSpotActive = 0;
	}

	MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
	WRITE_BYTE(MSG_SCOPE);
	WRITE_BYTE(m_fInZoom);
	WRITE_ELSE(1+1);
	MESSAGE_END();

	if (!m_fSpotActive && m_pSpot)
	{
		m_pSpot->Killed( NULL, GIB_NORMAL );
		m_pSpot = NULL;
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/spot_off.wav", 1, ATTN_NORM);
	}

	m_flNextSecondaryAttack = gpGlobals->time + 0.4;
}

void Cdeagle::Reload( void )
{
	ZoomReset();

	if (m_pSpot)
	{
		m_pSpot->Killed( NULL, GIB_NEVER );
		m_pSpot = NULL;
	}
	DefaultReload( 7, DEAGLE_RELOAD, 1.8, 0.6 );
}

void Cdeagle::WeaponIdle( void )
{
	UpdateSpot( );

	if ( m_flTimeWeaponIdle > gpGlobals->time )
		return;

	if (m_pPlayer->m_iFOV != 90)
		return;

	int iAnim;
	switch ( RANDOM_LONG( 0, 3 ) )
	{
	case 0:	
		iAnim = DEAGLE_FIDGET;	
		break;
	
	case 1:	
		iAnim = DEAGLE_IDLE1;	
		break;
	
	case 2:	
		iAnim = DEAGLE_IDLE2;	
		break;
	
	default:
	case 3:
		iAnim = DEAGLE_IDLE3;
		break;
	}

	SendWeaponAnim( iAnim );

	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (10, 15);
}

void Cdeagle::UpdateSpot( void )
{
	if (m_pPlayer->m_iFOV == 90)
	return;

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

		m_pSpot->pev->scale = flLength / (850 + RANDOM_FLOAT(1, 100));
		int m_iSpotBright = (1 / log(flLength / 0.3))*1700;
		if (m_iSpotBright > 255 ) m_iSpotBright = 255;

		m_iSpotBright = m_iSpotBright + RANDOM_LONG (1, flLength / 200);
		m_pSpot->pev->renderamt = m_iSpotBright;	
  		UTIL_SetOrigin( m_pSpot->pev, tr.vecEndPos + tr.vecPlaneNormal * 0.1);
	}
}