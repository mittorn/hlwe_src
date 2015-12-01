#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "decals.h"
#include "explode.h"
#include "monsters.h"
#include "weapons.h"

// Spark Shower
class CShower : public CBaseEntity
{
	void Spawn( void );
	void Think( void );
	void Touch( CBaseEntity *pOther );
	int ObjectCaps( void ) { return FCAP_DONT_SAVE; }
};

LINK_ENTITY_TO_CLASS( spark_shower, CShower );

void CShower::Spawn( void )
{
	pev->velocity = RANDOM_FLOAT( 200, 300 ) * pev->angles;
	pev->velocity.x += RANDOM_FLOAT(-100.f,100.f);
	pev->velocity.y += RANDOM_FLOAT(-100.f,100.f);
	if ( pev->velocity.z >= 0 )
		pev->velocity.z += 200;
	else
		pev->velocity.z -= 200;
	pev->movetype = MOVETYPE_BOUNCE;
	pev->gravity = 0.5;
	pev->nextthink = gpGlobals->time + 0.1;
	pev->solid = SOLID_NOT;
	SET_MODEL( edict(), "models/projectiles.mdl");	// Need a model, just use the grenade, we don't draw it anyway

	UTIL_SetSize(pev, g_vecZero, g_vecZero );
	pev->effects |= EF_NODRAW;
	pev->speed = RANDOM_FLOAT( 0.5, 1.5 );

	pev->angles = g_vecZero;
}


void CShower::Think( void )
{
	FX_Explosion( pev->origin, EXPLOSION_SPARKSHOWER );

	pev->speed -= 0.1;
	if ( pev->speed > 0 )
		pev->nextthink = gpGlobals->time + 0.1;
	else
		UTIL_Remove( this );
	pev->flags &= ~FL_ONGROUND;
}

void CShower::Touch( CBaseEntity *pOther )
{
	if ( pev->flags & FL_ONGROUND )
		pev->velocity = pev->velocity * 0.1;
	else
		pev->velocity = pev->velocity * 0.6;

	if ( (pev->velocity.x*pev->velocity.x+pev->velocity.y*pev->velocity.y) < 10.0 )
		pev->speed = 0;
}

class CEnvExplosion : public CBaseMonster
{
public:
	void Spawn( );
	void EXPORT Smoke ( void );
	void KeyValue( KeyValueData *pkvd );
	void RealUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	int m_iMagnitude;// how large is the fireball? how much damage?
	int m_spriteScale; // what's the exact fireball sprite scale? 
};

TYPEDESCRIPTION	CEnvExplosion::m_SaveData[] = 
{
	DEFINE_FIELD( CEnvExplosion, m_iMagnitude, FIELD_INTEGER ),
	DEFINE_FIELD( CEnvExplosion, m_spriteScale, FIELD_INTEGER ),
};

IMPLEMENT_SAVERESTORE( CEnvExplosion, CBaseMonster );
LINK_ENTITY_TO_CLASS( env_explosion, CEnvExplosion );

void CEnvExplosion::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "iMagnitude"))
	{
		m_iMagnitude = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}

void CEnvExplosion::Spawn( void )
{ 
	pev->solid = SOLID_NOT;
	pev->effects = EF_NODRAW;

	pev->movetype = MOVETYPE_NONE;
	float flSpriteScale;
	flSpriteScale = (m_iMagnitude/3);
	m_spriteScale = (int)flSpriteScale;
}

void CEnvExplosion::RealUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{ 
	pev->model = iStringNull;//invisible
	pev->solid = SOLID_NOT;// intangible
	pev->origin = pev->origin + Vector(0,0,12);

	// do damage
	if ( !( pev->spawnflags & SF_ENVEXPLOSION_NODAMAGE ) )
		RadiusDamage ( pev, pev, m_iMagnitude, CLASS_NONE, DMG_BLAST );

	MESSAGE_BEGIN( MSG_ALL, gmsgWorldExp );
	WRITE_COORD( pev->origin.x );
	WRITE_COORD( pev->origin.y );
	WRITE_COORD( pev->origin.z );
	WRITE_SHORT( m_spriteScale );
	WRITE_BYTE( (pev->spawnflags & SF_ENVEXPLOSION_NOSMOKE)?0:1 );
	WRITE_BYTE( (pev->spawnflags & SF_ENVEXPLOSION_NOSPARKS)?0:1 );
	WRITE_BYTE( (pev->spawnflags & SF_ENVEXPLOSION_NOFIREBALL)?0:1 );
	WRITE_BYTE( (pev->spawnflags & SF_ENVEXPLOSION_NORING)?0:1 );
	WRITE_BYTE( (pev->spawnflags & SF_ENVEXPLOSION_NODECAL)?0:1 );
	MESSAGE_END();

	if ( !(pev->spawnflags & SF_ENVEXPLOSION_REPEATABLE) )
		UTIL_Remove( this );
}

// HACKHACK -- create one of these and fake a keyvalue to get the right explosion setup
void ExplosionCreate( const Vector &center, const Vector &angles, edict_t *pOwner, int magnitude, BOOL doDamage )
{
	KeyValueData	kvd;
	char			buf[128];

	CBaseEntity *pExplosion = CBaseEntity::Create( "env_explosion", center, angles, pOwner );
	sprintf( buf, "%3d", magnitude );
	kvd.szKeyName = "iMagnitude";
	kvd.szValue = buf;
	pExplosion->KeyValue( &kvd );
	if ( !doDamage )
		pExplosion->pev->spawnflags |= SF_ENVEXPLOSION_NODAMAGE;

	pExplosion->Spawn();
	pExplosion->Use( NULL, NULL, USE_TOGGLE, 0 );
}