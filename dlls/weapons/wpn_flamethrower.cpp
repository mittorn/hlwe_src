#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"
#include "projectiles.h"

enum flamethrower_e
{
	FLAMETHROWER_IDLE,
	FLAMETHROWER_IDLE2,
	FLAMETHROWER_FIRE,
	FLAMETHROWER_RELOAD,
	FLAMETHROWER_DRAW,
	FLAMETHROWER_HOLSTER,
	FLAMETHROWER_FLAME_ON
};

class CFLAMETHROWER : public CBasePlayerWeapon
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
	void EXPORT Burn( void );

	void BuyPrimaryAmmo( void );
	void SellWeapon( void );
	float m_flShootTime;
	BOOL burn;
	BOOL lastburn;
};
LINK_ENTITY_TO_CLASS( weapon_flamethrower, CFLAMETHROWER );

void CFLAMETHROWER::BuyPrimaryAmmo( void )
{
	BuyAmmo(100, (char*)pszAmmo1(), COST_HEAVY_AMMO);
}

void CFLAMETHROWER::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_FLAMETHROWER/2;
}

void CFLAMETHROWER::Spawn( )
{
	Precache( );
	burn=FALSE;
	SET_MODEL(ENT(pev), "models/p_flamethrower.mdl");
	m_iId = WEAPON_FLAMETHROWER;
	m_iDefaultAmmo = 200;
	pev->weapons = CLIP_FLAMETHROWER;
	FallInit();
}

void CFLAMETHROWER::Precache( void )
{
	PRECACHE_MODEL("models/v_flamethrower.mdl");
	PRECACHE_MODEL("models/p_flamethrower.mdl");
	PRECACHE_MODEL("sprites/flame.spr");
	PRECACHE_SOUND("weapons/flame_hitwall.wav");
	PRECACHE_SOUND("weapons/flamethrower.wav");
}

int CFLAMETHROWER::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "fuel";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 100;
	p->iSlot = 4;
	p->iPosition = 4;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iId = WEAPON_FLAMETHROWER;
	p->iWeight = FLAMETHROWER_WEIGHT;
	p->iMaxspeed = FLAMETHROWER_MAXSPEED;
	return 1;
}

BOOL CFLAMETHROWER::Deploy( )
{
	return DefaultDeploy( "models/v_flamethrower.mdl", "models/p_flamethrower.mdl", FLAMETHROWER_DRAW, "saw", 0.6 );
}

void CFLAMETHROWER::Holster( )
{
	if(burn==TRUE)
	{
		STOP_SOUND( ENT(m_pPlayer->pev), CHAN_WEAPON, "player/pl_burn.wav" );
		FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), 200, 0, FIREGUN_REMOVE );
		burn=FALSE;
	}
	m_fInReload = FALSE;
	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.6;
	SendWeaponAnim( FLAMETHROWER_HOLSTER );
}

void CFLAMETHROWER::PrimaryAttack( void )
{
	if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 0 || burn==FALSE)
 	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		return;
	}

	m_iClip--;
	m_iFiredAmmo++;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	if ( gpGlobals->time >= m_flShootTime )
	{
		SendWeaponAnim( FLAMETHROWER_FIRE );
		m_flShootTime = gpGlobals->time + 1;
	}

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	CFlame::ShootFlame( m_pPlayer->pev, m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 24 + gpGlobals->v_right * 2 + gpGlobals->v_up * -8, gpGlobals->v_forward * 900 );

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + 0.1;
	m_flTimeWeaponIdle = gpGlobals->time + 5;
}

void CFLAMETHROWER::SecondaryAttack( void )
{
	if (!m_iClip && burn==FALSE)
	return;

	if(burn==TRUE)
	{
		burn=FALSE;
		FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), 200, 0, FIREGUN_REMOVE );
	}
	else
	{
		burn=TRUE;
		SendWeaponAnim( FLAMETHROWER_FLAME_ON );
		FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), 200, 0, FIREGUN_FTHROWER );
	}
	m_flNextSecondaryAttack = m_flNextPrimaryAttack = gpGlobals->time + 0.2;
}

void CFLAMETHROWER::Reload( void )
{
	if (m_iClip)
	return;

	DefaultReload( 100, FLAMETHROWER_RELOAD, 4.3, 1.6 );
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), 200, 0, FIREGUN_REMOVE );
}

void CFLAMETHROWER::Burn( void )
{
	if(m_iClip && burn==1)
	{
		if ( gpGlobals->time >= m_flTimeUpdate)
		{
			m_iClip--;
			m_iFiredAmmo++;
			m_flTimeUpdate = gpGlobals->time + 5;
		}
	}

 	if (lastburn != burn && m_iClip <=0)
	{
		burn=FALSE;
		lastburn = burn;
		FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), 200, 0, FIREGUN_REMOVE );
		m_iClip=0;
	}
}

void CFLAMETHROWER::WeaponIdle( void )
{
	Burn();

	if (m_pPlayer->pev->waterlevel == 3 || m_iClip <=0 || burn==0)
		STOP_SOUND( ENT(m_pPlayer->pev), CHAN_WEAPON, "player/pl_burn.wav" );

	if ( gpGlobals->time >= m_flShockTime && burn==1)
	{
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "player/pl_burn.wav", 0.3, ATTN_NORM );
		m_flShockTime = gpGlobals->time + 1.8;
	}

	if (m_flTimeWeaponIdle > gpGlobals->time)
		return;

	int iAnim;
	switch ( RANDOM_LONG( 0, 1 ) )
	{
	case 0:	
		iAnim = FLAMETHROWER_IDLE;	
		break;
	default:
	case 1:
		iAnim = FLAMETHROWER_IDLE2;
		break;
	}
	SendWeaponAnim( iAnim );
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 10, 15 );
}