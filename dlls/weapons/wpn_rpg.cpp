#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"
#include "projectiles.h"

enum rpg_e
{
	RPG_IDLE,
	RPG_IDLE_EMPTY,
	RPG_RELOAD,
	RPG_FIRE,
	RPG_FIRE_SOLID,
	RPG_HOLSTER,
	RPG_HOLSTER_EMPTY,
	RPG_DRAW,
	RPG_DRAW_EMPTY
};

LINK_ENTITY_TO_CLASS( weapon_rpg, CRpg );

void CRpg::BuyPrimaryAmmo( void )
{
	BuyAmmo(1, (char*)pszAmmo1(), COST_LAUNCHER_AMMO);
}

void CRpg::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_RPG/2;
}

void CRpg::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_rpg.mdl");
	m_iId = WEAPON_RPG;
	m_iDefaultAmmo = 2;
	FallInit();
}

void CRpg::Precache( void )
{
	PRECACHE_MODEL("models/v_rpg.mdl");
	PRECACHE_MODEL("models/p_rpg.mdl");
	UTIL_PrecacheOther( "laser_spot" );

	PRECACHE_SOUND("weapons/rocketfire1.wav");
	PRECACHE_SOUND("weapons/rpg_reload.wav");
	PRECACHE_SOUND("weapons/rpgrocket_explode.wav");
	PRECACHE_SOUND ("weapons/rocket1.wav");
}

int CRpg::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "rockets";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 1;
	p->iSlot = 5;
	p->iPosition = 0;
	p->iId = WEAPON_RPG;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iWeight = RPG_WEIGHT;
	p->iMaxspeed = RPG_MAXSPEED;
	return 1;
}

BOOL CRpg::Deploy( )
{
	m_fSpotActive = 0;
	return DefaultDeploy( "models/v_rpg.mdl", "models/p_rpg.mdl", (m_iClip)?RPG_DRAW:RPG_DRAW_EMPTY, "rpg", 0.7 );
}

void CRpg::Holster( )
{
	m_fInReload = FALSE;
	m_cActiveRockets = 0;
	m_pPlayer->m_flNextAttack = gpGlobals->time  + 0.7;
	SendWeaponAnim( (m_iClip)?RPG_HOLSTER:RPG_HOLSTER_EMPTY );

	if (m_pSpot)
	{
		m_pSpot->Killed( NULL, GIB_NEVER );
		m_pSpot = NULL;
		m_fSpotActive = FALSE;
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/spot_off.wav", 1, ATTN_NORM);
	}
}

void CRpg::PrimaryAttack()
{
	if (m_pPlayer->pev->waterlevel == 3 || m_iClip <= 0)
 	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		return;
	}

	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	CRpgRocket::ShootRpgRocket( m_pPlayer->pev, m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 28 + gpGlobals->v_right * 8 + gpGlobals->v_up * -8, (m_pSpot)?gpGlobals->v_forward *250:gpGlobals->v_forward *2500, (m_pSpot)?TRUE:FALSE, this);
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), (m_pPlayer->m_fHeavyArmor)?RPG_FIRE_SOLID:RPG_FIRE, 0, FIREGUN_RPG );

	m_iClip--;
	m_flTimeWeaponIdle = gpGlobals->time + 5;
	m_flNextPrimaryAttack = gpGlobals->time + 1.5;
	if (!m_pPlayer->m_fHeavyArmor)
	m_pPlayer->pev->punchangle.x -= 11;
	UpdateSpot( );
}


void CRpg::SecondaryAttack()
{
	m_fSpotActive = !m_fSpotActive;

	if (!m_fSpotActive && m_pSpot)
	{
		m_pSpot->Killed( NULL, GIB_NORMAL );
		m_pSpot = NULL;
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/spot_off.wav", 1, ATTN_NORM);
		pev->skin = 1;
	}
	m_flNextSecondaryAttack = gpGlobals->time + 0.5;
}

void CRpg::Reload( void )
{
	if (m_iClip)
	return;

	if (m_cActiveRockets && m_fSpotActive)
	return;

	if ( m_pSpot && m_fSpotActive )
	{
		m_pSpot->Suspend( 2.1 );
		m_flNextSecondaryAttack = gpGlobals->time  + 2.1;
	}
	DefaultReload( 1, RPG_RELOAD, 2 );
}

void CRpg::WeaponIdle( void )
{
	UpdateSpot( );

	if ( m_flTimeWeaponIdle > gpGlobals->time)
		return;

	if (m_iClip)
		SendWeaponAnim( RPG_IDLE );
	else
		SendWeaponAnim( RPG_IDLE_EMPTY );
	m_flTimeWeaponIdle = gpGlobals->time + 8;
}

void CRpg::UpdateSpot( void )
{
	if (m_fSpotActive)
	{
		if (!m_pSpot)
		{
			m_pSpot = CLaserSpot::CreateSpotRpg();
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/spot_on.wav", 1, ATTN_NORM);
			pev->skin = 0;
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
