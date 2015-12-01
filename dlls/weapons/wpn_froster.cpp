#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"
#include "decals.h"
#include "projectiles.h"

enum froster_e 
{
	FROSTER_IDLE,
	FROSTER_IDLE2,
	FROSTER_FIRE,
	FROSTER_FIRE_GREN,
	FROSTER_FIRE_SOLID,
	FROSTER_FIRE_GREN_SOLID,
	FROSTER_DRAW,
	FROSTER_HOLSTER,
	FROSTER_RELOAD
};

class CFroster : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);

	BOOL Deploy( void );
	void Holster( void );
	void Reload( void );

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	void SwitchFireMode( void );
	void WeaponIdle( void );

	void BuyPrimaryAmmo( void );
	void BuySecondaryAmmo( void );
	void SellWeapon( void );
	int SecondaryAmmoIndex( void );

	void ZoomMode( void );
	void GrenadeLauncher( void );
	void ZoomReset( void );
};
LINK_ENTITY_TO_CLASS( weapon_froster, CFroster );

int CFroster::SecondaryAmmoIndex( void )
{
	return m_iSecondaryAmmoType;
}

void CFroster::BuyPrimaryAmmo( void )
{
	BuyAmmo(50, (char*)pszAmmo1(), COST_HEAVY_AMMO);
}

void CFroster::BuySecondaryAmmo( void )
{
	BuyAmmo(2, (char*)pszAmmo2(), COST_ASSAULT_AMMO_SEC);
}

void CFroster::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_FROSTER/2;
}

void CFroster::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_froster.mdl");
	m_iId = WEAPON_FROSTER;
	firemode= FIREMODE_GLAUNCHER;
	pev->weapons = CLIP_FROSTER;
	m_iDefaultAmmo = 100;
	m_iDefaultAmmo2 = 2;
	FallInit();
}

BOOL CFroster::Deploy( void )
{
	m_iLastFiremode = -1;
	return DefaultDeploy( "models/v_froster.mdl", "models/p_froster.mdl", FROSTER_DRAW, "saw" );
}

void CFroster::Holster( )
{
	ZoomReset();
	m_fInReload = FALSE;
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.6;

	MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
	WRITE_BYTE(MSG_FIREMODE);
	WRITE_BYTE(0);
	WRITE_ELSE(1+1);
	MESSAGE_END();

	SendWeaponAnim( FROSTER_HOLSTER );
}

void CFroster::ZoomReset(void)
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

int CFroster::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "cell";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = "Cgrenade";
	p->iMaxAmmo2 = 999;
	p->iMaxClip = 50;
	p->iSlot = 4;
	p->iPosition = 3;
	p->iId = WEAPON_FROSTER;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iWeight = FROSTER_WEIGHT;
	p->iMaxspeed = FROSTER_MAXSPEED;
	return 1;
}

void CFroster::Precache( void )
{
	PRECACHE_MODEL("models/v_froster.mdl");
	PRECACHE_MODEL("models/p_froster.mdl");

	PRECACHE_SOUND("weapons/cluster_bounce.wav");
	PRECACHE_SOUND("weapons/cluster_explode.wav");
	PRECACHE_SOUND("weapons/froster_fire.wav");
	PRECACHE_SOUND("weapons/froster_firegrenade.wav");
}

void CFroster::PrimaryAttack( void )
{
	if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 0)
 	{
		PlayEmptySound(5);
		m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		return;
	}
	m_iFiredAmmo++;
	m_iClip--;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	CFrostball::ShootFrostball( m_pPlayer->pev, m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 24 + gpGlobals->v_right * 4 + gpGlobals->v_up * -2, gpGlobals->v_forward * 5000);
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?FROSTER_FIRE_SOLID:FROSTER_FIRE, 0, FIREGUN_FROSTER );

	m_flNextPrimaryAttack = gpGlobals->time + 0.22;
	m_flTimeWeaponIdle = gpGlobals->time + 5;
}

void CFroster::SecondaryAttack()
{
	if(firemode==FIREMODE_SCOPE)
		ZoomMode();
	if(firemode==FIREMODE_GLAUNCHER)
		GrenadeLauncher();
}

void CFroster::ZoomMode( void )
{
	if (m_fInZoom == 0) 
	{
		m_pPlayer->m_iFOV = 45;
		m_fInZoom = 1;

		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
		WRITE_BYTE(MSG_SCOPE);
		WRITE_BYTE(4);
		WRITE_ELSE(1+1);
		MESSAGE_END();
	}
	else if (m_fInZoom == 1) 
	{
		m_pPlayer->m_iFOV = 90;
		m_fInZoom = 0;

		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
		WRITE_BYTE(MSG_SCOPE);
		WRITE_BYTE(0);
		WRITE_ELSE(1+1);
		MESSAGE_END();
	}

	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/zoom.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
	m_flNextSecondaryAttack = gpGlobals->time+0.5;
}

void CFroster::GrenadeLauncher( void )
{
	if (m_pPlayer->pev->waterlevel == 3 || m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] <= 0)
 	{
		PlayEmptySound( );
		m_flNextSecondaryAttack = gpGlobals->time + 0.5;
		return;
	}
	m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType]--;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	CClMomma::ShootClusterMomma( m_pPlayer->pev, m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 28 + gpGlobals->v_right * 10 + gpGlobals->v_up * -5, gpGlobals->v_forward * 1500, 3 );
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?FROSTER_FIRE_GREN_SOLID:FROSTER_FIRE_GREN, 1, FIREGUN_FROSTER );

	m_flNextPrimaryAttack = gpGlobals->time + 1;
	m_flNextSecondaryAttack = gpGlobals->time + 2.1;
	if (!m_pPlayer->m_fHeavyArmor)
		m_pPlayer->pev->punchangle.x -= 6;
}

void CFroster::SwitchFireMode( void )
{
	if(firemode==FIREMODE_GLAUNCHER)
		firemode=FIREMODE_SCOPE;
	else
		firemode=FIREMODE_GLAUNCHER;
	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.1;
}

void CFroster::WeaponIdle( void )
{
	if( m_iLastFiremode != firemode)
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, m_pPlayer->pev);
		WRITE_BYTE(MSG_FIREMODE);
		WRITE_BYTE(firemode);
		WRITE_ELSE(1+1);
		MESSAGE_END();
		m_iLastFiremode = firemode;
	}

	if ( m_flTimeWeaponIdle > gpGlobals->time )
		return;

	if (m_pPlayer->m_iFOV != 90)
		return;

	int iAnim;
	switch ( RANDOM_LONG( 0, 1 ) )
	{
		case 0: 
			iAnim = FROSTER_IDLE; 
		break;

	default:
		case 1:
			iAnim = FROSTER_IDLE2; 
		break;
	}
	SendWeaponAnim( iAnim );
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT(10,15);
}

void CFroster::Reload( void )
{
	if (m_iClip)
	return;

	ZoomReset();
	DefaultReload( 50, FROSTER_RELOAD, 4.5, 1.2 );
}
