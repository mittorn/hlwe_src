#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "projectiles.h"

enum redeemir_e
{
	REDEEMER_IDLE,
	REDEEMER_DRAW,
	REDEEMER_FIRE,
	REDEEMER_FIRE_SOLID,
	REDEEMER_HOLSTER,
	REDEEMER_RELOAD
};

class CRedeemer : public CBasePlayerWeapon
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

	void BuyPrimaryAmmo( void );
	void SellWeapon( void );
};
LINK_ENTITY_TO_CLASS( weapon_redeemer, CRedeemer );


void CRedeemer::BuyPrimaryAmmo( void )
{
	BuyAmmo(1, (char*)pszAmmo1(), COST_EXPERIMENTAL_AMMO);
}

void CRedeemer::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_REDEEMER/2;
}

void CRedeemer::Spawn( )
{
	Precache( );
	m_iId = WEAPON_REDEEMER;
	SET_MODEL(ENT(pev), "models/p_redeemer.mdl");
	m_iDefaultAmmo = 2;
	FallInit();
}

void CRedeemer::Precache( void )
{
	PRECACHE_MODEL("models/v_redeemer.mdl");
	PRECACHE_MODEL("models/p_redeemer.mdl");

	PRECACHE_SOUND ("weapons/redeemer_WH_fly.wav");
	PRECACHE_SOUND ("weapons/redeemer_WH_explode.wav");
	PRECACHE_SOUND ("weapons/redeemer_fire.wav");
}

int CRedeemer::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "nuke";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 1;
	p->iSlot = 5;
	p->iPosition = 4;
	p->iId = m_iId = WEAPON_REDEEMER;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY | ITEM_FLAG_NOAUTORELOAD;
	p->iWeight = REDEEMER_WEIGHT;
	p->iMaxspeed = REDEEMER_MAXSPEED;
	return 1;
}

BOOL CRedeemer::Deploy( )
{
	return DefaultDeploy( "models/v_redeemer.mdl", "models/p_redeemer.mdl", REDEEMER_DRAW, "gauss", 1 );
}

void CRedeemer::Holster( )
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.7;
	SendWeaponAnim( REDEEMER_HOLSTER );
	m_fInReload = FALSE;
}

void CRedeemer::PrimaryAttack()
{
	if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 0)
 	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		return;
	}

	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	SendWeaponAnim((m_pPlayer->m_fHeavyArmor)?REDEEMER_FIRE_SOLID:REDEEMER_FIRE);
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/redeemer_fire.wav", 1.0, ATTN_LOW);
	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	CNuke::ShootNuke(m_pPlayer->pev, m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 32 + gpGlobals->v_up * 2 + gpGlobals->v_right * -2, gpGlobals->v_forward * 1500, FALSE );

	m_iClip--;
	m_flNextPrimaryAttack = gpGlobals->time + 1.5;
	m_flTimeWeaponIdle = gpGlobals->time + 1.5;
	if (!m_pPlayer->m_fHeavyArmor)
	m_pPlayer->pev->punchangle.x -= 15;
}

void CRedeemer::SecondaryAttack()
{
	if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 0)
 	{
		PlayEmptySound( );
		m_flNextSecondaryAttack = gpGlobals->time + 0.5;
		return;
	}

	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	SendWeaponAnim((m_pPlayer->m_fHeavyArmor)?REDEEMER_FIRE_SOLID:REDEEMER_FIRE);
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/redeemer_fire.wav", 1.0, ATTN_LOW);
	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	CNuke::ShootNuke(m_pPlayer->pev, m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 32 + gpGlobals->v_up * 2 + gpGlobals->v_right * -2, gpGlobals->v_forward * 800, TRUE );

	m_iClip--;
	m_flNextPrimaryAttack = gpGlobals->time + 1.5;
	m_flTimeWeaponIdle = gpGlobals->time + 1.5;
	if (!m_pPlayer->m_fHeavyArmor)
	m_pPlayer->pev->punchangle.x -= 15;
}

void CRedeemer::WeaponIdle( void )
{
	if ( m_flTimeWeaponIdle > gpGlobals->time ) return;

	SendWeaponAnim( REDEEMER_IDLE );
	m_flTimeWeaponIdle = gpGlobals->time + 10.0;
}

void CRedeemer::Reload( void )
{
	if (m_iClip)
	return;
	DefaultReload( 1, REDEEMER_RELOAD, 3.6 );
}
