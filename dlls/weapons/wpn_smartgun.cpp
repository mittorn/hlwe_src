#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"
#include "projectiles.h"

enum Smartgun_e
{
	SMARTGUN_IDLE1,
	SMARTGUN_IDLE2,
	SMARTGUN_FIRE,
	SMARTGUN_FIRE_SOLID,
	SMARTGUN_DRAW,
	SMARTGUN_HOLSTER
};

class CSmartgun : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);

	BOOL Deploy( void );
	void Holster( void );

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	void WeaponIdle( void );
	BOOL ShouldWeaponIdle( void ) { return TRUE; };
	void SellWeapon( void );
	int AddDuplicate( CBasePlayerItem* );
};
LINK_ENTITY_TO_CLASS( weapon_smartgun, CSmartgun );

int CSmartgun::AddDuplicate( CBasePlayerItem *pOriginal )
{
	return false;//do not pick up second one
}

void CSmartgun::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_SMARTGUN/2;
}

void CSmartgun::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_smartgun.mdl");
	m_iId = WEAPON_SMARTGUN;
	m_iClip = -1;
	FallInit();
}

void CSmartgun::Precache( void )
{
	PRECACHE_MODEL("models/v_smartgun.mdl");
	PRECACHE_MODEL("models/p_smartgun.mdl");

	PRECACHE_SOUND("weapons/smartgun_fire.wav");
	PRECACHE_SOUND("weapons/smartgun_fire2.wav");
	PRECACHE_SOUND("weapons/shock_hitwall.wav");
	PRECACHE_SOUND("weapons/smartgun_lgtngball_fly.wav");
	PRECACHE_SOUND("weapons/smartgun_lgtngball_strike.wav");
}

BOOL CSmartgun::Deploy( void )
{
	return DefaultDeploy( "models/v_smartgun.mdl", "models/p_smartgun.mdl", SMARTGUN_DRAW, "onehanded", 0.6 );
}

void CSmartgun::Holster( )
{
	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.5;
	SendWeaponAnim( SMARTGUN_HOLSTER );
}

int CSmartgun::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 6;
	p->iPosition = 0;
	p->iId = WEAPON_SMARTGUN;
	p->iFlags = 0;
	p->iWeight = SMARTGUN_WEIGHT;
	return 1;
}

void CSmartgun::PrimaryAttack()
{
	if (m_pPlayer->pev->armorvalue < 11)
	{
		PlayEmptySound(2);
		m_flNextPrimaryAttack = gpGlobals->time+0.5;
		return;
	}
	if (m_pPlayer->pev->waterlevel == 3)
	{
		entvars_t *pevOwner = VARS( pev->owner );
		::WaterRadiusDamage( pev->origin, pev, pevOwner, (dmg_smartgun.value*5) * (mp_wpn_power.value/100), (dmg_smartgun.value*20) * (mp_wpn_power.value/100), CLASS_NONE, DMG_SHOCK  | DMG_NEVERGIB );
		FX_Explosion( pev->origin, EXPLOSION_BUBBLES);
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/shockgrenade_explo.wav", 0.55, ATTN_NORM);
		m_pPlayer->pev->armorvalue = 0;
	}

	m_pPlayer->pev->armorvalue -= 1;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	CShock::ShootShock( m_pPlayer->pev, m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 24 + gpGlobals->v_right * 5 + gpGlobals->v_up * -5, gpGlobals->v_forward * 400, FALSE );
	FX_FireGun(m_pPlayer->pev->angles, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?SMARTGUN_FIRE_SOLID:SMARTGUN_FIRE, 0, FIREGUN_SMARTGUN);

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + 0.3;
	m_flTimeWeaponIdle = gpGlobals->time + 3;
}

void CSmartgun::SecondaryAttack()
{
	if (m_pPlayer->pev->armorvalue < 11)
	{
		PlayEmptySound(4);
		m_flNextPrimaryAttack = gpGlobals->time+0.5;
		return;
	}

	if (m_pPlayer->pev->waterlevel == 3)
	{
		entvars_t *pevOwner = VARS( pev->owner );
		::WaterRadiusDamage( pev->origin, pev, pevOwner, (dmg_lgtng_ball.value*5) * (mp_wpn_power.value/100), (dmg_lgtng_ball.value*8) * (mp_wpn_power.value/100), CLASS_NONE, DMG_SHOCK  | DMG_NEVERGIB );
		FX_Explosion( pev->origin, EXPLOSION_BUBBLES);
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/shockgrenade_explo.wav", 0.55, ATTN_NORM);
		m_pPlayer->pev->armorvalue = 0;
	}

	m_pPlayer->pev->armorvalue -= 8;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	CShock::ShootShock( m_pPlayer->pev, m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 16 + gpGlobals->v_right * 5 + gpGlobals->v_up * -5, gpGlobals->v_forward * 700, TRUE );
	FX_FireGun(m_pPlayer->pev->angles, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?SMARTGUN_FIRE_SOLID:SMARTGUN_FIRE, 1, FIREGUN_SMARTGUN);

	m_flNextSecondaryAttack = m_flNextPrimaryAttack = gpGlobals->time + 1.2;
	m_flTimeWeaponIdle = gpGlobals->time + 3;
}

void CSmartgun::WeaponIdle( void )
{
	if ( m_flTimeWeaponIdle > gpGlobals->time )
		return;

	switch (RANDOM_LONG(0,1))
	{
		case 0:	SendWeaponAnim( SMARTGUN_IDLE1 ); break;
		case 1:	SendWeaponAnim( SMARTGUN_IDLE2 ); break;
	}
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 10, 15 );
}