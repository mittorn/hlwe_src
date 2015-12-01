#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"monsters.h"
#include	"schedule.h"
#include	"weapons.h"
#include	"mon_projectiles.h"
#include	"soundent.h"

#define	ZOMBIE_AE_ATTACK_RIGHT		0x01
#define	ZOMBIE_AE_ATTACK_LEFT		0x02
#define	ZOMBIE_AE_ATTACK_BOTH		0x03
#define	ZOMBIE_AE_ATTACK_RANGE		0x04

#define ZOMBIE_FLINCH_DELAY		2// at most one flinch every n secs

#define DAMAGE_NO			0
#define DAMAGE_BLOW_HEAD		1
#define DAMAGE_DESTROY_FLESH		2

#define BODYGROUP_BODY			0
#define BODYGROUP_HEAD			1

#define SUBMDL_HEAD_NORM		0
#define SUBMDL_HEAD_BLOWN		1
#define SUBMDL_HEAD_NO			2

#define SUBMDL_BODY_NORMAL		0
#define SUBMDL_BODY_SKELETON		1
#define SUBMDL_BODY_SKELETON_NOSKULL	2

class CZombie : public CBaseMonster
{
public:

	void Spawn( void );
	void RealReSpawn( void );
	void Precache( void );
	void SetYawSpeed( void );
	int  Classify ( void );
	void HandleAnimEvent( MonsterEvent_t *pEvent );
	int IgnoreConditions ( void );

	float m_flNextFlinch;
	float m_flNextSpitTime;
	float m_flNextPain;

	void PainSound( void );
	void AlertSound( void );
	void IdleSound( void );
	void AttackSound( void );
	void AttackMissSound( void );
	void AttackHitSound( void );

	BOOL CheckRangeAttack1 ( float flDot, float flDist );
	int RealTakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType );
	void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);

	//LLAPb AI start:
	void KeyValue( KeyValueData *pkvd );

	void MonsterThink( void );
	void StartTask ( Task_t *pTask );
	void RunTask ( Task_t *pTask );

	Schedule_t *GetSchedule( void );
	bool MonsterRespawnable() {return true;};

private:

	char *BuildRandomPath (const char **list);
	char *BuildPathByOffs (const char **list, int off);

	void FrozenStart();
	void FrozenEnd();

	void IgniteStart();
	void IgniteEnd();

	string_t m_iPath;
	float m_fNextIgnite;
	float m_fEndfrozen;
	int m_iIgniteCounter;
	int m_iInjuryType;
};
LINK_ENTITY_TO_CLASS( monster_zombie, CZombie );

extern StringBuf pool;

void CZombie::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "base_path"))
	{
		m_iPath = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseMonster::KeyValue( pkvd );
}

char* CZombie::BuildRandomPath (const char **list)
{
	static char buf[MAX_PATH];

	int len = strlen(STRING(m_iPath));
	int off = RANDOM_LONG(0, ARRAYSIZE(list) - 1);

	strncpy(buf, STRING(m_iPath), len);
	buf[len] = '/';
	strcpy(buf + len + 1, list[off]);

	return (char*)pool.ALLOC_MEM(buf);
}


char* CZombie::BuildPathByOffs (const char **list, int off)
{
	static char buf[MAX_PATH];

	int len = strlen(STRING(m_iPath));

	strncpy(buf, STRING(m_iPath), len);
	buf[len] = '/';
	strcpy(buf + len + 1, list[off]);

	return (char*)pool.ALLOC_MEM(buf);
}


Task_t tlDeadAwakening[] =
{
	{ TASK_DIE,			(float)0	},
	{ TASK_WAIT_RANDOM,		(float)20	},	//Wait for [0.1; 20] seconds
	{ TASK_CHECK_FOR_NOBODY_AROUND,	(float)0	},	//Can awake only if nobody standing on it's body to avoid hulls intersection
	{ TASK_AWAKE_FROM_DEAD,		(float)0	},	//Rais from the death :)
};

Schedule_t slDeadAwakening[] =
{
	{
		tlDeadAwakening,
		ARRAYSIZE( tlDeadAwakening ),
		0,											//No interruptions
		0,
		"Dead Awakening"
	}
};

void CZombie::MonsterThink( void )
{
	pev->nextthink = gpGlobals->time + 0.1;

	if (m_fNextIgnite && gpGlobals->time >= m_fNextIgnite)
	{
		if (m_iIgniteCounter >= 7)//burn for 7 seconds
		{
			IgniteEnd();
		}
		else
		{
			RealTakeDamage(pev, pev, 10, DMG_GENERIC|DMG_NEVERGIB);
			m_iIgniteCounter++;
			m_fNextIgnite = gpGlobals->time + 1;
		}

		if (pev->health <= 0)
		{
			SetBodygroup(BODYGROUP_HEAD, SUBMDL_HEAD_NO);

			if (m_iInjuryType & DAMAGE_BLOW_HEAD)
				SetBodygroup(BODYGROUP_BODY, SUBMDL_BODY_SKELETON_NOSKULL);
			else
				SetBodygroup(BODYGROUP_BODY, SUBMDL_BODY_SKELETON);

			m_iInjuryType |= DAMAGE_DESTROY_FLESH;
		}
	}

	if (!m_fEndfrozen)
		CBaseMonster::MonsterThink();//Do not run AI
	else if (gpGlobals->time >= m_fEndfrozen)
		FrozenEnd();
}

Schedule_t *CZombie :: GetSchedule ( void )
{
	switch	( m_MonsterState )
	{
	case MONSTERSTATE_DEAD:
		{
			return &slDeadAwakening[0];
		}
	}
	return CBaseMonster::GetSchedule();
}

void CZombie :: RunTask ( Task_t *pTask )
{
	switch ( pTask->iTask )
	{
	case TASK_CHECK_FOR_NOBODY_AROUND:
		{
			CBaseEntity *pEnt = NULL;

			while (pEnt = UTIL_FindEntityInSphere(pEnt, pev->origin + Vector(0, 0, 20), 18))
			{
				if (pEnt->edict() == edict()) continue;

				if (!pEnt->IsAlive()) continue;

				if (pEnt->pev->solid == SOLID_NOT || pEnt->pev->solid == SOLID_TRIGGER) continue;

				return;
			}

			TaskComplete();
		}
		break;

	case TASK_AWAKE_FROM_DEAD:
		{
			if ( pev->frame >= 255 )
			{
				ReSpawn();
				TaskComplete();
			}
		}
		break;

	default:
		CBaseMonster::RunTask(pTask);
		break;
	}
}

void CZombie :: StartTask ( Task_t *pTask )
{
	switch ( pTask->iTask )
	{
	case TASK_AWAKE_FROM_DEAD:
		{
			switch (RANDOM_LONG(0, 1))
			{
				case 0: EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "zombie/zo_wakeup1.wav", 1, ATTN_NORM, 0, 95 + RANDOM_LONG(0,9)); break;
				case 1: EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "zombie/zo_wakeup2.wav", 1, ATTN_NORM, 0, 95 + RANDOM_LONG(0,9)); break;
			}

			switch (m_Activity)
			{
			case ACT_DIESIMPLE:
			case ACT_DIEFORWARD:
				pev->sequence = LookupSequence("getup");
				break;

			default:
				pev->sequence = LookupSequence("getup2");
				break;
			}

			if (m_iInjuryType & DAMAGE_BLOW_HEAD)
				UTIL_SetSize( pev, VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX - Vector(0,0,10) );
			else
				UTIL_SetSize( pev, VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX );

			pev->solid = SOLID_SLIDEBOX;
			pev->frame = 0;
			ResetSequenceInfo( );
			break;
		}

	default:
		CBaseMonster::StartTask(pTask);
		break;
	}
}

BOOL CZombie :: CheckRangeAttack1 ( float flDot, float flDist )
{
	if ( IsMoving() && flDist >= 712 )
		return FALSE;

	if (m_iInjuryType & DAMAGE_DESTROY_FLESH)
		return FALSE;

	if ( flDist > 100 && flDist <= 984 && flDot >= 0.5 && gpGlobals->time >= m_flNextSpitTime )
	{
		if ( IsMoving() )
		{
			// don't spit again for a long time, resume chasing enemy.
			m_flNextSpitTime = gpGlobals->time + 5;
		}
		else
		{
			// not moving, so spit again pretty soon.
			m_flNextSpitTime = gpGlobals->time + 0.5;
		}
		return TRUE;
	}
	return FALSE;
}

int	CZombie :: Classify ( void )
{
	return	CLASS_DEAD;
}

void CZombie :: SetYawSpeed ( void )
{
	pev->yaw_speed = 150;
}

void CZombie :: TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType)
{
	if (!pev->takedamage) return;

	if (ptr->iHitgroup == HITGROUP_HEAD && !(m_iInjuryType & DAMAGE_BLOW_HEAD) && !m_fEndfrozen)
	{
		flDamage *= 2;

		if (flDamage >= pev->health)
		{
			if (m_iInjuryType & DAMAGE_DESTROY_FLESH)
			{
				SetBodygroup(BODYGROUP_HEAD, SUBMDL_HEAD_NO);
				SetBodygroup(BODYGROUP_BODY, SUBMDL_BODY_SKELETON_NOSKULL);
			}
			else
			{
				SetBodygroup(BODYGROUP_HEAD, SUBMDL_HEAD_BLOWN);
			}

			FX_PlrGib( ptr->vecEndPos, GIBBED_HEAD );
			pev->health = 0;
			m_iInjuryType |= DAMAGE_BLOW_HEAD;
			Killed(pevAttacker, 0);
			FrozenEnd();
			IgniteEnd();
			return;
		}
	}

	CBaseMonster::TraceAttack(pevAttacker, flDamage, vecDir, ptr, bitsDamageType);
}

int CZombie :: RealTakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType )
{
	if ( bitsDamageType & (DMG_RADIATION | DMG_BLIND | DMG_NERVEGAS | DMG_DROWN) )
	{
		flDamage = 0;
		return 0;
	}

	if (m_fEndfrozen)
		flDamage *= 2;

	if (bitsDamageType &(DMG_BLAST | DMG_NUKE | DMG_ENERGYBLAST | DMG_CRUSH))
		flDamage = flDamage*1.5;

	if (bitsDamageType & (DMG_SONIC | DMG_PLASMA))
		flDamage = flDamage*1.2;

	if (bitsDamageType &(DMG_SHOCK | DMG_IGNITE | DMG_BURN | DMG_ENERGYBEAM | DMG_GENERIC))
		flDamage = flDamage*1;

	if (bitsDamageType &(DMG_SLASH | DMG_ACID))
		flDamage = flDamage*0.6;

	if (bitsDamageType &( DMG_BULLET | DMG_BULLETMAGNUM | DMG_BULLETBUCKSHOT | DMG_FREEZE))
		flDamage = flDamage*0.4;

	if (bitsDamageType &(DMG_POISON))
		flDamage = flDamage*0.2;

	if ( IsAlive() && gpGlobals->time >= m_flNextPain && !m_fEndfrozen )
	{
		PainSound();
		m_flNextPain = gpGlobals->time + 5;
	}

	if ( (bitsDamageType &(DMG_BLAST) || m_fEndfrozen) && flDamage >= pev->health )
	{
		FX_PlrGib( pev->origin, (m_iInjuryType&DAMAGE_DESTROY_FLESH)?(GIBBED_IGNITE):((m_fEndfrozen)?GIBBED_FROZEN:GIBBED_BODY));
		UTIL_Remove(this);
		return 0;
	}
	if ( (bitsDamageType &(DMG_SHOCK | DMG_ENERGYBLAST)) && flDamage >= pev->health)
	{
		FX_PlrGib( pev->origin, GIBBED_ELECTRO);
		UTIL_Remove(this);
		return 0;
	}
	if ( (bitsDamageType &(DMG_PLASMA | DMG_NUKE | DMG_ENERGYBEAM | DMG_IGNITE)) && flDamage >= pev->health)
	{
		FX_PlrGib( pev->origin, GIBBED_IGNITE);
		UTIL_Remove(this);
		return 0;
	}
	else if ((bitsDamageType &(DMG_ACID)) && flDamage >= pev->health)
	{
		if (m_iInjuryType & DAMAGE_BLOW_HEAD)
			SetBodygroup(BODYGROUP_BODY, SUBMDL_BODY_SKELETON_NOSKULL);
		else
			SetBodygroup(BODYGROUP_BODY, SUBMDL_BODY_SKELETON);

		m_iInjuryType |= DAMAGE_DESTROY_FLESH;
		return 0;
	}

	if (!pev->takedamage)
		return 0;

	if (bitsDamageType & DMG_FREEZE)
	{
		IgniteEnd();
		FrozenStart();
	}

	if (bitsDamageType & DMG_IGNITE)
	{
		FrozenEnd();
		IgniteStart();
	}

	return CBaseMonster::RealTakeDamage( pevInflictor, pevAttacker, flDamage, bitsDamageType );
}

void CZombie::IgniteStart()
{
	if (!m_fNextIgnite)
	{
		Create( "fx_spawner_burn", pev->origin, pev->origin, edict());
		m_fNextIgnite = gpGlobals->time + 1;
		m_iIgniteCounter = 0;
	}

	EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/pl_burn.wav", 0.8, ATTN_NORM);
}

void CZombie::IgniteEnd()
{
 	if (m_fNextIgnite)
	{
		m_fNextIgnite = 0;
		STOP_SOUND(ENT(pev), CHAN_VOICE, "player/pl_burn.wav" );
	}
}

void CZombie::FrozenStart()
{
	if ( GetTask()->iTask == TASK_AWAKE_FROM_DEAD )
		return;

	m_fEndfrozen = gpGlobals->time + FREEZE_DURATION;
	pev->renderfx = kRenderFxGlowShell;
	pev->rendercolor = Vector(0,90,250);
	Create( "fx_spawner_freeze", pev->origin, pev->origin, edict());

	if (IsAlive())
	{
		pev->sequence = 0;
		pev->framerate = 0;
		m_MonsterState = MONSTERSTATE_IDLE;
		Stop();
	}
	pev->velocity = pev->avelocity = g_vecZero;
}

void CZombie::FrozenEnd()
{
	if (!m_fEndfrozen)
		return;

	m_fEndfrozen = 0;
	pev->renderfx = 0;
	pev->rendercolor = g_vecZero;
	pev->rendermode = kRenderNormal;

	if (IsAlive()) m_MonsterState = MONSTERSTATE_ALERT;
}

void CZombie :: PainSound( void )
{
	switch (RANDOM_LONG(0,2))
	{
		case 0: EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "zombie/zo_pain1.wav", 1, ATTN_NORM, 0, 95 + RANDOM_LONG(0,9)); break;
		case 1: EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "zombie/zo_pain2.wav", 1, ATTN_NORM, 0, 95 + RANDOM_LONG(0,9)); break;
		case 2: EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "zombie/zo_pain3.wav", 1, ATTN_NORM, 0, 95 + RANDOM_LONG(0,9)); break;
	}
}

void CZombie :: AlertSound( void )
{
	switch (RANDOM_LONG(0,2))
	{
		case 0: EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "zombie/zo_alert1.wav", 1, ATTN_NORM, 0, 95 + RANDOM_LONG(0,9)); break;
		case 1: EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "zombie/zo_alert2.wav", 1, ATTN_NORM, 0, 95 + RANDOM_LONG(0,9)); break;
		case 2: EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "zombie/zo_alert3.wav", 1, ATTN_NORM, 0, 95 + RANDOM_LONG(0,9)); break;
	}
}

void CZombie :: IdleSound( void )
{
	switch (RANDOM_LONG(0,2))
	{
		case 0: EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "zombie/zo_idle1.wav", 1, ATTN_NORM, 0, 95 + RANDOM_LONG(0,9)); break;
		case 1: EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "zombie/zo_idle2.wav", 1, ATTN_NORM, 0, 95 + RANDOM_LONG(0,9)); break;
		case 2: EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "zombie/zo_idle3.wav", 1, ATTN_NORM, 0, 95 + RANDOM_LONG(0,9)); break;
	}
}

void CZombie :: AttackSound( void )
{
	switch (RANDOM_LONG(0,2))
	{
		case 0: EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "zombie/zo_attack1.wav", 1, ATTN_NORM, 0, 95 + RANDOM_LONG(0,9)); break;
		case 1: EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "zombie/zo_attack2.wav", 1, ATTN_NORM, 0, 95 + RANDOM_LONG(0,9)); break;
		case 2: EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "zombie/zo_attack3.wav", 1, ATTN_NORM, 0, 95 + RANDOM_LONG(0,9)); break;
	}
}

void CZombie :: AttackMissSound( void )
{
	switch (RANDOM_LONG(0,1))
	{
		case 0: EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "zombie/zo_strike_miss1.wav", 1, ATTN_NORM, 0, 95 + RANDOM_LONG(0,9)); break;
		case 1: EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "zombie/zo_strike_miss2.wav", 1, ATTN_NORM, 0, 95 + RANDOM_LONG(0,9)); break;
	}
}

void CZombie :: AttackHitSound( void )
{
	switch (RANDOM_LONG(0,1))
	{
		case 0: EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "zombie/zo_strike1.wav", 1, ATTN_NORM, 0, 95 + RANDOM_LONG(0,9)); break;
		case 1: EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "zombie/zo_strike2.wav", 1, ATTN_NORM, 0, 95 + RANDOM_LONG(0,9)); break;
	}
}

//=========================================================
// HandleAnimEvent - catches the monster-specific messages
// that occur when tagged animation frames are played.
//=========================================================
void CZombie :: HandleAnimEvent( MonsterEvent_t *pEvent )
{
	switch( pEvent->event )
	{
		case ZOMBIE_AE_ATTACK_RANGE:
		{
			Vector vecToss;
			UTIL_MakeAimVectors ( pev->angles );
			vecToss = VecCheckThrow( pev, GetGunPosition(), m_vecEnemyLKP, 750, 1 );

			AttackMissSound();
			CGuts::ShootGuts(pev, GetGunPosition(), vecToss);
		}
		break;

		case ZOMBIE_AE_ATTACK_RIGHT:
		{
			// do stuff for this event.
			CBaseEntity *pHurt = CheckTraceHullAttack( 70, zombie_dmg_melee.value, DMG_SLASH );
			if ( pHurt )
			{
				if ( pHurt->pev->flags & (FL_MONSTER|FL_CLIENT) )
				{
					pHurt->pev->punchangle.z = -18;
					pHurt->pev->punchangle.x = 5;
					pHurt->pev->velocity = pHurt->pev->velocity - gpGlobals->v_right * 100;
				}
				AttackHitSound();
			}
			else
				AttackMissSound();

			if (RANDOM_LONG(0,1))
				AttackSound();
		}
		break;

		case ZOMBIE_AE_ATTACK_LEFT:
		{
			// do stuff for this event.
			CBaseEntity *pHurt = CheckTraceHullAttack( 70, zombie_dmg_melee.value, DMG_SLASH );
			if ( pHurt )
			{
				if ( pHurt->pev->flags & (FL_MONSTER|FL_CLIENT) )
				{
					pHurt->pev->punchangle.z = 18;
					pHurt->pev->punchangle.x = 5;
					pHurt->pev->velocity = pHurt->pev->velocity + gpGlobals->v_right * 100;
				}
				AttackHitSound();
			}
			else
				AttackMissSound();

			if (RANDOM_LONG(0,1))
				AttackSound();
		}
		break;

		case ZOMBIE_AE_ATTACK_BOTH:
		{
			// do stuff for this event.
			CBaseEntity *pHurt = CheckTraceHullAttack( 70, zombie_dmg_melee.value*2, DMG_SLASH );
			if ( pHurt )
			{
				if ( pHurt->pev->flags & (FL_MONSTER|FL_CLIENT) )
				{
					pHurt->pev->punchangle.x = 5;
					pHurt->pev->velocity = pHurt->pev->velocity + gpGlobals->v_forward * -100;
				}
				AttackHitSound();
			}
			else
				AttackMissSound();

			if (RANDOM_LONG(0,1))
				AttackSound();
		}
		break;

		default:
			CBaseMonster::HandleAnimEvent( pEvent );
			break;
	}
}

//=========================================================
// Spawn
//=========================================================
void CZombie :: Spawn()
{
	Precache( );

	if (pev->model)
		SET_MODEL(ENT(pev), STRING(pev->model)); //LRC
	else
		SET_MODEL(ENT(pev), "models/monsters/zombie.mdl");

	UTIL_SetSize( pev, VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX );

	m_HackedGunPos	= Vector(0, 0, 36);//Ghoul: do you realy think that previously meat were been thrown from zombie's hands??? Dull!!!

	pev->solid	= SOLID_SLIDEBOX;
	pev->takedamage	= DAMAGE_AIM;
	pev->movetype	= MOVETYPE_STEP;
	pev->health	= zombie_health.value;
	pev->view_ofs	= VEC_VIEW;// position of the eyes relative to monster's origin.
	m_flFieldOfView	= 0.1;// indicates the width of this monster's forward view cone ( as a dotproduct result )
	m_MonsterState	= MONSTERSTATE_NONE;
	m_afCapability	= bits_CAP_DOORS_GROUP;
	m_iInjuryType 	= DAMAGE_NO;
	pev->skin	= RANDOM_LONG(0,3);
	MonsterInit();
}


void CZombie :: RealReSpawn()
{
	pev->takedamage		= DAMAGE_AIM;

	if (m_iInjuryType & DAMAGE_DESTROY_FLESH)
		pev->health	= zombie_health.value/2;
	else
		pev->health	= zombie_health.value;

	m_flFieldOfView		= 0.1;// indicates the width of this monster's forward view cone ( as a dotproduct result )

	m_MonsterState		= MONSTERSTATE_NONE;
	m_afCapability		= bits_CAP_DOORS_GROUP;

	MonsterInit();
}
//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CZombie :: Precache()
{
	if (pev->model)
		PRECACHE_MODEL((char*)STRING(pev->model));
	else
		PRECACHE_MODEL("models/monsters/zombie.mdl");

	PRECACHE_SOUND("zombie/zo_wakeup1.wav");
	PRECACHE_SOUND("zombie/zo_wakeup2.wav");
	PRECACHE_SOUND("zombie/zo_pain1.wav");
	PRECACHE_SOUND("zombie/zo_pain2.wav");
	PRECACHE_SOUND("zombie/zo_pain3.wav");
	PRECACHE_SOUND("zombie/zo_strike1.wav");
	PRECACHE_SOUND("zombie/zo_strike2.wav");
	PRECACHE_SOUND("zombie/zo_strike_miss1.wav");
	PRECACHE_SOUND("zombie/zo_strike_miss2.wav");
	PRECACHE_SOUND("zombie/zo_attack1.wav");
	PRECACHE_SOUND("zombie/zo_attack2.wav");
	PRECACHE_SOUND("zombie/zo_attack3.wav");
	PRECACHE_SOUND("zombie/zo_idle1.wav");
	PRECACHE_SOUND("zombie/zo_idle2.wav");
	PRECACHE_SOUND("zombie/zo_idle3.wav");
	PRECACHE_SOUND("zombie/zo_alert1.wav");
	PRECACHE_SOUND("zombie/zo_alert2.wav");
	PRECACHE_SOUND("zombie/zo_alert3.wav");
}	

//=========================================================
// AI Schedules Specific to this monster
//=========================================================

int CZombie::IgnoreConditions ( void )
{
	int iIgnore = CBaseMonster::IgnoreConditions();

	if ((m_Activity == ACT_MELEE_ATTACK1) || (m_Activity == ACT_MELEE_ATTACK1))
	{
		if (m_flNextFlinch >= gpGlobals->time)
			iIgnore |= (bits_COND_LIGHT_DAMAGE|bits_COND_HEAVY_DAMAGE);
	}

	if ((m_Activity == ACT_SMALL_FLINCH) || (m_Activity == ACT_BIG_FLINCH))
	{
		if (m_flNextFlinch < gpGlobals->time)
			m_flNextFlinch = gpGlobals->time + ZOMBIE_FLINCH_DELAY;
	}
	return iIgnore;
}
