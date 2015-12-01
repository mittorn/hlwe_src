#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "saverestore.h"
#include "func_break.h"
#include "decals.h"
#include "explode.h"
#include "player.h"
#include "weapons.h"

extern DLL_GLOBAL Vector	g_vecAttackDir;

// =================== FUNC_Breakable ==============================================

void CBreakable :: Killed( entvars_t *pevAttacker, int iGib )
{
	TakeDamage (pevAttacker, pevAttacker, pev->health + 10, DMG_ANNIHILATION);
	//Now breakable can respawn after being damaged with black hole
}

void CBreakable::KeyValue( KeyValueData* pkvd )
{
	if (FStrEq(pkvd->szKeyName, "material"))
	{
		int i = atoi( pkvd->szValue);

		if ((i < 0) || (i >= matLastMaterial))
			m_Material = matWood;
		else
			m_Material = (Materials)i;

		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "shards"))
	{
		m_iShards = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "shardvelocity") )
	{
		m_iShardVel = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "shardsize") )
	{
		m_iShardSize = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "explodemagnitude") )
	{
		ExplosionSetMagnitude( atoi( pkvd->szValue ) );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "lip") )
		pkvd->fHandled = TRUE;
	else
		CBaseDelay::KeyValue( pkvd );
}


//
// func_breakable - bmodel that breaks into pieces after taking damage
//
LINK_ENTITY_TO_CLASS( func_breakable, CBreakable );
TYPEDESCRIPTION CBreakable::m_SaveData[] =
{
	DEFINE_FIELD( CBreakable, m_Material, FIELD_INTEGER ),
	DEFINE_FIELD( CBreakable, m_angle, FIELD_FLOAT ),
};

IMPLEMENT_SAVERESTORE( CBreakable, CBaseEntity );


void CBreakable::Spawn( void )
{
    Precache( );

	if ( FBitSet( pev->spawnflags, SF_BREAK_TRIGGER_ONLY ) )
		pev->takedamage	= DAMAGE_NO;
	else
		pev->takedamage	= DAMAGE_YES;
  
	pev->solid	= SOLID_BSP;
	pev->movetype	= MOVETYPE_PUSH;
	m_angle		= pev->angles.y;
	pev->angles.y	= 0;
	pev->effects	= 0;

	SET_MODEL(ENT(pev), STRING(pev->model) );//set size and link into world.

	SetTouch( BreakTouch );
	if ( FBitSet( pev->spawnflags, SF_BREAK_TRIGGER_ONLY ) )		// Only break on trigger
		SetTouch( NULL );

		SetThink (NULL);

	// Flag unbreakable glass as "worldbrush" so it will block ALL tracelines
	if ( !IsBreakable() && pev->rendermode != kRenderNormal )
		pev->flags |= FL_WORLDBRUSH;

	oldorigin = pev->origin;
	m_fSize = (pev->absmax - pev->absmin).Length();
	oldhealth = pev->health;
	oldcenter = Center();
}

void CBreakable::RealReSpawn( void )
{
	if ( FBitSet( pev->spawnflags, SF_BREAK_TRIGGER_ONLY ) )
		pev->takedamage	= DAMAGE_NO;
	else
		pev->takedamage	= DAMAGE_YES;
  
	pev->solid	= SOLID_BSP;
	pev->movetype	= MOVETYPE_PUSH;
	m_angle		= pev->angles.y;
	pev->angles.y	= 0;
	pev->effects	= 0;
	pev->velocity	= g_vecZero;

	SET_MODEL(ENT(pev), STRING(pev->model) );//set size and link into world.

	SetTouch( BreakTouch );
	if ( FBitSet( pev->spawnflags, SF_BREAK_TRIGGER_ONLY ) )		// Only break on trigger
		SetTouch( NULL );

		SetThink (NULL);

	// Flag unbreakable glass as "worldbrush" so it will block ALL tracelines
	if ( !IsBreakable() && pev->rendermode != kRenderNormal )
		pev->flags |= FL_WORLDBRUSH;

	pev->origin = oldorigin;
	pev->health = oldhealth;
	SUB_UseTargets( NULL, USE_TOGGLE, 1 );
}

void CBreakable::RespawnThink ( void )
{
	pev->nextthink = gpGlobals->time + 1;

	CBaseEntity *pFound = NULL;

	while ((pFound = UTIL_FindEntityInSphere(pFound, oldcenter, m_fSize + 50)) != NULL)
	{
		if (pFound->IsPlayer() && pFound->IsAlive())
		{
			return;
		}
	}

	ReSpawn();
}

const char *CBreakable::pSoundsWood[] = 
{
	"debris/wood1.wav",
	"debris/wood2.wav",
	"debris/wood3.wav",
};

const char *CBreakable::pSoundsGrate[] = 
{
	"player/pl_grate1.wav",
	"player/pl_grate2.wav",
	"player/pl_grate3.wav",
	"player/pl_grate4.wav",
};

const char *CBreakable::pSoundsFlesh[] = 
{
	"debris/flesh1.wav",
	"debris/flesh2.wav",
	"debris/flesh3.wav",
	"debris/flesh4.wav",
};

const char *CBreakable::pSoundsMetal[] = 
{
	"debris/metal1.wav",
	"debris/metal2.wav",
	"debris/metal3.wav",
};

const char *CBreakable::pSoundsConcrete[] = 
{
	"debris/concrete1.wav",
	"debris/concrete2.wav",
	"debris/concrete3.wav",
};

const char *CBreakable::pSoundsGlass[] = 
{
	"debris/glass1.wav",
	"debris/glass2.wav",
	"debris/glass3.wav",
};

const char **CBreakable::MaterialSoundList( Materials precacheMaterial, int &soundCount )
{
	const char	**pSoundList = NULL;

    switch ( precacheMaterial ) 
	{
	case matWood:
		pSoundList = pSoundsWood;
		soundCount = ARRAYSIZE(pSoundsWood);
	break;

	case matGrate:
		pSoundList = pSoundsGrate;
		soundCount = ARRAYSIZE(pSoundsGrate);
	break;

	case matFlesh:
		pSoundList = pSoundsFlesh;
		soundCount = ARRAYSIZE(pSoundsFlesh);
	break;

	case matUnbreakableGlass:
	case matGlass:
	case matIce:
		pSoundList = pSoundsGlass;
		soundCount = ARRAYSIZE(pSoundsGlass);
	break;

	case matMetal:
	case matVent:
	case matComputer:
		pSoundList = pSoundsMetal;
		soundCount = ARRAYSIZE(pSoundsMetal);
	break;

	case matCinderBlock:
	case matCeilingTile:
	case matRocks:
	case matBrick:
	case matSandwall:
	case matConcrete:
		pSoundList = pSoundsConcrete;
		soundCount = ARRAYSIZE(pSoundsConcrete);
	break;

	case matNone:
	default:
		soundCount = 0;
		break;
	}

	return pSoundList;
}

void CBreakable::MaterialSoundPrecache( Materials precacheMaterial )
{
	const char	**pSoundList;
	int		i, soundCount = 0;

	pSoundList = MaterialSoundList( precacheMaterial, soundCount );

	for ( i = 0; i < soundCount; i++ )
	{
		PRECACHE_SOUND( (char *)pSoundList[i] );
	}
}

void CBreakable::MaterialSoundRandom( edict_t *pEdict, Materials soundMaterial, float volume )
{
	const char	**pSoundList;
	int		soundCount = 0;

	pSoundList = MaterialSoundList( soundMaterial, soundCount );

	if ( soundCount )
		EMIT_SOUND( pEdict, CHAN_BODY, pSoundList[ RANDOM_LONG(0,soundCount-1) ], volume, 1.0 );
}


void CBreakable::Precache( void )
{
	PRECACHE_MODEL("models/w_worldgibs.mdl");
	MaterialSoundPrecache( m_Material );
}

void CBreakable::DamageSound( void )
{
	int pitch;
	float fvol;
	char *rgpsz[6];
	int i;
	int material = m_Material;

	if (RANDOM_LONG(0,2))
		pitch = PITCH_NORM;
	else
		pitch = 95 + RANDOM_LONG(0,34);

	fvol = RANDOM_FLOAT(0.75, 1.0);

	if (material == matComputer && RANDOM_LONG(0,1))
		material = matMetal;

	switch (material)
	{
	case matGlass:
	case matIce:
	case matUnbreakableGlass:
		rgpsz[0] = "debris/glass1.wav";
		rgpsz[1] = "debris/glass2.wav";
		rgpsz[2] = "debris/glass3.wav";
		i = 3;
	break;

	case matWood:
		rgpsz[0] = "debris/wood1.wav";
		rgpsz[1] = "debris/wood2.wav";
		rgpsz[2] = "debris/wood3.wav";
		i = 3;
	break;

	case matGrate:
		rgpsz[0] = "player/pl_grate1.wav";
		rgpsz[1] = "player/pl_grate2.wav";
		rgpsz[2] = "player/pl_grate3.wav";
		rgpsz[3] = "player/pl_grate4.wav";
		i = 4;
	break;

	case matComputer:
	case matMetal:
	case matVent:
		rgpsz[0] = "debris/metal1.wav";
		rgpsz[1] = "debris/metal3.wav";
		rgpsz[2] = "debris/metal2.wav";
		i = 3;
	break;

	case matFlesh:
		rgpsz[0] = "debris/flesh1.wav";
		rgpsz[1] = "debris/flesh2.wav";
		rgpsz[2] = "debris/flesh3.wav";
		rgpsz[2] = "debris/flesh4.wav";
		i = 4;
	break;

	case matRocks:
	case matCinderBlock:
	case matCeilingTile:
	case matBrick:
	case matSandwall:
	case matConcrete:
		rgpsz[0] = "debris/concrete1.wav";
		rgpsz[1] = "debris/concrete2.wav";
		rgpsz[2] = "debris/concrete3.wav";
		i = 3;
	break;
	}

	if (i)
		EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, rgpsz[RANDOM_LONG(0,i-1)], fvol, ATTN_NORM, 0, pitch);
}

void CBreakable::BreakTouch( CBaseEntity *pOther )
{
	float flDamage;
	entvars_t*	pevToucher = pOther->pev;
	
	// only players can break these right now
	if ( !pOther->IsPlayer() || !IsBreakable() )
	{
        return;
	}

	if ( FBitSet ( pev->spawnflags, SF_BREAK_TOUCH ) )
	{// can be broken when run into 
		flDamage = pevToucher->velocity.Length() * 0.01;

		if (flDamage >= pev->health)
		{
			SetTouch( NULL );
			TakeDamage(pevToucher, pevToucher, flDamage, DMG_CRUSH);

			// do a little damage to player if we broke glass or computer
			pOther->TakeDamage( pev, pev, flDamage/4, DMG_SLASH );
		}
	}

	if ( FBitSet ( pev->spawnflags, SF_BREAK_PRESSURE ) && pevToucher->absmin.z >= pev->maxs.z - 2 )
	{// can be broken when stood upon
		
		// play creaking sound here.

	if (flDamage >= 1)
		DamageSound();

		SetThink ( Die );
		SetTouch( NULL );
		
		if ( m_flDelay == 0 )
		{
			m_flDelay = 0.1;
		}

		pev->nextthink = pev->ltime + m_flDelay;

	}

}


//
// Smash the our breakable object
//

// Break when triggered
void CBreakable::RealUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( IsBreakable() && !(pev->effects & EF_NODRAW))
	{
		pev->angles.y = m_angle;
		UTIL_MakeVectors(pev->angles);
		g_vecAttackDir = gpGlobals->v_forward;

		Die();
	}
}


void CBreakable::TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType )
{
	if (bitsDamageType & (DMG_BLIND|DMG_NERVEGAS))
	return;

	// random spark if this is a 'computer' object
	if (RANDOM_LONG(0,1) )
	{
		switch( m_Material )
		{
			case matComputer:
				FX_Explosion( ptr->vecEndPos, EXPLOSION_SPARKSHOWER_SND );
			break;
			
			case matUnbreakableGlass:
				FX_Explosion( ptr->vecEndPos, EXPLOSION_ARMORIMPACT );
			break;
		}
	}
	CBaseDelay::TraceAttack( pevAttacker, flDamage, vecDir, ptr, bitsDamageType );
}

//=========================================================
// Special takedamage for func_breakable. Allows us to make
// exceptions that are breakable-specific
// bitsDamageType indicates the type of damage sustained ie: DMG_CRUSH
//=========================================================
int CBreakable :: RealTakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType )
{
	if ( bitsDamageType & (DMG_RADIATION | DMG_BLIND | DMG_NERVEGAS | DMG_DROWN) )
		flDamage = 0;

	Vector	vecTemp;
	vecTemp = pevInflictor->origin - ( pev->absmin + ( pev->size * 0.5 ) );

	if (!IsBreakable())
		return 0;

// this global is still used for glass and other non-monster killables, along with decals.
	g_vecAttackDir = vecTemp.Normalize();
		
// do the damage
	pev->health -= flDamage;
	if (pev->health <= 0 && pev->takedamage)
	{
		pev->takedamage = DAMAGE_NO;
		pev->deadflag = DEAD_DEAD;
		Die();
		return 0;
	}

	// Make a shard noise each time func breakable is hit.
	// Don't play shard noise if cbreakable actually died.

	if (flDamage >= 1)
		DamageSound();

	return 1;
}


void CBreakable::Die( void )
{
	Vector vecSpot;// shard origin
	CBaseEntity *pEntity = NULL;

	switch (m_Material)
	{
	case matGlass:
		m_idShard = SHARD_GLASS;
	break;

	case matUnbreakableGlass:
		m_idShard = SHARD_UNBR_GLASS;
	break;

	case matWood:
		m_idShard = SHARD_WOOD;
	break;

	case matComputer:
		m_idShard = SHARD_COMPUTER;
	break;

	case matMetal:
		m_idShard = SHARD_METALL;
	break;

	case matFlesh:
		m_idShard = SHARD_FLESH;
	break;

	case matRocks:
		m_idShard = SHARD_ROCK;
	break;

	case matCinderBlock:
		m_idShard = SHARD_CONCRETE_BLOCK;
	break;

	case matCeilingTile:
		m_idShard = SHARD_CEILING_TILE;
	break;

	case matGrate:
		m_idShard = SHARD_GRATE;
	break;

	case matVent:
		m_idShard = SHARD_VENT;
	break;

	case matBrick:
		m_idShard = SHARD_BRICK;
	break;

	case matConcrete:
		m_idShard = SHARD_CONCRETE;
	break;

	case matIce:
		m_idShard = SHARD_ICE;
	break;

	case matSandwall:
		m_idShard = SHARD_SANDWALL;
	break;
	}
    
	vecSpot = pev->origin + (pev->mins + pev->maxs) * 0.5;

	if (m_iShards == 0 ) //based on explodemagnitude value)
	{
		if (ExplosionMagnitude() <= 0)
			m_iShards = 6; //default
		else
			m_iShards = (ExplosionMagnitude()/25); //based on explodemagnitude value
	}
	else
		m_iShards = m_iShards;

	if (m_iShardVel <= 0)
		m_iShardVel = 100;

	if (m_iShardSize <= 0)
		m_iShardSize = 10;

	FX_BreakGib(vecSpot, m_iShardVel, m_iShardSize, m_iShards, m_idShard );

	// !!! HACK  This should work!
	// Build a box above the entity that looks like an 8 pixel high sheet
	Vector mins = pev->absmin;
	Vector maxs = pev->absmax;
	mins.z = pev->absmax.z;
	maxs.z += 8;

	// BUGBUG -- can only find 256 entities on a breakable -- should be enough
	CBaseEntity *pList[256];
	int count = UTIL_EntitiesInBox( pList, 256, mins, maxs, FL_ONGROUND );
	if ( count )
	{
		for ( int i = 0; i < count; i++ )
		{
			ClearBits( pList[i]->pev->flags, FL_ONGROUND );
			pList[i]->pev->groundentity = NULL;
		}
	}

	pev->solid = SOLID_NOT;
	// Fire targets on break
	SUB_UseTargets( NULL, USE_TOGGLE, 0 );
	pev->effects = EF_NODRAW;

	if ( Explodable() )
		ExplosionCreate( Center(), pev->angles, edict(), ExplosionMagnitude(), TRUE );

	if (pev->spawnflags & SF_NOT_RESPAWNABLE)
		UTIL_Remove(this);
	else
	{
		SetThink ( RespawnThink );
		pev->nextthink = gpGlobals->time + BREAKABLE_RESPAWN_TIME;
	}
}


BOOL CBreakable :: IsBreakable( void ) 
{ 
	return m_Material != matUnbreakableGlass;
}

class CPushable : public CBreakable
{
public:
	void	Spawn ( void );
	void	RealReSpawn ( void );
	void	Die ( void );
	void	Precache( void );
	void	Touch ( CBaseEntity *pOther );
	void	Move( CBaseEntity *pMover, int push );
	void	Pop ( Vector vecDir );
	void	KeyValue( KeyValueData *pkvd );
	void	RealUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void	EXPORT StopSound( void );

	virtual int	ObjectCaps( void ) { return (CBaseEntity :: ObjectCaps() & ~FCAP_ACROSS_TRANSITION) | FCAP_CONTINUOUS_USE; }
	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );

	inline float MaxSpeed( void ) { return m_maxSpeed; }
	
	// breakables use an overridden takedamage
	virtual int RealTakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType );
	void	TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType );
	static	TYPEDESCRIPTION m_SaveData[];

	static char *m_soundNames[3];
	int		m_lastSound;	// no need to save/restore, just keeps the same sound from playing twice in a row
	float	m_maxSpeed;
	float	m_soundTime;

	int my_bbox;
};

TYPEDESCRIPTION	CPushable::m_SaveData[] = 
{
	DEFINE_FIELD( CPushable, m_maxSpeed, FIELD_FLOAT ),
	DEFINE_FIELD( CPushable, m_soundTime, FIELD_TIME ),
};

IMPLEMENT_SAVERESTORE( CPushable, CBreakable );

LINK_ENTITY_TO_CLASS( func_pushable, CPushable );

char *CPushable :: m_soundNames[3] = { "debris/pushbox1.wav", "debris/pushbox2.wav", "debris/pushbox3.wav" };


void CPushable :: Spawn( void )
{
		switch( my_bbox )
		{
		case 0:	// Point
			UTIL_SetSize(pev, Vector(-8, -8, -8), Vector(8, 8, 8));
			break;

		case 2: // Big Hull!?!?	!!!BUGBUG Figure out what this hull really is
			UTIL_SetSize(pev, VEC_DUCK_HULL_MIN*2, VEC_DUCK_HULL_MAX*2);
			break;

		case 3: // Player duck
			UTIL_SetSize(pev, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX);
			break;

		default:
		case 1: // Player
			UTIL_SetSize(pev, VEC_HULL_MIN, VEC_HULL_MAX);
			break;
		}

	if ( pev->spawnflags & SF_PUSH_BREAKABLE )
		CBreakable::Spawn();
	else
		Precache( );

	pev->movetype	= MOVETYPE_PUSHSTEP;
	pev->solid		= SOLID_BBOX;
	SET_MODEL( ENT(pev), STRING(pev->model) );

	if ( pev->friction > 399 )
		pev->friction = 399;

	m_maxSpeed = 400 - pev->friction;
	SetBits( pev->flags, FL_FLOAT );
	pev->friction = 0;
	
	pev->origin.z += 1;	// Pick up off of the floor
	UTIL_SetOrigin( pev, pev->origin );

	// Multiply by area of the box's cross-section (assume 1000 units^3 standard volume)
	pev->skin = ( pev->skin * (pev->maxs.x - pev->mins.x) * (pev->maxs.y - pev->mins.y) ) * 0.0005;
	m_soundTime = 0;

	oldorigin = pev->origin;
	m_fSize = (pev->absmax - pev->absmin).Length();
	oldhealth = pev->health;
	oldcenter = Center();
}

void CPushable :: RealReSpawn( void )
{
	if ( FBitSet( pev->spawnflags, SF_BREAK_TRIGGER_ONLY ) )
		pev->takedamage	= DAMAGE_NO;
	else
		pev->takedamage	= DAMAGE_YES;
  
	m_angle		= pev->angles.y;
	pev->angles.y	= 0;
	pev->effects	= 0;
	pev->solid	= SOLID_BBOX;

	SetTouch( BreakTouch );
	if ( FBitSet( pev->spawnflags, SF_BREAK_TRIGGER_ONLY ) )		// Only break on trigger
		SetTouch( NULL );

		SetThink (NULL);

	pev->movetype	= MOVETYPE_PUSHSTEP;

	pev->friction = 0;
	SetBits( pev->flags, FL_FLOAT );
	UTIL_SetOrigin( pev, oldorigin );

	pev->health = oldhealth;
	pev->velocity = g_vecZero;
	pev->skin = ( pev->skin * (pev->maxs.x - pev->mins.x) * (pev->maxs.y - pev->mins.y) ) * 0.0005;
	m_soundTime = 0;
}

void CPushable::Die( void )
{
	Vector vecSpot;// shard origin
	CBaseEntity *pEntity = NULL;

	switch (m_Material)
	{
	case matGlass:
		m_idShard = SHARD_GLASS;
	break;

	case matUnbreakableGlass:
		m_idShard = SHARD_UNBR_GLASS;
	break;

	case matWood:
		m_idShard = SHARD_WOOD;
	break;

	case matComputer:
		m_idShard = SHARD_COMPUTER;
	break;

	case matMetal:
		m_idShard = SHARD_METALL;
	break;

	case matFlesh:
		m_idShard = SHARD_FLESH;
	break;

	case matRocks:
		m_idShard = SHARD_ROCK;
	break;

	case matCinderBlock:
		m_idShard = SHARD_CONCRETE_BLOCK;
	break;

	case matCeilingTile:
		m_idShard = SHARD_CEILING_TILE;
	break;

	case matGrate:
		m_idShard = SHARD_GRATE;
	break;

	case matVent:
		m_idShard = SHARD_VENT;
	break;

	case matBrick:
		m_idShard = SHARD_BRICK;
	break;

	case matConcrete:
		m_idShard = SHARD_CONCRETE;
	break;

	case matIce:
		m_idShard = SHARD_ICE;
	break;

	case matSandwall:
		m_idShard = SHARD_SANDWALL;
	break;
	}
    
	vecSpot = pev->origin + (pev->mins + pev->maxs) * 0.5;

	if (m_iShards == 0 ) //based on explodemagnitude value)
	{
		if (ExplosionMagnitude() <= 0)
			m_iShards = 6; //default
		else
			m_iShards = (ExplosionMagnitude()/25); //based on explodemagnitude value
	}
	else
		m_iShards = m_iShards;

	if (m_iShardVel <= 0)
		m_iShardVel = 100;

	if (m_iShardSize <= 0)
		m_iShardSize = 10;

	FX_BreakGib(vecSpot, m_iShardVel, m_iShardSize, m_iShards, m_idShard );

	// !!! HACK  This should work!
	// Build a box above the entity that looks like an 8 pixel high sheet
	Vector mins = pev->absmin;
	Vector maxs = pev->absmax;
	mins.z = pev->absmax.z;
	maxs.z += 8;

	// BUGBUG -- can only find 256 entities on a breakable -- should be enough
	CBaseEntity *pList[256];
	int count = UTIL_EntitiesInBox( pList, 256, mins, maxs, FL_ONGROUND );
	if ( count )
	{
		for ( int i = 0; i < count; i++ )
		{
			ClearBits( pList[i]->pev->flags, FL_ONGROUND );
			pList[i]->pev->groundentity = NULL;
		}
	}

	pev->solid = SOLID_NOT;

	// Fire targets on break
	SUB_UseTargets( NULL, USE_TOGGLE, 0 );

	pev->effects |= EF_NODRAW;
	pev->takedamage	= DAMAGE_NO;

	if ( Explodable() )
	{
		ExplosionCreate( Center(), pev->angles, edict(), ExplosionMagnitude(), TRUE );
	}

	if (pev->spawnflags & SF_NOT_RESPAWNABLE)
		UTIL_Remove(this);
	else
	{
		SetThink ( RespawnThink );
		pev->nextthink = gpGlobals->time + BREAKABLE_RESPAWN_TIME;
	}
}

void CPushable :: Precache( void )
{
	for ( int i = 0; i < 3; i++ )
		PRECACHE_SOUND( m_soundNames[i] );

	if ( pev->spawnflags & SF_PUSH_BREAKABLE )
		CBreakable::Precache( );
}


void CPushable :: KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq(pkvd->szKeyName, "size") )
	{
		my_bbox = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq(pkvd->szKeyName, "buoyancy") )
	{
		pev->skin = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBreakable::KeyValue( pkvd );
}


// Pull the func_pushable
void CPushable :: RealUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if (pev->effects & EF_NODRAW) return; 

	if ( !pActivator || !pActivator->IsPlayer() )
	{
		if ( pev->spawnflags & SF_PUSH_BREAKABLE )
			this->CBreakable::RealUse( pActivator, pCaller, useType, value );
		return;
	}

	if ( pActivator->pev->velocity != g_vecZero )
		Move( pActivator, 0 );
}


void CPushable :: Touch( CBaseEntity *pOther )
{
	if ( FClassnameIs( pOther->pev, "worldspawn" ) )
		return;

	Move( pOther, 1 );
}


void CPushable :: Pop( Vector vecDir )
{
	pev->velocity.x += vecDir.x;
	pev->velocity.y += vecDir.y;
}


void CPushable :: Move( CBaseEntity *pOther, int push )
{
	entvars_t*	pevToucher = pOther->pev;
	int playerTouch = 0;

	// Is entity standing on this pushable ?
	if ( FBitSet(pevToucher->flags,FL_ONGROUND) && pevToucher->groundentity && VARS(pevToucher->groundentity) == pev )
	{
		// Only push if floating
		if ( pev->waterlevel > 0 )
			pev->velocity.z += pevToucher->velocity.z * 0.1;

		return;
	}


	if ( pOther->IsPlayer() )
	{
		if ( push && !(pevToucher->button & (IN_FORWARD|IN_USE)) )	// Don't push unless the player is pushing forward and NOT use (pull)
			return;
		playerTouch = 1;
	}

	float factor;

	if ( playerTouch )
	{
		if ( !(pevToucher->flags & FL_ONGROUND) )	// Don't push away from jumping/falling players unless in water
		{
			if ( pev->waterlevel < 1 )
				return;
			else 
				factor = 0.1;
		}
		else
			factor = 1;
	}
	else 
		factor = 0.25;

	pev->velocity.x += pevToucher->velocity.x * factor;
	pev->velocity.y += pevToucher->velocity.y * factor;

	float length = sqrt( pev->velocity.x * pev->velocity.x + pev->velocity.y * pev->velocity.y );
	if ( push && (length > MaxSpeed()) )
	{
		pev->velocity.x = (pev->velocity.x * MaxSpeed() / length );
		pev->velocity.y = (pev->velocity.y * MaxSpeed() / length );
	}
	if ( playerTouch )
	{
		pevToucher->velocity.x = pev->velocity.x;
		pevToucher->velocity.y = pev->velocity.y;
		if ( (gpGlobals->time - m_soundTime) > 0.7 )
		{
			m_soundTime = gpGlobals->time;
			if ( length > 0 && FBitSet(pev->flags,FL_ONGROUND) )
			{
				m_lastSound = RANDOM_LONG(0,2);
				EMIT_SOUND(ENT(pev), CHAN_WEAPON, m_soundNames[m_lastSound], 0.5, ATTN_NORM);
			}
			else
				STOP_SOUND( ENT(pev), CHAN_WEAPON, m_soundNames[m_lastSound] );
		}
	}
}

int CPushable::RealTakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType )
{
	if ( pev->spawnflags & SF_PUSH_BREAKABLE )
		return CBreakable::RealTakeDamage( pevInflictor, pevAttacker, flDamage, bitsDamageType );

	return 1;
}

void CPushable::TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType )
{
	Pop ((vecDir.Normalize() * 3 + (Center() - ptr->vecEndPos).Normalize() * 1.5) * flDamage );

	CBreakable::TraceAttack(pevAttacker, flDamage, vecDir, ptr, bitsDamageType);
}