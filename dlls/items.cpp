#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"
#include "skill.h"
#include "gamerules.h"
#include "shake.h"
#include "items.h"

extern int gmsgItemPickup;

void CItem::Spawn()
{ 
	pev->movetype = MOVETYPE_TOSS;
	pev->solid = SOLID_TRIGGER;

	UTIL_SetSize(pev, Vector(-16, -16, 0), Vector(16, 16, 16));
	
	SetTouch(ItemTouch);

	if (DROP_TO_FLOOR(ENT(pev)) == 0)
	{
		ALERT(at_error, "Item %s fell out of level at %f,%f,%f", STRING( pev->classname ), pev->origin.x, pev->origin.y, pev->origin.z);
		UTIL_Remove( this );
		return;
	}

	if (!m_flRespawnTime)
		m_flRespawnTime = ITEM_RESPAWN_TIME;
}

//-----------------------------------------------------------------------------
// Purpose: Bring the item back
//-----------------------------------------------------------------------------
void CItem::Materialize()
{
	// Become visible and touchable
	pev->effects &= ~EF_NODRAW;
	SetTouch( ItemTouch );

	// Play respawn sound
	EMIT_SOUND_DYN( ENT(pev), CHAN_WEAPON, "items/item_respawn.wav", 1, ATTN_NORM, 0, 100 );
}

//-----------------------------------------------------------------------------
// Purpose: Setup the item's respawn in the time set
//-----------------------------------------------------------------------------
void CItem::Respawn( float flTime )
{
	pev->effects |= EF_NODRAW;
	SetTouch( NULL );

	if (!m_flMoneyRandomRespawn)
	{
		UTIL_SetOrigin( pev, pev->origin );
	}
		else
	{
		CBaseEntity *pSpot = NULL;
		for ( int i = RANDOM_LONG(1,5); i > 0; i-- )

		pSpot = UTIL_FindEntityByClassname( pSpot, "info_money_start" );

	if (pSpot)
	{
		Vector tmp = pSpot->pev->origin;
		UTIL_SetOrigin( pev, tmp );
		DROP_TO_FLOOR ( ENT(pev) );
	}
}

	// Come back in time
	SetThink ( Materialize );
	pev->nextthink = gpGlobals->time + flTime;
}


//-----------------------------------------------------------------------------
// Purpose: Touch function that calls the virtual touch function
//-----------------------------------------------------------------------------
void CItem::ItemTouch( CBaseEntity *pOther )
{
	// if it's not a player, ignore
	if ( !pOther->IsPlayer() )
	{
		return;
	}

	CBasePlayer *pPlayer = (CBasePlayer *)pOther;

	// ok, a player is touching this item, but can he have it?
	if ( !g_pGameRules->CanHaveItem( pPlayer, this ) )
	{
		// no? Ignore the touch.
		return;
	}

	if (MyTouch( pPlayer ))
	{
		SUB_UseTargets( pOther, USE_TOGGLE, 0 );
		SetTouch( NULL );
		
		// player grabbed the item. 
		g_pGameRules->PlayerGotItem( pPlayer, this );
		if ( g_pGameRules->ItemShouldRespawn( this ) == GR_ITEM_RESPAWN_YES )
		{
			Respawn(m_flRespawnTime); 
		}
		else
		{
			UTIL_Remove( this );
		}
	}
}

class CItemSuit : public CItem
{
	void Spawn( void )
	{ 
		SET_MODEL(ENT(pev), "models/player/gordon/gordon.mdl");
		ResetSequenceInfo();
		SetBoneController(0, 0);
		SetBoneController(1, 0);
		SetBoneController(2, 0);
		SetBoneController(3, 0);
		pev->body = 4;
		pev->sequence = 152;
		CItem::Spawn( );
	}
	BOOL MyTouch( CBasePlayer *pPlayer )
	{
		if ( pPlayer->pev->weapons & (1<<WEAPON_SUIT) )
			return FALSE;

	EMIT_SOUND( pPlayer->edict(), CHAN_STREAM, "items/money_pickup.wav", 1, ATTN_NORM );

		pPlayer->pev->weapons |= (1<<WEAPON_SUIT);
		return TRUE;
	}
};

LINK_ENTITY_TO_CLASS(item_suit, CItemSuit);

class CItemHealthkit : public CItem
{
	void Spawn( void )
	{ 
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = 1;
		CItem::Spawn( );
	}

	BOOL MyTouch( CBasePlayer *pPlayer )
	{
		if ( pPlayer->pev->deadflag != DEAD_NO )
		{
			return FALSE;
		}

		if ( pPlayer->TakeHealth(give_healthkit.value, DMG_GENERIC ) )
		{
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

			MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pPlayer->pev );
				WRITE_STRING( STRING(pev->classname) );
			MESSAGE_END();

			EMIT_SOUND(ENT(pPlayer->pev), CHAN_ITEM, "items/smallmedkit1.wav", 1, ATTN_NORM);

		return TRUE;		
	}
	return FALSE;
	}
};
LINK_ENTITY_TO_CLASS(item_healthkit, CItemHealthkit);

class CItemBattery : public CItem
{
	void Spawn( void )
	{ 
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = 2;
		CItem::Spawn( );
	}

	BOOL MyTouch( CBasePlayer *pPlayer )
	{
		if ( pPlayer->pev->deadflag != DEAD_NO )
		{
			return FALSE;
		}
	float flMaxArmor;

	if (pPlayer->m_fPowerShield)
		flMaxArmor = MAX_SHIELD_BATTERY;
	else if (pPlayer->m_fPortableHEV)
		flMaxArmor = MAX_PHEV_BATTERY;
	else
		flMaxArmor = MAX_NORMAL_BATTERY;

	if (pPlayer->pev->armorvalue < flMaxArmor)
	{
		pPlayer->pev->armorvalue += give_battery.value;
		pPlayer->pev->armorvalue = min(pPlayer->pev->armorvalue, flMaxArmor);
		pPlayer->m_iFlashBattery = pPlayer->m_iFlashBattery += give_battery.value;

		if (pPlayer->m_iFlashBattery > 100)
			pPlayer->m_iFlashBattery = 100;

		EMIT_SOUND( pPlayer->edict(), CHAN_STREAM, "items/suitchargeok1.wav", 1, ATTN_NORM );

		MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pPlayer->pev );
		WRITE_STRING( STRING(pev->classname) );
		MESSAGE_END();

		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pPlayer->pev);
		WRITE_BYTE(MSG_FLASHBATTERY);
		WRITE_BYTE(pPlayer->m_iFlashBattery);
		WRITE_ELSE(1+1);
		MESSAGE_END();

	return TRUE;		
	}
	return FALSE;
	}
};

LINK_ENTITY_TO_CLASS(item_battery, CItemBattery);


class CItemLongJump : public CItem
{
	void Spawn( void )
	{ 
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = 3;
		CItem::Spawn( );
	}
	BOOL MyTouch( CBasePlayer *pPlayer )
	{
		if ( pPlayer->m_fLongJump )
			return FALSE;

	if ( pPlayer->m_fHeavyArmor )
			return FALSE;

	pPlayer->m_fLongJump = TRUE;// player now has longjump module
	g_engfuncs.pfnSetPhysicsKeyValue( pPlayer->edict(), "slj", "1" );

	MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pPlayer->pev );
	WRITE_STRING( STRING(pev->classname) );
	MESSAGE_END();

	EMIT_SOUND( pPlayer->edict(), CHAN_STREAM, "items/longjump.wav", 1, ATTN_NORM );
	return TRUE;		
	}
};

LINK_ENTITY_TO_CLASS( item_longjump, CItemLongJump );

// Power Up Items

class CPortableHEV : public CItem
{
	void Spawn( void )
	{ 
		m_flRespawnTime = 100;
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = 2;
		CItem::Spawn( ); 
	}
	BOOL MyTouch( CBasePlayer *pPlayer ) 
	{
		if ( pPlayer->m_fPortableHEV )
		return FALSE;

		pPlayer->m_fPortableHEV = TRUE; 

		MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pPlayer->pev ); 
		WRITE_STRING( STRING(pev->classname) ); 
		MESSAGE_END();

		EMIT_SOUND(ENT(pPlayer->pev), CHAN_STREAM, "items/PortableHEV.wav", 0.8, ATTN_NORM);
		return TRUE; 
	}
};

LINK_ENTITY_TO_CLASS( item_portableHEV, CPortableHEV ); 


class CPortableHealthkit : public CItem
{
	void Spawn( void )
	{ 
		m_flRespawnTime = 80;
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = 1;
		CItem::Spawn( ); 
	}
	BOOL MyTouch( CBasePlayer *pPlayer ) 
	{
		if ( pPlayer->m_fPortableHealthkit )
		return FALSE;

		pPlayer->m_fPortableHealthkit = TRUE; 

		MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pPlayer->pev ); 
		WRITE_STRING( STRING(pev->classname) ); 
		MESSAGE_END();

		EMIT_SOUND(ENT(pPlayer->pev), CHAN_STREAM, "items/PortableHealthkit.wav", 0.8, ATTN_NORM);
		return TRUE; 
	}
};

LINK_ENTITY_TO_CLASS( item_portableHealthkit, CPortableHealthkit ); 

class CCloak : public CItem
{
	void Spawn( void )
	{ 
		m_flRespawnTime = 80;
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = 12;
		CItem::Spawn( ); 
	}
	BOOL MyTouch( CBasePlayer *pPlayer ) 
	{
		if ( pPlayer->m_fCloak )
		return FALSE;

		pPlayer->m_fCloak = TRUE; 

		MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pPlayer->pev ); 
		WRITE_STRING( STRING(pev->classname) ); 
		MESSAGE_END();

		pPlayer->m_iFlashBattery = 100;

		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pPlayer->pev);
		WRITE_BYTE(MSG_FLASHBATTERY);
		WRITE_BYTE(pPlayer->m_iFlashBattery);
		WRITE_ELSE(1+1);
		MESSAGE_END();

		EMIT_SOUND(ENT(pPlayer->pev), CHAN_STREAM, "items/Cloak.wav", 0.8, ATTN_NORM);
		return TRUE; 
	}
};

LINK_ENTITY_TO_CLASS( item_cloak, CCloak ); 

class CPowerShield : public CItem
{
	void Spawn( void )
	{ 
		m_flRespawnTime = 80;
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = 4;
		CItem::Spawn( ); 
	}
	BOOL MyTouch( CBasePlayer *pPlayer ) 
	{
	if ( pPlayer->m_fPowerShield )
	return FALSE;

		pPlayer->m_fPowerShield = TRUE; 

		MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pPlayer->pev ); 
		WRITE_STRING( STRING(pev->classname) ); 
		MESSAGE_END();

		pPlayer->pev->armorvalue += 100;
		EMIT_SOUND(ENT(pPlayer->pev), CHAN_STREAM, "items/PowerShield.wav", 0.8, ATTN_NORM);
	return TRUE; 
	}
};

LINK_ENTITY_TO_CLASS( item_PowerShield, CPowerShield );

class CAntigrav : public CItem
{
	void Spawn( void )
	{ 
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = 5;
		CItem::Spawn( ); 
	}
	BOOL MyTouch( CBasePlayer *pPlayer ) 
	{
	if ( pPlayer->m_fAntigrav )
	return FALSE;

	if(pPlayer->m_fHeavyArmor)
	return FALSE;

		pPlayer->m_fAntigrav = TRUE; 

		MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pPlayer->pev ); 
		WRITE_STRING( STRING(pev->classname) ); 
		MESSAGE_END();

		EMIT_SOUND(ENT(pPlayer->pev), CHAN_STREAM, "items/Antigrav.wav", 0.8, ATTN_NORM);
		pPlayer->m_iFlashBattery = 100;

		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pPlayer->pev);
		WRITE_BYTE(MSG_FLASHBATTERY);
		WRITE_BYTE(pPlayer->m_iFlashBattery);
		WRITE_ELSE(1+1);
		MESSAGE_END();
	return TRUE; 
	}
};
LINK_ENTITY_TO_CLASS( item_antigrav, CAntigrav );

class CPt : public CItem
{
	void Spawn( void )
	{ 
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = 6;
		CItem::Spawn( ); 
	}
	BOOL MyTouch( CBasePlayer *pPlayer ) 
	{
		if ( pPlayer->m_fpt )
		return FALSE;

		pPlayer->m_fpt = TRUE; 

		MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pPlayer->pev ); 
		WRITE_STRING( STRING(pev->classname) ); 
		MESSAGE_END();

		EMIT_SOUND(ENT(pPlayer->pev), CHAN_STREAM, "items/pt.wav", 0.8, ATTN_NORM);
		pPlayer->m_iFlashBattery = 100;

		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pPlayer->pev);
		WRITE_BYTE(MSG_FLASHBATTERY);
		WRITE_BYTE(pPlayer->m_iFlashBattery);
		WRITE_ELSE(1+1);
		MESSAGE_END();
	return TRUE; 
	}
};

LINK_ENTITY_TO_CLASS( item_pt, CPt );

class CKevlar : public CItem
{
	void Spawn( void )
	{ 
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = 7;
		CItem::Spawn( ); 
	}
	BOOL MyTouch( CBasePlayer *pPlayer ) 
	{
		if ( pPlayer->m_iKevlarBattery >= 25)
		return FALSE;

		if(pPlayer->m_fHeavyArmor)
		return FALSE;

		pPlayer->m_fKevlar = TRUE; 
		EMIT_SOUND(ENT(pPlayer->pev), CHAN_STREAM, "items/kevlar.wav", 0.8, ATTN_NORM);
		pPlayer->m_iKevlarBattery = 100;
	return TRUE; 
	}
};
LINK_ENTITY_TO_CLASS( item_kevlar, CKevlar );

class CHeavyArmor : public CItem
{
	void Spawn( void )
	{ 
		SET_MODEL(ENT(pev), "models/player/player_harmor/player_harmor.mdl");
		ResetSequenceInfo();
		SetBoneController(0, 0);
		SetBoneController(1, 0);
		SetBoneController(2, 0);
		SetBoneController(3, 0);
		pev->sequence = 152;
		CItem::Spawn( ); 
	}
	BOOL MyTouch( CBasePlayer *pPlayer ) 
	{
		if ( pPlayer->m_iHarmorBattery >= 50)
		return FALSE;

		pPlayer->m_fHeavyArmor = TRUE; 
		pPlayer->pev->maxspeed = HARMOR_MAXSPEED;
		g_engfuncs.pfnSetPhysicsKeyValue( pPlayer->edict(), "harm", "1" );
   		EMIT_SOUND(ENT(pPlayer->pev), CHAN_STREAM, "items/harmor.wav", 0.8, ATTN_NORM);
		pPlayer->m_iHarmorBattery = 500;


		char *mdl = g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer(pPlayer->edict() ), "model" );
		strncpy (pPlayer->oldmodel, mdl, TEAM_NAME_LENGTH);
		g_engfuncs.pfnSetClientKeyValue(pPlayer->entindex(), g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model", "player_harmor" );

	if(pPlayer->m_fLongJump)
	{
		pPlayer->m_fLongJump = FALSE;
		g_engfuncs.pfnSetPhysicsKeyValue( pPlayer->edict(), "slj", "0" );
		pPlayer->GiveNamedItem( "item_longjump" );
	}

	if(pPlayer->m_fAntigrav)
	{
		pPlayer->m_fAntigrav = FALSE;
		pPlayer->GiveNamedItem( "item_antigrav" );
	}

	if(pPlayer->m_fKevlar)
	{
		pPlayer->m_iKevlarBattery = 0;
		pPlayer->m_fKevlar = FALSE;
		pPlayer->GiveNamedItem( "item_kevlar" );
	}
	return TRUE; 
	}
};
LINK_ENTITY_TO_CLASS( item_harmor, CHeavyArmor );

class CItemFlashBattery : public CItem
{
	void Spawn( void )
	{ 
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = 8;
		CItem::Spawn( );
	}
		BOOL MyTouch( CBasePlayer *pPlayer )
		{
	if (pPlayer->m_iFlashBattery >= 99)
	return FALSE;

		pPlayer->m_iFlashBattery = 100;

		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pPlayer->pev);
		WRITE_BYTE(MSG_FLASHBATTERY);
		WRITE_BYTE(pPlayer->m_iFlashBattery);
		WRITE_ELSE(1+1);
		MESSAGE_END();
		EMIT_SOUND( pPlayer->edict(), CHAN_STREAM, "items/flashbattery.wav", 1, ATTN_NORM );

		MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pPlayer->pev );
		WRITE_STRING( STRING(pev->classname) );
		MESSAGE_END();
	return TRUE;
	}
};

LINK_ENTITY_TO_CLASS( item_flashbattery, CItemFlashBattery );

class CItemMoney : public CItem
{
	void Spawn( void )
	{ 
		m_flRespawnTime = 40;
		m_flMoneyRandomRespawn = 1;
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = 9;
		CItem::Spawn( );
	}
	BOOL MyTouch( CBasePlayer *pPlayer )
	{
		if (pPlayer->m_flMoneyAmount >= MAX_MONEY)
		return FALSE;

		if ( mp_nobuy.value > 0 )
		return FALSE;

		pPlayer->m_flMoneyAmount += 2500;
		EMIT_SOUND( pPlayer->edict(), CHAN_STREAM, "items/money_pickup.wav", 1, ATTN_NORM );

		MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pPlayer->pev );
		WRITE_STRING( STRING(pev->classname) );
		MESSAGE_END();
	return TRUE;
	}
};

LINK_ENTITY_TO_CLASS( item_money, CItemMoney );

class CItemMoneyCase : public CItem
{
	void Spawn( void )
	{ 
		m_flRespawnTime = 120;
		m_flMoneyRandomRespawn = 1;
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = 10;
		CItem::Spawn( );
	}
	BOOL MyTouch( CBasePlayer *pPlayer )
	{
		if (pPlayer->m_flMoneyAmount >= MAX_MONEY)
		return FALSE;

		if ( mp_nobuy.value > 0 )
		return FALSE;

		pPlayer->m_flMoneyAmount += 50000;
		EMIT_SOUND( pPlayer->edict(), CHAN_STREAM, "items/money_pickup.wav", 1, ATTN_NORM );

		MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pPlayer->pev );
		WRITE_STRING( STRING(pev->classname) );
		MESSAGE_END();
	return TRUE;
	}
};
LINK_ENTITY_TO_CLASS( item_moneycase, CItemMoneyCase );

//================== SPECIFIC ITEM FOR CTF GAME MODE ====================
//================= stolen from XDM, thanks to XWider ===================

class CCaptureObject : public CBaseAnimating
{
public:
	void Spawn(void);
	void Precache(void);
	void Think(void);
	void Touch(CBaseEntity *pOther);
	void RealUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);

	void Captured(CBaseEntity *pPlayer);
	void Taken(CBaseEntity *pPlayer);
	void Drop(CBaseEntity *pPlayer);
	void Return(CBaseEntity *pPlayer);
	void Reset(CBaseEntity *pPlayer);

	void KeyValue( KeyValueData *pkvd );

	void RealReSpawn( void );

	enum
	{
		CO_STAY,
		CO_CARRIED,
		CO_DROPPED,
	};

	enum
	{
		COU_CAPTURE = 0,
		COU_TAKEN,
		COU_DROP = 2,
		COU_RETURN = 3
	};
};

LINK_ENTITY_TO_CLASS(item_flag, CCaptureObject);

void CCaptureObject :: KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "touching_team"))
	{
		pev->team = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CBaseAnimating::KeyValue( pkvd );
}

void CCaptureObject::Spawn(void)
{
	if (!g_pGameRules->IsCTF())
	{
		UTIL_Remove(this);
		return;
	}
	Precache();
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_TRIGGER;
	pev->takedamage = DAMAGE_NO;
	pev->scale = 0.4;

	if (pev->model)
		SET_MODEL(ENT(pev), STRING(pev->model));
	else
		SET_MODEL(ENT(pev), "models/w_flag.mdl");

	UTIL_SetSize(pev, Vector(-32,-32,0), Vector(32,32,64));
	UTIL_SetOrigin(pev, pev->origin);

	if (pev->team == 0)
		pev->skin = 0;
	else if (pev->team == 1)
		pev->skin = 1;

	pev->startpos = pev->origin;
	pev->v_angle = pev->angles;
	pev->animtime = gpGlobals->time + 0.5;
	Reset(NULL);

	pev->impulse = CO_STAY;
	pev->nextthink = gpGlobals->time + 0.5;
}

void CCaptureObject::Precache(void)
{
	if (pev->model)
		PRECACHE_MODEL((char *)STRING(pev->model));
	else
		PRECACHE_MODEL("models/w_flag.mdl");

	PRECACHE_SOUND("game/ctf_alarm.wav");
	PRECACHE_SOUND("game/ctf_captured.wav");
}

void CCaptureObject::Touch(CBaseEntity *pOther)
{
	if (!pOther->IsPlayer())
		return;

	if (pev->impulse == CO_CARRIED)
		return;

	if (!pOther->IsAlive())
		return;

	CBasePlayer *pPlayer = (CBasePlayer*)pOther;

	if (g_pGameRules->GetTeamIndex(pPlayer->m_szTeamName) != pev->team)
	{
		if (pev->impulse == CO_DROPPED)
			Return(pOther);
	}
	else
		Taken(pOther);
}

void CCaptureObject::Think(void)
{
	if (pev->impulse == CO_DROPPED)
	{
		if (pev->dmgtime > 0 && pev->dmgtime <= gpGlobals->time)
			Return(NULL);
	}
	else if (pev->impulse == CO_CARRIED)
	{
		if (pev->aiment)// player
		{
			pev->origin = pev->aiment->v.origin;

			if (pev->aiment->v.velocity.Length() > 220)// value from CBasePlayer::SetAnimation
				pev->sequence = 2;
			else
				pev->sequence = 1;
		}
		else// ?!!
			Return(NULL);
	}

	pev->nextthink = gpGlobals->time + 0.1;
}

void CCaptureObject::RealUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (value == COU_CAPTURE)
	{
		Captured(pActivator);
	}
	else if (value == COU_TAKEN)
	{
		Taken(pActivator);
	}
	else if (value == COU_DROP)
	{
		Drop(pActivator);
	}
	else if (value == COU_RETURN)
	{
		Return(pActivator);
	}
}

void CCaptureObject::Reset(CBaseEntity *pPlayer)
{
	if (pPlayer && pPlayer->IsPlayer())
	{
		CBasePlayer *pClient = (CBasePlayer *)pPlayer;
		if (pClient)
			pClient->m_pCarryingObject = NULL;
	}

	ResetSequenceInfo();
	pev->movetype = MOVETYPE_NONE;
	pev->aiment = NULL;
	pev->solid = SOLID_TRIGGER;
	pev->sequence = 0;
}

void CCaptureObject::Drop(CBaseEntity *pPlayer)
{
	if (pPlayer != NULL)
	{
		if (pPlayer->IsAlive())// player used 'drop' command
		{
			UTIL_MakeVectors (pPlayer->pev->angles);
			pev->velocity = pPlayer->pev->velocity + gpGlobals->v_forward * 200;
		}
		else
			pev->velocity = pPlayer->pev->velocity;
	}

	Reset(pPlayer);
	pev->impulse = CO_DROPPED;
	pev->movetype = MOVETYPE_TOSS;
	pev->sequence = 0;
	pev->dmgtime = gpGlobals->time + ctf_flagstay.value;//give 45 seconds
}

void CCaptureObject::RealReSpawn( void )
{
	Return(NULL);
}

void CCaptureObject::Return(CBaseEntity *pPlayer)
{
	Reset(pPlayer);
	pev->impulse = CO_STAY;
	pev->origin = pev->startpos;
	UTIL_SetOrigin(pev, pev->origin);
	pev->angles = pev->v_angle;
}

void CCaptureObject::Captured(CBaseEntity *pPlayer)
{
	g_pGameRules->AddScoreToTeam(pev->team, 10);
	UTIL_EmitAmbientSound ( ENT(0), g_vecZero, "game/ctf_captured.wav", VOL_NORM, ATTN_NONE, 0, PITCH_NORM);
	UTIL_ShowMessage("#FLAG_CAPTURED", pPlayer);
	Reset(pPlayer);
	pev->impulse = CO_STAY;
	pev->origin = pev->startpos;
	UTIL_SetOrigin(pev, pev->origin);
	pev->angles = pev->v_angle;

	if (pPlayer == NULL)
		return;

	if (!pPlayer->IsPlayer())
		return;

	CBasePlayer *plr = (CBasePlayer *)pPlayer;
	plr->AddMoney (GIVE_CTF_WIN);
}

void CCaptureObject::Taken(CBaseEntity *pPlayer)
{
	if (!pPlayer)
		return;

	if (pPlayer->IsPlayer())
	{
		CBasePlayer *pClient = (CBasePlayer *)pPlayer;
		if (pClient)
		{
			pClient->m_pCarryingObject = this;
		}
	}

	EMIT_SOUND(ENT(pev), CHAN_VOICE, "game/ctf_alarm.wav", 0.5, ATTN_NORM);
	UTIL_ShowMessage("#FLAG_TAKEN", pPlayer);

	pev->impulse = CO_CARRIED;
	pev->solid = SOLID_NOT;
	pev->aiment = pPlayer->edict();
	pev->movetype = MOVETYPE_FOLLOW;
	pev->sequence = 2;
}