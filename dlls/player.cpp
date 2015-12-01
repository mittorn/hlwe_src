#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "trains.h"
#include "nodes.h"
#include "weapons.h"
#include "soundent.h"
#include "monsters.h"
#include "shake.h"
#include "decals.h"
#include "gamerules.h"
#include "game.h"
#include "hltv.h"
#include "skill.h"
#include "projectiles.h"

extern DLL_GLOBAL ULONG	g_ulModelIndexPlayer;
extern DLL_GLOBAL BOOL	g_fGameOver;
extern DLL_GLOBAL BOOL	g_fDrawLines;
extern DLL_GLOBAL int	gDisplayTitle;

BOOL gInitHUD = TRUE;

extern void CopyToBodyQue(entvars_t* pev, BOOL Ignite);
extern void respawn(entvars_t *pev, BOOL fCopyCorpse);
extern Vector VecBModelOrigin(entvars_t *pevBModel );

// the world node graph
extern CGraph	WorldGraph;

#define TRAIN_ACTIVE	0x80 
#define TRAIN_NEW	0xc0
#define TRAIN_OFF	0x00
#define TRAIN_NEUTRAL	0x01
#define TRAIN_SLOW	0x02
#define TRAIN_MEDIUM	0x03
#define TRAIN_FAST	0x04 
#define TRAIN_BACK	0x05

#define	FLASH_DRAIN_TIME 3

// Global Savedata for player
TYPEDESCRIPTION	CBasePlayer::m_playerSaveData[] = 
{
	DEFINE_FIELD( CBasePlayer, m_flFlashLightTime, FIELD_TIME ),
	DEFINE_FIELD( CBasePlayer, m_iFlashBattery, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_iKevlarBattery, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_iHarmorBattery, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_flMoneyAmount, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_iWeapons2, FIELD_INTEGER ),

	DEFINE_FIELD( CBasePlayer, m_afButtonLast, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_afButtonPressed, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_afButtonReleased, FIELD_INTEGER ),

	DEFINE_ARRAY( CBasePlayer, m_rgItems, FIELD_INTEGER, MAX_ITEMS ),
	DEFINE_FIELD( CBasePlayer, m_afPhysicsFlags, FIELD_INTEGER ),

	DEFINE_FIELD( CBasePlayer, m_flTimeStepSound, FIELD_TIME ),
	DEFINE_FIELD( CBasePlayer, m_flTimeWeaponIdle, FIELD_TIME ),
	DEFINE_FIELD( CBasePlayer, m_flSwimTime, FIELD_TIME ),
	DEFINE_FIELD( CBasePlayer, m_flDuckTime, FIELD_TIME ),
	DEFINE_FIELD( CBasePlayer, m_flWallJumpTime, FIELD_TIME ),

	DEFINE_FIELD( CBasePlayer, m_lastDamageAmount, FIELD_INTEGER ),

	DEFINE_ARRAY( CBasePlayer, m_rgpPlayerItems, FIELD_CLASSPTR, MAX_ITEM_TYPES ),
	DEFINE_FIELD( CBasePlayer, m_pActiveItem, FIELD_CLASSPTR ),
	DEFINE_FIELD( CBasePlayer, m_pLastItem, FIELD_CLASSPTR ),
	DEFINE_FIELD( CBasePlayer, m_pNextItem, FIELD_CLASSPTR ),
	
	DEFINE_ARRAY( CBasePlayer, m_rgAmmo, FIELD_INTEGER, MAX_AMMO_SLOTS ),
	DEFINE_FIELD( CBasePlayer, m_idrowndmg, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_idrownrestored, FIELD_INTEGER ),

	DEFINE_FIELD( CBasePlayer, m_iTrain, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_bitsHUDDamage, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_flFallVelocity, FIELD_FLOAT ),
	DEFINE_FIELD( CBasePlayer, m_iTargetVolume, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_iExtraSoundTypes, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_fLongJump, FIELD_BOOLEAN ),
	DEFINE_FIELD( CBasePlayer, m_fInitHUD, FIELD_BOOLEAN ),
	DEFINE_FIELD( CBasePlayer, m_tbdPrev, FIELD_TIME ),

	DEFINE_FIELD( CBasePlayer, m_fPortableHEV, FIELD_BOOLEAN ),
	DEFINE_FIELD( CBasePlayer, m_fpt, FIELD_BOOLEAN ),
	DEFINE_FIELD( CBasePlayer, m_fTurretSentry, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_fTurretMissile, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_fTurretIon, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_fPortableHealthkit, FIELD_BOOLEAN ),
	DEFINE_FIELD( CBasePlayer, m_fCloak, FIELD_BOOLEAN ),
	DEFINE_FIELD( CBasePlayer, m_fPowerShield, FIELD_BOOLEAN ),
	DEFINE_FIELD( CBasePlayer, m_fAntigrav, FIELD_BOOLEAN ),
	DEFINE_FIELD( CBasePlayer, m_fKevlar, FIELD_BOOLEAN ),
	DEFINE_FIELD( CBasePlayer, m_fHeavyArmor, FIELD_BOOLEAN ),

	DEFINE_FIELD( CBasePlayer, m_iHideHUD, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_iFOV, FIELD_INTEGER ),

// rain tutorial
	DEFINE_FIELD( CBasePlayer, Rain_dripsPerSecond, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, Rain_windX, FIELD_FLOAT ),
	DEFINE_FIELD( CBasePlayer, Rain_windY, FIELD_FLOAT ),
	DEFINE_FIELD( CBasePlayer, Rain_randX, FIELD_FLOAT ),
	DEFINE_FIELD( CBasePlayer, Rain_randY, FIELD_FLOAT ),

	DEFINE_FIELD( CBasePlayer, Rain_ideal_dripsPerSecond, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, Rain_ideal_windX, FIELD_FLOAT ),
	DEFINE_FIELD( CBasePlayer, Rain_ideal_windY, FIELD_FLOAT ),
	DEFINE_FIELD( CBasePlayer, Rain_ideal_randX, FIELD_FLOAT ),
	DEFINE_FIELD( CBasePlayer, Rain_ideal_randY, FIELD_FLOAT ),
	DEFINE_FIELD( CBasePlayer, Rain_endFade, FIELD_TIME ),
	DEFINE_FIELD( CBasePlayer, Rain_nextFadeUpdate, FIELD_TIME ),
};	
int gmsgMSGManager = 0;

int gmsgShake = 0;
int gmsgFade = 0;
int gmsgResetHUD = 0;
int gmsgInitHUD = 0;
int gmsgCurWeapon = 0;
int gmsgDamage = 0;
int gmsgWeaponList = 0;
int gmsgAmmoX = 0;
int gmsgHudText = 0;
int gmsgDeathMsg = 0;
int gmsgScoreInfo = 0;
int gmsgTeamInfo = 0;
int gmsgTeamScore = 0;
int gmsgGameMode = 0;
int gmsgMOTD = 0;
int gmsgServerName = 0;
int gmsgAmmoPickup = 0;
int gmsgWeapPickup = 0;
int gmsgItemPickup = 0;
int gmsgHideWeapon = 0;
int gmsgSetCurWeap = 0;
int gmsgSayText = 0;
int gmsgTextMsg = 0;
int gmsgVGUIMenu = 0;
int gmsgTeamNames = 0;
int gmsgStatusText = 0;
int gmsgStatusValue = 0; 
int gmsgSpecTank = 0;

int gmsgExplosion = 0;
int gmsgWorldExp = 0;
int gmsgImpBullet = 0;
int gmsgImpRocket = 0;
int gmsgImpBeam = 0;
int gmsgRain = 0;
int gmsgAurora = 0;
int gmsgFireGun = 0;
int gmsgFireBeam = 0;
int gmsgBrassClip = 0;
int gmsgJoinMenu = 0;
int gmsgPlrGib = 0;
int gmsgTrail = 0;
int gmsgBreakGib = 0;

// for MSG Manager
void WRITE_ELSE(int i)
{
	for(int j=i;j<MSGMGRMAXSIZE;j++)
		WRITE_BYTE(0);
}

void LinkUserMessages( void )
{
	// Already taken care of?
	if ( gmsgInitHUD )
		return;

	gmsgDamage = REG_USER_MSG( "Damage", 12);
	gmsgTeamInfo = REG_USER_MSG( "TeamInfo", -1);
	gmsgTeamScore = REG_USER_MSG( "TeamScore", -1);

	gmsgInitHUD = REG_USER_MSG("InitHUD", 0 );		// called every time a new player joins the server
	gmsgResetHUD = REG_USER_MSG("ResetHUD", 1);		// called every respawn
	gmsgCurWeapon = REG_USER_MSG("CurWeapon", 3);
	gmsgHudText = REG_USER_MSG( "HudText", -1 );
	gmsgSayText = REG_USER_MSG( "SayText", -1 );
	gmsgTextMsg = REG_USER_MSG( "TextMsg", -1 );
	gmsgWeaponList = REG_USER_MSG("WeaponList", -1);
	gmsgDeathMsg = REG_USER_MSG( "DeathMsg", -1 );
	gmsgScoreInfo = REG_USER_MSG( "ScoreInfo", 9);
	gmsgGameMode = REG_USER_MSG( "GameMode", 1 );
	gmsgMOTD = REG_USER_MSG( "MOTD", -1 );
	gmsgServerName = REG_USER_MSG( "ServerName", -1 );
	gmsgAmmoPickup = REG_USER_MSG( "AmmoPickup", 2 );
	gmsgWeapPickup = REG_USER_MSG( "WeapPickup", 1 );
	gmsgItemPickup = REG_USER_MSG( "ItemPickup", -1 );
	gmsgHideWeapon = REG_USER_MSG( "HideWeapon", 1 );
	gmsgShake = REG_USER_MSG("ScreenShake", sizeof(ScreenShake));
	gmsgFade = REG_USER_MSG("ScreenFade", sizeof(ScreenFade));
	gmsgAmmoX = REG_USER_MSG("AmmoX", 3);
	gmsgTeamNames = REG_USER_MSG( "TeamNames", -1 );
	gmsgStatusText = REG_USER_MSG("StatusText", -1);
	gmsgStatusValue = REG_USER_MSG("StatusValue", 3); 
	gmsgAurora = REG_USER_MSG( "Aurora", -1);
	gmsgSpecTank = REG_USER_MSG("SpecTank", -1);

	gmsgMSGManager = REG_USER_MSG( "MSGManager", MSGMGRMAXSIZE );
	gmsgRain = REG_USER_MSG("Rain", 15);
	gmsgImpBullet = REG_USER_MSG("ImpBullet", 21);
	gmsgImpRocket = REG_USER_MSG("ImpRocket", 15);
	gmsgImpBeam = REG_USER_MSG("ImpBeam", 14);
	gmsgExplosion = REG_USER_MSG("Explosion", 7);
	gmsgWorldExp = REG_USER_MSG("WorldExp", 13);
	gmsgFireBeam = REG_USER_MSG("FireBeam", 19);
	gmsgFireGun = REG_USER_MSG("FireGun", 10);
	gmsgVGUIMenu = REG_USER_MSG("VGUIMenu", 1);
	gmsgBrassClip = REG_USER_MSG("BrassClip", 14);
	gmsgJoinMenu = REG_USER_MSG("JoinMenu", 3);
	gmsgPlrGib = REG_USER_MSG("PlrGib", 7);
	gmsgTrail = REG_USER_MSG("Trail", 9);
	gmsgBreakGib = REG_USER_MSG("BreakGib", 11);
}

LINK_ENTITY_TO_CLASS( player, CBasePlayer );


Vector VecVelocityForDamage(float flDamage)
{
	Vector vec(RANDOM_FLOAT(-100,100), RANDOM_FLOAT(-100,100), RANDOM_FLOAT(200,300));

	if (flDamage > -50)
		vec = vec * 0.7;
	else if (flDamage > -200)
		vec = vec * 2;
	else
		vec = vec * 10;
	
	return vec;
}

int TrainSpeed(int iSpeed, int iMax)
{
	float fSpeed, fMax;
	int iRet = 0;

	fMax = (float)iMax;
	fSpeed = iSpeed;

	fSpeed = fSpeed/fMax;

	if (iSpeed < 0)
		iRet = TRAIN_BACK;
	else if (iSpeed == 0)
		iRet = TRAIN_NEUTRAL;
	else if (fSpeed < 0.33)
		iRet = TRAIN_SLOW;
	else if (fSpeed < 0.66)
		iRet = TRAIN_MEDIUM;
	else
		iRet = TRAIN_FAST;

	return iRet;
}

void CBasePlayer :: DeathSound( void )
{
	switch (RANDOM_LONG(0,3))
	{
		case 0:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/death1.wav", 1, ATTN_NORM); break;
		case 1:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/death2.wav", 1, ATTN_NORM); break;
		case 2:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/death3.wav", 1, ATTN_NORM); break;
		case 3:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/death4.wav", 1, ATTN_NORM); break;
	}
}


int CBasePlayer :: TakeHealth( float flHealth, int bitsDamageType )
{
	return CBaseMonster :: TakeHealth (flHealth, bitsDamageType);
}

Vector CBasePlayer :: GetGunPosition( )
{
	Vector origin;
	origin = pev->origin + pev->view_ofs;
	return origin;
}

//=========================================================
// TraceAttack
//=========================================================
#define ZERO 1.0e-8

void CBasePlayer :: TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType)
{
	if ( (pev->takedamage == DAMAGE_NO) || (IsAlive() == FALSE) )
		return;

	CBaseEntity *pAttacker = CBaseEntity::Instance(pevAttacker);

//  can't hurt yourself, if this is set!
	if (CVAR_GET_FLOAT("mp_noselfdamage")==1 && pAttacker==this )
		return;

//teammates can repair heavy armor
	if ( m_fHeavyArmor && (bitsDamageType & (DMG_REGENARMOR)) )
	{
		m_iHarmorBattery += min(flDamage, 500 - m_iHarmorBattery);
		flDamage = 0;
	}


//  can't hurt teammates, if this is set!
	if ( g_pGameRules->PlayerRelationship( this, pAttacker ) == GR_TEAMMATE && pAttacker != this )
	{
		if (CVAR_GET_FLOAT( "mp_noteamdamage" )== 1)
		return;
	}

	if ( !g_pGameRules->FPlayerCanTakeDamage(this, pAttacker) )
		return;

	if (pev->takedamage)
	{

//=====================
//Moved from TakeDamage
//=====================
	if (bitsDamageType & DMG_KNOCKBACK)
	{
		if(!pev->armorvalue)
		if(!m_fHeavyArmor)
		pev->velocity = pev->velocity + vecDir * (flDamage*20);
		else
		pev->velocity = pev->velocity + vecDir * (flDamage*5);
	}

//================================================
//Shieldgun shield code (simplified by LLAPb)
//================================================
	if (m_fShieldProtection == TRUE)
	{
		UTIL_MakeVectors (pev->angles);//Do not count visible angle

		Vector dir2D = gpGlobals->v_forward;
		dir2D.z = 0;

		Vector hit2D = ptr->vecEndPos - pev->origin;
		hit2D.z = 0;

		float angle = DotProduct(dir2D.Normalize(), hit2D.Normalize());

		if (angle > ZERO)
		{
			if (bitsDamageType & (DMG_BLAST | DMG_SHOCK | DMG_FREEZE))
				flDamage = flDamage*0.7;

			if (bitsDamageType & (DMG_SLASH | DMG_BULLET | DMG_BULLETBUCKSHOT))
			{
				flDamage = flDamage*0;
				FX_Explosion( ptr->vecEndPos, EXPLOSION_ARMORIMPACT );
				return;
			}

			if (bitsDamageType & (DMG_PARALYZE | DMG_ACID | DMG_POISON))
				flDamage = flDamage*0.3;

			if (bitsDamageType & (DMG_NERVEGAS | DMG_RADIATION | DMG_DROWN | DMG_CRUSH ))
				flDamage = flDamage;

			if (bitsDamageType & (DMG_IGNITE | DMG_BLIND | DMG_BURN))
				flDamage = flDamage*0.45;

			if (bitsDamageType & DMG_BULLETMAGNUM)
			{	
				flDamage = flDamage*0.35;
				FX_Explosion( ptr->vecEndPos, EXPLOSION_ARMORIMPACT );
			}
			
			if (bitsDamageType & (DMG_ENERGYBLAST | DMG_SONIC | DMG_ENERGYBEAM | DMG_PLASMA | DMG_NUKE))
			{
				flDamage = flDamage*0.85;
				FX_Explosion( ptr->vecEndPos, EXPLOSION_ARMORIMPACT );
			}
		}
	}
//=====================
//Shieldgun shield code
//=====================
//Heavy armor protection FX_
// пули и лазеры опред. сами этот тип поверхности и рисуют нужный эффект
//	if (!pev->armorvalue && m_fHeavyArmor && !(bitsDamageType & (DMG_NERVEGAS | DMG_RADIATION | DMG_DROWN | DMG_BLIND | DMG_ACID | DMG_POISON | DMG_BURN | DMG_IGNITE | DMG_FREEZE | DMG_IGNOREARMOR | DMG_REGENARMOR)))
//		FX_Explosion( ptr->vecEndPos, EXPLOSION_ARMORIMPACT );


//Heavy armor protection

		m_LastHitGroup = ptr->iHitgroup;

		if (pev->armorvalue && !(bitsDamageType & (DMG_NERVEGAS | DMG_RADIATION | DMG_DROWN | DMG_BLIND | DMG_IGNOREARMOR | DMG_REGENARMOR)))
		{
			flDamage *= dmg_player_armor.value;
		}
		else if (!pev->armorvalue && (bitsDamageType & ( DMG_RADIATION | DMG_BLIND | DMG_NERVEGAS | DMG_BURN | DMG_IGNITE)))
		{
			flDamage *= dmg_player_chest.value;
		}
		else
		switch (ptr->iHitgroup)
		{
		case HITGROUP_GENERIC:
		break;

		case HITGROUP_HEAD:
			lastDamageType |= DMG_HEADSHOT;

			if (m_fHeavyArmor)
				flDamage *= dmg_player_stomach.value;
			else
				flDamage *= dmg_player_head.value;

			if ((bitsDamageType & DMG_BULLETBUCKSHOT) && flDamage > pev->health && !m_fFrozen && !m_fIgnite && !m_fHeavyArmor)
			{
				FX_PlrGib( ptr->vecEndPos, GIBBED_HEAD );
				pev->body = 2;
			}
			if (!(bitsDamageType & DMG_NOBLOOD))
			{
				if (!m_fPortableHealthkit && flDamage > 100 && !m_fFrozen && !m_fIgnite && !m_fHeavyArmor)
				{
					FX_PlrGib( ptr->vecEndPos, GIBBED_HEAD );
					pev->body = 2;
				}
			}
			if (!(bitsDamageType & DMG_NOBLOOD))
			{
				if (m_fPortableHealthkit && flDamage > 150 && !m_fFrozen && !m_fIgnite && !m_fHeavyArmor)
				{
					FX_PlrGib( ptr->vecEndPos, GIBBED_HEAD );
					pev->body = 2;
				}
			}
			if (!(bitsDamageType & DMG_NOBLOOD) && !m_fHeavyArmor)
			{
				FX_FireBeam( ptr->vecEndPos, vecDir, ptr->vecPlaneNormal, BEAM_BLOODSTREAM  );

				switch (RANDOM_LONG(0,2))
				{
					case 0:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/headshot1.wav", 1, ATTN_NORM); break;
					case 1:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/headshot2.wav", 1, ATTN_NORM); break;
					case 2:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/headshot3.wav", 1, ATTN_NORM); break;
				}
			}
		break;

		case HITGROUP_CHEST:
			lastDamageType &= ~DMG_HEADSHOT;
			flDamage *= dmg_player_chest.value;
		break;

		case HITGROUP_STOMACH:
			lastDamageType &= ~DMG_HEADSHOT;

			if (m_fHeavyArmor)
				flDamage *= dmg_player_chest.value;
			else
				flDamage *= dmg_player_stomach.value;
		break;


		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
			lastDamageType &= ~DMG_HEADSHOT;
			flDamage *= dmg_player_arm.value;
		break;

		case HITGROUP_LEFTLEG:
		case HITGROUP_RIGHTLEG:
			lastDamageType &= ~DMG_HEADSHOT;
			flDamage *= dmg_player_leg.value;
		break;

		default:
			lastDamageType &= ~DMG_HEADSHOT;
		break;
		}

		if (!(bitsDamageType & DMG_NOBLOOD) && !m_fHeavyArmor)
			TraceBleed( flDamage, vecDir, ptr, bitsDamageType );

	AddMultiDamage( pevAttacker, this, flDamage, bitsDamageType );
	}
}

#define ARMOR_RATIO	 	0.01	// Armor Takes 99% of the damage
#define ARMOR_BONUS	 	0.33	// Each Point of Armor is work 1/3 points of health
#define ARMOR_BONUS_SHIELD	0.25    // If we have PowerShield item, each Point of Armor is work 1/4 points of health

int CBasePlayer :: RealTakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType )
{
	if ( (pev->takedamage == DAMAGE_NO) || (IsAlive() == FALSE) )
		return 0;

	CBaseEntity *pAttacker = CBaseEntity::Instance(pevAttacker);
	CBaseEntity *pInflictor= CBaseEntity::Instance(pevInflictor);

//  can't hurt yourself, if this is set!
	if (CVAR_GET_FLOAT("mp_noselfdamage")==1 && pAttacker==this )
		return 0;

//  can't hurt teammates, if this is set!
	if ( g_pGameRules->PlayerRelationship( this, pAttacker ) == GR_TEAMMATE && pAttacker != this )
	{
		if (CVAR_GET_FLOAT( "mp_noteamdamage" )== 1)
		return 0;
	}

	int iContents = UTIL_PointContents (pev->origin);
	// have suit diagnose the problem - ie: report damage type
	int bitsDamage = bitsDamageType;
	int fTookDamage;
	float flRatio;
	float flBonus;
	float flHealthPrev = pev->health;

	if (m_fPowerShield)
	flBonus = ARMOR_BONUS_SHIELD;
	else
	flBonus = ARMOR_BONUS;

	flRatio = ARMOR_RATIO;

	if (m_fFrozen && !(bitsDamageType & DMG_REGENARMOR))
	flDamage = flDamage * 2;
	                     
//for correct time-based damage Kills
	if (bitsDamageType & DMG_FREEZE)
		pev->euser1 = ENT(pevAttacker);
	if (bitsDamageType & DMG_IGNITE)
		pev->euser2 = ENT(pevAttacker);
	if (bitsDamageType & DMG_RADIATION)
		pev->euser3 = ENT(pevAttacker);
	if (bitsDamageType & DMG_POISON)
		m_LastAttacker1 = ENT(pevAttacker);
	if (bitsDamageType & DMG_ACID)
		m_LastAttacker3 = ENT(pevAttacker);
	if (bitsDamageType & (DMG_NERVEGAS))
		pev->euser4 = ENT(pevAttacker);
	if (bitsDamageType & (DMG_CONCUSSION))
		m_fConcussLevel = 40;

//=================
//Armor protection
//=================

// Kevlar armor...
if ( !pev->armorvalue && m_fKevlar && (HITGROUP_HEAD||HITGROUP_CHEST||HITGROUP_STOMACH))
{
	if (bitsDamageType & (DMG_BLAST | DMG_SHOCK | DMG_FREEZE))
	{
		m_iKevlarBattery -= (flDamage/2);
		flDamage = flDamage*0.8;
	}
	if (bitsDamageType & (DMG_SLASH | DMG_BULLET))
	{
		m_iKevlarBattery -= (flDamage/15);
		flDamage = flDamage*0.5;
	}
	if (bitsDamageType & (DMG_BULLETBUCKSHOT))
	{
		m_iKevlarBattery -= (flDamage/12);
		flDamage = flDamage*0.45;
	}
	if (bitsDamageType & (DMG_CRUSH | DMG_BLIND | DMG_NERVEGAS | DMG_RADIATION | DMG_DROWN))
	{
		flDamage = flDamage;
	}
	if (bitsDamageType & (DMG_BULLETMAGNUM))
	{
		m_iKevlarBattery -= (flDamage/15);
		flDamage = flDamage*0.7;
	}
	if (bitsDamageType & (DMG_PARALYZE | DMG_ACID | DMG_POISON))
	{
		m_iKevlarBattery -= (flDamage/8);
		flDamage = flDamage*0.6;
	}
	if (bitsDamageType & (DMG_BURN | DMG_IGNITE))
	{
		m_iKevlarBattery -= (flDamage/3);
		flDamage = flDamage*0.6;
	}
	if (bitsDamageType & (DMG_ENERGYBLAST | DMG_SONIC | DMG_ENERGYBEAM | DMG_PLASMA | DMG_NUKE))
	{
		m_iKevlarBattery -= (flDamage/4);
		flDamage = flDamage*0.95;
	}
}
// Kevlar armor... END

// Heavy armor...
if (!pev->armorvalue && m_fHeavyArmor)
{
	if (bitsDamageType & (DMG_ENERGYBLAST | DMG_BLAST | DMG_ENERGYBEAM | DMG_PLASMA))
	{
		m_iHarmorBattery -= (flDamage/3);
		flDamage = flDamage*0.33;
	}
	if (bitsDamageType & (DMG_SLASH | DMG_BULLET))
	{
		m_iHarmorBattery -= (flDamage/20);
		flDamage = 0;
	}
	if (bitsDamageType & (DMG_BULLETBUCKSHOT))
	{
		m_iHarmorBattery -= (flDamage/12);
		flDamage = 0;
	}
	if (bitsDamageType & (DMG_RADIATION | DMG_DROWN))
	{
		flDamage = flDamage*0.25;
	}
	if (bitsDamageType & (DMG_BLIND))
	{
		flDamage = flDamage*0.5;
	}
	if (bitsDamageType & (DMG_BULLETMAGNUM | DMG_CRUSH))
	{
		m_iHarmorBattery -= (flDamage/15);
		flDamage = flDamage*0.1;
	}
	if (bitsDamageType & (DMG_PARALYZE | DMG_ACID | DMG_POISON | DMG_FREEZE))
	{
		m_iHarmorBattery -= (flDamage/12);
		flDamage = flDamage*0.12;
	}
	if (bitsDamageType & (DMG_BURN | DMG_IGNITE))
	{
		m_iHarmorBattery -=(flDamage/18);
		flDamage = flDamage*0.08;
	}
	if (bitsDamageType & (DMG_SONIC | DMG_NUKE))
	{
		m_iHarmorBattery -= flDamage;
		flDamage = flDamage*0.5;
	}
	if (bitsDamageType & (DMG_SHOCK))
	{
		m_iHarmorBattery -= (flDamage/8);
		flDamage = flDamage*0.25;
	}
	if (bitsDamageType & (DMG_NERVEGAS))
		flDamage = 0;
}
// Heavy armor... END

//=================
//Armor protection
//=================

	if (bitsDamageType & DMG_BLAST)
	{
		UTIL_ScreenShake( pev->origin, 25.0, 150.0, 2.0, 120);
		UTIL_MakeVectors ( pev->angles );
		if(!pev->armorvalue)
		if(!m_fHeavyArmor)
		pev->velocity = pev->velocity - gpGlobals->v_forward * (flDamage*8);
		else
		pev->velocity = pev->velocity - gpGlobals->v_forward * (flDamage*3);
	}

	if (bitsDamageType & DMG_BLIND)
	{
		Vector ToFlash = UTIL_VecToAngles (pInflictor->pev->origin - pev->origin);
		int alpha = (cos(UTIL_AngleDistance( ToFlash.y, pev->v_angle.y ) * 3.1415926 / 180) + 1) * 255 / 2;

		if (alpha > pev->fuser1)
		{
			UTIL_ScreenFade( this, Vector(255,255,255), flDamage, flDamage/10, (alpha>215)?255:alpha, FFADE_IN);
			pev->fuser1 = alpha;
		}
		flDamage = 0;
	}

///////////////////////////////////////
// Different death Types, based on DMG!
///////////////////////////////////////

	if (bitsDamageType & DMG_ANNIHILATION)
	{
		UTIL_ScreenFade( this, Vector(0,0,0), 6, mp_deadtime.value, 255, FFADE_IN | FFADE_MODULATE );
		pev->velocity = g_vecZero;
		pev->effects |= EF_NODRAW;
		m_fAnnihilated = TRUE;
		pev->armorvalue = 0;
		flDamage = pev->health+10;
	}

	float FullDamage = pev->armorvalue - flDamage * ((m_fPowerShield)?(2/3):(3/4));
	if (bitsDamageType & (DMG_PLASMA | DMG_NUKE | DMG_ENERGYBEAM) && FullDamage <= 0 && flDamage > pev->health)
	{
		UTIL_SetSize (pev, g_vecZero, g_vecZero);
		pev->effects |= EF_NODRAW;
		FX_PlrGib( pev->origin, GIBBED_IGNITE );
	}
	if (bitsDamageType & (DMG_ACID) && FullDamage <= 0 && flDamage > pev->health)
	{
		pev->body = 5;
	}

	if (bitsDamageType & DMG_FREEZE && FullDamage <= 0 && !m_fHeavyArmor)
	{
		IgniteEnd();
		if (!m_fFrozen)
		FrozenStart();
	}

	if (bitsDamageType & (DMG_SHOCK | DMG_ENERGYBLAST) && FullDamage <= 0 && flDamage > pev->health)
	{
		UTIL_SetSize (pev, g_vecZero, g_vecZero);
		pev->effects |= EF_NODRAW;
		FX_PlrGib( pev->origin, GIBBED_ELECTRO );
	}

	if (bitsDamageType & (DMG_IGNITE | DMG_NUKE) && FullDamage <= 0 && iContents != CONTENTS_WATER && !m_fHeavyArmor)
	{
		FrozenEnd();

		if (flDamage > pev->health)
			pev->body = 5;
		else if (flDamage > pev->health*5)
		{
			UTIL_SetSize (pev, g_vecZero, g_vecZero);
			pev->effects |= EF_NODRAW;
			FX_PlrGib( pev->origin, GIBBED_IGNITE );
		}
	}
///////////////////////////////////////
// Different death Types, based on DMG!
///////////////////////////////////////

	// Already dead
	if ( !IsAlive() )
		return 0;

	if ( !g_pGameRules->FPlayerCanTakeDamage(this, pAttacker ))
		return 0;

	// keep track of amount of damage last sustained
	m_lastDamageAmount = flDamage;

	// Armor. 
	if (pev->armorvalue && !(bitsDamageType &( DMG_DROWN | DMG_BLIND | DMG_NERVEGAS | DMG_IGNOREARMOR | DMG_REGENARMOR)))// armor doesn't protect against drown, blind and nerve gas damage!
	{
		float flNew = flDamage * flRatio;
		float flArmor;
		flArmor = (flDamage - flNew) * flBonus;

		// Does this use more armor than we have?
		if (flArmor > pev->armorvalue)
		{
			flArmor = pev->armorvalue;
			flArmor *= (1/flBonus);
			flNew = flDamage - flArmor;
			pev->armorvalue = 0;
			flDamage = flNew;

			if (m_fHeavyArmor)
			{
				m_iHarmorBattery -= (flNew/10);
				flDamage = (flNew/8);
			}
			if (m_fKevlar)
			{
				m_iKevlarBattery -= (flNew/4);
				flDamage = (flNew/2);
			}
		}
		else
		{
			pev->armorvalue -= flArmor;
			flDamage = flNew;
		}
	}

	// this cast to INT is critical!!! If a player ends up with 0.5 health, the engine will get that
	// as an int (zero) and think the player is dead! (this will incite a clientside screentilt, etc)
	fTookDamage = CBaseMonster::RealTakeDamage(pevInflictor, pevAttacker, (int)flDamage, bitsDamageType);


	if (pev->health < 15)
	{
		CBaseEntity *pAttacker = CBaseEntity::Instance(pevAttacker);

		if (pAttacker->IsPlayer())
			m_LastAttacker2 = ENT(pevAttacker); //bleed to death correct frags
		else
			m_LastAttacker2 = ENT(pevInflictor);
	}

	for (int i = 0; i < CDMG_TIMEBASED; i++)
		if (bitsDamageType & (DMG_PARALYZE << i))
			m_rgbTimeBasedDamage[i] = 0;

	// tell director about it
	MESSAGE_BEGIN( MSG_SPEC, SVC_DIRECTOR );
		WRITE_BYTE ( 9 );	// command length in bytes
		WRITE_BYTE ( DRC_CMD_EVENT );	// take damage event
		WRITE_SHORT( ENTINDEX(this->edict()) );	// index number of primary entity
		WRITE_SHORT( ENTINDEX(ENT(pevInflictor)) );	// index number of secondary entity
		WRITE_LONG( 5 );   // eventflags (priority and flags)
	MESSAGE_END();

	m_bitsDamageType |= bitsDamage; // Save this so we can report it to the client
	m_bitsHUDDamage = -1;  // make sure the damage bits get resent

	if (flDamage > 0)
	{
		if(!m_fFrozen)
		{
			if(!m_fHeavyArmor)
			{
				pev->punchangle.x -= RANDOM_FLOAT(-3,3);
				pev->punchangle.y -= RANDOM_FLOAT(-1,1);
			}
		}
	}
	return fTookDamage;
}

//=========================================================
// PackDeadPlayerItems - call this when a player dies to
// pack up the appropriate weapons and ammo items, and to
// destroy anything that shouldn't be packed.
//
// This is pretty brute force :(
//=========================================================
void CBasePlayer::PackDeadPlayerItems(void)
{
	int iWeaponRules, iAmmoRules, i;
	CBasePlayerWeapon *rgpPackWeapons[ 60 ];// 60 hardcoded for now. How to determine exactly how many weapons we have?
	int iPackAmmo[ MAX_AMMO_SLOTS + 1];
	int iPW = 0;// index into packweapons array
	int iPA = 0;// index into packammo array

	memset(rgpPackWeapons, NULL, sizeof(rgpPackWeapons) );
	memset(iPackAmmo, -1, sizeof(iPackAmmo) );

	// get the game rules 
	iWeaponRules = g_pGameRules->DeadPlayerWeapons( this );
 	iAmmoRules = g_pGameRules->DeadPlayerAmmo( this );

	if ( iWeaponRules == GR_PLR_DROP_GUN_NO && iAmmoRules == GR_PLR_DROP_AMMO_NO )
	{
		// nothing to pack. Remove the weapons and return. Don't call create on the box!
		RemoveAllItems( TRUE );
		return;
	}

// go through all of the weapons and make a list of the ones to pack
	for ( i = 0 ; i < MAX_ITEM_TYPES ; i++ )
	{
		if ( m_rgpPlayerItems[ i ] )
		{
			// there's a weapon here. Should I pack it?
			CBasePlayerItem *pPlayerItem = m_rgpPlayerItems[ i ];

			while ( pPlayerItem )
			{
				switch( iWeaponRules )
				{
				case GR_PLR_DROP_GUN_ACTIVE:
					if ( m_pActiveItem && pPlayerItem == m_pActiveItem )
					{
						// this is the active item. Pack it.
						rgpPackWeapons[ iPW++ ] = (CBasePlayerWeapon *)pPlayerItem;
					}
					break;

				case GR_PLR_DROP_GUN_ALL:
					rgpPackWeapons[ iPW++ ] = (CBasePlayerWeapon *)pPlayerItem;
					break;

				default:
					break;
				}

				pPlayerItem = pPlayerItem->m_pNext;
			}
		}
	}

// now go through ammo and make a list of which types to pack.
	if ( iAmmoRules != GR_PLR_DROP_AMMO_NO )
	{
		for ( i = 0 ; i < MAX_AMMO_SLOTS ; i++ )
		{
			if ( m_rgAmmo[ i ] > 0 )
			{
				// player has some ammo of this type.
				switch ( iAmmoRules )
				{
				case GR_PLR_DROP_AMMO_ALL:
					iPackAmmo[ iPA++ ] = i;
					break;

				case GR_PLR_DROP_AMMO_ACTIVE:
					if ( m_pActiveItem && i == m_pActiveItem->PrimaryAmmoIndex() ) 
					{
						// this is the primary ammo type for the active weapon
						iPackAmmo[ iPA++ ] = i;
					}
					else if ( m_pActiveItem && i == m_pActiveItem->SecondaryAmmoIndex() ) 
					{
						// this is the secondary ammo type for the active weapon
						iPackAmmo[ iPA++ ] = i;
					}
					break;

				default:
					break;
				}
			}
		}
	}

// create a box to pack the stuff into.
	CWeaponBox *pWeaponBox = (CWeaponBox *)CBaseEntity::Create( "weaponbox", pev->origin, pev->angles, edict() );
	pWeaponBox->pev->angles.x = 0;// don't let weaponbox tilt.
	pWeaponBox->pev->angles.z = 0;
	pWeaponBox->SetThink( CWeaponBox::Kill );

	if (m_fAnnihilated)
		pWeaponBox->pev->nextthink = gpGlobals->time;
	else
		pWeaponBox->pev->nextthink = gpGlobals->time + mp_wpnstaytime.value;

// back these two lists up to their first elements
	iPA = 0;
	iPW = 0;

// pack the ammo
	while ( iPackAmmo[ iPA ] != -1 )
	{
		pWeaponBox->PackAmmo( MAKE_STRING( CBasePlayerItem::AmmoInfoArray[ iPackAmmo[ iPA ] ].pszName ), m_rgAmmo[ iPackAmmo[ iPA ] ] );
		iPA++;
	}

// now pack all of the items in the lists
	while ( rgpPackWeapons[ iPW ] )
	{
		// weapon unhooked from the player. Pack it into der box.
		pWeaponBox->PackWeapon( rgpPackWeapons[ iPW ] );
		iPW++;
	}

	pWeaponBox->pev->velocity = pev->velocity * 1.2;// weaponbox has player's velocity, then some.
	RemoveAllItems( TRUE );// now strip off everything that wasn't handled by the code above.
}

void CBasePlayer::RemoveAllItems( BOOL removeSuit )
{
	if (m_pActiveItem)
	{
		m_pActiveItem->Holster( );
		m_pActiveItem = NULL;
	}

	m_pLastItem = NULL;

	int i;
	CBasePlayerItem *pPendingItem;
	for (i = 0; i < MAX_ITEM_TYPES; i++)
	{
		m_pActiveItem = m_rgpPlayerItems[i];
		while (m_pActiveItem)
		{
			pPendingItem = m_pActiveItem->m_pNext; 
			m_pActiveItem->Drop( );
			m_pActiveItem = pPendingItem;
		}
		m_rgpPlayerItems[i] = NULL;
	}
	m_pActiveItem = NULL;

	pev->viewmodel		= 0;
	pev->weaponmodel	= 0;

	if (m_fHeavyArmor)
	pev->maxspeed = HARMOR_MAXSPEED;
	else
	pev->maxspeed = NORMAL_MAXSPEED;
	
	if ( removeSuit )
	{
		pev->weapons = 0;
		m_iWeapons2 = 0;
	}
	else
	{
		pev->weapons &= ~WEAPON_ALLWEAPONS;
		m_iWeapons2 = 0;
	}

	for ( i = 0; i < MAX_AMMO_SLOTS;i++)
		m_rgAmmo[i] = 0;

	UpdateClientData();
	// send Selected Weapon Message to our client
	MESSAGE_BEGIN( MSG_ONE, gmsgCurWeapon, NULL, pev );
		WRITE_BYTE(0);
		WRITE_BYTE(0);
		WRITE_BYTE(0);
	MESSAGE_END();
}

/*
 * GLOBALS ASSUMED SET:  g_ulModelIndexPlayer
 *
 * ENTITY_METHOD(PlayerDie)
 */
entvars_t *g_pevLastInflictor;  // Set in combat.cpp.  Used to pass the damage inflictor for death messages.
								// Better solution:  Add as parameter to all Killed() functions.

void CBasePlayer::Killed( entvars_t *pevAttacker, int iGib )
{
	CSound *pSound;

	CBaseEntity *pKiller = CBaseEntity::Instance( pevAttacker );

	if (pKiller->IsPlayer() && pevAttacker != pev)
		AddMoney (mp_startmoney.value);

	if ( m_pActiveItem )
		m_pActiveItem->Holster( );

	m_pNextItem = NULL;

	g_pGameRules->PlayerKilled( this, pevAttacker, g_pevLastInflictor );

	// buz: spec tank
	if (m_pSpecTank)
	{
		m_pSpecTank->Use( this, this, USE_OFF, 0 );
		m_pSpecTank = NULL;
	}

	if (m_fCloakActivated)
	CloakToggle(FALSE);

	if (m_fAntigravActivated)
	AntigravToggle(FALSE);

	// this client isn't going to be thinking for a while, so reset the sound until they respawn
	pSound = CSoundEnt::SoundPointerForIndex( CSoundEnt::ClientSoundIndex( edict() ) );
	{
		if ( pSound )
		{
			pSound->Reset();
		}
	}

	SetAnimation( PLAYER_DIE );
	
	m_iRespawnFrames = 0;
	pev->modelindex = g_ulModelIndexPlayer;    // don't use eyes

	pev->deadflag		= DEAD_DYING;
	pev->movetype		= MOVETYPE_TOSS;
	ClearBits( pev->flags, FL_ONGROUND );

	// send "health" update message to zero
	m_iClientHealth = 0;
	MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pev);
	WRITE_BYTE( MSG_HEALTH );
	WRITE_BYTE( m_iClientHealth );
	WRITE_ELSE(1+1);
	MESSAGE_END();

	// Tell Ammo Hud that the player is dead
	MESSAGE_BEGIN( MSG_ONE, gmsgCurWeapon, NULL, pev );
		WRITE_BYTE(0);
		WRITE_BYTE(0XFF);
		WRITE_BYTE(0xFF);
	MESSAGE_END();

	// reset FOV
	pev->fov = m_iFOV = m_iClientFOV = 0;

	MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pev);
	WRITE_BYTE(MSG_SCOPE);
	WRITE_BYTE(0);
	WRITE_ELSE(1+1);
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pev);
	WRITE_BYTE(MSG_FOV);
	WRITE_BYTE(0);
	WRITE_ELSE(1+1);
	MESSAGE_END();

	if (!m_fAnnihilated)
		UTIL_ScreenFade( this, Vector(128,0,0), 6, mp_deadtime.value, 255, FFADE_IN | FFADE_MODULATE );

	if ((m_fFrozen || pev->health < -70 && iGib != GIB_NEVER ) || iGib == GIB_ALWAYS)
	{
		UTIL_SetSize (pev, g_vecZero, g_vecZero);
		pev->effects |= EF_NODRAW;
		pev->velocity = g_vecAttackDir * -1;
		FX_PlrGib( pev->origin, (m_fFrozen)?GIBBED_FROZEN:GIBBED_BODY);
	}
	DeathSound();

	if (m_fHeavyArmor)
	g_engfuncs.pfnSetClientKeyValue(entindex(), g_engfuncs.pfnGetInfoKeyBuffer( edict() ), "model", oldmodel);

	pev->angles.x = 0;
	pev->angles.z = 0;
	pev->velocity = g_vecZero;
	pev->avelocity = g_vecZero;

	SetThink(PlayerDeathThink);

	if (FlashlightIsOn())
	FlashlightTurnOff();

	pev->nextthink = gpGlobals->time + 0.1;
}

// Set the activity based on an event or current state
void CBasePlayer::SetAnimation( PLAYER_ANIM playerAnim )
{
	int animDesired;
	float speed;
	char szAnim[64];

	speed = pev->velocity.Length2D();

	if (pev->flags & FL_FROZEN)
	{
		speed = 0;
		playerAnim = PLAYER_IDLE;
	}

	switch (playerAnim) 
	{
	case PLAYER_JUMP:
		m_IdealActivity = ACT_HOP;
		break;
	
	case PLAYER_SUPERJUMP:
		m_IdealActivity = ACT_LEAP;
		break;
	
	case PLAYER_DIE:
		m_IdealActivity = GetDeathActivity( );
		break;

	case PLAYER_ATTACK1:	
		switch( m_Activity )
		{
		case ACT_HOVER:
		case ACT_SWIM:
		case ACT_HOP:
		case ACT_LEAP:
		case ACT_DIESIMPLE:
			m_IdealActivity = m_Activity;
			break;
		default:
			m_IdealActivity = ACT_RANGE_ATTACK1;
			break;
		}
		break;
	case PLAYER_IDLE:
	case PLAYER_WALK:
		if ( !FBitSet( pev->flags, FL_ONGROUND ) && (m_Activity == ACT_HOP || m_Activity == ACT_LEAP) )	// Still jumping
		{
			m_IdealActivity = m_Activity;
		}
		else if ( pev->waterlevel > 1 )
		{
			if ( speed == 0 )
				m_IdealActivity = ACT_HOVER;
			else
				m_IdealActivity = ACT_SWIM;
		}
		else
		{
			m_IdealActivity = ACT_WALK;
		}
		break;
	}

	switch (m_IdealActivity)
	{
	case ACT_HOVER:
	case ACT_LEAP:
	case ACT_SWIM:
	case ACT_HOP:
	case ACT_DIESIMPLE:
	default:
		if ( m_Activity == m_IdealActivity)
			return;
		m_Activity = m_IdealActivity;

		animDesired = LookupActivity( m_Activity );
		// Already using the desired animation?
		if (pev->sequence == animDesired)
			return;

		pev->gaitsequence = 0;
		pev->sequence		= animDesired;
		pev->frame			= 0;
		ResetSequenceInfo( );
		return;

	case ACT_RANGE_ATTACK1:
		if ( FBitSet( pev->flags, FL_DUCKING ) )	// crouching
			strcpy( szAnim, "crouch_shoot_" );
		else
			strcpy( szAnim, "ref_shoot_" );
		strcat( szAnim, m_szAnimExtention );
		animDesired = LookupSequence( szAnim );
		if (animDesired == -1)
			animDesired = 0;

		if ( pev->sequence != animDesired || !m_fSequenceLoops )
		{
			pev->frame = 0;
		}

		if (!m_fSequenceLoops)
		{
			pev->effects |= EF_NOINTERP;
		}

		m_Activity = m_IdealActivity;

		pev->sequence		= animDesired;
		ResetSequenceInfo( );
		break;

	case ACT_WALK:
		if (m_Activity != ACT_RANGE_ATTACK1 || m_fSequenceFinished)
		{
			if ( FBitSet( pev->flags, FL_DUCKING ) )	// crouching
				strcpy( szAnim, "crouch_aim_" );
			else
				strcpy( szAnim, "ref_aim_" );
			strcat( szAnim, m_szAnimExtention );
			animDesired = LookupSequence( szAnim );
			if (animDesired == -1)
				animDesired = 0;
			m_Activity = ACT_WALK;
		}
		else
		{
			animDesired = pev->sequence;
		}
	}

	if ( FBitSet( pev->flags, FL_DUCKING ) )
	{
		if ( speed == 0)
			pev->gaitsequence	= LookupActivity( ACT_CROUCHIDLE );
		else
			pev->gaitsequence	= LookupActivity( ACT_CROUCH );
	}
	else if ( speed > 220 )
	{
		pev->gaitsequence	= LookupActivity( ACT_RUN );
	}
	else if (speed > 0)
	{
		pev->gaitsequence	= LookupActivity( ACT_WALK );
	}
	else
	{
		// pev->gaitsequence	= LookupActivity( ACT_WALK );
		pev->gaitsequence	= LookupSequence( "deep_idle" );
	}


	// Already using the desired animation?
	if (pev->sequence == animDesired)
		return;

	//ALERT( at_console, "Set animation to %d\n", animDesired );
	// Reset to first frame of desired animation
	pev->sequence		= animDesired;
	pev->frame			= 0;
	ResetSequenceInfo( );
}

/*
===========
TabulateAmmo
This function is used to find and store 
all the ammo we have into the ammo vars.
============
*/
void CBasePlayer::TabulateAmmo()
{
}


/*
===========
WaterMove
============
*/
#define AIRTIME	12		// lung full of air lasts this many seconds

void CBasePlayer::WaterMove()
{
	int air;

	if (pev->movetype == MOVETYPE_NOCLIP)
		return;

	if (pev->health < 0)
		return;

	// waterlevel 0 - not in water
	// waterlevel 1 - feet in water
	// waterlevel 2 - waist in water
	// waterlevel 3 - head in water

	if (pev->waterlevel != 3) 
	{
		// not underwater play 'up for air' sound

		if (pev->air_finished < gpGlobals->time)
		{
			EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/pl_wade1.wav", 1, ATTN_NORM);
		}
		else if (pev->air_finished < gpGlobals->time + 9)
		{
			EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/pl_wade2.wav", 1, ATTN_NORM);
		}
		pev->air_finished = gpGlobals->time + AIRTIME;
		pev->dmg = 2;

		// if we took drowning damage, give it back slowly
		if (m_idrowndmg > m_idrownrestored)
		{
			m_bitsDamageType |= DMG_DROWNRECOVER;
			m_bitsDamageType &= ~DMG_DROWN;
			m_rgbTimeBasedDamage[itbd_DrownRecover] = 0;
		}

	}
	else
	{
		m_bitsDamageType &= ~DMG_DROWNRECOVER;
		m_rgbTimeBasedDamage[itbd_DrownRecover] = 0;

		if (pev->air_finished < gpGlobals->time)		// drown!
		{
			if (pev->pain_finished < gpGlobals->time)
			{
				// take drowning damage
				pev->dmg += 1;
				if (pev->dmg > 5)
					pev->dmg = 5;
				TakeDamage(VARS(eoNullEntity), VARS(eoNullEntity), pev->dmg, DMG_DROWN);

				switch (RANDOM_LONG(0,2))
				{
					case 0:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/pl_drown.wav", 1, ATTN_NORM); break;
					case 1:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/pl_drown3.wav", 1, ATTN_NORM); break;
					case 2:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/pl_drown2.wav", 1, ATTN_NORM); break;
				}

				pev->pain_finished = gpGlobals->time + 1.5;
				m_idrowndmg += pev->dmg;
			} 
		}
		else
		{
			m_bitsDamageType &= ~DMG_DROWN;
		}
	}

	if (!pev->waterlevel)
	{
		if (FBitSet(pev->flags, FL_INWATER))
		{       
			ClearBits(pev->flags, FL_INWATER);
		}
		return;
	}
	
	// make bubbles

	air = (int)(pev->air_finished - gpGlobals->time);
	if (!RANDOM_LONG(0,0x1f) && RANDOM_LONG(0,AIRTIME-1) >= air)
	{
		switch (RANDOM_LONG(0,3))
			{
			case 0:	EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_swim1.wav", 0.8, ATTN_NORM); break;
			case 1:	EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_swim2.wav", 0.8, ATTN_NORM); break;
			case 2:	EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_swim3.wav", 0.8, ATTN_NORM); break;
			case 3:	EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_swim4.wav", 0.8, ATTN_NORM); break;
		}
	}

	if (pev->watertype == CONTENT_LAVA)		// do damage
	{
		if (pev->dmgtime < gpGlobals->time)
			TakeDamage(VARS(eoNullEntity), VARS(eoNullEntity), 20 * pev->waterlevel, DMG_IGNITE);
	}
	else if (pev->watertype == CONTENT_SLIME)		// do damage
	{
		pev->dmgtime = gpGlobals->time + 1;
		TakeDamage(VARS(eoNullEntity), VARS(eoNullEntity), 10 * pev->waterlevel, DMG_ACID);
	}
	
	if (!FBitSet(pev->flags, FL_INWATER))
	{
		SetBits(pev->flags, FL_INWATER);
		pev->dmgtime = 0;
	}
}


// TRUE if the player is attached to a ladder
BOOL CBasePlayer::IsOnLadder( void )
{ 
	return ( pev->movetype == MOVETYPE_FLY );
}

void CBasePlayer::PlayerDeathThink(void)
{
	float flForward;

	if (FBitSet(pev->flags, FL_ONGROUND))
	{
		flForward = pev->velocity.Length() - 20;
		if (flForward <= 0)
			pev->velocity = g_vecZero;
		else    
			pev->velocity = flForward * pev->velocity.Normalize();
	}

	if (pev->flags & FL_ONGROUND && !m_fDropped && !m_fAnnihilated)
	{
		int pitch = 90 + RANDOM_LONG(0,30); 
		EMIT_SOUND_DYN(ENT(pev), CHAN_STATIC, "common/bodydrop3.wav", 1, ATTN_NORM, 0, pitch); 
		m_fDropped = TRUE;
	}

	if ( HasWeapons() )
		PackDeadPlayerItems();

	if (pev->modelindex && (!m_fSequenceFinished) && (pev->deadflag == DEAD_DYING))
	{
		StudioFrameAdvance( );

		m_iRespawnFrames++;				// Note, these aren't necessarily real "frames", so behavior is dependent on # of client movement commands
		if ( m_iRespawnFrames < 240 )   // Animations should be no longer than this
			return;
	}

	// once we're done animating our death and we're on the ground, we want to set movetype to None so our dead body won't do collisions and stuff anymore
	// this prevents a bug where the dead body would go to a player's head if he walked over it while the dead player was clicking their button to respawn
	if ( pev->movetype != MOVETYPE_NONE && FBitSet(pev->flags, FL_ONGROUND) )
		pev->movetype = MOVETYPE_NONE;

	if (pev->deadflag == DEAD_DYING)
		pev->deadflag = DEAD_DEAD;
	
	pev->effects |= EF_NOINTERP;
	pev->framerate = 0.0;

	BOOL fAnyButtonDown = (pev->button & ~IN_SCORE );
	
	// wait for all buttons released
	if (pev->deadflag == DEAD_DEAD)
	{
		if (fAnyButtonDown)
			return;

		if ( g_pGameRules->FPlayerCanRespawn( this ) )
		{
			m_fDeadTime = gpGlobals->time;
			pev->deadflag = DEAD_RESPAWNABLE;
		}

		if ( !(m_afPhysicsFlags & PFLAG_OBSERVER) && !IsBot())
			StartDeathCam();

		return;
	}
	
	if (!fAnyButtonDown && !( g_pGameRules->IsMultiplayer() && (gpGlobals->time > (m_fDeadTime + 3))) )
		return;

	pev->button = 0;
	m_iRespawnFrames = 0;
	StopAnimation();

	if (gpGlobals->time < (m_fDeadTime + mp_deadtime.value) || (m_afPhysicsFlags & PFLAG_OBSERVER))
	return;

	respawn(pev, !(m_afPhysicsFlags & PFLAG_OBSERVER) );// don't copy a corpse if we're in deathcam.
	pev->nextthink = -1;
}

//=========================================================
// StartDeathCam - find an intermission spot and send the
// player off into observer mode
//=========================================================
void CBasePlayer::StartDeathCam( void )
{
	if ( pev->view_ofs == g_vecZero && !IsBot() )
	{
		// don't accept subsequent attempts to StartDeathCam()
		return;
	}

	TraceResult tr;
	CopyToBodyQue( pev, m_fIgnite?1:0 );
	UTIL_TraceLine( pev->origin, pev->origin + Vector( 0, 0, 80 ), ignore_monsters, edict(), &tr );

	if (!g_pGameRules->IsCoOp())//No observer mode in single player game
		StartObserver( tr.vecEndPos, pev->v_angle );
}
// 
// PlayerUse - handles USE keypress
//
#define	PLAYER_SEARCH_RADIUS	(float)64

void CBasePlayer::PlayerUse ( void )
{
	// Was use pressed or released?
	if ( ! ((pev->button | m_afButtonPressed | m_afButtonReleased) & IN_USE) )
		return;

	// Hit Use on a train?
	if ( m_afButtonPressed & IN_USE )
	{
		// buz: spec tank
		if (m_pSpecTank)
		{
			m_pSpecTank->Use( this, this, USE_OFF, 0 );
			m_pSpecTank = NULL;
			return;
		}
		else
		{
			if ( m_afPhysicsFlags & PFLAG_ONTRAIN )
			{
				m_afPhysicsFlags &= ~PFLAG_ONTRAIN;
				m_iTrain = TRAIN_NEW|TRAIN_OFF;
				return;
			}
			else
			{	// Start controlling the train!
				CBaseEntity *pTrain = CBaseEntity::Instance( pev->groundentity );

				if ( pTrain && !(pev->button & IN_JUMP) && FBitSet(pev->flags, FL_ONGROUND) && (pTrain->ObjectCaps() & FCAP_DIRECTIONAL_USE) && pTrain->OnControls(pev) )
				{
					m_afPhysicsFlags |= PFLAG_ONTRAIN;
					m_iTrain = TrainSpeed(pTrain->pev->speed, pTrain->pev->impulse);
					m_iTrain |= TRAIN_NEW;
					EMIT_SOUND( ENT(pev), CHAN_ITEM, "plats/train_use1.wav", 0.8, ATTN_NORM);
					return;
				}
			}
		}
	}

	CBaseEntity *pObject = NULL;
	CBaseEntity *pClosest = NULL;
	Vector		vecLOS;
	float flMaxDot = VIEW_FIELD_NARROW;
	float flDot;

	UTIL_MakeVectors ( pev->v_angle );// so we know which way we are facing
	
	while ((pObject = UTIL_FindEntityInSphere( pObject, pev->origin, PLAYER_SEARCH_RADIUS )) != NULL)
	{

		if (pObject->ObjectCaps() & (FCAP_IMPULSE_USE | FCAP_CONTINUOUS_USE | FCAP_ONOFF_USE))
		{
			// !!!PERFORMANCE- should this check be done on a per case basis AFTER we've determined that
			// this object is actually usable? This dot is being done for every object within PLAYER_SEARCH_RADIUS
			// when player hits the use key. How many objects can be in that area, anyway? (sjb)
			vecLOS = (VecBModelOrigin( pObject->pev ) - (pev->origin + pev->view_ofs));
			
			// This essentially moves the origin of the target to the corner nearest the player to test to see 
			// if it's "hull" is in the view cone
			vecLOS = UTIL_ClampVectorToBox( vecLOS, pObject->pev->size * 0.5 );
			
			flDot = DotProduct (vecLOS , gpGlobals->v_forward);
			if (flDot > flMaxDot )
			{// only if the item is in front of the user
				pClosest = pObject;
				flMaxDot = flDot;
//				ALERT( at_console, "%s : %f\n", STRING( pObject->pev->classname ), flDot );
			}
//			ALERT( at_console, "%s : %f\n", STRING( pObject->pev->classname ), flDot );
		}
	}
	pObject = pClosest;

	// Found an object
	if (pObject )
	{
		//!!!UNDONE: traceline here to prevent USEing buttons through walls			
		int caps = pObject->ObjectCaps();

		if ( m_afButtonPressed & IN_USE )
			EMIT_SOUND( ENT(pev), CHAN_ITEM, "common/wpn_select.wav", 0.4, ATTN_NORM);

		if ( ( (pev->button & IN_USE) && (caps & FCAP_CONTINUOUS_USE) ) ||
			 ( (m_afButtonPressed & IN_USE) && (caps & (FCAP_IMPULSE_USE|FCAP_ONOFF_USE)) ) )
		{
			if ( caps & FCAP_CONTINUOUS_USE )
				m_afPhysicsFlags |= PFLAG_USING;

			pObject->Use( this, this, USE_SET, 1 );
		}
		// UNDONE: Send different USE codes for ON/OFF.  Cache last ONOFF_USE object to send 'off' if you turn away
		else if ( (m_afButtonReleased & IN_USE) && (pObject->ObjectCaps() & FCAP_ONOFF_USE) )	// BUGBUG This is an "off" use
		{
			pObject->Use( this, this, USE_SET, 0 );
		}
	}
	else
	{
		if ( m_afButtonPressed & IN_USE )
			EMIT_SOUND( ENT(pev), CHAN_ITEM, "common/wpn_denyselect.wav", 0.4, ATTN_NORM);
	}
}



void CBasePlayer::Jump()
{
	Vector		vecWallCheckDir;// direction we're tracing a line to find a wall when walljumping
	Vector		vecAdjustedVelocity;
	Vector		vecSpot;
	TraceResult	tr;
	
	if (pev->flags & FL_FROZEN)
		return;

	if (FBitSet(pev->flags, FL_WATERJUMP))
		return;
	
	if (pev->waterlevel >= 2)
		return;

	// jump velocity is sqrt( height * gravity * 2)

	// If this isn't the first frame pressing the jump button, break out.
	if ( !FBitSet( m_afButtonPressed, IN_JUMP ) )
		return;         // don't pogo stick

	if ( !(pev->flags & FL_ONGROUND) || !pev->groundentity )
		return;

// many features in this function use v_forward, so makevectors now.
	UTIL_MakeVectors (pev->angles);

	SetAnimation( PLAYER_JUMP );
	EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/pl_jump.wav", 0.5, ATTN_NORM);

	if ( m_fLongJump && pev->velocity.Length() > 50 )
	{
	EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/pl_jump_super.wav", 0.5, ATTN_NORM);
	SetAnimation( PLAYER_SUPERJUMP );
	}

	// If you're standing on a conveyor, add it's velocity to yours (for momentum)
	entvars_t *pevGround = VARS(pev->groundentity);
	if ( pevGround && (pevGround->flags & FL_CONVEYOR) )
	{
		pev->velocity = pev->velocity + pev->basevelocity;
	}
}



// This is a glorious hack to find free space when you've crouched into some solid space
// Our crouching collisions do not work correctly for some reason and this is easier
// than fixing the problem :(
void FixPlayerCrouchStuck( edict_t *pPlayer )
{
	TraceResult trace;

	// Move up as many as 18 pixels if the player is stuck.
	for ( int i = 0; i < 18; i++ )
	{
		UTIL_TraceHull( pPlayer->v.origin, pPlayer->v.origin, dont_ignore_monsters, head_hull, pPlayer, &trace );
		if ( trace.fStartSolid )
			pPlayer->v.origin.z ++;
		else
			break;
	}
}

void CBasePlayer::Duck( )
{
	if (pev->button & IN_DUCK) 
	{
		if ( m_IdealActivity != ACT_LEAP )
		{
			SetAnimation( PLAYER_WALK );
		}
	}
}

//
// ID's player as such.
//
int  CBasePlayer::Classify ( void )
{
	return CLASS_PLAYER;
}


void CBasePlayer::AddPoints( int score, BOOL bAllowNegativeScore )
{
	// Positive score always adds
	if ( score < 0 )
	{
		if ( !bAllowNegativeScore )
		{
			if ( pev->frags < 0 )		// Can't go more negative
				return;
			
			if ( -score > pev->frags )	// Will this go negative?
			{
				score = -pev->frags;		// Sum will be 0
			}
		}
	}

	pev->frags += score;

	MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
		WRITE_BYTE( ENTINDEX(edict()) );
		WRITE_SHORT( pev->frags );
		WRITE_SHORT( m_iDeaths );
		WRITE_SHORT( 0 );
		WRITE_SHORT( g_pGameRules->GetTeamIndex( m_szTeamName ) + 1 );
	MESSAGE_END();
}


void CBasePlayer::AddPointsToTeam( int score, BOOL bAllowNegativeScore )
{
	int index = entindex();

	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *pPlayer = UTIL_PlayerByIndex( i );

		if ( pPlayer && i != index )
		{
			if ( g_pGameRules->PlayerRelationship( this, pPlayer ) == GR_TEAMMATE )
			{
				pPlayer->AddPoints( score, bAllowNegativeScore );
			}
		}
	}
}

//Player ID
void CBasePlayer::InitStatusBar()
{
	m_flStatusBarDisappearDelay = 0;
	m_SbarString1[0] = m_SbarString0[0] = 0; 
}

void CBasePlayer::UpdateStatusBar()
{
	int newSBarState[ SBAR_END ];
	char sbuf0[ SBAR_STRING_SIZE ];
	char sbuf1[ SBAR_STRING_SIZE ];

	memset( newSBarState, 0, sizeof(newSBarState) );
	strcpy( sbuf0, m_SbarString0 );
	strcpy( sbuf1, m_SbarString1 );

	// Find an ID Target
	TraceResult tr;
	UTIL_MakeVectors( pev->v_angle + pev->punchangle );
	Vector vecSrc = EyePosition();
	Vector vecEnd = vecSrc + (gpGlobals->v_forward * MAX_ID_RANGE);
	UTIL_TraceLine( vecSrc, vecEnd, dont_ignore_monsters, edict(), &tr);

	if (tr.flFraction != 1.0)
	{
		if ( !FNullEnt( tr.pHit ) )
		{
			CBaseEntity *pEntity = CBaseEntity::Instance( tr.pHit );

			if (pEntity->pev->takedamage == DAMAGE_AIM && pEntity->Classify() != CLASS_MACHINE)
			{
				newSBarState[ SBAR_ID_TARGETNAME ] = ENTINDEX( pEntity->edict() );
					strcpy( sbuf1, "1 %p1\n2 Health: %i2%%\n3 Shield: %i3%%" );

				newSBarState[ SBAR_ID_TARGETHEALTH ] = pEntity->pev->health;
				newSBarState[ SBAR_ID_TARGETARMOR ] = pEntity->pev->armorvalue;

				m_flStatusBarDisappearDelay = gpGlobals->time + 0.5;
			}
			else if (pEntity->Classify() == CLASS_MACHINE && m_fTorch)
			{
				strcpy( sbuf1, "1 %p1\n2 Armor: %i2%%\n3 Shield: %i3%%" );
				newSBarState[ SBAR_ID_TARGETHEALTH ] = pEntity->pev->health;
				newSBarState[ SBAR_ID_TARGETARMOR ] = pEntity->pev->armorvalue;
     				m_flStatusBarDisappearDelay = gpGlobals->time + 0.5;
			}
		}
		else if ( m_flStatusBarDisappearDelay > gpGlobals->time )
		{
			// hold the values for a short amount of time after viewing the object
			newSBarState[ SBAR_ID_TARGETNAME ] = m_izSBarState[ SBAR_ID_TARGETNAME ];
			newSBarState[ SBAR_ID_TARGETHEALTH ] = m_izSBarState[ SBAR_ID_TARGETHEALTH ];
			newSBarState[ SBAR_ID_TARGETARMOR ] = m_izSBarState[ SBAR_ID_TARGETARMOR ];
		}
	}

	BOOL bForceResend = FALSE;

	if ( strcmp( sbuf0, m_SbarString0 ) )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgStatusText, NULL, pev );
			WRITE_BYTE( 0 );
			WRITE_STRING( sbuf0 );
		MESSAGE_END();

		strcpy( m_SbarString0, sbuf0 );

		// make sure everything's resent
		bForceResend = TRUE;
	}

	if ( strcmp( sbuf1, m_SbarString1 ) )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgStatusText, NULL, pev );
			WRITE_BYTE( 1 );
			WRITE_STRING( sbuf1 );
		MESSAGE_END();

		strcpy( m_SbarString1, sbuf1 );

		// make sure everything's resent
		bForceResend = TRUE;
	}

	// Check values and send if they don't match
	for (int i = 1; i < SBAR_END; i++)
	{
		if ( newSBarState[i] != m_izSBarState[i] || bForceResend )
		{
			MESSAGE_BEGIN( MSG_ONE, gmsgStatusValue, NULL, pev );
				WRITE_BYTE( i );
				WRITE_SHORT( newSBarState[i] );
			MESSAGE_END();

			m_izSBarState[i] = newSBarState[i];
		}
	}
}

#define CLIMB_SHAKE_FREQUENCY	22	// how many frames in between screen shakes when climbing
#define	MAX_CLIMB_SPEED			200	// fastest vertical climbing speed possible
#define	CLIMB_SPEED_DEC			15	// climbing deceleration rate
#define	CLIMB_PUNCH_X			-7  // how far to 'punch' client X axis when climbing
#define CLIMB_PUNCH_Z			7	// how far to 'punch' client Z axis when climbing

void CBasePlayer::PreThink(void)
{
	int buttonsChanged = (m_afButtonLast ^ pev->button);	// These buttons have changed this frame
	
	// Debounced button codes for pressed/released
	// UNDONE: Do we need auto-repeat?
	m_afButtonPressed =  buttonsChanged & pev->button;		// The changed ones still down are "pressed"
	m_afButtonReleased = buttonsChanged & (~pev->button);	// The ones not down are "released"

	g_pGameRules->PlayerThink( this );

	if ( g_fGameOver )
		return;         // intermission or finale

	UTIL_MakeVectors(pev->v_angle);             // is this still used?


	//LLAPb: Keeping correct functions order is very important
	CheckTimeBasedDamage();

	UpdateClientData();

	if (m_fCloak)
		CloakUpdate();
	if (m_fAntigrav)
		AntigravUpdate();

	if (pev->deadflag >= DEAD_DYING)
	{
		Observer_HandleButtons();
		PlayerDeathThink();
		return;
	}
	
	ItemPreFrame( );
	WaterMove();

	// So the correct flags get sent to client asap.
	//
	if ( m_afPhysicsFlags & PFLAG_ONTRAIN )
		pev->flags |= FL_ONTRAIN;
	else 
		pev->flags &= ~FL_ONTRAIN;

	// Train speed control
	if ( m_afPhysicsFlags & PFLAG_ONTRAIN )
	{
		CBaseEntity *pTrain = CBaseEntity::Instance( pev->groundentity );
		float vel;
		
		if ( !pTrain )
		{
			TraceResult trainTrace;
			// Maybe this is on the other side of a level transition
			UTIL_TraceLine( pev->origin, pev->origin + Vector(0,0,-38), ignore_monsters, ENT(pev), &trainTrace );

			// HACKHACK - Just look for the func_tracktrain classname
			if ( trainTrace.flFraction != 1.0 && trainTrace.pHit )
			pTrain = CBaseEntity::Instance( trainTrace.pHit );


			if ( !pTrain || !(pTrain->ObjectCaps() & FCAP_DIRECTIONAL_USE) || !pTrain->OnControls(pev) )
			{
				//ALERT( at_error, "In train mode with no train!\n" );
				m_afPhysicsFlags &= ~PFLAG_ONTRAIN;
				m_iTrain = TRAIN_NEW|TRAIN_OFF;
				return;
			}
		}
		else if ( !FBitSet( pev->flags, FL_ONGROUND ) || FBitSet( pTrain->pev->spawnflags, SF_TRACKTRAIN_NOCONTROL ) || (pev->button & (IN_MOVELEFT|IN_MOVERIGHT) ) )
		{
			// Turn off the train if you jump, strafe, or the train controls go dead
			m_afPhysicsFlags &= ~PFLAG_ONTRAIN;
			m_iTrain = TRAIN_NEW|TRAIN_OFF;
			return;
		}

		pev->velocity = g_vecZero;
		vel = 0;
		if ( m_afButtonPressed & IN_FORWARD )
		{
			vel = 1;
			pTrain->Use( this, this, USE_SET, (float)vel );
		}
		else if ( m_afButtonPressed & IN_BACK )
		{
			vel = -1;
			pTrain->Use( this, this, USE_SET, (float)vel );
		}

		if (vel)
		{
			m_iTrain = TrainSpeed(pTrain->pev->speed, pTrain->pev->impulse);
			m_iTrain |= TRAIN_ACTIVE|TRAIN_NEW;
		}

	} else if (m_iTrain & TRAIN_ACTIVE)
		m_iTrain = TRAIN_NEW; // turn off train

	if (pev->button & IN_JUMP)
	{
		// If on a ladder, jump off the ladder
		// else Jump
		Jump();
	}


	// If trying to duck, already ducked, or in the process of ducking
	if ((pev->button & IN_DUCK) || FBitSet(pev->flags,FL_DUCKING) || (m_afPhysicsFlags & PFLAG_DUCKING) )
		Duck();

	if ( !FBitSet ( pev->flags, FL_ONGROUND ) )
	{
		m_flFallVelocity = -pev->velocity.z;
	}

	// Clear out ladder pointer
	m_hEnemy = NULL;

	if ( m_afPhysicsFlags & PFLAG_ONBARNACLE )
	{
		pev->velocity = g_vecZero;
	}
}

void CBasePlayer::CheckTimeBasedDamage() 
{
	int iContents = UTIL_PointContents ( pev->origin );

	if (pev->fuser1 > 0)
		pev->fuser1 -= 0.2;

	int i;
	BYTE bDuration = 0;
	static float gtbdPrev = 0.0;

	// only check for time based damage approx. every 2 seconds
	if (abs(gpGlobals->time - m_tbdPrev) < 0.5)
		return;

	m_tbdPrev = gpGlobals->time;//LLAPb: moved here, to keep buyzone updating

//=================BUYZONE CHECK=================

	m_iInBuyZone = g_pGameRules->IsAllowedToBuy(this);

//===============BUYZONE CHECK END===============

	if (!(m_bitsDamageType & DMG_TIMEBASED))
		return;

	for (i = 0; i < CDMG_TIMEBASED; i++)
	{
		if (m_bitsDamageType & (DMG_PARALYZE << i) )
		{
			switch (i)
			{
		case itbd_Paralyze:
				if (!pev->armorvalue && !m_fHeavyArmor && !m_fPortableHealthkit)
				{
					pev->maxspeed = 100;
					lastDamageType |= DMG_PARALYZE;
					TakeDamage(pev, pev, 0, DMG_IGNOREARMOR);
					lastDamageType &= ~DMG_PARALYZE;
					bDuration = PARALYZE_DURATION;
				}
				else
					m_bitsDamageType &= ~(DMG_PARALYZE << i);	
				break;
		case itbd_NerveGas:
				if (!m_fHeavyArmor && !m_fPortableHealthkit)
				{
					if (gpGlobals->time >= m_flScreamTime && IsAlive())
					{
						switch (RANDOM_LONG(0,1))
						{
							case 0:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/nervegas1.wav", 1, ATTN_NORM); break;
							case 1:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/nervegas2.wav", 1, ATTN_NORM); break;
						}
						m_flScreamTime = gpGlobals->time + 2;
					}
					lastDamageType |= DMG_NERVEGAS;
					TakeDamage(VARS(pev->euser4), VARS(pev->euser4), NERVEGAS_DAMAGE, DMG_IGNOREARMOR);
					lastDamageType &= ~DMG_NERVEGAS;
					bDuration = NERVEGAS_DURATION;
				}
				else
					m_bitsDamageType &= ~(DMG_PARALYZE << i);	
				break;
		case itbd_Poison:
				if (!pev->armorvalue && !m_fHeavyArmor && !m_fPortableHealthkit)
				{
					lastDamageType |= DMG_POISON;
					TakeDamage(VARS(m_LastAttacker1), VARS(m_LastAttacker1), POISON_DAMAGE, DMG_IGNOREARMOR);
					lastDamageType &= ~DMG_POISON;
					bDuration = POISON_DURATION;
				}
				else
					m_bitsDamageType &= ~(DMG_PARALYZE << i);	
				break;
		case itbd_Radiation:
				if (!pev->armorvalue && !m_fHeavyArmor && !m_fPortableHealthkit)
				{
					lastDamageType |= DMG_RADIATION;
					TakeDamage(VARS(pev->euser3), VARS(pev->euser3), RADIATION_DAMAGE, DMG_IGNOREARMOR);
					lastDamageType &= ~DMG_RADIATION;
					bDuration = RADIATION_DURATION;
				}
				else
					m_bitsDamageType &= ~(DMG_PARALYZE << i);	
				break;
		case itbd_DrownRecover:
				if (m_idrowndmg > m_idrownrestored)
				{
					int idif = min(m_idrowndmg - m_idrownrestored, 10);
					TakeHealth(idif, DMG_GENERIC);
					m_idrownrestored += idif;
				}
				bDuration = 4;	// get up to 5*10 = 50 points back
				break;
		case itbd_Acid:
				if (!pev->armorvalue && !m_fHeavyArmor && !m_fPortableHealthkit)
				{
					TakeDamage(VARS(m_LastAttacker3), VARS(m_LastAttacker3), ACID_DAMAGE, DMG_IGNOREARMOR);
					bDuration = ACID_DURATION;
					lastDamageType &= ~DMG_ACID;
				}
				else
					m_bitsDamageType &= ~(DMG_PARALYZE << i);	
				break;
		case itbd_Burn:
				if (!pev->armorvalue && iContents != CONTENTS_WATER && !m_fKevlar && !m_fHeavyArmor)
				{
					lastDamageType |= DMG_IGNITE;
					TakeDamage(VARS(pev->euser2), VARS(pev->euser2), BURN_DAMAGE, DMG_IGNOREARMOR);
					lastDamageType &= ~DMG_IGNITE;
					bDuration = BURN_DURATION;
					IgniteStart();

					CBaseEntity *pOther = NULL;
					while ((pOther = UTIL_FindEntityInSphere( pOther, pev->origin, 70 )) != NULL)
					{
						if (pOther->pev->takedamage == DAMAGE_AIM && pOther->pev->deadflag != DEAD_DEAD && pOther->pev->health >=1 )
						{
							TraceResult tr;
							UTIL_TraceLine( pev->origin, pOther->pev->origin, dont_ignore_monsters, ENT(pev), &tr );
							CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
							if (pEntity != NULL && pEntity->pev->takedamage)
							{
								ClearMultiDamage( );
								pEntity->TraceAttack( pev, BURN_DAMAGE, pev->velocity, &tr, DMG_BURN);
								ApplyMultiDamage( pev, pev );
							}
						}
					}
				}
				else
				{
					m_bitsDamageType &= ~(DMG_PARALYZE << i);	
					IgniteEnd();
				}
				break;
			case itbd_Freeze:
				if (!pev->armorvalue && !m_fHeavyArmor)
				{
					lastDamageType |= DMG_FREEZE;
					TakeDamage(VARS(pev->euser1), VARS(pev->euser1), FREEZE_DAMAGE, DMG_IGNOREARMOR | DMG_ALWAYSGIB );
					lastDamageType &= ~DMG_FREEZE;
					bDuration = FREEZE_DURATION;

					if (gpGlobals->time >= m_fFrFadeTime)
					{
						UTIL_ScreenFade( this, Vector(0,90,250), FREEZE_DURATION*2, FREEZE_DURATION, 128, FFADE_IN );
						m_fFrFadeTime = gpGlobals->time + FREEZE_DURATION+0.1;
					}
				}
				else if (m_fHeavyArmor)
				{
					bDuration = FREEZE_DURATION;

					if (gpGlobals->time >= m_fFrFadeTime)
					{
						pev->maxspeed = HARMOR_MAXSPEED/2;
						UTIL_ScreenFade( this, Vector(0,90,250), FREEZE_DURATION*2, FREEZE_DURATION, 64, FFADE_IN );
						m_fFrFadeTime = gpGlobals->time + FREEZE_DURATION+0.1;
					}
				}
				else
				{
					m_bitsDamageType &= ~(DMG_PARALYZE << i);	
					FrozenEnd();
				}
				break;
			default:
				bDuration = 0;
			}

			if (m_rgbTimeBasedDamage[i])
			{
				// decrement damage duration, detect when done.
				if (!m_rgbTimeBasedDamage[i] || --m_rgbTimeBasedDamage[i] == 0)
				{
					m_rgbTimeBasedDamage[i] = 0;
					// if we're done, clear damage bits
					m_bitsDamageType &= ~(DMG_PARALYZE << i);	

					if (m_fHeavyArmor)
						pev->maxspeed = HARMOR_MAXSPEED;
					else
						pev->maxspeed = NORMAL_MAXSPEED;

					FrozenEnd();
					IgniteEnd();
				}
			}
			else
				// first time taking this damage type - init damage duration
				m_rgbTimeBasedDamage[i] = bDuration;
		}
	}
}

/*
================
CheckPowerups

Check for turning off powerups

GLOBALS ASSUMED SET:  g_ulModelIndexPlayer
================
*/
static void CheckPowerups(entvars_t *pev)
{
	if (pev->health <= 0)
		return;

	pev->modelindex = g_ulModelIndexPlayer;    // don't use eyes
}


//=========================================================
// UpdatePlayerSound - updates the position of the player's
// reserved sound slot in the sound list.
//=========================================================
void CBasePlayer :: UpdatePlayerSound ( void )
{
	int iBodyVolume;
	int iVolume;
	CSound *pSound;

	pSound = CSoundEnt::SoundPointerForIndex( CSoundEnt :: ClientSoundIndex( edict() ) );

	if ( !pSound )
	{
		ALERT ( at_console, "Client lost reserved sound!\n" );
		return;
	}

	pSound->m_iType = bits_SOUND_NONE;

	// now calculate the best target volume for the sound. If the player's weapon
	// is louder than his body/movement, use the weapon volume, else, use the body volume.
	
	if ( FBitSet ( pev->flags, FL_ONGROUND ) )
	{	
		iBodyVolume = pev->velocity.Length(); 

		// clamp the noise that can be made by the body, in case a push trigger,
		// weapon recoil, or anything shoves the player abnormally fast. 
		if ( iBodyVolume > 512 )
		{
			iBodyVolume = 512;
		}
	}
	else
	{
		iBodyVolume = 0;
	}

	if ( pev->button & IN_JUMP )
	{
		iBodyVolume += 100;
	}

	// if target volume is greater than the player sound's current volume, we paste the new volume in 
	// immediately. If target is less than the current volume, current volume is not set immediately to the
	// lower volume, rather works itself towards target volume over time. This gives monsters a much better chance
	// to hear a sound, especially if they don't listen every frame.
	iVolume = pSound->m_iVolume;

	if ( m_iTargetVolume > iVolume )
	{
		iVolume = m_iTargetVolume;
	}
	else if ( iVolume > m_iTargetVolume )
	{
		iVolume -= 250 * gpGlobals->frametime;

		if ( iVolume < m_iTargetVolume )
		{
			iVolume = 0;
		}
	}

	if ( gpGlobals->time > m_flStopExtraSoundTime )
	{
		// since the extra sound that a weapon emits only lasts for one client frame, we keep that sound around for a server frame or two 
		// after actual emission to make sure it gets heard.
		m_iExtraSoundTypes = 0;
	}

	if ( pSound )
	{
		pSound->m_vecOrigin = pev->origin;
		pSound->m_iType |= ( bits_SOUND_PLAYER | m_iExtraSoundTypes );
		pSound->m_iVolume = iVolume;
	}
}


void CBasePlayer::PostThink()
{
	if ( g_fGameOver )
		goto pt_end;         // intermission or finale

	if (!IsAlive())
		goto pt_end;

	// buz: spec tank
	if (m_pSpecTank)
	{
		if ( m_pSpecTank->OnControls( pev ) && !pev->weaponmodel )
		{  
			m_pSpecTank->Use( this, this, USE_SET, 2 );	// try fire the gun
		}
		else
		{  // they've moved off the platform
			m_pSpecTank->Use( this, this, USE_OFF, 0 );
			m_pSpecTank = NULL;
		}
	}

// do weapon stuff
	ItemPostFrame( );


// check to see if player landed hard enough to make a sound
// falling farther than half of the maximum safe distance, but not as far a max safe distance will
// play a bootscrape sound, and no damage will be inflicted. Fallling a distance shorter than half
// of maximum safe distance will make no sound. Falling farther than max safe distance will play a 
// fallpain sound, and damage will be inflicted based on how far the player fell

	if ( (FBitSet(pev->flags, FL_ONGROUND)) && (pev->health > 0) && m_flFallVelocity >= PLAYER_FALL_PUNCH_THRESHHOLD )
	{
		if (pev->watertype == CONTENT_WATER)
		{
			EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_wade1.wav", 1, ATTN_NORM);
		}
		else if ( m_flFallVelocity > PLAYER_MAX_SAFE_FALL_SPEED )
		{// after this point, we start doing damage
			
			float flFallDamage = g_pGameRules->FlPlayerFallDamage( this );

			if ( flFallDamage > pev->health )
			{//splat
				// note: play on item channel because we play footstep landing on body channel
				EMIT_SOUND(ENT(pev), CHAN_ITEM, "common/bodysplat.wav", 1, ATTN_NORM);
			}

			if ( flFallDamage > 0 )
			{
				TakeDamage(VARS(eoNullEntity), VARS(eoNullEntity), flFallDamage, DMG_CRUSH); 
				pev->punchangle.x = 0;
			}
		}

		if ( IsAlive() )
		{
			SetAnimation( PLAYER_WALK );
		}
    }

	if (FBitSet(pev->flags, FL_ONGROUND))
	{		
		if (m_flFallVelocity > 64 && !g_pGameRules->IsMultiplayer())
		{
			CSoundEnt::InsertSound ( bits_SOUND_PLAYER, pev->origin, m_flFallVelocity, 0.2 );
		}
		m_flFallVelocity = 0;
	}

	// select the proper animation for the player character	
	if ( IsAlive() )
	{
		if (!pev->velocity.x && !pev->velocity.y)
			SetAnimation( PLAYER_IDLE );
		else if ((pev->velocity.x || pev->velocity.y) && (FBitSet(pev->flags, FL_ONGROUND)))
			SetAnimation( PLAYER_WALK );
		else if (pev->waterlevel > 1)
			SetAnimation( PLAYER_WALK );
	}

	StudioFrameAdvance( );
	CheckPowerups(pev);

	UpdatePlayerSound();

pt_end:
	// fix the corpses. 
	if (pev->deadflag == DEAD_NO) v_LastAngles = pev->angles;
	else pev->angles = v_LastAngles;

	//LLAPb: Moved here, so we can check this in spectator mode
	m_afButtonLast = pev->button;
return;
}


// checks if the spot is clear of players
BOOL IsSpawnPointValid( CBaseEntity *pPlayer, CBaseEntity *pSpot )
{
	CBaseEntity *ent = NULL;

	if ( !pSpot->IsTriggered( pPlayer ) )
	{
		return FALSE;
	}

	while ( (ent = UTIL_FindEntityInSphere( ent, pSpot->pev->origin, 128 )) != NULL )
	{
		// if ent is a client, don't spawn on 'em
		if ( ent->IsPlayer() && ent != pPlayer && ent->IsAlive() )
			return FALSE;
	}

	return TRUE;
}


DLL_GLOBAL CBaseEntity	*g_pLastSpawn;
inline int FNullEnt( CBaseEntity *ent ) { return (ent == NULL) || FNullEnt( ent->edict() ); }

/*
============
EntSelectSpawnPoint

Returns the entity to spawn at

USES AND SETS GLOBAL g_pLastSpawn
============
*/
edict_t *EntSelectSpawnPoint( CBaseEntity *pPlayer )
{
	if (!pPlayer->IsPlayer()) return INDEXENT(0);

	CBaseEntity *pSpot;
	edict_t		*player;
	char		spotName[22] = " ";
	CBasePlayer *pplayer = (CBasePlayer *)pPlayer;

	if (g_pGameRules->IsTeamplay())
	{
		switch (g_pGameRules->GetTeamIndex( pplayer->m_szTeamName ))
		{
		case 0:
			strcpy(spotName, "info_player_start");
			break;
		case 1:
		default:
			strcpy(spotName, "info_player_deathmatch");
			break;
		}
	}
	else
	{
		int S = 0,
			D = 0;

		CBaseEntity *pDot = NULL;

		while ((pDot = UTIL_FindEntityByClassname(pDot, "info_player_start")) != NULL)
			++S;

		pDot = NULL;

		while ((pDot = UTIL_FindEntityByClassname(pDot, "info_player_deathmatch")) != NULL)
			++D;


		if (RANDOM_LONG(1, S + D) <= S)
		{

			strcpy(spotName, "info_player_start");
		}
		else
		{
			strcpy(spotName, "info_player_deathmatch");
		}
	}
	player = pPlayer->edict();

	if (pplayer->m_pCarryingObject != NULL)
		pplayer->m_pCarryingObject->Use (pplayer, pplayer, USE_TOGGLE, 2);// COU_DROPPED = 2

	if (g_pGameRules->IsCoOp())
	{

		pSpot = UTIL_FindEntityByClassname( g_pLastSpawn, "info_player_coop");
		if ( !FNullEnt(pSpot) )
			goto ReturnSpot;
		pSpot = UTIL_FindEntityByClassname( g_pLastSpawn, "info_player_start");
		if ( !FNullEnt(pSpot) ) 
			goto ReturnSpot;
	}
	else if ( g_pGameRules->IsDeathmatch() )
	{

		pSpot = g_pLastSpawn;
		// Randomize the start spot
		for ( int i = RANDOM_LONG(1,5); i > 0; i-- )
			pSpot = UTIL_FindEntityByClassname( pSpot, spotName );
		if ( FNullEnt( pSpot ) )  // skip over the null point
			pSpot = UTIL_FindEntityByClassname( pSpot, spotName );

		CBaseEntity *pFirstSpot = pSpot;
		do 
		{
			if ( pSpot )
			{
				// check if pSpot is valid
				if ( IsSpawnPointValid( pPlayer, pSpot ) )
				{
					if ( pSpot->pev->origin == Vector( 0, 0, 0 ) )
					{
						pSpot = UTIL_FindEntityByClassname( pSpot, spotName );
						continue;
					}
					// if so, go to pSpot
					goto ReturnSpot;
				}
			}
			// increment pSpot
			pSpot = UTIL_FindEntityByClassname( pSpot, spotName );
		}
		while ( pSpot != pFirstSpot ); // loop if we're not back to the start


		// we haven't found a place to spawn yet,  so kill any guy at the first spawn point and spawn there
		if ( !FNullEnt( pSpot ) )
		{

			CBaseEntity *ent = NULL;
			while ( (ent = UTIL_FindEntityInSphere( ent, pSpot->pev->origin, 72 )) != NULL )
			{
				// if ent is a client, kill em (unless they are ourselves)
				if ( ent->IsPlayer() && !(ent->edict() == player) && ent->IsAlive() )
					ent->TakeDamage( VARS(INDEXENT(0)), VARS(INDEXENT(0)), 700, DMG_IGNOREARMOR);
			}
			goto ReturnSpot;
		}
	}

	// WHAT THE FUCK IS THIS???
	// If startspot is set, (re)spawn there.
	if ( !FStringNull( gpGlobals->startspot ) && strlen(STRING(gpGlobals->startspot)))
	{

		pSpot = UTIL_FindEntityByTargetname( NULL, STRING(gpGlobals->startspot) );
		if ( !FNullEnt(pSpot) )
			goto ReturnSpot;
	}

ReturnSpot:
	if ( FNullEnt( pSpot ) )
	{
		ALERT(at_error, "PutClientInServer: no info_player_start on level");
		return INDEXENT(0);
	}

	g_pLastSpawn = pSpot;
	return pSpot->edict();
}

void CBasePlayer::BuyDeadItems ( void )
{
for (int i = 0; i < 70; i++)
{
	if (DeadItems[i] > 0)
		switch (i)
		{
			case DEAD_BUY_MEDKIT: GiveNamedItem( "weapon_medkit" ); break;
			case DEAD_BUY_CROWBAR: GiveNamedItem( "weapon_crowbar" ); break;
			case DEAD_BUY_TURRETKIT: GiveNamedItem( "weapon_turretkit" ); break;
			case DEAD_BUY_BANDSAW: GiveNamedItem( "weapon_bandsaw" ); break;
			case DEAD_BUY_LIGHTSABER: GiveNamedItem( "weapon_lightsaber" ); break;
			case DEAD_BUY_GLOCK:
				{
					for (int j = 0; j < DeadItems[i]; j++)
						GiveNamedItem( "weapon_glock" );
				}
			break;
			case DEAD_BUY_USP: GiveNamedItem( "weapon_usp" ); break;
			case DEAD_BUY_DEAGLE: GiveNamedItem( "weapon_deagle" ); break;
			case DEAD_BUY_PYTHON: GiveNamedItem( "weapon_python" ); break;
			case DEAD_BUY_UZI:
				{
					for (int j = 0; j < DeadItems[i]; j++)
						GiveNamedItem( "weapon_uzi" );
				}
			break;
			case DEAD_BUY_SHIELDGUN: GiveNamedItem( "weapon_shieldgun" ); break;
			case DEAD_BUY_SHOTGUN: GiveNamedItem( "weapon_shotgun" ); break;
			case DEAD_BUY_AUTOSHOTGUN: GiveNamedItem( "weapon_autoshotgun" ); break;
			case DEAD_BUY_30MMSG: GiveNamedItem( "weapon_30mmsg" ); break;
			case DEAD_BUY_MP5: GiveNamedItem( "weapon_mp5" ); break;
			case DEAD_BUY_M16: GiveNamedItem( "weapon_m16" ); break;
			case DEAD_BUY_AKIMBOGUN: GiveNamedItem( "weapon_akimbogun" ); break;
			case DEAD_BUY_AK74: GiveNamedItem( "weapon_ak74" ); break;
			case DEAD_BUY_CROSSBOW: GiveNamedItem( "weapon_crossbow" ); break;
			case DEAD_BUY_G11: GiveNamedItem( "weapon_g11" ); break;
			case DEAD_BUY_U2: GiveNamedItem( "weapon_u2" ); break;
			case DEAD_BUY_SVD: GiveNamedItem( "weapon_svd" ); break;
			case DEAD_BUY_AWP: GiveNamedItem( "weapon_awp" ); break;
			case DEAD_BUY_BARETT: GiveNamedItem( "weapon_barett" ); break;
			case DEAD_BUY_M249: GiveNamedItem( "weapon_m249" ); break;
			case DEAD_BUY_MINIGUN: GiveNamedItem( "weapon_minigun" ); break;
			case DEAD_BUY_MACHINEGUN: GiveNamedItem( "weapon_machinegun" ); break;
			case DEAD_BUY_TORCH: GiveNamedItem( "weapon_torch" ); break;
			case DEAD_BUY_NAILGUN: GiveNamedItem( "weapon_nailgun" ); break;
			case DEAD_BUY_FROSTER: GiveNamedItem( "weapon_froster" ); break;
			case DEAD_BUY_FLAMETHROWER: GiveNamedItem( "weapon_flamethrower" ); break;
			case DEAD_BUY_FLAKCANNON: GiveNamedItem( "weapon_flakcannon" ); break;
			case DEAD_BUY_BFG: GiveNamedItem( "weapon_bfg" ); break;
			case DEAD_BUY_RPG: GiveNamedItem( "weapon_rpg" ); break;
			case DEAD_BUY_INCENDIARY: GiveNamedItem( "weapon_incendiary" ); break;
			case DEAD_BUY_WHL: GiveNamedItem( "weapon_whl" ); break;
			case DEAD_BUY_DEVASTATOR: GiveNamedItem( "weapon_devastator" ); break;
			case DEAD_BUY_REDEEMER: GiveNamedItem( "weapon_redeemer" ); break;
			case DEAD_BUY_SMARTGUN: GiveNamedItem( "weapon_smartgun" ); break;
			case DEAD_BUY_TESLAGUN: GiveNamedItem( "weapon_teslagun" ); break;
			case DEAD_BUY_EGON: GiveNamedItem( "weapon_egon" ); break;
			case DEAD_BUY_PLASMARIFLE: GiveNamedItem( "weapon_plasmarifle" ); break;
			case DEAD_BUY_PHOTONGUN: GiveNamedItem( "weapon_photongun" ); break;
			case DEAD_BUY_GAUSS: GiveNamedItem( "weapon_gauss" ); break;
			case DEAD_BUY_TAUCANNON: GiveNamedItem( "weapon_taucannon" ); break;
			case DEAD_BUY_GLUONGUN: GiveNamedItem( "weapon_gluongun" ); break;
			case DEAD_BUY_DISPLACER: GiveNamedItem( "weapon_displacer" ); break;
			case DEAD_BUY_FLASHBANG:
				{
					for (int j = 0; j < DeadItems[i]; j++)
						GiveNamedItem( "weapon_flashbang" );
				}
			break;
			case DEAD_BUY_HANDGRENADE:
				{
					for (int j = 0; j < DeadItems[i]; j++)
						GiveNamedItem( "weapon_handgrenade" );
				}
			break;
			case DEAD_BUY_TRIPMINE:
				{
					for (int j = 0; j < DeadItems[i]; j++)
						GiveNamedItem( "weapon_tripmine" );
				}
			break;
			case DEAD_BUY_SATCHEL:
				{
					for (int j = 0; j < DeadItems[i]; j++)
						GiveNamedItem( "weapon_satchel" );
				}
			break;
			case DEAD_BUY_C4:
				{
					for (int j = 0; j < DeadItems[i]; j++)
						GiveNamedItem( "weapon_c4" );
				}
			break;
			case DEAD_BUY_BLASTER: GiveNamedItem( "weapon_blaster" ); break;
			case DEAD_BUY_BIORIFLE: GiveNamedItem( "weapon_biorifle" ); break;
			case DEAD_BUY_PULSERIFLE: GiveNamedItem( "weapon_pulserifle" ); break;
			case DEAD_BUY_M72: GiveNamedItem( "weapon_m72" ); break;
			case DEAD_BUY_SATELLITE: GiveNamedItem( "weapon_satellite" ); break;
			case DEAD_BUY_CHRONOSCEPTOR: GiveNamedItem( "weapon_chronosceptor" ); break;
			case DEAD_BUY_BATTERY:
				{
					for (int j = 0; j < DeadItems[i]; j++)
						GiveNamedItem( "item_battery" );
				}
			break;
			case DEAD_BUY_KEVLAR: GiveNamedItem( "item_kevlar" ); break;
			case DEAD_BUY_HARMOR: GiveNamedItem( "item_harmor" ); break;
			case DEAD_BUY_LONGJUMP: GiveNamedItem( "item_longjump" ); break;
			case DEAD_BUY_ANTIGRAV: GiveNamedItem( "item_antigrav" ); break;
			case DEAD_BUY_CLOAK: GiveNamedItem( "item_cloak" ); break;
			case DEAD_BUY_PT: GiveNamedItem( "item_pt" ); break;
			case DEAD_BUY_POWERSHIELD: GiveNamedItem( "item_PowerShield" ); break;
			case DEAD_BUY_PORTABLEHEALTHKIT: GiveNamedItem( "item_portableHealthkit" ); break;
			case DEAD_BUY_PORTABLEHEV: GiveNamedItem( "item_portableHEV" ); break;
		}
		DeadItems[i] = 0;
	}
}

void CBasePlayer::Spawn( void )
{
	lastDamageType = 0; 
	pev->body 		= 0;
	pev->classname		= MAKE_STRING("player");
	pev->health		= 100;
	pev->armorvalue		= 0;
	pev->takedamage		= DAMAGE_AIM;
	pev->solid		= SOLID_SLIDEBOX;
	pev->movetype		= MOVETYPE_WALK;
	pev->max_health		= pev->health;
	pev->flags		&= FL_PROXY;	// keep proxy flag sey by engine
	pev->flags		|= FL_CLIENT;
	pev->air_finished	= gpGlobals->time + 12;
	pev->dmg		= 2;				// initial water damage
	pev->effects		= 0;
	pev->deadflag		= DEAD_NO;
	pev->dmg_take		= 0;
	pev->dmg_save		= 0;
	pev->friction		= 1.0;
	pev->gravity		= 1.0;
	pev->fuser1		= 0.0;
	m_bitsHUDDamage		= -1;
	m_bitsDamageType	= 0;
	m_afPhysicsFlags	= 0;
	m_fLongJump		= 0; 
	m_fPortableHEV		= 0; 
	m_fpt			= 0; 
	m_fTurretSentry		= 0; 
	m_fTurretIon		= 0; 
	m_fTurretMissile	= 0; 
	m_fPortableHealthkit	= 0;
	m_fCloak		= 0; 
	m_fCloakActivated	= 0;	
	m_fAntigrav		= 0; 
	m_fAntigravActivated	= 0;	
	m_fKevlar		= 0; 
	m_fHeavyArmor		= 0; 
	m_fPowerShield		= 0; 
	pev->rendermode		= kRenderNormal;
	pev->renderfx 		= 0;
	m_fFrozen 		= 0;
	m_fIgnite 		= 0;
	m_fDropped 		= 0;
	m_fAnnihilated		= 0; 
	m_fConcussLevel		= 0;
	m_fVodkaShots		= 0;

	m_LastAttacker1	= m_LastAttacker2 = m_LastAttacker3 = NULL;

	UTIL_ScreenFade( this, Vector(255,128,0), 0, 0, 0, FFADE_IN );
	m_iWeapons2 = FALSE; // weapon limit 'BUG' FIX!!!
	pev->weapons = FALSE;

	lastKillTime = gpGlobals->time - (MK_DELAY_TIME - 1); //if player kills smb. on spawn don't let em recieve multikill
	KillsAmount = 0;

	// rain tutorial
	Rain_dripsPerSecond = 0;
	Rain_windX = 0;
	Rain_windY = 0;
	Rain_randX = 0;
	Rain_randY = 0;
	Rain_ideal_dripsPerSecond = 0;
	Rain_ideal_windX = 0;
	Rain_ideal_windY = 0;
	Rain_ideal_randX = 0;
	Rain_ideal_randY = 0;
	Rain_endFade = 0;
	Rain_nextFadeUpdate = 0;

	g_engfuncs.pfnSetPhysicsKeyValue( edict(), "slj", "0" );
	g_engfuncs.pfnSetPhysicsKeyValue( edict(), "harm", "0" );
	g_engfuncs.pfnSetPhysicsKeyValue( edict(), "hl", "1" );

	pev->fov = m_iFOV	= 0;// init field of view.
	m_iClientFOV		= -1; // make sure fov reset is sent
	m_flNextDecalTime	= 0;// let this player decal as soon as he spawns.
	m_flTimeStepSound	= 0;
	m_iStepLeft 		= 0;
	m_flFieldOfView		= 0.5;// some monsters use this to determine whether or not the player is looking at them.
	m_flNextAttack		= UTIL_WeaponTimeBase();
	m_iFlashBattery 	= 100;
	m_iKevlarBattery 	= m_iHarmorBattery = 0;
	m_flFlashLightTime 	= 1; // force first message
	m_flFallVelocity 	= 0;

	g_pGameRules->SetDefaultPlayerTeam( this );
	g_pGameRules->GetPlayerSpawnSpot( this );

	SET_MODEL(ENT(pev), "models/player/gordon/gordon.mdl");
	g_ulModelIndexPlayer = pev->modelindex;

	pev->sequence		= LookupActivity( ACT_IDLE );

	if ( FBitSet(pev->flags, FL_DUCKING) ) 
		UTIL_SetSize(pev, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX);
	else
		UTIL_SetSize(pev, VEC_HULL_MIN, VEC_HULL_MAX);

	pev->view_ofs = VEC_VIEW;
	Precache();
	m_HackedGunPos		= Vector( 0, 32, 0 );

	if ( m_iPlayerSound == SOUNDLIST_EMPTY )
	{
		ALERT ( at_console, "Couldn't alloc player sound slot!\n" );
	}

	m_pLastItem = NULL;
	m_fInitHUD = TRUE;
	m_iClientHideHUD = -1;  // force this to be recalculated
	m_fWeapon = FALSE;
	m_pClientActiveItem = NULL;
	m_iClientBattery = -1;

	m_iClientKevlar = -1;
	m_iClientHarmor = -1;
	m_iClientPShield = -1;
	m_iClientCloak = -1;
	m_iClientLongjump = -1;
	m_iClientAntigrav = -1;
	m_iClientPt = -1;
	m_iClientPhealth = -1;
	m_iClientPHEV = -1;
	m_iClientTurretS = -1;
	m_iClientTurretM = -1;
	m_iClientTurretI = -1;
	m_iClientMoney = -1;
	m_iClientBuyZone = -1;

	// reset all ammo values to 0
	for ( int i = 0; i < MAX_AMMO_SLOTS; i++ )
	{
		m_rgAmmo[i] = 0;
		m_rgAmmoLast[i] = 0;  // client ammo values also have to be reset  (the death hud clear messages does on the client side)
	}

	m_lastx = m_lasty = 0;
	m_flNextChatTime = gpGlobals->time;

	g_pGameRules->PlayerSpawn( this );
	BuyDeadItems();	//if you buy any item when you dead, it will be added to ammunition when you'll respawn
}


void CBasePlayer :: Precache( void )
{
	// in the event that the player JUST spawned, and the level node graph
	// was loaded, fix all of the node graph pointers before the game starts.
	
	// !!!BUGBUG - now that we have multiplayer, this needs to be moved!
	if ( WorldGraph.m_fGraphPresent && !WorldGraph.m_fGraphPointersSet )
	{
		if ( !WorldGraph.FSetGraphPointers() )
			ALERT ( at_console, "**Graph pointers were not set!\n");
		else
			ALERT ( at_console, "**Graph Pointers Set!\n" );
	}

	m_bitsDamageType = 0;
	m_bitsHUDDamage = -1;
	m_iClientBattery = -1;
	m_iClientHarmor = -1;
	m_iClientKevlar = -1;
	m_iClientPShield = -1;
	m_iClientCloak = -1;
	m_iClientLongjump = -1;
	m_iClientAntigrav = -1;
	m_iClientPt = -1;
	m_iClientPhealth = -1;
	m_iClientPHEV = -1;
	m_iClientMoney = -1;
	m_iClientBuyZone = -1;
	m_iClientTurretS = -1;
	m_iClientTurretM = -1;
	m_iClientTurretI = -1;

	m_iTrain = TRAIN_NEW;

	// Make sure any necessary user messages have been registered
	LinkUserMessages();

	m_iUpdateTime = 5;  // won't update for 1/2 a second

	if ( gInitHUD )
		m_fInitHUD = TRUE;

	// rain tutorial
	Rain_needsUpdate = 1;
}


int CBasePlayer::Save( CSave &save )
{
	if ( !CBaseMonster::Save(save) )
		return 0;

	return save.WriteFields( "PLAYER", this, m_playerSaveData, ARRAYSIZE(m_playerSaveData) );
}

int CBasePlayer::Restore( CRestore &restore )
{
	if ( !CBaseMonster::Restore(restore) )
		return 0;

	int status = restore.ReadFields( "PLAYER", this, m_playerSaveData, ARRAYSIZE(m_playerSaveData) );

	SAVERESTOREDATA *pSaveData = (SAVERESTOREDATA *)gpGlobals->pSaveData;
	// landmark isn't present.
	if ( !pSaveData->fUseLandmark )
	{
		ALERT( at_console, "No Landmark:%s\n", pSaveData->szLandmarkName );

		// default to normal spawn
		edict_t* pentSpawnSpot = EntSelectSpawnPoint( this );
		pev->origin = VARS(pentSpawnSpot)->origin + Vector(0,0,1);
		pev->angles = VARS(pentSpawnSpot)->angles;
	}
	pev->v_angle.z = 0;	// Clear out roll
	pev->angles = pev->v_angle;
	pev->fixangle = TRUE;           // turn this way immediately
	g_ulModelIndexPlayer = pev->modelindex;

	if ( FBitSet(pev->flags, FL_DUCKING) ) 
		UTIL_SetSize(pev, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX);
	else
		UTIL_SetSize(pev, VEC_HULL_MIN, VEC_HULL_MAX);

	g_engfuncs.pfnSetPhysicsKeyValue( edict(), "hl", "1" );

	if (m_fLongJump && !m_fHeavyArmor)
		g_engfuncs.pfnSetPhysicsKeyValue( edict(), "slj", "1" );
	else
		g_engfuncs.pfnSetPhysicsKeyValue( edict(), "slj", "0" );

	if (m_fHeavyArmor)
		g_engfuncs.pfnSetPhysicsKeyValue( edict(), "harm", "1" );
	else
		g_engfuncs.pfnSetPhysicsKeyValue( edict(), "harm", "0" );

	return status;
}

void CBasePlayer::SelectNextItem( int iItem )
{
	CBasePlayerItem *pItem;

	pItem = m_rgpPlayerItems[ iItem ];

	if (!pItem)
		return;

	if (pItem == m_pActiveItem)
	{
		// select the next one in the chain
		pItem = m_pActiveItem->m_pNext;
		if (! pItem)
		{
			return;
		}

		CBasePlayerItem *pLast;
		pLast = pItem;
		while (pLast->m_pNext)
			pLast = pLast->m_pNext;

		// relink chain
		pLast->m_pNext = m_pActiveItem;
		m_pActiveItem->m_pNext = NULL;
		m_rgpPlayerItems[ iItem ] = pItem;
	}

	// FIX, this needs to queue them up and delay
	if (m_pActiveItem)
	{
		m_pActiveItem->Holster( );
	}

	QueueItem(pItem);

	if (m_pActiveItem)
	{
		m_pActiveItem->Deploy( );
		m_pActiveItem->UpdateItemInfo( );
	}
}

void CBasePlayer::QueueItem(CBasePlayerItem *pItem)
{
	if(!m_pActiveItem)// no active weapon
	{
		m_pActiveItem = pItem;
		return;// just set this item as active
	}
    	else
	{
		m_pLastItem = m_pActiveItem;
		m_pActiveItem = NULL;// clear current
	}
	m_pNextItem = pItem;// add item to queue
}

void CBasePlayer::SelectItem(const char *pstr)
{
	if (!pstr)
		return;

	CBasePlayerItem *pItem = NULL;

	for (int i = 0; i < MAX_ITEM_TYPES; i++)
	{
		if (m_rgpPlayerItems[i])
		{
			pItem = m_rgpPlayerItems[i];

			while (pItem)
			{
				if (FClassnameIs(pItem->pev, pstr))
					break;
				pItem = pItem->m_pNext;
			}
		}

		if (pItem)
			break;
	}

	if (!pItem)
		return;

	if (!pItem->CanDeploy())
		return;

	if (IsOnLadder())//Can not select weapons on ladders now
		return;

	if (pItem == m_pActiveItem)
		return;

	// FIX, this needs to queue them up and delay
	if (m_pActiveItem)
		m_pActiveItem->Holster( );

	QueueItem(pItem);

	if (m_pActiveItem)
	{
		m_pActiveItem->Deploy( );
		m_pActiveItem->UpdateItemInfo( );
	}
}


void CBasePlayer::SelectLastItem(void)
{
	if (!m_pLastItem || !m_pLastItem->CanDeploy())
		return;

	if (IsOnLadder())
		return;

	if (m_pActiveItem && !m_pActiveItem->CanHolster())
		return;

	// FIX, this needs to queue them up and delay
	if (m_pActiveItem)
		m_pActiveItem->Holster( );

	QueueItem(m_pLastItem);

	if (m_pActiveItem)
	{
		m_pActiveItem->Deploy( );
		m_pActiveItem->UpdateItemInfo( );
	}
}

//==============================================
// HasWeapons - do I have any weapons at all?
//==============================================
BOOL CBasePlayer::HasWeapons( void )
{
	int i;

	for ( i = 0 ; i < MAX_ITEM_TYPES ; i++ )
	{
		if ( m_rgpPlayerItems[ i ] )
		{
			return TRUE;
		}
	}

	return FALSE;
}

void CBasePlayer::SelectPrevItem( int iItem )
{
}

const char *CBasePlayer::TeamID( void )
{
	if ( pev == NULL )		// Not fully connected yet
		return "";

	// return their team name
	return m_szTeamName;
}

//==============================================
// !!!UNDONE:ultra temporary SprayCan entity to apply
// decal frame at a time. For PreAlpha CD
//==============================================
class CSprayCan : public CBaseEntity
{
public:
	void	Spawn ( entvars_t *pevOwner );
	void	Think( void );

	virtual int	ObjectCaps( void ) { return FCAP_DONT_SAVE; }
};

void CSprayCan::Spawn ( entvars_t *pevOwner )
{
	pev->origin = pevOwner->origin + Vector ( 0 , 0 , 32 );
	pev->angles = pevOwner->v_angle;
	pev->owner = ENT(pevOwner);
	pev->frame = 0;

	pev->nextthink = gpGlobals->time + 0.1;
	EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/sprayer.wav", 1, ATTN_NORM);
}

void CSprayCan::Think( void )
{
	TraceResult	tr;	
	int playernum;
	int nFrames;
	CBasePlayer *pPlayer;
	
	pPlayer = (CBasePlayer *)GET_PRIVATE(pev->owner);

	if (pPlayer)
		nFrames = pPlayer->GetCustomDecalFrames();
	else
		nFrames = -1;

	playernum = ENTINDEX(pev->owner);
	
	// ALERT(at_console, "Spray by player %i, %i of %i\n", playernum, (int)(pev->frame + 1), nFrames);

	UTIL_MakeVectors(pev->angles);
	UTIL_TraceLine ( pev->origin, pev->origin + gpGlobals->v_forward * 128, ignore_monsters, pev->owner, & tr);

	// No customization present.
	if (nFrames == -1)
	{
		UTIL_DecalTrace( &tr, DECAL_SCORCH1 );
		UTIL_Remove( this );
	}
	else
	{
		UTIL_PlayerDecalTrace( &tr, playernum, pev->frame, TRUE );
		// Just painted last custom frame.
		if ( pev->frame++ >= (nFrames - 1))
			UTIL_Remove( this );
	}

	pev->nextthink = gpGlobals->time + 0.1;
}

class	CBloodSplat : public CBaseEntity
{
public:
	void	Spawn ( entvars_t *pevOwner );
	void	Spray ( void );
};

void CBloodSplat::Spawn ( entvars_t *pevOwner )
{
	pev->origin = pevOwner->origin + Vector ( 0 , 0 , 32 );
	pev->angles = pevOwner->v_angle;
	pev->owner = ENT(pevOwner);

	SetThink ( Spray );
	pev->nextthink = gpGlobals->time + 0.1;
}

void CBloodSplat::Spray ( void )
{
	TraceResult	tr;	
	UTIL_MakeVectors(pev->angles);
	UTIL_TraceLine ( pev->origin, pev->origin + gpGlobals->v_forward * 128, ignore_monsters, pev->owner, & tr);
	UTIL_BloodDecalTrace(&tr);
	SetThink ( SUB_Remove );
	pev->nextthink = gpGlobals->time + 0.1;
}

//==============================================


void CBasePlayer::GiveNamedItem( const char *pszName )
{
	edict_t	*pent;

	int istr = MAKE_STRING(pszName);

	pent = CREATE_NAMED_ENTITY(istr);
	if ( FNullEnt( pent ) )
	{
		ALERT ( at_console, "NULL Ent in GiveNamedItem!\n" );
		return;
	}
	VARS( pent )->origin = pev->origin;
	pent->v.spawnflags |= SF_NORESPAWN;

	DispatchSpawn( pent );
	DispatchTouch( pent, ENT( pev ) );
}

CBaseEntity *FindEntityForward( CBaseEntity *pMe )
{
	TraceResult tr;

	UTIL_MakeVectors(pMe->pev->v_angle);
	UTIL_TraceLine(pMe->pev->origin + pMe->pev->view_ofs,pMe->pev->origin + pMe->pev->view_ofs + gpGlobals->v_forward * 16384, dont_ignore_monsters, pMe->edict(), &tr );
	if ( tr.flFraction != 1.0 && !FNullEnt( tr.pHit) )
	{
		CBaseEntity *pHit = CBaseEntity::Instance( tr.pHit );
		return pHit;
	}
	return NULL;
}


BOOL CBasePlayer :: FlashlightIsOn( void )
{
	return FBitSet(pev->effects, EF_DIMLIGHT);
}


void CBasePlayer :: FlashlightTurnOn( void )
{
	if (m_iFlashBattery <= 0)
	return;

	if ( (pev->weapons & (1<<WEAPON_SUIT)) )
	{
		EMIT_SOUND_DYN( ENT(pev), CHAN_WEAPON, "items/flashlight1.wav", 1.0, ATTN_NORM, 0, PITCH_NORM );
		SetBits(pev->effects, EF_DIMLIGHT);

		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pev);
		WRITE_BYTE(MSG_FLASHLIGHT);
		WRITE_BYTE(1);
		WRITE_BYTE(m_iFlashBattery);
		WRITE_ELSE(1+2);
		MESSAGE_END();

		m_flFlashLightTime = FLASH_DRAIN_TIME + gpGlobals->time;
	}
}


void CBasePlayer :: FlashlightTurnOff( void )
{
	EMIT_SOUND_DYN( ENT(pev), CHAN_WEAPON, "items/flashlight1.wav", 1.0, ATTN_NORM, 0, PITCH_NORM );
	ClearBits(pev->effects, EF_DIMLIGHT);

	MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pev);
	WRITE_BYTE(MSG_FLASHLIGHT);
	WRITE_BYTE(0);
	WRITE_BYTE(m_iFlashBattery);
	WRITE_ELSE(1+2);
	MESSAGE_END();

	m_flFlashLightTime = 0;
}

/*
===============
ForceClientDllUpdate

When recording a demo, we need to have the server tell us the entire client state
so that the client side .dll can behave correctly.
Reset stuff so that the state is transmitted.
===============
*/
void CBasePlayer :: ForceClientDllUpdate( void )
{
	m_iClientHealth  = -1;
	m_iClientBattery = -1;
	m_iClientKevlar = -1;
	m_iClientHarmor = -1;
	m_iClientPShield = -1;
	m_iClientCloak = -1;
	m_iClientLongjump = -1;
	m_iClientAntigrav = -1;
	m_iClientPt = -1;
	m_iClientPhealth = -1;
	m_iClientPHEV = -1;
	m_iClientMoney = -1;
	m_iClientBuyZone = -1;
	m_iClientTurretS = -1;
	m_iClientTurretM = -1;
	m_iClientTurretI = -1;

	m_iTrain |= TRAIN_NEW;  // Force new train message.
	m_fWeapon = FALSE;          // Force weapon send
	m_fKnownItem = FALSE;    // Force weaponinit messages.
	m_fInitHUD = TRUE;		// Force HUD gmsgResetHUD message

	// Now force all the necessary messages
	//  to be sent.
	UpdateClientData();
}

/*
============
ImpulseCommands
============
*/
extern float g_flWeaponCheat;

void CBasePlayer::ImpulseCommands( )
{
	TraceResult	tr;

	// Handle use events
	PlayerUse();

	if (pev->flags & FL_FROZEN)
	{
		pev->impulse = 0;
		return;
	}
		
	int iImpulse = (int)pev->impulse;
	switch (iImpulse)
	{
	case 100:
        // temporary flashlight for level designers
	        if ( FlashlightIsOn() )
			FlashlightTurnOff();
	        else 
			FlashlightTurnOn();
		break;

	case 150:
	{
		if(!m_fpt)
		{
			ClientPrint(pev, HUD_PRINTCENTER, UTIL_VarArgs("You don't have Personal Teleport!"));
			break;
			return;
		}

		if (m_iFlashBattery < 75)
		{
			ClientPrint(pev, HUD_PRINTCENTER, UTIL_VarArgs("Not enough ENERGY to activate PT!\n"));
			break;
			return;
		}

		edict_t *pSpot = EntSelectSpawnPoint(this);
		if (pSpot)
		{
			m_iFlashBattery -= 75;
			MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pev);
			WRITE_BYTE(MSG_FLASHBATTERY);
			WRITE_BYTE(m_iFlashBattery);
			WRITE_ELSE(1+1);
			MESSAGE_END();

			Vector tmp = pSpot->v.origin;
			tmp.z -= pev->mins.z;
			tmp.z++;
			UTIL_SetOrigin( pev, tmp );

			pev->angles = pSpot->v.angles;
			pev->velocity = pev->basevelocity = g_vecZero;
			FX_Explosion( tmp, EXPLOSION_PTELEPORT );
		}
		break;
	}
	case	201:// paint decal
		
		if ( gpGlobals->time < m_flNextDecalTime )
			break;

		UTIL_MakeVectors(pev->v_angle);
		UTIL_TraceLine ( pev->origin + pev->view_ofs, pev->origin + pev->view_ofs + gpGlobals->v_forward * 128, ignore_monsters, ENT(pev), & tr);

		if ( tr.flFraction != 1.0 )
		{// line hit something, so paint a decal
			m_flNextDecalTime = gpGlobals->time + 30;
			CSprayCan *pCan = GetClassPtr((CSprayCan *)NULL);
			pCan->Spawn( pev );
		}
		break;
	default:
		// check all of the cheat impulse commands now
		CheatImpulseCommands( iImpulse );
		break;
	}
	pev->impulse = 0;
}

//=========================================================
//=========================================================
void CBasePlayer::CheatImpulseCommands( int iImpulse )
{
	CBaseEntity *pEntity;
	TraceResult tr;

	switch ( iImpulse )
	{
	case 107:
		{
			TraceResult tr;
			Vector vecSrc = pev->origin + pev->view_ofs;
			Vector vecEnd = pev->origin + pev->view_ofs + gpGlobals->v_forward * 1500;
			edict_t	*pWorld = g_engfuncs.pfnPEntityOfEntIndex( 0 );

			UTIL_TraceLine(vecSrc, vecEnd, ignore_monsters, edict(), &tr);

			if ( tr.pHit )
				pWorld = tr.pHit;
			const char *pTextureName = TRACE_TEXTURE( pWorld, vecSrc, vecEnd );
			if ( pTextureName )
				ALERT( at_console, "Texture: %s\n", pTextureName );
		}
		break;

	case 101:
	{
		if ( g_flWeaponCheat == 0.0 )
		return;

		if (m_flMoneyAmount < MAX_MONEY)
		GiveNamedItem("item_moneycase");

		break;
	}
	case 103:
		// What the hell are you doing? I'm looking monster's Hit Points!
		pEntity = FindEntityForward( this );
		if ( pEntity )
		{
			CBaseMonster *pMonster = pEntity->MyMonsterPointer();
			if ( pMonster )
				pMonster->ReportAIState();
		}
		break;
	}
}

//
// Add a weapon to the player (Item == Weapon == Selectable Object)
//
int CBasePlayer::AddPlayerItem( CBasePlayerItem *pItem )
{
	CBasePlayerItem *pInsert;

	pInsert = m_rgpPlayerItems[pItem->iItemSlot()];

	while (pInsert)
	{
		if (FClassnameIs( pInsert->pev, STRING( pItem->pev->classname) ))
		{
			int bResult;

			if (bResult = pItem->AddDuplicate( pInsert ))
			{
				g_pGameRules->PlayerGotWeapon ( this, pItem );
				pItem->CheckRespawn();

				// ugly hack to update clip w/o an update clip message
				pInsert->UpdateItemInfo( );
				if (m_pActiveItem)
					m_pActiveItem->UpdateItemInfo( );

				pItem->Kill( );
			}
			return bResult;//LLAPb: another logic
		}
		pInsert = pInsert->m_pNext;
	}


	if (pItem->AddToPlayer( this ))
	{
		g_pGameRules->PlayerGotWeapon ( this, pItem );
		pItem->CheckRespawn();

		pItem->m_pNext = m_rgpPlayerItems[pItem->iItemSlot()];
		m_rgpPlayerItems[pItem->iItemSlot()] = pItem;

		// should we switch to this item?
		if ( g_pGameRules->FShouldSwitchWeapon( this, pItem ) )
		{
			SwitchWeapon( pItem );
		}

		pItem->AttachToPlayer( this );//LLAPb: moved here, fix up a bug with akimbo weapons pickup

		return TRUE;
	}
	return FALSE;
}


int CBasePlayer::RemovePlayerItem( CBasePlayerItem *pItem )
{
	if (m_pActiveItem == pItem)
	{
		pItem->Holster( );
		pItem->pev->nextthink = 0;// crowbar may be trying to swing again, etc.
		pItem->SetThink( NULL );
		m_pActiveItem = NULL;
		pev->viewmodel = 0;
		pev->weaponmodel = 0;
	}
	else if ( m_pLastItem == pItem )
		m_pLastItem = NULL;

	CBasePlayerItem *pPrev = m_rgpPlayerItems[pItem->iItemSlot()];

	if (pPrev == pItem)
	{
		m_rgpPlayerItems[pItem->iItemSlot()] = pItem->m_pNext;
		return TRUE;
	}
	else
	{
		while (pPrev && pPrev->m_pNext != pItem)
		{
			pPrev = pPrev->m_pNext;
		}
		if (pPrev)
		{
			pPrev->m_pNext = pItem->m_pNext;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CBasePlayer::RemoveNamedPlayerItem( const char *pszItemName )
{
	CBasePlayerItem *pItem;
	int i;

	for ( i = 0 ; i < MAX_ITEM_TYPES ; i++ )
	{
		pItem = m_rgpPlayerItems[ i ];
		while (pItem)
		{
			if ( !strcmp(pszItemName, STRING(pItem->pev->classname ) ) )
			{
				if (pItem->m_iId < 32)
					pev->weapons &= ~(1<<pItem->m_iId);
				else
					m_iWeapons2 &= ~(1<<(pItem->m_iId - 32));

				RemovePlayerItem( pItem );
				return TRUE;
			}
		pItem = pItem->m_pNext;
		}
	}
	return FALSE;
}
// Returns the unique ID for the ammo, or -1 if error
//
int CBasePlayer :: GiveAmmo( int iCount, char *szName, int iMax )
{
	if ( !szName )
	{
		// no ammo.
		return -1;
	}

	if ( !g_pGameRules->CanHaveAmmo( this, szName, iMax ) )
	{
		// game rules say I can't have any more of this ammo type.
		return -1;
	}

	int i = 0;

	i = GetAmmoIndex( szName );

	if ( i < 0 || i >= MAX_AMMO_SLOTS )
		return -1;

	int iAdd = min( iCount, iMax - m_rgAmmo[i] );
	if ( iAdd < 1 )
		return i;

	m_rgAmmo[ i ] += iAdd;


	if ( gmsgAmmoPickup )  // make sure the ammo messages have been linked first
	{
		// Send the message that ammo has been picked up
		MESSAGE_BEGIN( MSG_ONE, gmsgAmmoPickup, NULL, pev );
			WRITE_BYTE( GetAmmoIndex(szName) );		// ammo ID
			WRITE_BYTE( iAdd );		// amount
		MESSAGE_END();
	}

	TabulateAmmo();

	return i;
}


/*
============
ItemPreFrame

Called every frame by the player PreThink
============
*/
void CBasePlayer::ItemPreFrame()
{
	if ( gpGlobals->time < m_flNextAttack )
	{
		return;
	}

	if (!m_pActiveItem)// XWider
	{
		if(m_pNextItem)
		{
			m_pActiveItem = m_pNextItem;
			m_pActiveItem->Deploy();
			m_pActiveItem->UpdateItemInfo();
			m_pNextItem = NULL;
		}
	}

	if (!m_pActiveItem)
		return;

	m_pActiveItem->ItemPreFrame( );
}


/*
============
ItemPostFrame

Called every frame by the player PostThink
============
*/
void CBasePlayer::ItemPostFrame()
{
	static int fInSelect = FALSE;

	// check if the player is using a tank
	if ( m_pSpecTank )
		return;

	if ( !IsObserver() && m_pActiveItem )
	{
		if ( IsOnLadder() )
		{
			if (pev->weaponmodel)
			{
				m_pActiveItem->Holster();
				pev->weaponmodel = 0;
			}

			return;
		}
	}

    if ( gpGlobals->time < m_flNextAttack )
		return;

	ImpulseCommands();//Should this be called in spectator mode?0_o Probably, I must break this procedure in two parts...

	if ( IsObserver() )
		return;

	if (!m_pActiveItem)
		return;

	m_pActiveItem->ItemPostFrame( );
}

int CBasePlayer::AmmoInventory( int iAmmoIndex )
{
	if (iAmmoIndex == -1)
	{
		return -1;
	}

	return m_rgAmmo[ iAmmoIndex ];
}

int CBasePlayer::GetAmmoIndex(const char *psz)
{
	int i;

	if (!psz)
		return -1;

	for (i = 1; i < MAX_AMMO_SLOTS; i++)
	{
		if ( !CBasePlayerItem::AmmoInfoArray[i].pszName )
			continue;

		if (stricmp( psz, CBasePlayerItem::AmmoInfoArray[i].pszName ) == 0)
			return i;
	}

	return -1;
}

// Called from UpdateClientData
// makes sure the client has all the necessary ammo info,  if values have changed
void CBasePlayer::SendAmmoUpdate(void)
{
	for (int i=0; i < MAX_AMMO_SLOTS;i++)
	{
		if (m_rgAmmo[i] != m_rgAmmoLast[i])
		{
			m_rgAmmoLast[i] = m_rgAmmo[i];

			ASSERT( m_rgAmmo[i] >= 0 );
			ASSERT( m_rgAmmo[i] < 999 );

			// send "Ammo" update message
			MESSAGE_BEGIN( MSG_ONE, gmsgAmmoX, NULL, pev );
				WRITE_BYTE( i );
				WRITE_SHORT( max( min( m_rgAmmo[i], 999 ), 0 ) );  // clamp the value to one byte
			MESSAGE_END();
		}
	}
}

void CBasePlayer::EntsSendClientData(void)// XDM
{
	CBaseEntity *pEntity = NULL;
	edict_t *pEdict = g_engfuncs.pfnPEntityOfEntIndex(1);
	for (int i = 1; i < gpGlobals->maxEntities; i++, pEdict++)
	{
		if (!pEdict)
			continue;

		if (pEdict->free)
			continue;

		pEntity = CBaseEntity::Instance(pEdict);
		if (!pEntity)
			continue;

		pEntity->SendClientData(this, MSG_ONE);
		pEntity = NULL;
	}
}

/*
=========================================================
	UpdateClientData

resends any changed player HUD info to the client.
Called every frame by PlayerPreThink
Also called at start of demo recording and playback by
ForceClientDllUpdate to ensure the demo gets messages
reflecting all of the HUD state info.
=========================================================
*/

void CBasePlayer :: UpdateClientData( void )
{

//Bleeding!!
	if (IsAlive() && pev->health < 15 && (gpGlobals->time >= m_flBleedTime) )
	{
		FX_Explosion( pev->origin, EXPLOSION_BLOODDRIPS );
		lastDamageType |= DMG_IGNOREARMOR;
//		TakeDamage(VARS(m_LastAttacker2), VARS(m_LastAttacker2), 1, DMG_IGNOREARMOR);
		TakeDamage(VARS(eoNullEntity), VARS(eoNullEntity), 1, DMG_IGNOREARMOR);

		lastDamageType &= ~DMG_IGNOREARMOR;
		m_flBleedTime = gpGlobals->time + 1.5;
	}

// Concussion

	if ( m_fConcussLevelChange <= gpGlobals->time ) 
	{
		if (m_fConcussLevel > 0)
		{
			if (m_fConcussLevel <= 1 || !IsAlive())
			{
				MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pev);
				WRITE_BYTE(MSG_CONCUSS);
				WRITE_BYTE(0);
				WRITE_ELSE(1+1);
				MESSAGE_END();

				m_fConcussLevel = 0;
			}
			else
			{
				MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pev);
				WRITE_BYTE(MSG_CONCUSS);
				WRITE_BYTE(m_fConcussLevel);
				WRITE_ELSE(1+1);
				MESSAGE_END();

				m_fConcussLevel--;
			}
		}

		if (m_fVodkaShots > 0) m_fVodkaShots --;//Don't drain less than 0

		m_fConcussLevelChange = gpGlobals->time + 0.5;
	}
	// ====== rain tutorial =======

	// calculate and update rain fading
	if (Rain_endFade > 0)
	{
		if (gpGlobals->time < Rain_endFade)
		{ // we're in fading process
			if (Rain_nextFadeUpdate <= gpGlobals->time)
			{
				int secondsLeft = Rain_endFade - gpGlobals->time + 1;

				Rain_dripsPerSecond += (Rain_ideal_dripsPerSecond - Rain_dripsPerSecond) / secondsLeft;
				Rain_windX += (Rain_ideal_windX - Rain_windX) / (float)secondsLeft;
				Rain_windY += (Rain_ideal_windY - Rain_windY) / (float)secondsLeft;
				Rain_randX += (Rain_ideal_randX - Rain_randX) / (float)secondsLeft;
				Rain_randY += (Rain_ideal_randY - Rain_randY) / (float)secondsLeft;

				Rain_nextFadeUpdate = gpGlobals->time + 1; // update once per second
				Rain_needsUpdate = 1;

				ALERT(at_aiconsole, "Rain fading: curdrips: %i, idealdrips %i\n", Rain_dripsPerSecond, Rain_ideal_dripsPerSecond);
			}
		}
		else
		{
			Rain_nextFadeUpdate = 0;
			Rain_endFade = 0;

			Rain_dripsPerSecond = Rain_ideal_dripsPerSecond;
			Rain_windX = Rain_ideal_windX;
			Rain_windY = Rain_ideal_windY;
			Rain_randX = Rain_ideal_randX;
			Rain_randY = Rain_ideal_randY;
			Rain_needsUpdate = 1;
		}		
	}


	// send rain message
	if (Rain_needsUpdate)
	{
	//search for rain_settings entity
		edict_t *pFind; 
		pFind = FIND_ENTITY_BY_CLASSNAME( NULL, "rain_settings" );
		if (!FNullEnt( pFind ))
		{
		// rain allowed on this map
			CBaseEntity *pEnt = CBaseEntity::Instance( pFind );
			CRainSettings *pRainSettings = (CRainSettings *)pEnt;

			float raindistance = pRainSettings->Rain_Distance;
			float rainheight = pRainSettings->pev->origin[2];
			int rainmode = pRainSettings->Rain_Mode;

			// search for constant rain_modifies
			pFind = FIND_ENTITY_BY_CLASSNAME( NULL, "rain_modify" );
			while ( !FNullEnt( pFind ) )
			{
				if (pFind->v.spawnflags & 1)
				{
					// copy settings to player's data and clear fading
					CBaseEntity *pEnt = CBaseEntity::Instance( pFind );
					CRainModify *pRainModify = (CRainModify *)pEnt;

					Rain_dripsPerSecond = pRainModify->Rain_Drips;
					Rain_windX = pRainModify->Rain_windX;
					Rain_windY = pRainModify->Rain_windY;
					Rain_randX = pRainModify->Rain_randX;
					Rain_randY = pRainModify->Rain_randY;

					Rain_endFade = 0;
					break;
				}
				pFind = FIND_ENTITY_BY_CLASSNAME( pFind, "rain_modify" );
			}

			MESSAGE_BEGIN(MSG_ONE, gmsgRain, NULL, pev);
				WRITE_SHORT(Rain_dripsPerSecond);
				WRITE_COORD(raindistance);
				WRITE_COORD(Rain_windX);
				WRITE_COORD(Rain_windY);
				WRITE_COORD(Rain_randX);
				WRITE_COORD(Rain_randY);
				WRITE_BYTE(rainmode);
				WRITE_COORD(rainheight);
			MESSAGE_END();
		}
		else
		{
			Rain_dripsPerSecond = 0;
			Rain_windX = 0;
			Rain_windY = 0;
			Rain_randX = 0;
			Rain_randY = 0;
			Rain_ideal_dripsPerSecond = 0;
			Rain_ideal_windX = 0;
			Rain_ideal_windY = 0;
			Rain_ideal_randX = 0;
			Rain_ideal_randY = 0;
			Rain_endFade = 0;
			Rain_nextFadeUpdate = 0;
		}

		Rain_needsUpdate = 0;
	}
	// ====== end rain =======

	if (m_fInitHUD)
	{
		m_fInitHUD = FALSE;
		gInitHUD = FALSE;

		MESSAGE_BEGIN( MSG_ONE, gmsgResetHUD, NULL, pev );
		WRITE_BYTE( 0 );
		MESSAGE_END();

		if ( !m_fGameHUDInitialized )
		{
			MESSAGE_BEGIN( MSG_ONE, gmsgInitHUD, NULL, pev );
			MESSAGE_END();

			EntsSendClientData();// XDM

			g_pGameRules->InitHUD( this );
			m_fGameHUDInitialized = TRUE;
			if ( g_pGameRules->IsMultiplayer() )
			{
				FireTargets( "game_playerjoin", this, this, USE_TOGGLE, 0 );
			}
		}

		FireTargets( "game_playerspawn", this, this, USE_TOGGLE, 0 );

		InitStatusBar();
	}

	if ( m_iHideHUD != m_iClientHideHUD )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgHideWeapon, NULL, pev );
		WRITE_BYTE( m_iHideHUD );
		MESSAGE_END();

		m_iClientHideHUD = m_iHideHUD;
	}

	if ( m_iFOV != m_iClientFOV )
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pev);
		WRITE_BYTE(MSG_FOV);
		WRITE_BYTE(m_iFOV);
		WRITE_ELSE(1+1);
		MESSAGE_END();
	}

	if (gDisplayTitle)
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pev);
		WRITE_BYTE( MSG_GAMETITLE );
		WRITE_BYTE( 1 );
		WRITE_ELSE(1+1);
		MESSAGE_END();
		gDisplayTitle = 0;
	}


// Portable HEV - armor regeneration
if (m_fPortableHEV && pev->armorvalue < MAX_PHEV_BATTERY)
	{
	armorregencounter++;

	if (armorregencounter == 20)
	{
	armorregencounter = 0;

	pev->armorvalue += 1;
	if (pev->armorvalue > MAX_PHEV_BATTERY)
	pev->armorvalue = MAX_PHEV_BATTERY;
	}
}

if (m_fPortableHEV && m_fPowerShield && pev->armorvalue < MAX_SHIELD_PHEV_BATTERY )
{
	armorregencounterPS++;

	if (armorregencounterPS == 30)
	{
		armorregencounterPS = 0;
		pev->armorvalue += 1;
		if (pev->armorvalue > MAX_SHIELD_PHEV_BATTERY)
		pev->armorvalue = MAX_SHIELD_PHEV_BATTERY;
	}
}

if (m_fKevlar && m_iKevlarBattery <= 0)
{
	m_fKevlar = FALSE; 
	ClientPrint(pev, HUD_PRINTCENTER, UTIL_VarArgs("Your Kevlar Vest has been destroyed!\n"));
}

if (m_fHeavyArmor && m_iHarmorBattery <= 0)
{
	FX_Explosion( pev->origin, EXPLOSION_TURRET );
	g_engfuncs.pfnSetPhysicsKeyValue(edict(), "harm", "0" );
	g_engfuncs.pfnSetClientKeyValue(entindex(), g_engfuncs.pfnGetInfoKeyBuffer( edict() ), "model", oldmodel);
	m_fHeavyArmor = FALSE; 

	//remove Machinegun, if player has this weapon
	RemoveNamedPlayerItem("weapon_machinegun");
}

if (m_fPowerShield && !m_fPortableHEV && !pev->armorvalue)
{
	Create( "pshield_det", pev->origin, pev->origin, edict());
	ClientPrint(pev, HUD_PRINTCENTER, UTIL_VarArgs("Your Power Shield has been destroyed!\n"));
	m_fPowerShield = FALSE; 
}

//Money system
	if (m_flMoneyAmount > MAX_MONEY)
		m_flMoneyAmount = MAX_MONEY;
	else if (m_flMoneyAmount < 0)
		m_flMoneyAmount = 0;

// Portable HEALTHKIT - health regeneration
if (m_fPortableHealthkit && pev->health < 150)
	{
	regencounter++;

	if (regencounter == 20)
	{
	regencounter = 0;

	pev->health += 1;
	if (pev->health > 150)
	pev->health = 150;
	}
}

// item icons update messages START

	if (m_flMoneyAmount != m_iClientMoney)
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pev);
		WRITE_BYTE( MSG_MONEY );
		WRITE_LONG((int)m_flMoneyAmount);
		WRITE_ELSE(1+4);
		MESSAGE_END();
		m_iClientMoney = m_flMoneyAmount;
	}

	if (m_iInBuyZone != m_iClientBuyZone)
	{
		m_iClientBuyZone = m_iInBuyZone;
		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pev);
		WRITE_BYTE( MSG_BUYZONE );
		WRITE_BYTE(m_iInBuyZone);
		WRITE_ELSE( 1+1 );
		MESSAGE_END();
	}

	if (m_fCloak != m_iClientCloak)
	{
		m_iClientCloak = m_fCloak;
		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pev);
		WRITE_BYTE( MSG_CLOAK );
		WRITE_BYTE(m_fCloak);
		WRITE_ELSE(1+1);
		MESSAGE_END();
	}

	if (m_fPowerShield != m_iClientPShield)
	{
		m_iClientPShield = m_fPowerShield;
		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pev);
		WRITE_BYTE( MSG_PSHIELD );
		WRITE_BYTE(m_fPowerShield);
		WRITE_ELSE(1+1);
		MESSAGE_END();
	}

	if (m_fLongJump != m_iClientLongjump)
	{
		m_iClientLongjump = m_fLongJump;
		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pev);
		WRITE_BYTE( MSG_LONGJUMP );
		WRITE_BYTE(m_fLongJump);
		WRITE_ELSE(1+1);
		MESSAGE_END();
	}
	if (m_fpt != m_iClientPt)
	{
		m_iClientPt = m_fpt;
		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pev);
		WRITE_BYTE( MSG_PT );
		WRITE_BYTE(m_fpt);
		WRITE_ELSE(1+1);
		MESSAGE_END();
	}

	if (m_fPortableHEV != m_iClientPHEV)
	{
		m_iClientPHEV = m_fPortableHEV;
		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pev);
		WRITE_BYTE( MSG_PHEV );
		WRITE_BYTE(m_fPortableHEV);
		WRITE_ELSE(1+1);
		MESSAGE_END();
	}

	if (m_fPortableHealthkit != m_iClientPhealth)
	{
		m_iClientPhealth = m_fPortableHealthkit;
		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pev);
		WRITE_BYTE( MSG_PHK );
		WRITE_BYTE(m_fPortableHealthkit);
		WRITE_ELSE(1+1);
		MESSAGE_END();
	}

	if (m_fAntigrav != m_iClientAntigrav)
	{
		m_iClientAntigrav = m_fAntigrav;
		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pev);
		WRITE_BYTE( MSG_ANTIGRAV );
		WRITE_BYTE(m_fAntigrav);
		WRITE_ELSE(1+1);
		MESSAGE_END();
	}

	if (m_fTurretSentry != m_iClientTurretS)
	{
		m_iClientTurretS = m_fTurretSentry;
		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pev);
		WRITE_BYTE( MSG_TURRETSENTRY );
		WRITE_BYTE(m_fTurretSentry);
		WRITE_ELSE(1+1);
		MESSAGE_END();
	}
	if (m_fTurretMissile != m_iClientTurretM)
	{
		m_iClientTurretM = m_fTurretMissile;
		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pev);
		WRITE_BYTE( MSG_TURRETMISSILE );
		WRITE_BYTE(m_fTurretMissile);
		WRITE_ELSE(1+1);
		MESSAGE_END();
	}
	if (m_fTurretIon != m_iClientTurretI)
	{
		m_iClientTurretI = m_fTurretIon;
		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pev);
		WRITE_BYTE( MSG_TURRETION );
		WRITE_BYTE(m_fTurretIon);
		WRITE_ELSE(1+1);
		MESSAGE_END();
	}

// item icons update messages END

	if (pev->health != m_iClientHealth)
	{
		m_iClientHealth = pev->health;
		int iHealth = max( pev->health, 0 );  // make sure that no negative health values are sent
		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pev);
		WRITE_BYTE( MSG_HEALTH );
		WRITE_BYTE( iHealth );
		WRITE_ELSE(1+1);
		MESSAGE_END();
	}

	if (pev->armorvalue != m_iClientBattery)
	{
		m_iClientBattery = pev->armorvalue;
		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pev);
		WRITE_BYTE( MSG_BATTERY );
		WRITE_SHORT( (int)pev->armorvalue);
		WRITE_ELSE(1+2);
		MESSAGE_END();
	}

	if (m_iKevlarBattery != m_iClientKevlar)
	{
		m_iClientKevlar = m_iKevlarBattery;

		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pev);
		WRITE_BYTE( MSG_KEVLAR );
		WRITE_BYTE(m_iKevlarBattery);
		WRITE_ELSE(1+1);
		MESSAGE_END();
	}

	if (m_iHarmorBattery != m_iClientHarmor)
	{
		m_iClientHarmor = m_iHarmorBattery;

		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pev);
		WRITE_BYTE( MSG_HARMOR );
		WRITE_SHORT(m_iHarmorBattery);
		WRITE_ELSE(1+2);
		MESSAGE_END();
	}

	if (pev->dmg_take || pev->dmg_save || m_bitsHUDDamage != m_bitsDamageType)
	{
		// Comes from inside me if not set
		Vector damageOrigin = pev->origin;
		// send "damage" message
		// causes screen to flash, and pain compass to show direction of damage
		edict_t *other = pev->dmg_inflictor;
		if ( other )
		{
			CBaseEntity *pEntity = CBaseEntity::Instance(other);
			if ( pEntity )
				damageOrigin = pEntity->Center();
		}

		// only send down damage type that have hud art
		int visibleDamageBits = m_bitsDamageType & DMG_SHOWNHUD;

		MESSAGE_BEGIN( MSG_ONE, gmsgDamage, NULL, pev );
			WRITE_BYTE( pev->dmg_save );
			WRITE_BYTE( pev->dmg_take );
			WRITE_LONG( visibleDamageBits );
			WRITE_COORD( damageOrigin.x );
			WRITE_COORD( damageOrigin.y );
			WRITE_COORD( damageOrigin.z );
		MESSAGE_END();
	
		pev->dmg_take = 0;
		pev->dmg_save = 0;
		m_bitsHUDDamage = m_bitsDamageType;
		
		// Clear off non-time-based damage indicators
		m_bitsDamageType &= DMG_TIMEBASED;
	}

	// Update Flashlight
	if ((m_flFlashLightTime) && (m_flFlashLightTime <= gpGlobals->time))
	{
		if (FlashlightIsOn())
		{
			if (m_iFlashBattery)
			{
				m_flFlashLightTime = FLASH_DRAIN_TIME + gpGlobals->time;
				m_iFlashBattery--;
				
				if (m_iFlashBattery <= 0)
					FlashlightTurnOff();
			}
		}
		else
		{
			m_flFlashLightTime = 0;
		}
		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pev);
		WRITE_BYTE(MSG_FLASHBATTERY);
		WRITE_BYTE(m_iFlashBattery);
		WRITE_ELSE(1+1);
		MESSAGE_END();
	}


	if (m_iTrain & TRAIN_NEW)
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pev);
		WRITE_BYTE( MSG_TRAIN );
		WRITE_BYTE(m_iTrain & 0xF);
		WRITE_ELSE(1+1);
		MESSAGE_END();

		m_iTrain &= ~TRAIN_NEW;
	}

	//
	// New Weapon?
	//
	if (!m_fKnownItem)
	{
		m_fKnownItem = TRUE;

		// Send ALL the weapon info now
		int i;

		for (i = 0; i < MAX_WEAPONS; i++)
		{
			ItemInfo& II = CBasePlayerItem::ItemInfoArray[i];

			if ( !II.iId )
				continue;

			const char *pszName;
			if (!II.pszName)
				pszName = "Empty";
			else
				pszName = II.pszName;

			MESSAGE_BEGIN( MSG_ONE, gmsgWeaponList, NULL, pev );  
				WRITE_STRING(pszName);			// string	weapon name
				WRITE_BYTE(GetAmmoIndex(II.pszAmmo1));	// byte		Ammo Type
				WRITE_BYTE(II.iMaxAmmo1);				// byte     Max Ammo 1
				WRITE_BYTE(GetAmmoIndex(II.pszAmmo2));	// byte		Ammo2 Type
				WRITE_BYTE(II.iMaxAmmo2);				// byte     Max Ammo 2
				WRITE_BYTE(II.iSlot);					// byte		bucket
				WRITE_BYTE(II.iPosition);				// byte		bucket pos
				WRITE_BYTE(II.iId);						// byte		id (bit index into pev->weapons)
				WRITE_BYTE(II.iFlags);					// byte		Flags
			MESSAGE_END();
		}
	}


	SendAmmoUpdate();

	// buz: update spec machinegun state
	if (m_pSpecTank)
		m_pSpecTank->Use( this, this, USE_SET, 3 ); // update client data

	// Update all the items
	for ( int i = 0; i < MAX_ITEM_TYPES; i++ )
	{
		if ( m_rgpPlayerItems[i] )  // each item updates it's successors
			m_rgpPlayerItems[i]->UpdateClientData( this );
	}

	// Cache and client weapon change
	m_pClientActiveItem = m_pActiveItem;
	m_iClientFOV = m_iFOV;

	// Update Status Bar
	if ( m_flNextSBarUpdateTime < gpGlobals->time )
	{
		UpdateStatusBar();
		m_flNextSBarUpdateTime = gpGlobals->time + 0.2;
	}
}

void CBasePlayer :: EnableControl(BOOL fControl)
{
	if (!fControl)
		pev->flags |= FL_FROZEN;
	else
		pev->flags &= ~FL_FROZEN;

}

/*
=============
SetCustomDecalFrames

  UNDONE:  Determine real frame limit, 8 is a placeholder.
  Note:  -1 means no custom frames present.
=============
*/
void CBasePlayer :: SetCustomDecalFrames( int nFrames )
{
	if (nFrames > 0 &&
		nFrames < 8)
		m_nCustomSprayFrames = nFrames;
	else
		m_nCustomSprayFrames = -1;
}

/*
=============
GetCustomDecalFrames

  Returns the # of custom frames this player's custom clan logo contains.
=============
*/
int CBasePlayer :: GetCustomDecalFrames( void )
{
	return m_nCustomSprayFrames;
}


//=========================================================
// DropPlayerItem - drop the named item, or if no name,
// the active item. 
//=========================================================
void CBasePlayer::DropPlayerItem ( char *pszItemName )
{
	if ( !g_pGameRules->IsMultiplayer() || (mp_weaponstay.value > 0) )
	{
		// no dropping in single player.
		return;
	}

	if ( !strlen( pszItemName ) )
	{
		// if this string has no length, the client didn't type a name!
		// assume player wants to drop the active item.
		// make the string null to make future operations in this function easier
		pszItemName = NULL;
	} 

	if ( g_pGameRules->DeadPlayerWeapons( this ) == GR_PLR_DROP_GUN_NO )
		return;

	CBasePlayerItem *pWeapon;
	int i;

	pev->viewmodel = 0;
	pev->weaponmodel = 0;
	if (m_fHeavyArmor)
	pev->maxspeed = HARMOR_MAXSPEED;
	else
	pev->maxspeed = NORMAL_MAXSPEED;

	for ( i = 0 ; i < MAX_ITEM_TYPES ; i++ )
	{
		pWeapon = m_rgpPlayerItems[ i ];

		while ( pWeapon )
		{
			if ( pszItemName )
			{
				// try to match by name. 
				if ( !strcmp( pszItemName, STRING( pWeapon->pev->classname ) ) )
				{
					// match! 
					break;
				}
			}
			else
			{
				// trying to drop active item
				if ( pWeapon == m_pActiveItem )
				{
					// active item!
					break;
				}
			}
			pWeapon = pWeapon->m_pNext; 
		}

		
		// if we land here with a valid pWeapon pointer, that's because we found the 
		// item we want to drop and hit a BREAK;  pWeapon is the item.
		if ( pWeapon )
		{
			g_pGameRules->GetNextBestWeapon( this, pWeapon );
			UTIL_MakeVectors ( pev->angles ); 

			if (pWeapon->m_iId < 32)
				pev->weapons &= ~(1<<pWeapon->m_iId);// take item off hud
			else
				m_iWeapons2 &= ~(1<<(pWeapon->m_iId - 32));// take item off hud

			UTIL_MakeVectors ( pev->angles );
			CWeaponBox *pWeaponBox = (CWeaponBox *)CBaseEntity::Create( "weaponbox", pev->origin + gpGlobals->v_forward * 10, pev->angles, edict() );
			pWeaponBox->pev->angles.x = 0;
			pWeaponBox->pev->angles.z = 0;
			pWeaponBox->PackWeapon( pWeapon );
			pWeaponBox->pev->velocity = gpGlobals->v_forward * 200;
			
			// drop all ammo for this weapon.
			int	iAmmoIndex, iAmmoIndex2;

			iAmmoIndex = GetAmmoIndex (pWeapon->pszAmmo1());
			iAmmoIndex2 = GetAmmoIndex (pWeapon->pszAmmo2());
			
			if (iAmmoIndex != -1)
			{
					// pack up all the ammo, this weapon is its own ammo type
					pWeaponBox->PackAmmo( MAKE_STRING(pWeapon->pszAmmo1()), m_rgAmmo[ iAmmoIndex ] );
					m_rgAmmo[ iAmmoIndex ] = 0; 
			}
			if (iAmmoIndex2 != -1)
			{
					pWeaponBox->PackAmmo( MAKE_STRING(pWeapon->pszAmmo2()), m_rgAmmo[ iAmmoIndex2 ] );
					m_rgAmmo[ iAmmoIndex2 ] = 0; 
			}

			return;// we're done, so stop searching with the FOR loop.
		}
	}
}

//SELL WEAPONS!
void CBasePlayer::SellCurrentWeapon ( )
{
	CBasePlayerItem *pWeapon = m_pActiveItem;

	if ( pWeapon != NULL)
	{
	pev->viewmodel = 0;
	pev->weaponmodel = 0;

	if (m_fHeavyArmor)
		pev->maxspeed = HARMOR_MAXSPEED;
	else
		pev->maxspeed = NORMAL_MAXSPEED;

	CBasePlayerItem *pGun = m_pActiveItem->GetWeaponPtr();
	    if (pGun)
	        ((CBasePlayerWeapon*)pGun)->SellWeapon();

		EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/money_pickup.wav", 1, ATTN_NORM);

		g_pGameRules->GetNextBestWeapon( this, pWeapon );

		if (pWeapon->m_iId < 32)
			pev->weapons &= ~(1<<pWeapon->m_iId);// take item off hud
		else
			m_iWeapons2 &= ~(1<<(pWeapon->m_iId - 32));// take item off hud

		pGun->SetThink( CBasePlayerItem::DestroyItem );
		pGun->pev->nextthink = gpGlobals->time;
	}
}

//=========================================================
// HasPlayerItem Does the player already have this item?
//=========================================================
BOOL CBasePlayer::HasPlayerItem( CBasePlayerItem *pCheckItem )
{
	CBasePlayerItem *pItem = m_rgpPlayerItems[pCheckItem->iItemSlot()];

	while (pItem)
	{
		if (FClassnameIs( pItem->pev, STRING( pCheckItem->pev->classname) ))
		{
			return TRUE;
		}
		pItem = pItem->m_pNext;
	}

	return FALSE;
}

//=========================================================
// HasNamedPlayerItem Does the player already have this item?
//=========================================================
BOOL CBasePlayer::HasNamedPlayerItem( const char *pszItemName )
{
	CBasePlayerItem *pItem;
	int i;
 
	for ( i = 0 ; i < MAX_ITEM_TYPES ; i++ )
	{
		pItem = m_rgpPlayerItems[ i ];
		
		while (pItem)
		{
			if ( !strcmp( pszItemName, STRING( pItem->pev->classname ) ) )
			{
				return TRUE;
			}
			pItem = pItem->m_pNext;
		}
	}

	return FALSE;
}

//=========================================================
// 
//=========================================================
BOOL CBasePlayer :: SwitchWeapon( CBasePlayerItem *pWeapon ) 
{
	if (!pWeapon->CanDeploy())
		return FALSE;

	if (IsOnLadder())
		return false;

	if (m_pActiveItem)
		m_pActiveItem->Holster();

	QueueItem(pWeapon);

	if (m_pActiveItem)// XWider: QueueItem sets it if we have no current weapon
	{
		m_pActiveItem->Deploy( );
		m_pActiveItem->UpdateItemInfo( );
	}
	return TRUE;
}


void CBasePlayer::CloakToggle(BOOL activate)
{
	if (!m_fCloak)
		return;

	if (activate && !m_fCloakActivated)
	{
		m_fCloakActivated = TRUE;
		EMIT_SOUND_DYN ( ENT(pev), CHAN_STATIC, "player/pl_cloak_noise.wav", 1, ATTN_NORM, 0, 100 );
		pev->renderfx = kRenderFxGlowShell;
		pev->renderamt = 5;
	}
	else if (!activate && m_fCloakActivated)
	{
		m_fCloakActivated = FALSE;
		STOP_SOUND(ENT(pev), CHAN_STATIC, "player/pl_cloak_noise.wav" );
		EMIT_SOUND_DYN ( ENT(pev), CHAN_VOICE, "player/pl_cloak_deactivate.wav", 1, ATTN_NORM, 0, 100 );
		pev->renderfx &= ~kRenderFxGlowShell;
		pev->renderamt = 1;
	}
}

void CBasePlayer::CloakUpdate()
{
	if (m_fCloakActivated)
	{
		CloakDrainCounter++;
		if (CloakDrainCounter == 20)
		{
			CloakDrainCounter = 0;
			m_iFlashBattery--;

			MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pev);
			WRITE_BYTE(MSG_FLASHLIGHT);
			WRITE_BYTE(1);
			WRITE_BYTE(m_iFlashBattery);
			WRITE_ELSE(1+2);
			MESSAGE_END();
		}

		if (m_iFlashBattery <= 0)
		CloakToggle(FALSE);

		if (pev->flags & FL_FROZEN)
			CloakToggle(FALSE);
	}
}

void CBasePlayer::AntigravToggle(BOOL activate)
{
	if (!m_fAntigrav)
	return;

	if (activate && !m_fAntigravActivated)
	{
		m_fAntigravActivated = TRUE;
		pev->gravity = 1;
		pev->gravity *= 0.1;
		EMIT_SOUND(ENT(pev), CHAN_STATIC, "player/pl_antigrav_fly.wav", 0.8, ATTN_NORM );
	}
	else if (!activate && m_fAntigravActivated)
	{
		m_fAntigravActivated = FALSE;
		STOP_SOUND(ENT(pev), CHAN_STATIC, "player/pl_antigrav_fly.wav" );
		EMIT_SOUND_DYN ( ENT(pev), CHAN_VOICE, "player/pl_antigrav_deactivate.wav", 0.8, ATTN_NORM, 0, 100 );
		pev->gravity = 1;
		pev->gravity *= 1;
	}
}

void CBasePlayer::AntigravUpdate()
{
	if (m_fAntigravActivated)
	{
		AntigravDrainCounter++;
		if (AntigravDrainCounter == 35)
		{
			AntigravDrainCounter = 0;
			m_iFlashBattery--;

			MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pev);
			WRITE_BYTE(MSG_FLASHLIGHT);
			WRITE_BYTE(1);
			WRITE_BYTE(m_iFlashBattery);
			WRITE_ELSE(1+2);
			MESSAGE_END();
		}

		if (m_iFlashBattery <= 0)
			AntigravToggle(FALSE);

		if (pev->flags & FL_FROZEN)
			AntigravToggle(FALSE);
	}
}

class CRevertSaved : public CPointEntity
{
public:
	void	RealUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void	EXPORT MessageThink( void );
	void	EXPORT LoadThink( void );
	void	KeyValue( KeyValueData *pkvd );

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	inline	float	Duration( void ) { return pev->dmg_take; }
	inline	float	HoldTime( void ) { return pev->dmg_save; }
	inline	float	MessageTime( void ) { return m_messageTime; }
	inline	float	LoadTime( void ) { return m_loadTime; }

	inline	void	SetDuration( float duration ) { pev->dmg_take = duration; }
	inline	void	SetHoldTime( float hold ) { pev->dmg_save = hold; }
	inline	void	SetMessageTime( float time ) { m_messageTime = time; }
	inline	void	SetLoadTime( float time ) { m_loadTime = time; }

private:
	float	m_messageTime;
	float	m_loadTime;
};

LINK_ENTITY_TO_CLASS( player_loadsaved, CRevertSaved );

TYPEDESCRIPTION	CRevertSaved::m_SaveData[] = 
{
	DEFINE_FIELD( CRevertSaved, m_messageTime, FIELD_FLOAT ),	// These are not actual times, but durations, so save as floats
	DEFINE_FIELD( CRevertSaved, m_loadTime, FIELD_FLOAT ),
};

IMPLEMENT_SAVERESTORE( CRevertSaved, CPointEntity );

void CRevertSaved :: KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "duration"))
	{
		SetDuration( atof(pkvd->szValue) );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "holdtime"))
	{
		SetHoldTime( atof(pkvd->szValue) );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "messagetime"))
	{
		SetMessageTime( atof(pkvd->szValue) );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "loadtime"))
	{
		SetLoadTime( atof(pkvd->szValue) );
		pkvd->fHandled = TRUE;
	}
	else 
		CPointEntity::KeyValue( pkvd );
}

void CRevertSaved :: RealUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	UTIL_ScreenFadeAll( pev->rendercolor, Duration(), HoldTime(), pev->renderamt, FFADE_OUT );
	pev->nextthink = gpGlobals->time + MessageTime();
	SetThink( MessageThink );
}


void CRevertSaved :: MessageThink( void )
{
	UTIL_ShowMessageAll( STRING(pev->message) );
	float nextThink = LoadTime() - MessageTime();
	if ( nextThink > 0 ) 
	{
		pev->nextthink = gpGlobals->time + nextThink;
		SetThink( LoadThink );
	}
	else
		LoadThink();
}


void CRevertSaved :: LoadThink( void )
{
	if ( !gpGlobals->deathmatch )
	{
		SERVER_COMMAND("reload\n");
	}
}


//=========================================================
// Multiplayer intermission spots.
//=========================================================
class CInfoIntermission:public CPointEntity
{
	void Spawn( void );
	void Think( void );
};

void CInfoIntermission::Spawn( void )
{
	UTIL_SetOrigin( pev, pev->origin );
	pev->solid = SOLID_NOT;
	pev->effects = EF_NODRAW;
	pev->v_angle = g_vecZero;

	pev->nextthink = gpGlobals->time + 2;// let targets spawn!

}

void CInfoIntermission::Think ( void )
{
	edict_t *pTarget;

	// find my target
	pTarget = FIND_ENTITY_BY_TARGETNAME( NULL, STRING(pev->target) );

	if ( !FNullEnt(pTarget) )
	{
		pev->v_angle = UTIL_VecToAngles( (pTarget->v.origin - pev->origin).Normalize() );
		pev->v_angle.x = -pev->v_angle.x;
	}
}

LINK_ENTITY_TO_CLASS( info_intermission, CInfoIntermission );

// Money system
void CBasePlayer::AddMoney(int Money)
{
	if ( mp_nobuy.value > 0 )
	return;
	m_flMoneyAmount += Money;
}

void CBasePlayer::NoMoney(void)
{
	ClientPrint(pev, HUD_PRINTCENTER, UTIL_VarArgs("Not enough money!"));
}

void CBasePlayer::HaveItem(void)
{
	ClientPrint(pev, HUD_PRINTCENTER, UTIL_VarArgs("You already have this ITEM!"));
}

void CBasePlayer::HaveWeapon(void)
{
	ClientPrint(pev, HUD_PRINTCENTER, UTIL_VarArgs("You already have this WEAPON!"));
}

///////////////////////////////////////////////
//Does something *interesting* with player!!!//
///////////////////////////////////////////////
void CBasePlayer::FrozenStart(void)
{
	m_fFrozen = TRUE;
	pev->renderfx = kRenderFxGlowShell;
	pev->rendercolor = Vector(0,90,250);
	pev->flags |= FL_FROZEN;

	Create( "fx_spawner_freeze", pev->origin, pev->origin, edict());

	if (m_pActiveItem != NULL)
	{
	CBasePlayerItem *pGun = m_pActiveItem->GetWeaponPtr();
		if (pGun)
			((CBasePlayerWeapon*)pGun)->m_flNextPrimaryAttack = ((CBasePlayerWeapon*)pGun)->m_flNextSecondaryAttack = ((CBasePlayerWeapon*)pGun)->m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + FREEZE_DURATION;
	}
}

void CBasePlayer::FrozenEnd(void)
{
	if (!m_fFrozen)
	return;

	m_fFrozen = FALSE;
	m_bitsDamageType &= ~(DMG_FREEZE);	
	pev->renderfx = 0;
	pev->rendercolor = g_vecZero;
	pev->rendermode = kRenderNormal;
	pev->flags &= ~FL_FROZEN;

	UTIL_ScreenFade(this, pev->rendercolor, 0, 0, 255, FFADE_IN);
	FX_Trail(pev->origin, entindex(), PROJ_REMOVE);

	if (m_pActiveItem != NULL)
	{
	CBasePlayerItem *pGun = m_pActiveItem->GetWeaponPtr();
		if (pGun)
			((CBasePlayerWeapon*)pGun)->m_flNextPrimaryAttack = ((CBasePlayerWeapon*)pGun)->m_flNextSecondaryAttack = ((CBasePlayerWeapon*)pGun)->m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.1;
	}
}

void CBasePlayer::IgniteStart(void)
{
	if (!m_fIgnite)
	{
		Create( "fx_spawner_burn", pev->origin, pev->origin, edict());
		m_fLastIgnite = m_fIgnite;
	}

	m_fIgnite = TRUE;
	EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/pl_burn.wav", 0.8, ATTN_NORM);

	if (!IsAlive())
	{
		pev->body = 5;
		TraceResult tr;
		UTIL_TraceLine( pev->origin, pev->origin + Vector(0,0,-80), ignore_monsters, edict(), &tr );
		UTIL_DecalTrace(&tr, DECAL_SCORCH1 + RANDOM_LONG(0,2));
	return;
	}
	int pitch = 90 + RANDOM_LONG(0,20);
	if ( gpGlobals->time >= m_flScreamTime )
	{
		switch ( RANDOM_LONG(0,1))
		{
			case 0:	EMIT_SOUND_DYN (ENT(pev), CHAN_VOICE, "player/pl_burn_pain1.wav", 1.0, ATTN_NORM, 0, pitch);	break;
			case 1:	EMIT_SOUND_DYN (ENT(pev), CHAN_VOICE, "player/pl_burn_pain2.wav", 1.0, ATTN_NORM, 0, pitch);	break;
		}
		m_flScreamTime = gpGlobals->time + 2;
	}
}
void CBasePlayer::IgniteEnd(void)
{
 	if (m_fLastIgnite != m_fIgnite)
	{
		m_fIgnite=FALSE;
		m_fLastIgnite = m_fIgnite;
		m_bitsDamageType &= ~(DMG_IGNITE);	
		STOP_SOUND(ENT(pev), CHAN_VOICE, "player/pl_burn.wav" );
	}
} 