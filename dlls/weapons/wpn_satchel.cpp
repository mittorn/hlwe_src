#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"
#include "projectiles.h"

class CSatchel : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo(ItemInfo *p);
	void PrimaryAttack( void );
	void SecondaryAttack( void );
	int AddDuplicate( CBasePlayerItem *pOriginal );
	BOOL CanDeploy( void );
	BOOL Deploy( void );
	BOOL IsUseable( void );
	
	void Holster( void );
	void WeaponIdle( void );
	void Throw( void );
	int m_chargeReady;
	void SellWeapon( void );
};

void CSatchel::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] * COST_SATCHEL/2);
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] = 0;
}

enum satchel_e
{
	SATCHEL_IDLE1 = 0,
	SATCHEL_FIDGET1,
	SATCHEL_DRAW,
	SATCHEL_DROP,
	SATCHEL_HOLSTER,
	SATCHEL_IDLE_RADIO,
	SATCHEL_FIDGET_RADIO,
	SATCHEL_DRAW_RADIO,
	SATCHEL_FIRE_RADIO,
	SATCHEL_HOLSTER_RADIO
};
LINK_ENTITY_TO_CLASS( weapon_satchel, CSatchel );

int CSatchel::AddDuplicate( CBasePlayerItem *pOriginal )
{
	CSatchel *pSatchel;
	pSatchel = (CSatchel *)pOriginal;

	if ( pSatchel->m_chargeReady != 0 )
	{
		// player has some satchels deployed. Refuse to add more.
		return FALSE;
	}
	return CBasePlayerWeapon::AddDuplicate ( pOriginal );
}

void CSatchel::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_satchel.mdl");
	m_iId = WEAPON_SATCHEL;
	m_iDefaultAmmo = 1;
	FallInit();
}

void CSatchel::Precache( void )
{
	PRECACHE_MODEL("models/v_satchel.mdl");
	PRECACHE_MODEL("models/p_satchel.mdl");
	PRECACHE_MODEL("models/p_satchel_radio.mdl");
	PRECACHE_SOUND("weapons/g_bounce1.wav");
}

int CSatchel::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "Satchel Charge";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 7;
	p->iPosition = 3;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY | ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE;
	p->iId = WEAPON_SATCHEL;
	p->iWeight = SATCHEL_WEIGHT;
	p->iMaxspeed = SATCHEL_MAXSPEED;
	return 1;
}

BOOL CSatchel::IsUseable( void )
{
	if ( m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] > 0 ) 
	{
		// player is carrying some satchels
		return TRUE;
	}

	if ( m_chargeReady != 0 )
	{
		// player isn't carrying any satchels, but has some out
		return TRUE;
	}
	return FALSE;
}

BOOL CSatchel::CanDeploy( void )
{
	if ( m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] > 0 ) 
		return TRUE;

	if ( m_chargeReady != 0 )
		return TRUE;

	return FALSE;
}

BOOL CSatchel::Deploy( )
{
	m_pPlayer->m_flNextAttack = gpGlobals->time + 1.0;
	m_flTimeWeaponIdle = gpGlobals->time + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );

	if ( m_chargeReady )
		return DefaultDeploy( "models/v_satchel.mdl", "models/p_satchel_radio.mdl", SATCHEL_DRAW_RADIO, "hive" );
	else
		return DefaultDeploy( "models/v_satchel.mdl", "models/p_satchel.mdl", SATCHEL_DRAW, "trip" );
	return TRUE;
}

void CSatchel::Holster( )
{
	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.5;
	
	if ( m_chargeReady )
		SendWeaponAnim( SATCHEL_HOLSTER_RADIO );
	else
		SendWeaponAnim( SATCHEL_DROP );

	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "common/null.wav", 1.0, ATTN_NORM);

	if ( !m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] && !m_chargeReady )
	{
		m_pPlayer->pev->weapons &= ~(1<<WEAPON_SATCHEL);
		SetThink( DestroyItem );
		pev->nextthink = gpGlobals->time + 0.1;
	}
}

void CSatchel::PrimaryAttack()
{
	switch (m_chargeReady)
	{
	case 0:
		{
		Throw( );
		}
		break;
	case 1:
		{
		SendWeaponAnim( SATCHEL_FIRE_RADIO );
		edict_t *pPlayer = m_pPlayer->edict( );
		CBaseEntity *pSatchel = NULL;

		while ((pSatchel = UTIL_FindEntityInSphere( pSatchel, m_pPlayer->pev->origin, 16384)) != NULL)
		{
			if (FClassnameIs( pSatchel->pev, "monster_satchel"))
			{
				if (pSatchel->pev->owner == pPlayer)
				{
					pSatchel->Use( m_pPlayer, m_pPlayer, USE_ON, 0 );
					m_chargeReady = 2;
				}
			}
		}

		m_chargeReady = 2;
		m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		m_flNextSecondaryAttack = gpGlobals->time + 0.5;
		m_flTimeWeaponIdle = gpGlobals->time + 0.5;
		break;
		}

	case 2:
		{
		}
		break;
	}
}


void CSatchel::SecondaryAttack( void )
{
	PrimaryAttack();
}


void CSatchel::Throw( void )
{
	if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] )
	{
		UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
		CSCharge::ShootSCharge( m_pPlayer->pev, m_pPlayer->pev->origin, gpGlobals->v_forward * 350 + m_pPlayer->pev->velocity );

		m_pPlayer->pev->weaponmodel = MAKE_STRING("models/p_satchel_radio.mdl");
		SendWeaponAnim( SATCHEL_DRAW_RADIO );
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

		m_chargeReady = 1;
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
		m_flNextPrimaryAttack = gpGlobals->time + 1.0;
		m_flNextSecondaryAttack = gpGlobals->time + 0.5;
	}
}


void CSatchel::WeaponIdle( void )
{
	if ( m_flTimeWeaponIdle > gpGlobals->time )
		return;

	switch( m_chargeReady )
	{
	case 0:
		SendWeaponAnim( SATCHEL_FIDGET1 );
		strcpy( m_pPlayer->m_szAnimExtention, "trip" );
		break;
	case 1:
		SendWeaponAnim( SATCHEL_FIDGET_RADIO );
		strcpy( m_pPlayer->m_szAnimExtention, "hive" );
		break;
	case 2:
		if ( !m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] )
		{
			m_chargeReady = 0;
			RetireWeapon();
			return;
		}

		m_pPlayer->pev->weaponmodel = MAKE_STRING("models/p_satchel.mdl");

		SendWeaponAnim( SATCHEL_DRAW );

		strcpy( m_pPlayer->m_szAnimExtention, "trip" );

		m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		m_flNextSecondaryAttack = gpGlobals->time + 0.5;
		m_chargeReady = 0;
		break;
	}
	m_flTimeWeaponIdle = gpGlobals->time + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );// how long till we do this again.
}

//=========================================================
// DeactivateSatchels - removes all satchels owned by
// the provided player. Should only be used upon death.
//
// Made this global on purpose.
//=========================================================
void DeactivateSatchels( CBasePlayer *pOwner )
{
	edict_t *pFind; 
	pFind = FIND_ENTITY_BY_CLASSNAME( NULL, "monster_satchel" );

	while ( !FNullEnt( pFind ) )
	{
		CBaseEntity *pEnt = CBaseEntity::Instance( pFind );
		CSCharge *pSatchel = (CSCharge *)pEnt;

		if ( pSatchel )
		{
			if ( pSatchel->pev->owner == pOwner->edict() )
			{
				pSatchel->Deactivate();
			}
		}
		pFind = FIND_ENTITY_BY_CLASSNAME( pFind, "monster_satchel" );
	}
}