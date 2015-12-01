#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"
#include "projectiles.h"

enum WHL_e
{
	WHL_IDLE,
	WHL_FIDGET,
	WHL_RELOAD,
	WHL_FIRE,
	WHL_FIRE_SOLID,
	WHL_DRAW,
	WHL_HOLSTER
};

class CWHL : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	void Reload( void );
	int GetItemInfo(ItemInfo *p);

	BOOL Deploy( void );
	void Holster( void );

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	void WeaponIdle( void );
	void SellWeapon( void );
	void BuyPrimaryAmmo( void );
	int m_ControlledRocket;
};
LINK_ENTITY_TO_CLASS( weapon_whl, CWHL );

void CWHL::BuyPrimaryAmmo( void )
{
	BuyAmmo(1, (char*)pszAmmo1(), COST_ASSAULT_AMMO_SEC);
}

void CWHL::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_WHL/2;
}

void CWHL::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_whl.mdl");
	m_iId = WEAPON_WHL;
	m_iDefaultAmmo = 2;
	FallInit();
}

void CWHL::Precache( void )
{
	PRECACHE_MODEL("models/v_whl.mdl");
	PRECACHE_MODEL("models/p_whl.mdl");

	PRECACHE_SOUND("weapons/WHL_fire.wav");
	PRECACHE_SOUND("weapons/WHL_rocket_fly.wav");
}

int CWHL::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "Dmissile";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 1;
	p->iSlot = 5;
	p->iPosition = 2;
	p->iId = WEAPON_WHL;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY | ITEM_FLAG_NOAUTORELOAD;
	p->iWeight = WHL_WEIGHT;
	p->iMaxspeed = WHL_MAXSPEED;
	return 1;
}

BOOL CWHL::Deploy( )
{
	return DefaultDeploy( "models/v_WHL.mdl", "models/p_WHL.mdl", WHL_DRAW, "rpg" );
}

void CWHL::Holster( )
{
	m_fInReload = FALSE;
	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.7;
	SendWeaponAnim(WHL_HOLSTER);
}

void CWHL::PrimaryAttack()
{
	if (m_iClip <= 0)
	{
		m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		return;
	}

	SendWeaponAnim((m_pPlayer->m_fHeavyArmor)?WHL_FIRE_SOLID:WHL_FIRE);
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/WHL_fire.wav", 1.0, ATTN_LOW);
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	CWhlProjectile::ShootWhlProjectile(m_pPlayer->pev, m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 28 + gpGlobals->v_right * 10 + gpGlobals->v_up * -5, gpGlobals->v_forward * 2250, FALSE );

	m_iClip--;
	m_flNextPrimaryAttack = gpGlobals->time + 1.5;
	m_flTimeWeaponIdle = gpGlobals->time + 1.5;
	if (!m_pPlayer->m_fHeavyArmor)
	m_pPlayer->pev->punchangle.x -= 12;
}


void CWHL::SecondaryAttack()
{
	if (m_iClip <= 0)
	{
		m_flNextSecondaryAttack = gpGlobals->time + 0.15;
		return;
	}

	SendWeaponAnim((m_pPlayer->m_fHeavyArmor)?WHL_FIRE_SOLID:WHL_FIRE);
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/WHL_fire.wav", 1.0, ATTN_LOW);
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	CWhlProjectile::ShootWhlProjectile(m_pPlayer->pev, m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 28 + gpGlobals->v_right * 10 + gpGlobals->v_up * -5, gpGlobals->v_forward * 2000, TRUE );

	m_iClip--;
	m_flNextPrimaryAttack = gpGlobals->time + 1.5;
	m_flTimeWeaponIdle = gpGlobals->time + 1.5;
	if (!m_pPlayer->m_fHeavyArmor)
	m_pPlayer->pev->punchangle.x -= 12;
}

void CWHL::WeaponIdle( void )
{
	if ( m_flTimeWeaponIdle > gpGlobals->time ) return;

	int iAnim;
	switch ( RANDOM_LONG( 0, 1 ) )
	{
	case 0:	
		iAnim = WHL_FIDGET;	
		break;
	
	default:
	case 1:
		iAnim = WHL_IDLE;
		break;
	}

	SendWeaponAnim( iAnim );
	m_flTimeWeaponIdle = gpGlobals->time + 10.0;
}

void CWHL::Reload( void )
{
	if (m_iClip)
	return;
	DefaultReload( 1, WHL_RELOAD, 2.5 );
}
