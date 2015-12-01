#ifndef FUNC_BREAK_H
#define FUNC_BREAK_H

typedef enum
{
	matGlass = 0, 
	matWood, matMetal,
	matFlesh,
	matCinderBlock,
	matCeilingTile,
	matComputer,
	matUnbreakableGlass,
	matRocks,
	matGrate,
	matVent,
	matBrick,
	matConcrete,
	matIce,
	matSandwall,
	matNone,
	matLastMaterial
}
Materials;

#define	SHARD_GLASS		0
#define	SHARD_WOOD		1
#define	SHARD_METALL		2
#define	SHARD_FLESH		3
#define	SHARD_CONCRETE_BLOCK	4
#define	SHARD_CEILING_TILE	5
#define	SHARD_COMPUTER		6
#define	SHARD_UNBR_GLASS	7
#define	SHARD_ROCK		8
#define	SHARD_GRATE		9
#define	SHARD_VENT		10	
#define	SHARD_BRICK		11		
#define	SHARD_CONCRETE		12		
#define	SHARD_ICE		13		
#define	SHARD_SANDWALL		14		

class CBreakable : public CBaseDelay
{
public:
	float m_fSize;
	Vector oldorigin;
	float oldhealth;
	Vector oldcenter;

	// basic functions
	void Spawn( void );
	virtual void RealReSpawn ( void );
	void Precache( void );
	void KeyValue( KeyValueData* pkvd);
	void EXPORT BreakTouch( CBaseEntity *pOther );
	void RealUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void DamageSound( void );

	void Killed( entvars_t *pevAttacker, int iGib );//Hacked for now. Not fast.

	void EXPORT RespawnThink ( void );

	virtual int RealTakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType );
	virtual void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType );

	BOOL IsBreakable( void );
	BOOL SparkWhenHit( void );

	bool IsRespawnable ( void ) { return true; }

	virtual void EXPORT	Die( void );
	virtual int		ObjectCaps( void ) { return (CBaseEntity :: ObjectCaps() & ~FCAP_ACROSS_TRANSITION); }
	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );

	inline BOOL		Explodable( void ) { return ExplosionMagnitude() > 0; }
	inline int		ExplosionMagnitude( void ) { return pev->impulse; }
	inline void		ExplosionSetMagnitude( int magnitude ) { pev->impulse = magnitude; }

	static void MaterialSoundPrecache( Materials precacheMaterial );
	static void MaterialSoundRandom( edict_t *pEdict, Materials soundMaterial, float volume );
	static const char **MaterialSoundList( Materials precacheMaterial, int &soundCount );

	static const char *pSoundsWood[];
	static const char *pSoundsGrate[];
	static const char *pSoundsFlesh[];
	static const char *pSoundsGlass[];
	static const char *pSoundsMetal[];
	static const char *pSoundsConcrete[];

	static	TYPEDESCRIPTION m_SaveData[];

	Materials	m_Material;
	int		m_idShard;
	int		m_iShards;
	float		m_angle;
	int		m_iShardVel;
	int		m_iShardSize;
};

#endif	// FUNC_BREAK_H
