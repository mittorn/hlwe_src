#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "saverestore.h"
#include "skill.h"
#include "gamerules.h"
#include "weapons.h"
#include "player.h"

extern int gmsgMSGManager;

enum CHARGETYPE
{
	CHARGETYPE_HEALTH = 1,
	CHARGETYPE_ARMOR = 2,
	CHARGETYPE_FBATTERY = 3
};

class CRecharger : public CBaseToggle
{
	string_t myModel;

	CHARGETYPE	m_chargeType;

	int	m_iJuice;
	int	m_iOn;	// 0 = off, 1 = startup, 2 = going
	float   m_flSoundTime, m_flNextCharge;

	void EXPORT RespawnThink( void );

public:

	bool IsRespawnable ( void ) { return true; }

	void Killed(entvars_t *pevAttacker, int iGib);
	void Spawn( );
	void RealReSpawn( void );
	void Precache( );
	void EXPORT Off(void);
	void EXPORT Recharge(void);
	void KeyValue( KeyValueData *pkvd );
	void RealUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	int  ObjectCaps( void ) { return (CBaseToggle :: ObjectCaps() | FCAP_CONTINUOUS_USE) & ~FCAP_ACROSS_TRANSITION; }

	void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
	int RealTakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);
};

LINK_ENTITY_TO_CLASS(func_recharger, CRecharger);

void CRecharger::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "chargermdl"))
	{
		myModel = ALLOC_STRING((char *)pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "charger_type"))
	{
		m_chargeType = (CHARGETYPE)atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CRecharger::KeyValue( pkvd );
}

void CRecharger::Precache()
{
	PRECACHE_MODEL( (char *)STRING(myModel) ); 
}

void CRecharger::Spawn()
{
	Precache();
	pev->solid	= SOLID_BBOX;
	pev->movetype	= MOVETYPE_NONE;
	pev->takedamage = DAMAGE_YES;
	pev->health = pev->max_health = 125;
	SET_MODEL(ENT(pev), (char *)STRING(myModel) );
	m_iJuice = give_charger.value;
	m_iOn = 0;
	UTIL_SetOrigin(pev, pev->origin);
	pev->skin = 0;			
	
	if (pev->angles.y == 0 || pev->angles.y == 360)
		UTIL_SetSize (pev, Vector(-5, -10, -15), Vector(5, 10, 15));
	else if (pev->angles.y == 90 || pev->angles.y == 180)
		UTIL_SetSize (pev, Vector(-10, -5, -15), Vector(10, 5, 15));
	else
		UTIL_SetSize (pev, Vector(-8, -8, -15), Vector(8, 8, 15));
}

void CRecharger::RespawnThink ( )
{
	CBaseEntity *pFound = NULL;

	while ((pFound = UTIL_FindEntityInSphere(pFound, pev->origin, 75)) != NULL)
	{
		if (pFound->IsPlayer() && pFound->IsAlive())
		{
			pev->nextthink = gpGlobals->time + 1;
			return;
		}
	}

	ReSpawn();
}

void CRecharger::RealReSpawn ( void )
{
	switch( m_chargeType )
	{
		case CHARGETYPE_HEALTH:
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/medshot4.wav", 1.0, ATTN_NORM );
		break;

		case CHARGETYPE_ARMOR:
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/suitchargeok1.wav", 0.85, ATTN_NORM );
		break;

		case CHARGETYPE_FBATTERY:
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/flashbattery.wav", 0.85, ATTN_NORM );
		break;
	}

	SetThink ( SUB_DoNothing );
	pev->takedamage = DAMAGE_YES;
	pev->effects = 0;
	pev->health = pev->max_health = 125;
	m_iJuice = give_charger.value;
	m_iOn = 0;
	pev->skin = 0;			
	
	if (pev->angles.y == 0 || pev->angles.y == 360)
		UTIL_SetSize (pev, Vector(-5, -10, -15), Vector(5, 10, 15));
	else if (pev->angles.y == 90 || pev->angles.y == 180)
		UTIL_SetSize (pev, Vector(-10, -5, -15), Vector(10, 5, 15));
	else
		UTIL_SetSize (pev, Vector(-8, -8, -15), Vector(8, 8, 15));
}

void CRecharger::RealUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{ 
	if (!pActivator)
		return;

	if (!pev->health)
		return;

	if (!FClassnameIs(pActivator->pev, "player"))
		return;

	if (m_iJuice <= 0)
	{
		Off();
		pev->skin = 1;			

		switch( m_chargeType )
		{
			case CHARGETYPE_HEALTH:
				if (m_flSoundTime <= gpGlobals->time)
				{
					EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/medshotno1.wav", 1.0, ATTN_NORM );
					m_flSoundTime = gpGlobals->time + 1;
				}
			break;

			case CHARGETYPE_ARMOR:
				if (m_flSoundTime <= gpGlobals->time)
				{
					EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/suitchargeno1.wav", 0.85, ATTN_NORM );
					m_flSoundTime = gpGlobals->time + 1;
				}
			break;

			case CHARGETYPE_FBATTERY:
				if (m_flSoundTime <= gpGlobals->time)
				{
					EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/flashcharger_no.wav", 0.85, ATTN_NORM );
					m_flSoundTime = gpGlobals->time + 1;
				}
			break;
		}

		return;
	}

	m_hActivator = pActivator;
	CBasePlayer *pPlayer = (CBasePlayer *)pActivator;

	switch( m_chargeType )
	{
	case CHARGETYPE_HEALTH:
		if (m_hActivator->pev->health >= 100)
		{
			if (m_flSoundTime <= gpGlobals->time)
			{
				EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/medshotno1.wav", 1.0, ATTN_NORM );
				m_flSoundTime = gpGlobals->time + 1;
				Off();
			}
			return;
		}
	break;

	case CHARGETYPE_ARMOR:
		if ( m_hActivator->pev->armorvalue >= MAX_NORMAL_BATTERY)
		{
			if (m_flSoundTime <= gpGlobals->time)
			{
				EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/suitchargeno1.wav", 0.85, ATTN_NORM );
				m_flSoundTime = gpGlobals->time + 1;
				Off();
			}
			return;
		}
	break;

	case CHARGETYPE_FBATTERY:
		if (pPlayer->m_iFlashBattery >= 100)
		{
			if (m_flSoundTime <= gpGlobals->time)
			{
				EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/flashcharger_no.wav", 0.85, ATTN_NORM );
				m_flSoundTime = gpGlobals->time + 1;
				Off();
			}
			return;
		}
		break;
	}

	pev->nextthink = gpGlobals->time + 0.5;
	SetThink(Off);

	if (m_flNextCharge >= gpGlobals->time)
		return;

	if (m_iOn == 0)
	{
		m_iOn = 1;

		switch( m_chargeType )
		{
			case CHARGETYPE_HEALTH:
				EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/medshot4.wav", 1.0, ATTN_NORM );
			break;

			case CHARGETYPE_ARMOR:
				EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/suitchargeok1.wav", 0.85, ATTN_NORM );
			break;

			case CHARGETYPE_FBATTERY:
				EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/flashcharger_start.wav", 0.85, ATTN_NORM );
			break;
		}
	}

	else if (m_iOn >= 1)
	{
		m_iOn = 2;

		switch( m_chargeType )
		{
			case CHARGETYPE_HEALTH:
				m_iJuice--;
				m_hActivator->pev->health += 1;

				if (m_flSoundTime <= gpGlobals->time)
				{
					EMIT_SOUND(ENT(pev), CHAN_BODY, "items/medcharge4.wav", 1.0, ATTN_NORM );
					m_flSoundTime = gpGlobals->time + 0.7;
				}

	pPlayer->m_bitsDamageType &= ~(DMG_NERVEGAS);	
	pPlayer->m_bitsDamageType &= ~(DMG_RADIATION);	
	pPlayer->m_bitsDamageType &= ~(DMG_POISON);	
	pPlayer->m_bitsDamageType &= ~(DMG_ACID);	

	if (pPlayer->m_bitsDamageType & DMG_CONCUSSION)
	{
		pPlayer->m_bitsDamageType &= ~(DMG_CONCUSSION);	
		pPlayer->m_fConcussLevel = 2;
	}

	if (pPlayer->m_bitsDamageType & DMG_PARALYZE)
	{
		pPlayer->m_bitsDamageType &= ~(DMG_PARALYZE);	
		pPlayer->pev->maxspeed = 320;
	}
			break;

			case CHARGETYPE_FBATTERY:
				m_iJuice--;
				pPlayer->m_iFlashBattery += 1;

				MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pPlayer->pev);
				WRITE_BYTE(MSG_FLASHBATTERY);
				WRITE_BYTE(pPlayer->m_iFlashBattery);
				WRITE_ELSE(1+1);
				MESSAGE_END();

				if (m_flSoundTime <= gpGlobals->time)
				{
					EMIT_SOUND(ENT(pev), CHAN_BODY, "items/flashcharger_run.wav", 0.85, ATTN_NORM );
					m_flSoundTime = gpGlobals->time + 2;
				}
			break;

			case CHARGETYPE_ARMOR:
				m_iJuice--;
				m_hActivator->pev->armorvalue += 1;

				if (m_flSoundTime <= gpGlobals->time)
				{
					EMIT_SOUND(ENT(pev), CHAN_BODY, "items/suitcharge1.wav", 0.85, ATTN_NORM );
					m_flSoundTime = gpGlobals->time + 2.03;
				}
			break;
		}
	}
	m_flNextCharge = gpGlobals->time + 0.1;
}

void CRecharger::Recharge(void)
{
	switch( m_chargeType )
	{
		case CHARGETYPE_HEALTH:
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/medshot4.wav", 1.0, ATTN_NORM );
		break;

		case CHARGETYPE_ARMOR:
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/suitchargeok1.wav", 0.85, ATTN_NORM );
		break;

		case CHARGETYPE_FBATTERY:
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/flashbattery.wav", 0.85, ATTN_NORM );
		break;
	}

	m_iJuice = give_charger.value;
	pev->skin = 0;			
	SetThink( SUB_DoNothing );
}

void CRecharger::Off(void)
{
	if (m_iOn > 1)
	{
		switch( m_chargeType )
		{
			case CHARGETYPE_HEALTH:
				STOP_SOUND( ENT(pev), CHAN_BODY, "items/medcharge4.wav" );
			break;

			case CHARGETYPE_ARMOR:
				STOP_SOUND( ENT(pev), CHAN_BODY, "items/suitcharge1.wav" );
			break;

			case CHARGETYPE_FBATTERY:
				STOP_SOUND( ENT(pev), CHAN_BODY, "items/flashcharger_run.wav" );
			break;
		}
	}

	m_iOn = 0;

	if (m_iJuice <= 0)
	{
		pev->nextthink = gpGlobals->time + CHARGER_REACTIVATE_TIME;
		SetThink(Recharge);
	}
	else
		SetThink( SUB_DoNothing );
}

void CRecharger::Killed(entvars_t *pevAttacker, int iGib)
{
	Off();
	pev->health = 0;
	pev->takedamage = DAMAGE_NO;
	SetThink (RespawnThink);
	pev->nextthink = gpGlobals->time + CHARGER_REACTIVATE_TIME;//45 seconds till respawn
	pev->effects = EF_NODRAW;
	UTIL_SetSize(pev, g_vecZero, g_vecZero);

	if (m_chargeType == CHARGETYPE_HEALTH)
		FX_Explosion( pev->origin, EXPLOSION_MEDKIT );
	else
		FX_Explosion( pev->origin, EXPLOSION_HEVCHARGER );
}

void CRecharger :: TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType)
{
	if (!(bitsDamageType & (DMG_NERVEGAS | DMG_RADIATION | DMG_DROWN | DMG_BLIND | DMG_IGNOREARMOR)))
		FX_Explosion( ptr->vecEndPos, EXPLOSION_ARMORIMPACT );

	if (!pev->takedamage) 
		return;

	AddMultiDamage( pevAttacker, this, flDamage, bitsDamageType );
}

int CRecharger :: RealTakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType)
{
	if ( bitsDamageType & (DMG_RADIATION | DMG_BLIND | DMG_NERVEGAS | DMG_DROWN) )
		flDamage = 0;

	if ( (bitsDamageType & (DMG_REGENARMOR)) )
	{
		pev->health += min (flDamage, pev->max_health - pev->health);
		flDamage = 0;
	}

	if (bitsDamageType &(DMG_BLAST | DMG_NUKE | DMG_ENERGYBLAST | DMG_CRUSH))
	flDamage = flDamage*2;

	if (bitsDamageType & (DMG_SONIC | DMG_PLASMA))
	flDamage = flDamage*1.5;

	if (bitsDamageType &(DMG_ENERGYBEAM | DMG_GENERIC))
	flDamage = flDamage*0.75;

	if (bitsDamageType &(DMG_SHOCK | DMG_IGNITE | DMG_BURN | DMG_BULLETMAGNUM))
	flDamage = flDamage*0.6;

	if (bitsDamageType &(DMG_SLASH | DMG_BULLET | DMG_BULLETBUCKSHOT | DMG_FREEZE))
	flDamage = flDamage*0.45;

	if (bitsDamageType &(DMG_ACID | DMG_POISON))
	flDamage = flDamage*0.2;

	float flDamageTook =  CBaseToggle::RealTakeDamage(pevInflictor, pevAttacker, (int)flDamage, bitsDamageType);

	if (pev->health <= 0)
	{
		if (m_chargeType == CHARGETYPE_HEALTH)
			::RadiusDamage (pev->origin, pevInflictor, pevAttacker, 100, 250, CLASS_NONE, DMG_BLAST);
		else
			::RadiusDamage (pev->origin, pevInflictor, pevAttacker, 125, 300, CLASS_NONE, DMG_ENERGYBLAST | DMG_NEVERGIB);
		return 0;
	}
	return flDamageTook;
}