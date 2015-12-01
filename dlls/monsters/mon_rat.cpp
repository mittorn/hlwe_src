#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "projectiles.h"

enum w_rat_e
{
	RAT_IDLE1 = 0,
	RAT_FIDGET,
	RAT_JUMP,
	RAT_RUN
};

class CRat : public CBaseMonster
{
	void Spawn( void );
	void Precache( void );
	int  Classify( void );
	void EXPORT SuperBounceTouch( CBaseEntity *pOther );
	void EXPORT HuntThink( void );
	void Killed( entvars_t *pevAttacker, int iGib );

	virtual int	Save( CSave &save ); 
	virtual int	Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	static float m_flNextBounceSoundTime;

	Vector m_vecTarget;
	float m_flNextHunt;
	float m_flNextHit;
	Vector m_posPrev;
};

float CRat::m_flNextBounceSoundTime = 0;

LINK_ENTITY_TO_CLASS( monster_rat, CRat );

TYPEDESCRIPTION	CRat::m_SaveData[] = 
{
	DEFINE_FIELD( CRat, m_vecTarget, FIELD_VECTOR ),
	DEFINE_FIELD( CRat, m_flNextHunt, FIELD_TIME ),
	DEFINE_FIELD( CRat, m_flNextHit, FIELD_TIME ),
	DEFINE_FIELD( CRat, m_posPrev, FIELD_POSITION_VECTOR )
};

IMPLEMENT_SAVERESTORE( CRat, CBaseMonster );

int CRat :: Classify ( void )
{
	return CLASS_INSECT;
}

void CRat :: Spawn( void )
{
	Precache( );
	if (pev->model)
		SET_MODEL(ENT(pev), STRING(pev->model));
	else
		SET_MODEL(ENT(pev), "models/monsters/rat.mdl");

	UTIL_SetSize(pev, Vector( -6, -6, 0), Vector(6, 6, 10));
	UTIL_SetOrigin( pev, pev->origin );

	SetTouch( SuperBounceTouch );
	SetThink( HuntThink );
	pev->nextthink = gpGlobals->time + 0.1;
	m_flNextHunt = gpGlobals->time + 1E6;

	pev->flags 	|= FL_MONSTER;
	pev->takedamage	= DAMAGE_AIM;
	pev->health	= 10;
	pev->gravity	= 0.5;
	pev->friction	= 0.5;
	pev->movetype 	= MOVETYPE_BOUNCE;
	pev->solid 	= SOLID_BBOX;
	m_flFieldOfView = 0;

	m_flNextBounceSoundTime = gpGlobals->time;
	pev->sequence = RAT_RUN;
	ResetSequenceInfo( );
}

void CRat::Precache( void )
{
	if (pev->model)
		PRECACHE_MODEL((char*)STRING(pev->model));
	else
		PRECACHE_MODEL("models/monsters/rat.mdl");

	PRECACHE_SOUND("rat/rat_hunt1.wav");
	PRECACHE_SOUND("rat/rat_hunt2.wav");
	PRECACHE_SOUND("rat/rat_hunt3.wav");
}

void CRat :: Killed( entvars_t *pevAttacker, int iGib )
{
	FX_Trail( pev->origin, entindex(), PROJ_GUTS_DETONATE );

	pev->model = iStringNull;
	SetThink( SUB_Remove );
	SetTouch( NULL );
	pev->nextthink = gpGlobals->time + 0.1;
	pev->takedamage = DAMAGE_NO;
}

void CRat::HuntThink( void )
{
	if (!IsInWorld())
	{
		SetTouch( NULL );
		UTIL_Remove( this );
		return;
	}
	
	StudioFrameAdvance( );
	pev->nextthink = gpGlobals->time + 0.1;

	// float
	if (pev->waterlevel != 0)
	{
		if (pev->movetype == MOVETYPE_BOUNCE)
		{
			pev->movetype = MOVETYPE_FLY;
		}
		pev->velocity = pev->velocity * 0.9;
		pev->velocity.z += 8.0;
	}
	else if (pev->movetype = MOVETYPE_FLY)
	{
		pev->movetype = MOVETYPE_BOUNCE;
	}

	if (m_flNextHunt > gpGlobals->time)
		return;

	m_flNextHunt = gpGlobals->time + 2.0;
	
	CBaseEntity *pOther = NULL;
	Vector vecDir;
	TraceResult tr;

	Vector vecFlat = pev->velocity;
	vecFlat.z = 0;
	vecFlat = vecFlat.Normalize( );

	UTIL_MakeVectors( pev->angles );

	if (m_hEnemy == NULL || !m_hEnemy->IsAlive())
	{
		Look( 512 );
		m_hEnemy = BestVisibleEnemy( );
	}

	if (m_hEnemy != NULL)
	{
		if (FVisible( m_hEnemy ))
		{
			vecDir = m_hEnemy->EyePosition() - pev->origin;
			m_vecTarget = vecDir.Normalize( );
		}

		float flVel = pev->velocity.Length();
		float flAdj = 50.0 / (flVel + 10.0);

		if (flAdj > 1.2)
			flAdj = 1.2;
		
		pev->velocity = pev->velocity * flAdj + m_vecTarget * 300;
	}

	if (pev->flags & FL_ONGROUND)
	{
		pev->avelocity = Vector( 0, 0, 0 );
	}
	else
	{
		if (pev->avelocity == Vector( 0, 0, 0))
		{
			pev->avelocity.x = RANDOM_FLOAT( -100, 100 );
			pev->avelocity.z = RANDOM_FLOAT( -100, 100 );
		}
	}

	if ((pev->origin - m_posPrev).Length() < 1.0)
	{
		pev->velocity.x = RANDOM_FLOAT( -100, 100 );
		pev->velocity.y = RANDOM_FLOAT( -100, 100 );
	}
	m_posPrev = pev->origin;
	pev->angles = UTIL_VecToAngles( pev->velocity );
	pev->angles.z = 0;
	pev->angles.x = 0;
}

void CRat::SuperBounceTouch( CBaseEntity *pOther )
{
	TraceResult tr = UTIL_GetGlobalTrace( );
	pev->angles.x = 0;
	pev->angles.z = 0;

	// avoid bouncing too much
	if (m_flNextHit > gpGlobals->time)
		return;

	if ( pOther->pev->takedamage && m_flNextAttack < gpGlobals->time )
	{
		// attack!
		// make sure it's me who has touched them
		if (tr.pHit == pOther->edict())
		{
			// and it's not another ratgrenade
			if (tr.pHit->v.modelindex != pev->modelindex)
			{
				ClearMultiDamage( );
				pOther->TraceAttack(pev, 12, gpGlobals->v_forward, &tr, DMG_SLASH ); 
				ApplyMultiDamage( pev, pev );

				// make bite sound
				EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "rat/rat_bite1.wav", 1.0, ATTN_NORM, 0, 100);
				m_flNextAttack = gpGlobals->time + 0.5;
			}
		}
	}

	m_flNextHit = gpGlobals->time + 0.1;
	m_flNextHunt = gpGlobals->time;

	if ( gpGlobals->time < m_flNextBounceSoundTime )
		return;

	if (!(pev->flags & FL_ONGROUND))
	{
		float flRndSound = RANDOM_FLOAT (0,1);

		if ( flRndSound <= 0.33 )
			EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "rat/rat_hunt1.wav", 1, ATTN_NORM, 0, 100);		
		else if (flRndSound <= 0.66)
			EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "rat/rat_hunt1.wav", 1, ATTN_NORM, 0, 100);
		else 
			EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "rat/rat_hunt1.wav", 1, ATTN_NORM, 0, 100);
	}
	m_flNextBounceSoundTime = gpGlobals->time + 0.5;
}