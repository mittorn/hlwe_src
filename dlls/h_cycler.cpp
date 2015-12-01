#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "animation.h"
#include "weapons.h"
#include "player.h"


#define TEMP_FOR_SCREEN_SHOTS
#ifdef TEMP_FOR_SCREEN_SHOTS //===================================================

class CCycler : public CBaseMonster
{
public:
	void GenericCyclerSpawn(char *szModel, Vector vecMin, Vector vecMax);
	virtual int	ObjectCaps( void ) { return (CBaseEntity :: ObjectCaps() | FCAP_IMPULSE_USE); }
	int RealTakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType );
	void Spawn( void );
	void Think( void );
	//void Pain( float flDamage );
	void RealUse ( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	// Don't treat as a live target
	virtual BOOL IsAlive( void ) { return FALSE; }

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	int			m_animate;
};

TYPEDESCRIPTION	CCycler::m_SaveData[] = 
{
	DEFINE_FIELD( CCycler, m_animate, FIELD_INTEGER ),
};

IMPLEMENT_SAVERESTORE( CCycler, CBaseMonster );


//
// we should get rid of all the other cyclers and replace them with this.
//
class CGenericCycler : public CCycler
{
public:
	void Spawn( void ) { GenericCyclerSpawn( (char *)STRING(pev->model), Vector(-16, -16, 0), Vector(16, 16, 72) ); }
};
LINK_ENTITY_TO_CLASS( cycler, CGenericCycler );



// Cycler member functions

void CCycler :: GenericCyclerSpawn(char *szModel, Vector vecMin, Vector vecMax)
{
	if (!szModel || !*szModel)
	{
		ALERT(at_error, "cycler at %.0f %.0f %0.f missing modelname", pev->origin.x, pev->origin.y, pev->origin.z );
		REMOVE_ENTITY(ENT(pev));
		return;
	}

	pev->classname		= MAKE_STRING("cycler");
	UTIL_PrecacheModel( szModel );
	UTIL_SetModel(ENT(pev),	szModel);

	CCycler::Spawn( );

	UTIL_SetSize(pev, vecMin, vecMax);
}


void CCycler :: Spawn( )
{
	InitBoneControllers();
	pev->solid		= SOLID_SLIDEBOX;
	pev->takedamage		= DAMAGE_NO;
	pev->effects		= 0;
	pev->yaw_speed		= 5;
	pev->ideal_yaw		= pev->angles.y;
	ChangeYaw( 360 );
	
	m_flFrameRate		= 75;
	m_flGroundSpeed		= 0;

	pev->nextthink		+= 1.0;

	ResetSequenceInfo( );

	if (pev->sequence != 0 || pev->frame != 0)
	{
		m_animate = 0;
		pev->framerate = 0;
	}
	else
	{
		m_animate = 1;
	}
}




//
// cycler think
//
void CCycler :: Think( void )
{
	pev->nextthink = gpGlobals->time + 0.1;

	if (m_animate)
	{
		StudioFrameAdvance ( );
	}
	if (m_fSequenceFinished && !m_fSequenceLoops)
	{
		// ResetSequenceInfo();
		// hack to avoid reloading model every frame
		pev->animtime = gpGlobals->time;
		pev->framerate = 1.0;
		m_fSequenceFinished = FALSE;
		m_flLastEventCheck = gpGlobals->time;
		pev->frame = 0;
		if (!m_animate)
			pev->framerate = 0.0;	// FIX: don't reset framerate
	}
}

//
// CyclerUse - starts a rotation trend
//
void CCycler :: RealUse ( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	m_animate = !m_animate;
	if (m_animate)
		pev->framerate = 1.0;
	else
		pev->framerate = 0.0;
}

//
// CyclerPain , changes sequences when shot
//
//void CCycler :: Pain( float flDamage )
int CCycler :: RealTakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType )
{
	if (m_animate)
	{
		pev->sequence++;

		ResetSequenceInfo( );

		if (m_flFrameRate == 0.0)
		{
			pev->sequence = 0;
			ResetSequenceInfo( );
		}
		pev->frame = 0;
	}
	else
	{
		pev->framerate = 1.0;
		StudioFrameAdvance ( 0.1 );
		pev->framerate = 0;
		ALERT( at_console, "sequence: %d, frame %.0f\n", pev->sequence, pev->frame );
	}

	return 0;
}

#endif


class CCyclerSprite : public CBaseEntity
{
public:
	void Spawn( void );
	void Think( void );
	void RealUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	virtual int	ObjectCaps( void ) { return (CBaseEntity :: ObjectCaps() | FCAP_DONT_SAVE | FCAP_IMPULSE_USE); }
	virtual int	RealTakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType );
	void	Animate( float frames );

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	inline int	ShouldAnimate( void ) { return m_animate && m_maxFrame > 1.0; }
	int		m_animate;
	float		m_lastTime;
	float		m_maxFrame;
};

LINK_ENTITY_TO_CLASS( cycler_sprite, CCyclerSprite );

TYPEDESCRIPTION	CCyclerSprite::m_SaveData[] = 
{
	DEFINE_FIELD( CCyclerSprite, m_animate, FIELD_INTEGER ),
	DEFINE_FIELD( CCyclerSprite, m_lastTime, FIELD_TIME ),
	DEFINE_FIELD( CCyclerSprite, m_maxFrame, FIELD_FLOAT ),
};

IMPLEMENT_SAVERESTORE( CCyclerSprite, CBaseEntity );


void CCyclerSprite::Spawn( void )
{
	pev->solid			= SOLID_SLIDEBOX;
	pev->movetype		= MOVETYPE_NONE;
	pev->takedamage		= DAMAGE_YES;
	pev->effects		= 0;

	pev->frame			= 0;
	pev->nextthink		= gpGlobals->time + 0.1;
	m_animate			= 1;
	m_lastTime			= gpGlobals->time;

	UTIL_PrecacheModel( (char *)STRING(pev->model) );
	UTIL_SetModel( ENT(pev), STRING(pev->model) );

	m_maxFrame = (float) MODEL_FRAMES( pev->modelindex ) - 1;
}


void CCyclerSprite::Think( void )
{
	if ( ShouldAnimate() )
		Animate( pev->framerate * (gpGlobals->time - m_lastTime) );

	pev->nextthink		= gpGlobals->time + 0.1;
	m_lastTime = gpGlobals->time;
}


void CCyclerSprite::RealUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	m_animate = !m_animate;
	ALERT( at_console, "Sprite: %s\n", STRING(pev->model) );
}


int	CCyclerSprite::RealTakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType )
{
	if ( m_maxFrame > 1.0 )
	{
		Animate( 1.0 );
	}
	return 1;
}

void CCyclerSprite::Animate( float frames )
{ 
	pev->frame += frames;
	if ( m_maxFrame > 0 )
		pev->frame = fmod( pev->frame, m_maxFrame );
}