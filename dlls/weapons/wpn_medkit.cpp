#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"
#include "decals.h"

enum Medkit_e
{
	MEDKIT_IDLE,
	MEDKIT_LONGIDLE,
	MEDKIT_LONGUSE,
	MEDKIT_SHORTUSE,
	MEDKIT_HOLSTER,
	MEDKIT_DRAW
};

class CMedkit : public CBasePlayerWeapon
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

	void BuyPrimaryAmmo( void );
	void SellWeapon( void );
};
LINK_ENTITY_TO_CLASS( weapon_medkit, CMedkit );

void CMedkit::BuyPrimaryAmmo( void )
{
	BuyAmmo(give_healthkit.value * 2, (char*)pszAmmo1(), COST_MEDKIT);
}

void CMedkit::SellWeapon( void )
{
	m_pPlayer->m_flMoneyAmount += ((m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]/(give_healthkit.value * 2)) * COST_MEDKIT/2);
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] = 0;
}

void CMedkit::Spawn( )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/p_medkit.mdl");
	m_iId = WEAPON_MEDKIT;
	m_iDefaultAmmo = give_healthkit.value * 2;
	FallInit();
}

void CMedkit::Precache( void )
{
	PRECACHE_MODEL("models/v_medkit.mdl");
	PRECACHE_MODEL("models/p_medkit.mdl");
	PRECACHE_SOUND("weapons/medkit_fire.wav");
}

BOOL CMedkit::Deploy( void )
{
	return DefaultDeploy( "models/v_medkit.mdl", "models/p_medkit.mdl", MEDKIT_DRAW, "trip", 0.8 );
}

void CMedkit::Holster( )
{
	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.5;
	SendWeaponAnim( MEDKIT_HOLSTER );
}

int CMedkit::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "Health";
	p->iMaxAmmo1 = 999;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 0;
	p->iPosition = 1;
	p->iId = WEAPON_MEDKIT;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY;
	p->iWeight = MEDKIT_WEIGHT;
	p->iMaxspeed = MEDKIT_MAXSPEED;
	return 1;
}

void CMedkit::PrimaryAttack()
{
	if (m_pPlayer->pev->health > 149)
	{
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "items/medshotno1.wav", 0.9, ATTN_NORM );
		m_flNextPrimaryAttack = gpGlobals->time+0.5;
		return;
	}

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "items/medshotno1.wav", 0.9, ATTN_NORM );
		m_pPlayer->m_flNextAttack = gpGlobals->time + 0.5;
		return;
	}

	if ( gpGlobals->time >= m_flShockTime )
	{
		SendWeaponAnim( MEDKIT_SHORTUSE );
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "items/medshot4.wav", 0.9, ATTN_NORM );
		m_flShockTime = gpGlobals->time + 1.1;
	}

	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
	m_pPlayer->pev->health += 1;

	m_pPlayer->m_bitsDamageType &= ~(DMG_NERVEGAS);	
	m_pPlayer->m_bitsDamageType &= ~(DMG_RADIATION);	
	m_pPlayer->m_bitsDamageType &= ~(DMG_POISON);	
	m_pPlayer->m_bitsDamageType &= ~(DMG_ACID);	

	if (m_pPlayer->m_bitsDamageType & DMG_CONCUSSION)
	{
		m_pPlayer->m_bitsDamageType &= ~(DMG_CONCUSSION);	
		m_pPlayer->m_fConcussLevel = 2;
	}

	if (m_pPlayer->m_bitsDamageType & DMG_PARALYZE)
	{
		m_pPlayer->m_bitsDamageType &= ~(DMG_PARALYZE);	
		m_pPlayer->pev->maxspeed = 320;
	}

	m_flNextPrimaryAttack = gpGlobals->time + 0.15;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 10, 15 );
}

void CMedkit::SecondaryAttack()
{
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "items/medshotno1.wav", 0.9, ATTN_NORM );
		m_pPlayer->m_flNextAttack = gpGlobals->time + 0.5;
		return;
	}

	TraceResult tr;
	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	UTIL_TraceLine(m_pPlayer->GetGunPosition(), m_pPlayer->GetGunPosition() + (gpGlobals->v_forward*50), dont_ignore_monsters, m_pPlayer->edict(), &tr);

	if (!tr.pHit->v.takedamage)
		return;

	CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	if (g_pGameRules->PlayerRelationship(m_pPlayer, pEntity) == GR_TEAMMATE)
	{
		if (pEntity->pev->health > 149)
		{
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "items/medshotno1.wav", 0.9, ATTN_NORM );
			return;
		}
		pEntity->TakeDamage ( m_pPlayer->pev, m_pPlayer->pev, -1, DMG_GENERIC);
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;

		if ( gpGlobals->time >= m_flShockTime )
		{
			SendWeaponAnim( MEDKIT_SHORTUSE );
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "items/medshot4.wav", 0.9, ATTN_NORM );
			m_flShockTime = gpGlobals->time + 1;
		}
	}
	else
	{
		pEntity->TakeDamage ( m_pPlayer->pev, m_pPlayer->pev, dmg_medkit.value* (mp_wpn_power.value/100), DMG_POISON);
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
		if ( gpGlobals->time >= m_flShockTime )
		{
			SendWeaponAnim( MEDKIT_LONGUSE );
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/medkit_fire.wav", 0.9, ATTN_NORM );
			m_flShockTime = gpGlobals->time + 1.5;
		}
	}
	m_flNextPrimaryAttack= m_flNextSecondaryAttack = gpGlobals->time + 0.2;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 5, 7 );
}

void CMedkit::WeaponIdle( void )
{
	ResetEmptySound( );

	if (m_flTimeWeaponIdle > gpGlobals->time)
		return;

	int iAnim;
	switch ( RANDOM_LONG( 0, 1 ) )
	{
	case 0:	
		iAnim = MEDKIT_LONGIDLE;	
		break;
	
	default:
	case 1:
		iAnim = MEDKIT_IDLE;
		break;
	}

	SendWeaponAnim( iAnim );

	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 10, 15 );
}