#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "projectiles.h"

enum satellite_e
{
	SATELLITE_IDLE,
	SATELLITE_FIDGET,
	SATELLITE_HOLSTER,
	SATELLITE_DRAW,
	SATELLITE_FIRE
};

class CSatellite : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);

	void PrimaryAttack( void );
	void UpdateScreen ( void );
	BOOL Deploy( void );
	void Holster( );
	void Reload( void );
	void WeaponIdle( void );
	void SellWeapon( void );

	int AddDuplicate( CBasePlayerItem* );
};
LINK_ENTITY_TO_CLASS( weapon_satellite, CSatellite );

int CSatellite::AddDuplicate( CBasePlayerItem *pOriginal )
{
	return false;//do not pick up second one
}

void CSatellite::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += COST_SATELLITE/2;
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] = 0;
}

void CSatellite::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_satellite.mdl");
	m_iId = WEAPON_SATELLITE;
	m_iDefaultAmmo = 200;
	FallInit();
}

void CSatellite::Precache( void )
{
	PRECACHE_MODEL("models/v_satellite.mdl");
	PRECACHE_MODEL("models/p_satellite.mdl");

	PRECACHE_SOUND( "weapons/satellite_activate.wav" );
	PRECACHE_SOUND( "weapons/satellite_strike.wav" );
	PRECACHE_SOUND( "weapons/satellite_idle.wav" );
}

int CSatellite::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "power";
	p->iMaxAmmo1 = 200;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 8;
	p->iPosition = 4;
	p->iId = WEAPON_SATELLITE;
	p->iWeight = SATELLITE_WEIGHT;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY | ITEM_FLAG_NOAUTORELOAD;
	p->iMaxspeed = SATELLITE_MAXSPEED;
	return 1;
}

BOOL CSatellite::Deploy( )
{
	return DefaultDeploy( "models/v_satellite.mdl", "models/p_satellite.mdl", SATELLITE_DRAW, "hive" );
}

void CSatellite::Holster( )
{
	STOP_SOUND( ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/satellite_idle.wav" );
	SendWeaponAnim( SATELLITE_HOLSTER );
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.65;
}

void CSatellite::PrimaryAttack()
{
	if (m_pPlayer->pev->waterlevel == 3)
		return;

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 199)
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("Not enough power to Satellite Strike!"));
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "buttons/button2.wav", 0.9, ATTN_NORM );
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.5;
		return;
	}
	TraceResult tr;
	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	UTIL_TraceLine(m_pPlayer->pev->origin, Vector(0,0,16000), ignore_monsters, edict(), &tr );
	if ( UTIL_PointContents(tr.vecEndPos) != CONTENT_SKY )
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("Satellite could not reach target!"));
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "buttons/button10.wav", 0.9, ATTN_NORM );
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.5;
		return;
	}

	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]=0;

	ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("Target locked! 15 Seconds to leave area!"));
	SendWeaponAnim( SATELLITE_FIRE );
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
 
	Vector vecSrc = m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 16 + gpGlobals->v_up * -15;
	Vector vecThrow = gpGlobals->v_forward;

	CSatelliteStrike::ShootSatelliteStrike( m_pPlayer->pev, vecSrc, vecThrow, 15 );
	EMIT_SOUND(ENT(pev), 0, "weapons/satellite_activate.wav", 1, ATTN_LOW_HIGH);
	
	m_flNextPrimaryAttack = gpGlobals->time + 5;
	m_flTimeWeaponIdle = gpGlobals->time + 0.5;
	m_flRechargeTime = gpGlobals->time + 18;
}

void CSatellite::Reload( void )
{
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 200)
		return;

	while (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 200 && m_flRechargeTime < gpGlobals->time)
	{
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]++;
		m_flRechargeTime += 1;
	}
}

void CSatellite::UpdateScreen ( void )
{
	if ( m_flTimeUpdate > UTIL_WeaponTimeBase() ) return;
	TraceResult tr;

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 199)
	{
	UTIL_TraceLine(m_pPlayer->pev->origin, Vector(0,0,16000), ignore_monsters, edict(), &tr );
	if (UTIL_PointContents(tr.vecEndPos) == CONTENT_SKY)
		{
			if( pev->skin >= 4 ) pev->skin = 0;
			pev->skin++;
		}
		else
		pev->skin = RANDOM_LONG(5,7);
	}
	else
	pev->skin = RANDOM_LONG(8,12);
	m_flTimeUpdate = UTIL_WeaponTimeBase() + 0.5;
}

void CSatellite::WeaponIdle( void )
{
	Reload( );
	UpdateScreen ();

	if ( gpGlobals->time >= m_flShockTime )
	{
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/satellite_idle.wav", 0.9, ATTN_NORM );
		m_flShockTime = gpGlobals->time + 2.45;
	}

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
	return;

	switch (RANDOM_LONG(0,1))
	{
		case 0:	SendWeaponAnim( SATELLITE_IDLE ); break;
		case 1:	SendWeaponAnim( SATELLITE_FIDGET ); break;
	}

	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT(10,15);
}