#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "customentity.h"
#include "effects.h"
#include "weapons.h"
#include "decals.h"
#include "func_break.h"
#include "shake.h"
#include "player.h"
#include "projectiles.h"

#define	SF_GIBSHOOTER_REPEATABLE	1 // allows a gibshooter to be refired
#define SF_FUNNEL_REVERSE		1 // funnel effect repels particles instead of attracting them.

// Lightning target, just alias landmark
LINK_ENTITY_TO_CLASS( info_target, CPointEntity );


// --------------------------------------------------
// 
// Beams
//
// --------------------------------------------------

LINK_ENTITY_TO_CLASS( beam, CBeam );

void CBeam::Spawn( void )
{
	pev->solid = SOLID_NOT;							// Remove model & collisions
	Precache( );
}

void CBeam::Precache( void )
{
	if ( pev->owner )
		SetStartEntity( ENTINDEX( pev->owner ) );
	if ( pev->aiment )
		SetEndEntity( ENTINDEX( pev->aiment ) );
}

void CBeam::SetStartEntity( int entityIndex ) 
{ 
	pev->sequence = (entityIndex & 0x0FFF) | ((pev->sequence&0xF000)<<12); 
	pev->owner = g_engfuncs.pfnPEntityOfEntIndex( entityIndex );
}

void CBeam::SetEndEntity( int entityIndex ) 
{ 
	pev->skin = (entityIndex & 0x0FFF) | ((pev->skin&0xF000)<<12); 
	pev->aiment = g_engfuncs.pfnPEntityOfEntIndex( entityIndex );
}


// These don't take attachments into account
const Vector &CBeam::GetStartPos( void )
{
	if ( GetType() == BEAM_ENTS )
	{
		edict_t *pent =  g_engfuncs.pfnPEntityOfEntIndex( GetStartEntity() );
		return pent->v.origin;
	}
	return pev->origin;
}


const Vector &CBeam::GetEndPos( void )
{
	int type = GetType();
	if ( type == BEAM_POINTS || type == BEAM_HOSE )
	{
		return pev->angles;
	}

	edict_t *pent =  g_engfuncs.pfnPEntityOfEntIndex( GetEndEntity() );
	if ( pent )
		return pent->v.origin;
	return pev->angles;
}


CBeam *CBeam::BeamCreate( const char *pSpriteName, int width )
{
	// Create a new entity with CBeam private data
	CBeam *pBeam = GetClassPtr( (CBeam *)NULL );
	pBeam->pev->classname = MAKE_STRING("beam");

	pBeam->BeamInit( pSpriteName, width );

	return pBeam;
}


void CBeam::BeamInit( const char *pSpriteName, int width )
{
	pev->flags |= FL_CUSTOMENTITY;
	SetColor( 255, 255, 255 );
	SetBrightness( 255 );
	SetNoise( 0 );
	SetFrame( 0 );
	SetScrollRate( 0 );
	pev->model = MAKE_STRING( pSpriteName );
	SetTexture( PRECACHE_MODEL( (char *)pSpriteName ) );
	SetWidth( width );
	pev->skin = 0;
	pev->sequence = 0;
	pev->rendermode = 0;
}


void CBeam::PointsInit( const Vector &start, const Vector &end )
{
	SetType( BEAM_POINTS );
	SetStartPos( start );
	SetEndPos( end );
	SetStartAttachment( 0 );
	SetEndAttachment( 0 );
	RelinkBeam();
}


void CBeam::HoseInit( const Vector &start, const Vector &direction )
{
	SetType( BEAM_HOSE );
	SetStartPos( start );
	SetEndPos( direction );
	SetStartAttachment( 0 );
	SetEndAttachment( 0 );
	RelinkBeam();
}


void CBeam::PointEntInit( const Vector &start, int endIndex )
{
	SetType( BEAM_ENTPOINT );
	SetStartPos( start );
	SetEndEntity( endIndex );
	SetStartAttachment( 0 );
	SetEndAttachment( 0 );
	RelinkBeam();
}

void CBeam::EntsInit( int startIndex, int endIndex )
{
	SetType( BEAM_ENTS );
	SetStartEntity( startIndex );
	SetEndEntity( endIndex );
	SetStartAttachment( 0 );
	SetEndAttachment( 0 );
	RelinkBeam();
}


void CBeam::RelinkBeam( void )
{
	const Vector &startPos = GetStartPos(), &endPos = GetEndPos();

	pev->mins.x = min( startPos.x, endPos.x );
	pev->mins.y = min( startPos.y, endPos.y );
	pev->mins.z = min( startPos.z, endPos.z );
	pev->maxs.x = max( startPos.x, endPos.x );
	pev->maxs.y = max( startPos.y, endPos.y );
	pev->maxs.z = max( startPos.z, endPos.z );
	pev->mins = pev->mins - pev->origin;
	pev->maxs = pev->maxs - pev->origin;

	UTIL_SetSize( pev, pev->mins, pev->maxs );
	UTIL_SetOrigin( pev, pev->origin );
}

#if 0
void CBeam::SetObjectCollisionBox( void )
{
	const Vector &startPos = GetStartPos(), &endPos = GetEndPos();

	pev->absmin.x = min( startPos.x, endPos.x );
	pev->absmin.y = min( startPos.y, endPos.y );
	pev->absmin.z = min( startPos.z, endPos.z );
	pev->absmax.x = max( startPos.x, endPos.x );
	pev->absmax.y = max( startPos.y, endPos.y );
	pev->absmax.z = max( startPos.z, endPos.z );
}
#endif


void CBeam::TriggerTouch( CBaseEntity *pOther )
{
	if ( pOther->pev->flags & (FL_CLIENT | FL_MONSTER) )
	{
		if ( pev->owner )
		{
			CBaseEntity *pOwner = CBaseEntity::Instance(pev->owner);
			pOwner->Use( pOther, this, USE_TOGGLE, 0 );
		}
		ALERT( at_console, "Firing targets!!!\n" );
	}
}


CBaseEntity *CBeam::RandomTargetname( const char *szName )
{
	int total = 0;

	CBaseEntity *pEntity = NULL;
	CBaseEntity *pNewEntity = NULL;
	while ((pNewEntity = UTIL_FindEntityByTargetname( pNewEntity, szName )) != NULL)
	{
		total++;
		if (RANDOM_LONG(0,total-1) < 1)
			pEntity = pNewEntity;
	}
	return pEntity;
}

void CBeam::DoSparks( const Vector &start, const Vector &end )
{
	if ( pev->spawnflags & (SF_BEAM_SPARKSTART|SF_BEAM_SPARKEND) )
	{
		if ( pev->spawnflags & SF_BEAM_SPARKSTART )
			FX_Explosion( start, EXPLOSION_SPARKSHOWER );
		if ( pev->spawnflags & SF_BEAM_SPARKEND )
			FX_Explosion( end, EXPLOSION_SPARKSHOWER );
	}
}

class CLightning : public CBeam
{
public:
	void	Spawn( void );
	void	Precache( void );
	void	KeyValue( KeyValueData *pkvd );
	void	Activate( void );

	void	EXPORT StrikeThink( void );
	void	EXPORT DamageThink( void );
	void	RandomArea( void );
	void	RandomPoint( Vector &vecSrc );
	void	Zap( const Vector &vecSrc, const Vector &vecDest );
	void	EXPORT StrikeUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void	EXPORT ToggleUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	
	inline BOOL ServerSide( void )
	{
		if ( m_life == 0 && !(pev->spawnflags & SF_BEAM_RING) )
			return TRUE;
		return FALSE;
	}

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	void	BeamUpdateVars( void );

	int		m_active;
	int		m_iszStartEntity;
	int		m_iszEndEntity;
	float	m_life;
	int		m_boltWidth;
	int		m_noiseAmplitude;
	int		m_brightness;
	int		m_speed;
	float	m_restrike;
	int		m_spriteTexture;
	int		m_iszSpriteName;
	int		m_frameStart;

	float	m_radius;
};

LINK_ENTITY_TO_CLASS( env_lightning, CLightning );
LINK_ENTITY_TO_CLASS( env_beam, CLightning );

// UNDONE: Jay -- This is only a test
#if _DEBUG
class CTripBeam : public CLightning
{
	void Spawn( void );
};
LINK_ENTITY_TO_CLASS( trip_beam, CTripBeam );

void CTripBeam::Spawn( void )
{
	CLightning::Spawn();
	SetTouch( TriggerTouch );
	pev->solid = SOLID_TRIGGER;
	RelinkBeam();
}
#endif



TYPEDESCRIPTION	CLightning::m_SaveData[] = 
{
	DEFINE_FIELD( CLightning, m_active, FIELD_INTEGER ),
	DEFINE_FIELD( CLightning, m_iszStartEntity, FIELD_STRING ),
	DEFINE_FIELD( CLightning, m_iszEndEntity, FIELD_STRING ),
	DEFINE_FIELD( CLightning, m_life, FIELD_FLOAT ),
	DEFINE_FIELD( CLightning, m_boltWidth, FIELD_INTEGER ),
	DEFINE_FIELD( CLightning, m_noiseAmplitude, FIELD_INTEGER ),
	DEFINE_FIELD( CLightning, m_brightness, FIELD_INTEGER ),
	DEFINE_FIELD( CLightning, m_speed, FIELD_INTEGER ),
	DEFINE_FIELD( CLightning, m_restrike, FIELD_FLOAT ),
	DEFINE_FIELD( CLightning, m_spriteTexture, FIELD_INTEGER ),
	DEFINE_FIELD( CLightning, m_iszSpriteName, FIELD_STRING ),
	DEFINE_FIELD( CLightning, m_frameStart, FIELD_INTEGER ),
	DEFINE_FIELD( CLightning, m_radius, FIELD_FLOAT ),
};

IMPLEMENT_SAVERESTORE( CLightning, CBeam );


void CLightning::Spawn( void )
{
	if ( FStringNull( m_iszSpriteName ) )
	{
		SetThink( SUB_Remove );
		return;
	}
	pev->solid = SOLID_NOT;							// Remove model & collisions
	Precache( );

	pev->dmgtime = gpGlobals->time;

	if ( ServerSide() )
	{
		SetThink( NULL );
		if ( pev->dmg > 0 )
		{
			SetThink( DamageThink );
			pev->nextthink = gpGlobals->time + 0.1;
		}
		if ( pev->targetname )
		{
			if ( !(pev->spawnflags & SF_BEAM_STARTON) )
			{
				pev->effects = EF_NODRAW;
				m_active = 0;
				pev->nextthink = 0;
			}
			else
				m_active = 1;
		
			SetUse( ToggleUse );
		}
	}
	else
	{
		m_active = 0;
		if ( !FStringNull(pev->targetname) )
		{
			SetUse( StrikeUse );
		}
		if ( FStringNull(pev->targetname) || FBitSet(pev->spawnflags, SF_BEAM_STARTON) )
		{
			SetThink( StrikeThink );
			pev->nextthink = gpGlobals->time + 1.0;
		}
	}
}

void CLightning::Precache( void )
{
	m_spriteTexture = PRECACHE_MODEL( (char *)STRING(m_iszSpriteName) );
	CBeam::Precache();
}


void CLightning::Activate( void )
{
	if ( ServerSide() )
		BeamUpdateVars();
}


void CLightning::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "LightningStart"))
	{
		m_iszStartEntity = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "LightningEnd"))
	{
		m_iszEndEntity = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "life"))
	{
		m_life = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "BoltWidth"))
	{
		m_boltWidth = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "NoiseAmplitude"))
	{
		m_noiseAmplitude = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "TextureScroll"))
	{
		m_speed = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "StrikeTime"))
	{
		m_restrike = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "texture"))
	{
		m_iszSpriteName = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "framestart"))
	{
		m_frameStart = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "Radius"))
	{
		m_radius = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "damage"))
	{
		pev->dmg = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBeam::KeyValue( pkvd );
}


void CLightning::ToggleUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( !ShouldToggle( useType, m_active ) )
		return;
	if ( m_active )
	{
		m_active = 0;
		pev->effects |= EF_NODRAW;
		pev->nextthink = 0;
	}
	else
	{
		m_active = 1;
		pev->effects &= ~EF_NODRAW;
		DoSparks( GetStartPos(), GetEndPos() );
		if ( pev->dmg > 0 )
		{
			pev->nextthink = gpGlobals->time;
			pev->dmgtime = gpGlobals->time;
		}
	}
}


void CLightning::StrikeUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( !ShouldToggle( useType, m_active ) )
		return;

	if ( m_active )
	{
		m_active = 0;
		SetThink( NULL );
	}
	else
	{
		SetThink( StrikeThink );
		pev->nextthink = gpGlobals->time + 0.1;
	}

	if ( !FBitSet( pev->spawnflags, SF_BEAM_TOGGLE ) )
		SetUse( NULL );
}


int IsPointEntity( CBaseEntity *pEnt )
{
	if ( !pEnt->pev->modelindex )
		return 1;
	if ( FClassnameIs( pEnt->pev, "info_target" ) || FClassnameIs( pEnt->pev, "info_landmark" ) || FClassnameIs( pEnt->pev, "path_corner" ) )
		return 1;

	return 0;
}


void CLightning::StrikeThink( void )
{
	if ( m_life != 0 )
	{
		if ( pev->spawnflags & SF_BEAM_RANDOM )
			pev->nextthink = gpGlobals->time + m_life + RANDOM_FLOAT( 0, m_restrike );
		else
			pev->nextthink = gpGlobals->time + m_life + m_restrike;
	}
	m_active = 1;

	if (FStringNull(m_iszEndEntity))
	{
		if (FStringNull(m_iszStartEntity))
		{
			RandomArea( );
		}
		else
		{
			CBaseEntity *pStart = RandomTargetname( STRING(m_iszStartEntity) );
			if (pStart != NULL)
				RandomPoint( pStart->pev->origin );
			else
				ALERT( at_console, "env_beam: unknown entity \"%s\"\n", STRING(m_iszStartEntity) );
		}
		return;
	}

	CBaseEntity *pStart = RandomTargetname( STRING(m_iszStartEntity) );
	CBaseEntity *pEnd = RandomTargetname( STRING(m_iszEndEntity) );

	if ( pStart != NULL && pEnd != NULL )
	{
		if ( IsPointEntity( pStart ) || IsPointEntity( pEnd ) )
		{
			if ( pev->spawnflags & SF_BEAM_RING)
			{
				// don't work
				return;
			}
		}

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			if ( IsPointEntity( pStart ) || IsPointEntity( pEnd ) )
			{
				if ( !IsPointEntity( pEnd ) )	// One point entity must be in pEnd
				{
					CBaseEntity *pTemp;
					pTemp = pStart;
					pStart = pEnd;
					pEnd = pTemp;
				}
				if ( !IsPointEntity( pStart ) )	// One sided
				{
					WRITE_BYTE( TE_BEAMENTPOINT );
					WRITE_SHORT( pStart->entindex() );
					WRITE_COORD( pEnd->pev->origin.x);
					WRITE_COORD( pEnd->pev->origin.y);
					WRITE_COORD( pEnd->pev->origin.z);
				}
				else
				{
					WRITE_BYTE( TE_BEAMPOINTS);
					WRITE_COORD( pStart->pev->origin.x);
					WRITE_COORD( pStart->pev->origin.y);
					WRITE_COORD( pStart->pev->origin.z);
					WRITE_COORD( pEnd->pev->origin.x);
					WRITE_COORD( pEnd->pev->origin.y);
					WRITE_COORD( pEnd->pev->origin.z);
				}


			}
			else
			{
				if ( pev->spawnflags & SF_BEAM_RING)
					WRITE_BYTE( TE_BEAMRING );
				else
					WRITE_BYTE( TE_BEAMENTS );
				WRITE_SHORT( pStart->entindex() );
				WRITE_SHORT( pEnd->entindex() );
			}

			WRITE_SHORT( m_spriteTexture );
			WRITE_BYTE( m_frameStart ); // framestart
			WRITE_BYTE( (int)pev->framerate); // framerate
			WRITE_BYTE( (int)(m_life*10.0) ); // life
			WRITE_BYTE( m_boltWidth );  // width
			WRITE_BYTE( m_noiseAmplitude );   // noise
			WRITE_BYTE( (int)pev->rendercolor.x );   // r, g, b
			WRITE_BYTE( (int)pev->rendercolor.y );   // r, g, b
			WRITE_BYTE( (int)pev->rendercolor.z );   // r, g, b
			WRITE_BYTE( pev->renderamt );	// brightness
			WRITE_BYTE( m_speed );		// speed
		MESSAGE_END();
		DoSparks( pStart->pev->origin, pEnd->pev->origin );
		if ( pev->dmg > 0 )
		{
			TraceResult tr;
			UTIL_TraceLine( pStart->pev->origin, pEnd->pev->origin, dont_ignore_monsters, NULL, &tr );
			BeamDamageInstant( &tr, pev->dmg );
		}
	}
}


void CBeam::BeamDamage( TraceResult *ptr )
{
	RelinkBeam();
	if ( ptr->flFraction != 1.0 && ptr->pHit != NULL )
	{
		CBaseEntity *pHit = CBaseEntity::Instance(ptr->pHit);
		if ( pHit )
		{
			ClearMultiDamage();
			pHit->TraceAttack( pev, pev->dmg * (gpGlobals->time - pev->dmgtime), (ptr->vecEndPos - pev->origin).Normalize(), ptr, DMG_ENERGYBEAM );
			ApplyMultiDamage( pev, pev );
			if ( pev->spawnflags & SF_BEAM_DECALS )
			{
				if ( pHit->IsBSPModel() )
					UTIL_DecalTrace( ptr, DECAL_BIGSHOT1 + RANDOM_LONG(0,4) );
			}
		}
	}
	pev->dmgtime = gpGlobals->time;
}


void CLightning::DamageThink( void )
{
	pev->nextthink = gpGlobals->time + 0.1;
	TraceResult tr;
	UTIL_TraceLine( GetStartPos(), GetEndPos(), dont_ignore_monsters, NULL, &tr );
	BeamDamage( &tr );
}



void CLightning::Zap( const Vector &vecSrc, const Vector &vecDest )
{
#if 1
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BEAMPOINTS);
		WRITE_COORD(vecSrc.x);
		WRITE_COORD(vecSrc.y);
		WRITE_COORD(vecSrc.z);
		WRITE_COORD(vecDest.x);
		WRITE_COORD(vecDest.y);
		WRITE_COORD(vecDest.z);
		WRITE_SHORT( m_spriteTexture );
		WRITE_BYTE( m_frameStart ); // framestart
		WRITE_BYTE( (int)pev->framerate); // framerate
		WRITE_BYTE( (int)(m_life*10.0) ); // life
		WRITE_BYTE( m_boltWidth );  // width
		WRITE_BYTE( m_noiseAmplitude );   // noise
		WRITE_BYTE( (int)pev->rendercolor.x );   // r, g, b
		WRITE_BYTE( (int)pev->rendercolor.y );   // r, g, b
		WRITE_BYTE( (int)pev->rendercolor.z );   // r, g, b
		WRITE_BYTE( pev->renderamt );	// brightness
		WRITE_BYTE( m_speed );		// speed
	MESSAGE_END();
#else
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE(TE_LIGHTNING);
		WRITE_COORD(vecSrc.x);
		WRITE_COORD(vecSrc.y);
		WRITE_COORD(vecSrc.z);
		WRITE_COORD(vecDest.x);
		WRITE_COORD(vecDest.y);
		WRITE_COORD(vecDest.z);
		WRITE_BYTE(10);
		WRITE_BYTE(50);
		WRITE_BYTE(40);
		WRITE_SHORT(m_spriteTexture);
	MESSAGE_END();
#endif
	DoSparks( vecSrc, vecDest );
}

void CLightning::RandomArea( void )
{
	int iLoops = 0;

	for (iLoops = 0; iLoops < 10; iLoops++)
	{
		Vector vecSrc = pev->origin;

		Vector vecDir1 = Vector( RANDOM_FLOAT( -1.0, 1.0 ), RANDOM_FLOAT( -1.0, 1.0 ),RANDOM_FLOAT( -1.0, 1.0 ) );
		vecDir1 = vecDir1.Normalize();
		TraceResult		tr1;
		UTIL_TraceLine( vecSrc, vecSrc + vecDir1 * m_radius, ignore_monsters, ENT(pev), &tr1 );

		if (tr1.flFraction == 1.0)
			continue;

		Vector vecDir2;
		do {
			vecDir2 = Vector( RANDOM_FLOAT( -1.0, 1.0 ), RANDOM_FLOAT( -1.0, 1.0 ),RANDOM_FLOAT( -1.0, 1.0 ) );
		} while (DotProduct(vecDir1, vecDir2 ) > 0);
		vecDir2 = vecDir2.Normalize();
		TraceResult		tr2;
		UTIL_TraceLine( vecSrc, vecSrc + vecDir2 * m_radius, ignore_monsters, ENT(pev), &tr2 );

		if (tr2.flFraction == 1.0)
			continue;

		if ((tr1.vecEndPos - tr2.vecEndPos).Length() < m_radius * 0.1)
			continue;

		UTIL_TraceLine( tr1.vecEndPos, tr2.vecEndPos, ignore_monsters, ENT(pev), &tr2 );

		if (tr2.flFraction != 1.0)
			continue;

		Zap( tr1.vecEndPos, tr2.vecEndPos );

		break;
	}
}


void CLightning::RandomPoint( Vector &vecSrc )
{
	int iLoops = 0;

	for (iLoops = 0; iLoops < 10; iLoops++)
	{
		Vector vecDir1 = Vector( RANDOM_FLOAT( -1.0, 1.0 ), RANDOM_FLOAT( -1.0, 1.0 ),RANDOM_FLOAT( -1.0, 1.0 ) );
		vecDir1 = vecDir1.Normalize();
		TraceResult		tr1;
		UTIL_TraceLine( vecSrc, vecSrc + vecDir1 * m_radius, ignore_monsters, ENT(pev), &tr1 );

		if ((tr1.vecEndPos - vecSrc).Length() < m_radius * 0.1)
			continue;

		if (tr1.flFraction == 1.0)
			continue;

		Zap( vecSrc, tr1.vecEndPos );
		break;
	}
}



void CLightning::BeamUpdateVars( void )
{
	int beamType;
	int pointStart, pointEnd;

	edict_t *pStart = FIND_ENTITY_BY_TARGETNAME ( NULL, STRING(m_iszStartEntity) );
	edict_t *pEnd = FIND_ENTITY_BY_TARGETNAME ( NULL, STRING(m_iszEndEntity) );
	pointStart = IsPointEntity( CBaseEntity::Instance(pStart) );
	pointEnd = IsPointEntity( CBaseEntity::Instance(pEnd) );

	pev->skin = 0;
	pev->sequence = 0;
	pev->rendermode = 0;
	pev->flags |= FL_CUSTOMENTITY;
	pev->model = m_iszSpriteName;
	SetTexture( m_spriteTexture );

	beamType = BEAM_ENTS;
	if ( pointStart || pointEnd )
	{
		if ( !pointStart )	// One point entity must be in pStart
		{
			edict_t *pTemp;
			// Swap start & end
			pTemp = pStart;
			pStart = pEnd;
			pEnd = pTemp;
			int swap = pointStart;
			pointStart = pointEnd;
			pointEnd = swap;
		}
		if ( !pointEnd )
			beamType = BEAM_ENTPOINT;
		else
			beamType = BEAM_POINTS;
	}

	SetType( beamType );
	if ( beamType == BEAM_POINTS || beamType == BEAM_ENTPOINT || beamType == BEAM_HOSE )
	{
		SetStartPos( pStart->v.origin );
		if ( beamType == BEAM_POINTS || beamType == BEAM_HOSE )
			SetEndPos( pEnd->v.origin );
		else
			SetEndEntity( ENTINDEX(pEnd) );
	}
	else
	{
		SetStartEntity( ENTINDEX(pStart) );
		SetEndEntity( ENTINDEX(pEnd) );
	}

	RelinkBeam();

	SetWidth( m_boltWidth );
	SetNoise( m_noiseAmplitude );
	SetFrame( m_frameStart );
	SetScrollRate( m_speed );
	if ( pev->spawnflags & SF_BEAM_SHADEIN )
		SetFlags( BEAM_FSHADEIN );
	else if ( pev->spawnflags & SF_BEAM_SHADEOUT )
		SetFlags( BEAM_FSHADEOUT );
}


LINK_ENTITY_TO_CLASS( env_laser, CLaser );

TYPEDESCRIPTION	CLaser::m_SaveData[] = 
{
	DEFINE_FIELD( CLaser, m_pSprite, FIELD_CLASSPTR ),
	DEFINE_FIELD( CLaser, m_iszSpriteName, FIELD_STRING ),
	DEFINE_FIELD( CLaser, m_firePosition, FIELD_POSITION_VECTOR ),
};

IMPLEMENT_SAVERESTORE( CLaser, CBeam );

void CLaser::Spawn( void )
{
	if ( FStringNull( pev->model ) )
	{
		SetThink( SUB_Remove );
		return;
	}
	pev->solid = SOLID_NOT;							// Remove model & collisions
	Precache( );

	SetThink( StrikeThink );
	pev->flags |= FL_CUSTOMENTITY;

	PointsInit( pev->origin, pev->origin );

	if ( !m_pSprite && m_iszSpriteName )
		m_pSprite = CSprite::SpriteCreate( STRING(m_iszSpriteName), pev->origin, TRUE );
	else
		m_pSprite = NULL;

	if ( m_pSprite )
		m_pSprite->SetTransparency( kRenderGlow, pev->rendercolor.x, pev->rendercolor.y, pev->rendercolor.z, pev->renderamt, pev->renderfx );

	if ( pev->targetname && !(pev->spawnflags & SF_BEAM_STARTON) )
		TurnOff();
	else
		TurnOn();
}

void CLaser::Precache( void )
{
	pev->modelindex = PRECACHE_MODEL( (char *)STRING(pev->model) );
	if ( m_iszSpriteName )
		PRECACHE_MODEL( (char *)STRING(m_iszSpriteName) );
}


void CLaser::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "LaserTarget"))
	{
		pev->message = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "width"))
	{
		SetWidth( atof(pkvd->szValue) );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "NoiseAmplitude"))
	{
		SetNoise( atoi(pkvd->szValue) );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "TextureScroll"))
	{
		SetScrollRate( atoi(pkvd->szValue) );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "texture"))
	{
		pev->model = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "EndSprite"))
	{
		m_iszSpriteName = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "framestart"))
	{
		pev->frame = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "damage"))
	{
		pev->dmg = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBeam::KeyValue( pkvd );
}


int CLaser::IsOn( void )
{
	if (pev->effects & EF_NODRAW)
		return 0;
	return 1;
}


void CLaser::TurnOff( void )
{
	pev->effects |= EF_NODRAW;
	pev->nextthink = 0;
	if ( m_pSprite )
		m_pSprite->TurnOff();
}


void CLaser::TurnOn( void )
{
	pev->effects &= ~EF_NODRAW;
	if ( m_pSprite )
		m_pSprite->TurnOn();
	pev->dmgtime = gpGlobals->time;
	pev->nextthink = gpGlobals->time;
}


void CLaser::RealUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	int active = IsOn();

	if ( !ShouldToggle( useType, active ) )
		return;
	if ( active )
	{
		TurnOff();
	}
	else
	{
		TurnOn();
	}
}


void CLaser::FireAtPoint( TraceResult &tr )
{
	SetEndPos( tr.vecEndPos );
	if ( m_pSprite )
		UTIL_SetOrigin( m_pSprite->pev, tr.vecEndPos );

	BeamDamage( &tr );
	DoSparks( GetStartPos(), tr.vecEndPos );
}

void CLaser::StrikeThink( void )
{
	CBaseEntity *pEnd = RandomTargetname( STRING(pev->message) );

	if ( pEnd )
		m_firePosition = pEnd->pev->origin;

	TraceResult tr;

	UTIL_TraceLine( pev->origin, m_firePosition, dont_ignore_monsters, NULL, &tr );
	FireAtPoint( tr );
	pev->nextthink = gpGlobals->time + 0.1;
}



class CGlow : public CPointEntity
{
public:
	void Spawn( void );
	void Think( void );
	void Animate( float frames );
	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	float		m_lastTime;
	float		m_maxFrame;
};

LINK_ENTITY_TO_CLASS( env_glow, CGlow );

TYPEDESCRIPTION	CGlow::m_SaveData[] = 
{
	DEFINE_FIELD( CGlow, m_lastTime, FIELD_TIME ),
	DEFINE_FIELD( CGlow, m_maxFrame, FIELD_FLOAT ),
};

IMPLEMENT_SAVERESTORE( CGlow, CPointEntity );

void CGlow::Spawn( void )
{
	pev->solid			= SOLID_NOT;
	pev->movetype		= MOVETYPE_NONE;
	pev->effects		= 0;
	pev->frame			= 0;

	UTIL_PrecacheModel( (char *)STRING(pev->model) );
	UTIL_SetModel( ENT(pev), STRING(pev->model) );

	m_maxFrame = (float) MODEL_FRAMES( pev->modelindex ) - 1;
	if ( m_maxFrame > 1.0 && pev->framerate != 0 )
		pev->nextthink	= gpGlobals->time + 0.1;

	m_lastTime = gpGlobals->time;
}


void CGlow::Think( void )
{
	Animate( pev->framerate * (gpGlobals->time - m_lastTime) );

	pev->nextthink		= gpGlobals->time + 0.1;
	m_lastTime			= gpGlobals->time;
}


void CGlow::Animate( float frames )
{ 
	if ( m_maxFrame > 0 )
		pev->frame = fmod( pev->frame + frames, m_maxFrame );
}


LINK_ENTITY_TO_CLASS( env_sprite, CSprite );

TYPEDESCRIPTION	CSprite::m_SaveData[] = 
{
	DEFINE_FIELD( CSprite, m_lastTime, FIELD_TIME ),
	DEFINE_FIELD( CSprite, m_maxFrame, FIELD_FLOAT ),
};

IMPLEMENT_SAVERESTORE( CSprite, CPointEntity );

void CSprite::Spawn( void )
{
	pev->solid			= SOLID_NOT;
	pev->movetype		= MOVETYPE_NONE;
	pev->effects		= 0;
	pev->frame			= 0;

	Precache();
	UTIL_SetModel( ENT(pev), STRING(pev->model) );

	m_maxFrame = (float) MODEL_FRAMES( pev->modelindex ) - 1;
	if ( pev->targetname && !(pev->spawnflags & SF_SPRITE_STARTON) )
		TurnOff();
	else
		TurnOn();
	
	// Worldcraft only sets y rotation, copy to Z
	if ( pev->angles.y != 0 && pev->angles.z == 0 )
	{
		pev->angles.z = pev->angles.y;
		pev->angles.y = 0;
	}
}


void CSprite::Precache( void )
{
	UTIL_PrecacheModel( (char *)STRING(pev->model) );

	// Reset attachment after save/restore
	if ( pev->aiment )
		SetAttachment( pev->aiment, pev->body );
	else
	{
		// Clear attachment
		pev->skin = 0;
		pev->body = 0;
	}
}


void CSprite::SpriteInit( const char *pSpriteName, const Vector &origin )
{
	pev->model = MAKE_STRING(pSpriteName);
	pev->origin = origin;
	Spawn();
}

CSprite *CSprite::SpriteCreate( const char *pSpriteName, const Vector &origin, BOOL animate )
{
	CSprite *pSprite = GetClassPtr( (CSprite *)NULL );
	pSprite->SpriteInit( pSpriteName, origin );
	pSprite->pev->classname = MAKE_STRING("env_sprite");
	pSprite->pev->solid = SOLID_NOT;
	pSprite->pev->movetype = MOVETYPE_NOCLIP;
	if ( animate )
		pSprite->TurnOn();

	return pSprite;
}


void CSprite::AnimateThink( void )
{
	Animate( pev->framerate * (gpGlobals->time - m_lastTime) );

	pev->nextthink		= gpGlobals->time + 0.1;
	m_lastTime			= gpGlobals->time;
}

void CSprite::AnimateUntilDead( void )
{
	if ( gpGlobals->time > pev->dmgtime )
		UTIL_Remove(this);
	else
	{
		AnimateThink();
		pev->nextthink = gpGlobals->time;
	}
}

void CSprite::Expand( float scaleSpeed, float fadeSpeed )
{
	pev->speed = scaleSpeed;
	pev->health = fadeSpeed;
	SetThink( ExpandThink );

	pev->nextthink	= gpGlobals->time;
	m_lastTime		= gpGlobals->time;
}


void CSprite::ExpandThink( void )
{
	float frametime = gpGlobals->time - m_lastTime;
	pev->scale += pev->speed * frametime;
	pev->renderamt -= pev->health * frametime;
	if ( pev->renderamt <= 0 )
	{
		pev->renderamt = 0;
		UTIL_Remove( this );
	}
	else
	{
		pev->nextthink		= gpGlobals->time + 0.1;
		m_lastTime			= gpGlobals->time;
	}
}


void CSprite::Animate( float frames )
{ 
	pev->frame += frames;
	if ( pev->frame > m_maxFrame )
	{
		if ( pev->spawnflags & SF_SPRITE_ONCE )
		{
			TurnOff();
		}
		else
		{
			if ( m_maxFrame > 0 )
				pev->frame = fmod( pev->frame, m_maxFrame );
		}
	}
}


void CSprite::TurnOff( void )
{
	pev->effects = EF_NODRAW;
	pev->nextthink = 0;
}


void CSprite::TurnOn( void )
{
	pev->effects = 0;
	if ( (pev->framerate && m_maxFrame > 1.0) || (pev->spawnflags & SF_SPRITE_ONCE) )
	{
		SetThink( AnimateThink );
		pev->nextthink = gpGlobals->time;
		m_lastTime = gpGlobals->time;
	}
	pev->frame = 0;
}


void CSprite::RealUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	int on = pev->effects != EF_NODRAW;
	if ( ShouldToggle( useType, on ) )
	{
		if ( on )
		{
			TurnOff();
		}
		else
		{
			TurnOn();
		}
	}
}


// Screen shake
class CShake : public CPointEntity
{
public:
	void	Spawn( void );
	void	RealUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void	KeyValue( KeyValueData *pkvd );

	inline	float	Amplitude( void ) { return pev->scale; }
	inline	float	Frequency( void ) { return pev->dmg_save; }
	inline	float	Duration( void ) { return pev->dmg_take; }
	inline	float	Radius( void ) { return pev->dmg; }

	inline	void	SetAmplitude( float amplitude ) { pev->scale = amplitude; }
	inline	void	SetFrequency( float frequency ) { pev->dmg_save = frequency; }
	inline	void	SetDuration( float duration ) { pev->dmg_take = duration; }
	inline	void	SetRadius( float radius ) { pev->dmg = radius; }
private:
};

LINK_ENTITY_TO_CLASS( env_shake, CShake );

// pev->scale is amplitude
// pev->dmg_save is frequency
// pev->dmg_take is duration
// pev->dmg is radius
// radius of 0 means all players
// NOTE: UTIL_ScreenShake() will only shake players who are on the ground

#define SF_SHAKE_EVERYONE	0x0001		// Don't check radius
// UNDONE: These don't work yet
#define SF_SHAKE_DISRUPT	0x0002		// Disrupt controls
#define SF_SHAKE_INAIR		0x0004		// Shake players in air

void CShake::Spawn( void )
{
	pev->solid			= SOLID_NOT;
	pev->movetype		= MOVETYPE_NONE;
	pev->effects		= 0;
	pev->frame			= 0;
	
	if ( pev->spawnflags & SF_SHAKE_EVERYONE )
		pev->dmg = 0;
}


void CShake::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "amplitude"))
	{
		SetAmplitude( atof(pkvd->szValue) );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "frequency"))
	{
		SetFrequency( atof(pkvd->szValue) );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "duration"))
	{
		SetDuration( atof(pkvd->szValue) );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "radius"))
	{
		SetRadius( atof(pkvd->szValue) );
		pkvd->fHandled = TRUE;
	}
	else
		CPointEntity::KeyValue( pkvd );
}


void CShake::RealUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	UTIL_ScreenShake( pev->origin, Amplitude(), Frequency(), Duration(), Radius() );
}


class CFade : public CPointEntity
{
public:
	void	Spawn( void );
	void	RealUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void	KeyValue( KeyValueData *pkvd );

	inline	float	Duration( void ) { return pev->dmg_take; }
	inline	float	HoldTime( void ) { return pev->dmg_save; }

	inline	void	SetDuration( float duration ) { pev->dmg_take = duration; }
	inline	void	SetHoldTime( float hold ) { pev->dmg_save = hold; }
private:
};

LINK_ENTITY_TO_CLASS( env_fade, CFade );

// pev->dmg_take is duration
// pev->dmg_save is hold duration
#define SF_FADE_IN				0x0001		// Fade in, not out
#define SF_FADE_MODULATE		0x0002		// Modulate, don't blend
#define SF_FADE_ONLYONE			0x0004

void CFade::Spawn( void )
{
	pev->solid			= SOLID_NOT;
	pev->movetype		= MOVETYPE_NONE;
	pev->effects		= 0;
	pev->frame			= 0;
}


void CFade::KeyValue( KeyValueData *pkvd )
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
	else
		CPointEntity::KeyValue( pkvd );
}


void CFade::RealUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	int fadeFlags = 0;
	
	if ( !(pev->spawnflags & SF_FADE_IN) )
		fadeFlags |= FFADE_OUT;

	if ( pev->spawnflags & SF_FADE_MODULATE )
		fadeFlags |= FFADE_MODULATE;

	if ( pev->spawnflags & SF_FADE_ONLYONE )
	{
		if ( pActivator->IsNetClient() )
		{
			UTIL_ScreenFade( pActivator, pev->rendercolor, Duration(), HoldTime(), pev->renderamt, fadeFlags );
		}
	}
	else
	{
		UTIL_ScreenFadeAll( pev->rendercolor, Duration(), HoldTime(), pev->renderamt, fadeFlags );
	}
	SUB_UseTargets( this, USE_TOGGLE, 0 );
}


class CMessage : public CPointEntity
{
public:
	void	Spawn( void );
	void	Precache( void );
	void	RealUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void	KeyValue( KeyValueData *pkvd );
private:
};

LINK_ENTITY_TO_CLASS( env_message, CMessage );


void CMessage::Spawn( void )
{
	Precache();

	pev->solid			= SOLID_NOT;
	pev->movetype		= MOVETYPE_NONE;

	switch( pev->impulse )
	{
	case 1: // Medium radius
		pev->speed = ATTN_STATIC;
		break;
	
	case 2:	// Large radius
		pev->speed = ATTN_NORM;
		break;

	case 3:	//EVERYWHERE
		pev->speed = ATTN_NONE;
		break;
	
	default:
	case 0: // Small radius
		pev->speed = ATTN_IDLE;
		break;
	}
	pev->impulse = 0;

	// No volume, use normal
	if ( pev->scale <= 0 )
		pev->scale = 1.0;
}


void CMessage::Precache( void )
{
	if ( pev->noise )
		PRECACHE_SOUND( (char *)STRING(pev->noise) );
}

void CMessage::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "messagesound"))
	{
		pev->noise = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "messagevolume"))
	{
		pev->scale = atof(pkvd->szValue) * 0.1;
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "messageattenuation"))
	{
		pev->impulse = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CPointEntity::KeyValue( pkvd );
}


void CMessage::RealUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	CBaseEntity *pPlayer = NULL;

	if ( pev->spawnflags & SF_MESSAGE_ALL )
		UTIL_ShowMessageAll( STRING(pev->message) );
	else
	{
		if ( pActivator && pActivator->IsPlayer() )
			pPlayer = pActivator;
		else
		{
			pPlayer = CBaseEntity::Instance( g_engfuncs.pfnPEntityOfEntIndex( 1 ) );
		}
		if ( pPlayer )
			UTIL_ShowMessage( STRING(pev->message), pPlayer );
	}
	if ( pev->noise )
	{
		EMIT_SOUND( edict(), CHAN_BODY, STRING(pev->noise), pev->scale, pev->speed );
	}
	if ( pev->spawnflags & SF_MESSAGE_ONCE )
		UTIL_Remove( this );

	SUB_UseTargets( this, USE_TOGGLE, 0 );
}



//=========================================================
// FunnelEffect
//=========================================================
class CEnvFunnel : public CBaseDelay
{
public:
	void	Spawn( void );
	void	Precache( void );
	void	RealUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	int		m_iSprite;	// Don't save, precache
};

void CEnvFunnel :: Precache ( void )
{
	m_iSprite = PRECACHE_MODEL ( "sprites/anim_spr2.spr" );
}

LINK_ENTITY_TO_CLASS( env_funnel, CEnvFunnel );

void CEnvFunnel::RealUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_LARGEFUNNEL );
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		WRITE_SHORT( m_iSprite );

		if ( pev->spawnflags & SF_FUNNEL_REVERSE )// funnel flows in reverse?
		{
			WRITE_SHORT( 1 );
		}
		else
		{
			WRITE_SHORT( 0 );
		}


	MESSAGE_END();

	SetThink( SUB_Remove );
	pev->nextthink = gpGlobals->time;
}

void CEnvFunnel::Spawn( void )
{
	Precache();
	pev->solid = SOLID_NOT;
	pev->effects = EF_NODRAW;
}

// =================== rain tutorial ========================

void CRainSettings::Spawn()
{
	pev->solid = SOLID_NOT;
	pev->movetype	= MOVETYPE_NONE;
	pev->effects |= EF_NODRAW;
}

void CRainSettings::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "m_flDistance"))
	{
		Rain_Distance = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "m_iMode"))
	{
		Rain_Mode = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else	
	{
		CBaseEntity::KeyValue( pkvd );
	}
}

LINK_ENTITY_TO_CLASS( rain_settings, CRainSettings );
LINK_ENTITY_TO_CLASS( rain_modify, CRainModify );

TYPEDESCRIPTION	CRainSettings::m_SaveData[] = 
{
	DEFINE_FIELD( CRainSettings, Rain_Distance, FIELD_FLOAT ),
	DEFINE_FIELD( CRainSettings, Rain_Mode, FIELD_INTEGER ),
};
IMPLEMENT_SAVERESTORE( CRainSettings, CBaseEntity );

void CRainModify::Spawn()
{
	pev->solid = SOLID_NOT;
	pev->movetype	= MOVETYPE_NONE;
	pev->effects |= EF_NODRAW;

	if (FStringNull(pev->targetname))
		pev->spawnflags |= 1;
}

void CRainModify::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "m_iDripsPerSecond"))
	{
		Rain_Drips = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "m_flWindX"))
	{
		Rain_windX = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "m_flWindY"))
	{
		Rain_windY = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "m_flRandX"))
	{
		Rain_randX = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "m_flRandY"))
	{
		Rain_randY = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "m_flTime"))
	{
		fadeTime = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else	
	{
		CBaseEntity::KeyValue( pkvd );
	}
}

void CRainModify::RealUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if (pev->spawnflags & 1)
		return; // constant

	if (gpGlobals->deathmatch)
	{
		ALERT(at_console, "Rain error: only static rain in multiplayer\n");
		return; // not in multiplayer
	}

	CBasePlayer *pPlayer;
	pPlayer = (CBasePlayer *)CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(1));

	if (fadeTime)
	{ // write to 'ideal' settings
		pPlayer->Rain_ideal_dripsPerSecond = Rain_Drips;
		pPlayer->Rain_ideal_randX = Rain_randX;
		pPlayer->Rain_ideal_randY = Rain_randY;
		pPlayer->Rain_ideal_windX = Rain_windX;
		pPlayer->Rain_ideal_windY = Rain_windY;

		pPlayer->Rain_endFade = gpGlobals->time + fadeTime;
		pPlayer->Rain_nextFadeUpdate = gpGlobals->time + 1;
	}
	else
	{
		pPlayer->Rain_dripsPerSecond = Rain_Drips;
		pPlayer->Rain_randX = Rain_randX;
		pPlayer->Rain_randY = Rain_randY;
		pPlayer->Rain_windX = Rain_windX;
		pPlayer->Rain_windY = Rain_windY;

		pPlayer->Rain_needsUpdate = 1;
	}
}

TYPEDESCRIPTION	CRainModify::m_SaveData[] = 
{
	DEFINE_FIELD( CRainModify, fadeTime, FIELD_FLOAT ),
	DEFINE_FIELD( CRainModify, Rain_Drips, FIELD_INTEGER ),
	DEFINE_FIELD( CRainModify, Rain_randX, FIELD_FLOAT ),
	DEFINE_FIELD( CRainModify, Rain_randY, FIELD_FLOAT ),
	DEFINE_FIELD( CRainModify, Rain_windX, FIELD_FLOAT ),
	DEFINE_FIELD( CRainModify, Rain_windY, FIELD_FLOAT ),
};
IMPLEMENT_SAVERESTORE( CRainModify, CBaseEntity );

//=========================================================
// Beverage Dispenser
// overloaded pev->frags, is now a flag for whether or not a can is stuck in the dispenser. 
// overloaded pev->health, is now how many cans remain in the machine.
//=========================================================
class CEnvBeverage : public CBaseDelay
{
public:
	void	Spawn( void );
	void	RealUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
};

LINK_ENTITY_TO_CLASS( env_beverage, CEnvBeverage );

void CEnvBeverage::RealUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( pev->frags != 0 || pev->health <= 0 )
	{
		// no more cans while one is waiting in the dispenser, or if I'm out of cans.
		return;
	}

	CBaseEntity *pCan = CBaseEntity::Create( "item_sodacan", pev->origin, pev->angles, edict() );	

	if ( pev->skin == 6 )
	{
		// random
		pCan->pev->skin = RANDOM_LONG( 0, 5 );
	}
	else
	{
		pCan->pev->skin = pev->skin;
	}

	pev->frags = 1;
	pev->health--;
}

void CEnvBeverage::Spawn( void )
{
	Precache();
	pev->solid = SOLID_NOT;
	pev->effects = EF_NODRAW;
	pev->frags = 0;

	if ( pev->health == 0 )
	{
		pev->health = 10;
	}
}

//=========================================================
// Soda can
//=========================================================
class CItemSoda : public CBaseEntity
{
public:
	void	Spawn( void );
	void	EXPORT CanThink ( void );
	void	EXPORT CanTouch ( CBaseEntity *pOther );
};

LINK_ENTITY_TO_CLASS( item_sodacan, CItemSoda );

void CItemSoda::Spawn( void )
{
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_TOSS;
	pev->body = 11;

	SET_MODEL ( ENT(pev), "models/w_items.mdl" );
	UTIL_SetSize ( pev, Vector ( 0, 0, 0 ), Vector ( 0, 0, 0 ) );
	
	SetThink (CanThink);
	pev->nextthink = gpGlobals->time + 0.5;
}

void CItemSoda::CanThink ( void )
{
	EMIT_SOUND (ENT(pev), CHAN_WEAPON, "weapons/g_bounce3.wav", 1, ATTN_NORM );

	pev->solid = SOLID_TRIGGER;
	UTIL_SetSize ( pev, Vector ( -8, -8, 0 ), Vector ( 8, 8, 8 ) );
	SetThink ( NULL );
	SetTouch ( CanTouch );
}

void CItemSoda::CanTouch ( CBaseEntity *pOther )
{
	if ( !pOther->IsPlayer() )
	{
		return;
	}

	pOther->TakeHealth( 1, DMG_GENERIC );

	if ( !FNullEnt( pev->owner ) )
	{
		pev->owner->v.frags = 0;
	}

	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->effects = EF_NODRAW;
	SetTouch ( NULL );
	SetThink ( SUB_Remove );
	pev->nextthink = gpGlobals->time;
}

//=================================================================
// CBaseLava
// lava balls spawner
//=================================================================
#define SF_LAVA_START_OFF 0x0001

class CBaseLava : public CBaseEntity
{
public:
	void Spawn(void);
	void KeyValue(KeyValueData *pkvd);
	void EXPORT LavaThink(void);
	void EXPORT LavaUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	BOOL bState;
};

LINK_ENTITY_TO_CLASS(func_lava, CBaseLava);

void CBaseLava::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "updatetime"))
	{
		pev->animtime = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "type"))
	{
		pev->impulse = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue(pkvd);
}

void CBaseLava::Spawn(void)
{
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->takedamage = DAMAGE_NO;

	SET_MODEL(ENT(pev), STRING(pev->model));
	UTIL_SetSize(pev, pev->mins, pev->maxs);
	SetThink(LavaThink);
	SetUse(LavaUse);

	if (!pev->animtime)
		pev->animtime = 1.0;

	if (FBitSet(pev->spawnflags, SF_LAVA_START_OFF))
	{
		bState = FALSE;
	}
	else
	{
		bState = TRUE;
		pev->nextthink = gpGlobals->time + 0.1;
	}
}

void CBaseLava::LavaThink(void)
{
	if (!bState)
		return;

	Vector vecSrc;
	vecSrc.x = pev->mins.x + RANDOM_FLOAT(0, pev->size.x);
	vecSrc.y = pev->mins.y + RANDOM_FLOAT(0, pev->size.y);
	vecSrc.z = pev->mins.z + RANDOM_FLOAT(0, pev->size.z);

	if (pev->impulse == 2)
		FX_Explosion( vecSrc, EXPLOSION_LAVA );
	else
		FX_Explosion( vecSrc, EXPLOSION_LAVA_FLAME );
	pev->nextthink = gpGlobals->time + pev->animtime;
}

void CBaseLava::LavaUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (!ShouldToggle(useType, bState))
		return;

	if (bState == TRUE)
	{
		bState = FALSE;
		pev->nextthink = gpGlobals->time;
	}
	else
	{
		bState = TRUE;
		pev->nextthink = gpGlobals->time + 0.1;
	}
}

//=======================================================================
// 		   Map particle effect (Xash)
//=======================================================================
#define SF_START_ON	0x0001
#define	STATE_OFF	0
#define STATE_ON	1

class CBaseParticle : public CPointEntity
{
public:
	void Spawn( void );
	void Precache( void ){ pev->netname = UTIL_PrecacheAurora(pev->message); }
	void KeyValue( KeyValueData *pkvd );
	void RealUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	virtual void SendClientData(CBasePlayer *pClient, int msgtype);

	void Switch( void );
	BOOL bState;
	EHANDLE	m_hActivator;
	EHANDLE	m_hTarget;
};

LINK_ENTITY_TO_CLASS( env_particle, CBaseParticle );

void CBaseParticle :: KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "aurora"))
	{
		pev->message = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CPointEntity::KeyValue( pkvd );
}

void CBaseParticle::Switch( void )
{
	int renderfx = (bState ? kRenderFxAurora : kRenderFxNone );
	if( pev->target )
	{
		CBaseEntity *pTarget = UTIL_FindEntityByTargetname(NULL, STRING(pev->target) );
		while (pTarget)
		{
			UTIL_SetAurora( pTarget, pev->netname);
			pTarget->pev->renderfx = renderfx;
			pTarget = UTIL_FindEntityByTargetname(pTarget, STRING(pev->target) );
		}
	}
	else
	{
		UTIL_SetAurora( this, pev->netname);
		pev->renderfx = renderfx;
	}
}

void CBaseParticle::Spawn( void )
{
	Precache();
	UTIL_SetModel(edict(), "sprites/animglow01.spr");
	UTIL_SetOrigin(pev, pev->origin);
	pev->solid = SOLID_NOT;
	pev->rendermode = kRenderTransAdd;
	pev->renderamt = 1;

	if(pev->spawnflags & SF_START_ON )
		bState = STATE_ON;
	else
		bState = STATE_OFF;
}

void CBaseParticle::RealUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (!ShouldToggle(useType, bState))
		return;

	bState = !bState;

	if (bState)
		SendClientData(NULL, MSG_PVS);
	else
		SendClientData(NULL, MSG_ALL);
}

void CBaseParticle::SendClientData(CBasePlayer *pClient, int msgtype)
{
	if (!bState)
	{
		if (msgtype == MSG_ONE)
			return;

		bState = STATE_OFF;
		Switch();
	}

	if (bState == USE_TOGGLE)
	{
		if(bState == STATE_ON) bState = USE_OFF;
		else bState = USE_ON;
	}
	if (bState == USE_ON)
	{
		bState = STATE_ON;
		Switch();
	}
	else if (bState == USE_OFF)
	{
		bState = STATE_OFF;
		Switch();
	}
}

// =================== ENV_MODEL =============================================

#define SF_ENVMODEL_OFF			1
#define SF_ENVMODEL_DROPTOFLOOR		2
#define SF_ENVMODEL_SOLID		4

TYPEDESCRIPTION CEnvModel::m_SaveData[] =
{
	DEFINE_FIELD( CEnvModel, m_iszSequence_On, FIELD_STRING ),
	DEFINE_FIELD( CEnvModel, m_iszSequence_Off, FIELD_STRING ),
	DEFINE_FIELD( CEnvModel, m_iAction_On, FIELD_INTEGER ),
	DEFINE_FIELD( CEnvModel, m_iAction_Off, FIELD_INTEGER ),
};
IMPLEMENT_SAVERESTORE( CEnvModel, CBaseAnimating );

LINK_ENTITY_TO_CLASS( env_model, CEnvModel );

void CEnvModel::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "m_iszSequence_On"))
	{
		m_iszSequence_On = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "m_iszSequence_Off"))
	{
		m_iszSequence_Off = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "m_iAction_On"))
	{
		m_iAction_On = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "m_iAction_Off"))
	{
		m_iAction_Off = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
	{
		CBaseAnimating::KeyValue( pkvd );
	}
}

void CEnvModel :: Spawn( void )
{
	Precache();
	SET_MODEL( ENT(pev), STRING(pev->model) );
	UTIL_SetOrigin(pev, pev->origin);

	if (pev->spawnflags & SF_ENVMODEL_SOLID)
	{
		pev->solid = SOLID_SLIDEBOX;
		UTIL_SetSize(pev, Vector(-10, -10, -10), Vector(10, 10, 10));	//LRCT
	}

	if (pev->spawnflags & SF_ENVMODEL_DROPTOFLOOR)
	{
		pev->origin.z += 1;
		DROP_TO_FLOOR ( ENT(pev) );
	}
	SetBoneController( 0, 0 );
	SetBoneController( 1, 0 );
	SetSequence();
	
	pev->nextthink = gpGlobals->time + 0.1;
}

void CEnvModel::Precache( void )
{
	PRECACHE_MODEL( (char *)STRING(pev->model) );
}

BOOL CEnvModel::GetState( void )
{ 
	if (pev->spawnflags & SF_ENVMODEL_OFF)
		return STATE_OFF;
	else
		return STATE_ON;
}

void CEnvModel::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if (ShouldToggle(useType, !(pev->spawnflags & SF_ENVMODEL_OFF)))
	{
		if (pev->spawnflags & SF_ENVMODEL_OFF)
			pev->spawnflags &= ~SF_ENVMODEL_OFF;
		else
			pev->spawnflags |= SF_ENVMODEL_OFF;

		SetSequence();
		pev->nextthink = gpGlobals->time + 0.1;
	}
}

void CEnvModel::Think( void )
{
	int iTemp;
	StudioFrameAdvance ( ); // set m_fSequenceFinished if necessary

	if (m_fSequenceFinished && !m_fSequenceLoops)
	{
		if (pev->spawnflags & SF_ENVMODEL_OFF)
			iTemp = m_iAction_Off;
		else
			iTemp = m_iAction_On;

		switch (iTemp)
		{
		case 2: // change state
			if (pev->spawnflags & SF_ENVMODEL_OFF)
				pev->spawnflags &= ~SF_ENVMODEL_OFF;
			else
				pev->spawnflags |= SF_ENVMODEL_OFF;
			SetSequence();
			break;
		default: //remain frozen
			return;
		}
	}
	pev->nextthink = gpGlobals->time + 0.1;
}

void CEnvModel :: SetSequence( void )
{
	int iszSeq;

	if (pev->spawnflags & SF_ENVMODEL_OFF)
		iszSeq = m_iszSequence_Off;
	else
		iszSeq = m_iszSequence_On;

	if (!iszSeq)
		return;
	pev->sequence = LookupSequence( STRING( iszSeq ));

	if (pev->sequence == -1)
	{
		if (pev->targetname)
			ALERT( at_error, "env_model %s: unknown sequence \"%s\"\n", STRING( pev->targetname ), STRING( iszSeq ));
		else
			ALERT( at_error, "env_model: unknown sequence \"%s\"\n", STRING( pev->targetname ), STRING( iszSeq ));
		pev->sequence = 0;
	}

	pev->frame = 0;
	ResetSequenceInfo( );

	if (pev->spawnflags & SF_ENVMODEL_OFF)
	{
		if (m_iAction_Off == 1)
			m_fSequenceLoops = 1;
		else
			m_fSequenceLoops = 0;
	}
	else
	{
		if (m_iAction_On == 1)
			m_fSequenceLoops = 1;
		else
			m_fSequenceLoops = 0;
	}
}
