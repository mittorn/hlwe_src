#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "effects.h"
#include "projectiles.h"
#include "gamerules.h"
#include "player.h"

#define TURRET_TURNRATE	30// angles per 0.1 second
#define TURRET_MAXWAIT	15// seconds turret will stay active w/o a target

typedef enum
{
	TURRET_ANIM_NONE = 0,
	TURRET_ANIM_FIRE,
	TURRET_ANIM_FIRE_ION,
	TURRET_ANIM_FIRE_MISSILE,
	TURRET_ANIM_SPIN,
	TURRET_ANIM_DEPLOY,
	TURRET_ANIM_RETIRE,
	TURRET_ANIM_DIE,
} TURRET_ANIM;

class CBaseTurret : public CBaseMonster
{
public:
	void Spawn(void);
	virtual int Classify(void);
	void GibMonster( void ) {}

	void EXPORT ActiveThink(void);
	void EXPORT SearchThink(void);
	void EXPORT AutoSearchThink(void);

	void EXPORT Deploy(void);
	void EXPORT Initialize(void);
	void EXPORT Materialize(void);
	virtual void Ping(void);
	CBaseEntity* BestVisibleEnemy ( void );
	void EXPORT TurretDeath(void);

	// other functions
	void SetTurretAnim(TURRET_ANIM anim);
	int MoveTurret(void);
	virtual void Shoot(Vector &vecSrc, Vector &vecDirToEnemy) { };
	virtual void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);

	int RealTakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType );
	int	m_iDeployHeight;
	int 	m_iMinPitch;
	int 	m_iBaseTurnRate;// angles per second
	float 	m_fTurnRate;// actual turn rate
	int	m_iOn;

	Vector 	m_vecLastSight;
	float 	m_flLastSight;	// Last time we saw a target
	float 	m_flMaxWait;	// Max time to seach w/o a target

	float	ShieldRegen;
	float	flMaxArmor;
	int	fTookDamage;

	// movement
	float	m_flStartYaw;
	Vector	m_vecCurAngles;
	Vector	m_vecGoalAngles;
	float	m_flPingTime;
	float	m_flShootTime;
	
	CBasePlayer	*pBuilder;
	virtual void UpdateInfoForBuilder ( void ) {};
};

void CBaseTurret::Spawn()
{ 
	pev->nextthink		= gpGlobals->time + 1;
	pev->movetype		= MOVETYPE_BOUNCE;
	pev->sequence		= 0;
	pev->frame		= 0;
	pev->solid		= SOLID_SLIDEBOX;
	pev->takedamage		= DAMAGE_YES;
	pev->angles.x 		= 0;
	pev->angles.y 		= 0;
	pev->angles.z 		= 0;

	pev->gravity 		= 2;
	pev->friction 		= 1;

	pev->scale = 0;
	pev->renderfx = kRenderFxGlowShell;
	pev->rendermode = kRenderTransAdd;
	pev->renderamt = 0;

	ResetSequenceInfo();
	SetBoneController(0, 0);
	SetBoneController(1, 0);
	m_flFieldOfView = VIEW_FIELD_FULL;

	if (pev->owner)
		pBuilder = (CBasePlayer *)CBaseEntity::Instance(pev->owner);
	pev->owner = NULL;
}

void CBaseTurret::Initialize(void)
{
	m_iOn = 0;
	SetBoneController( 0, 0 );
	SetBoneController( 1, 0 );

	if (m_iBaseTurnRate == 0)
	m_iBaseTurnRate = TURRET_TURNRATE;

	if (m_flMaxWait == 0)
	m_flMaxWait = TURRET_MAXWAIT;

	m_flStartYaw = pev->angles.y;
	m_vecGoalAngles.x = 0;

	m_flLastSight = gpGlobals->time + m_flMaxWait;

	FX_Explosion( pev->origin+ Vector(0,0,20), EXPLOSION_TURRET_SPAWN );
	SetThink(Materialize);		
	pev->nextthink = gpGlobals->time + 0.05;
}

void CBaseTurret::Materialize(void)
{
	pev->scale += 0.01;
	pev->renderamt += 3;

	if (pev->scale >= 1)
	{
		pev->scale = 1;
	}
	if (pev->renderamt >= 255)
	{
		pev->renderamt = 255;
		pev->renderfx = ~kRenderFxGlowShell;
		pev->rendermode = kRenderNormal;
		SetThink(AutoSearchThink);		
	}
	pev->nextthink = gpGlobals->time + 0.02;
}

void CBaseTurret::Ping( void )
{
	if ( gpGlobals->time >= m_flPingTime )
	{
		EMIT_SOUND(ENT(pev), CHAN_ITEM, "turret/tu_ping.wav", 1, ATTN_NORM);
		m_flPingTime = gpGlobals->time + 1;
	}
	if (pev->armorvalue < flMaxArmor)
	{
		ShieldRegen++;
		if (ShieldRegen == 10)
		{
			ShieldRegen = 0;
			pev->armorvalue += 1;
			if (pev->armorvalue > flMaxArmor)
			pev->armorvalue = flMaxArmor;
		}
	}
}

void CBaseTurret::ActiveThink(void)
{
	int fAttack = 0;
	Vector vecDirToEnemy;

	pev->nextthink = gpGlobals->time + 0.1;
	StudioFrameAdvance( );

	if ((!m_iOn) || (m_hEnemy == NULL))
	{
		m_hEnemy = NULL;
		m_flLastSight = gpGlobals->time + m_flMaxWait;
		SetThink(SearchThink);
		return;
	}

	if ( !m_hEnemy->IsAlive())
	{
		if (!m_flLastSight)
			m_flLastSight = gpGlobals->time;
		else
		{
			if (gpGlobals->time > m_flLastSight)
			{ 
				m_hEnemy = NULL;
				m_flLastSight = gpGlobals->time + m_flMaxWait;
				SetThink(SearchThink);
			return;
			}
		}
	}

	Vector vecMid = pev->origin + pev->view_ofs;
	Vector vecMidEnemy = m_hEnemy->BodyTarget( vecMid );
	// Look for our current enemy
	int fEnemyVisible = FBoxVisible(pev, m_hEnemy->pev, vecMidEnemy );	
	vecDirToEnemy = vecMidEnemy - vecMid;	// calculate dir and dist to enemy
	float flDistToEnemy = vecDirToEnemy.Length();
	Vector vec = UTIL_VecToAngles(vecMidEnemy - vecMid);	

	// Current enmey is not visible.
	if (!fEnemyVisible || (flDistToEnemy > 16384))
	{
		if (!m_flLastSight)
			m_flLastSight = gpGlobals->time;
		else
		{
			// Should we look for a new target?
			if (gpGlobals->time > m_flLastSight)
			{
				m_hEnemy = NULL;
				m_flLastSight = gpGlobals->time + m_flMaxWait;
				SetThink(SearchThink);
				return;
			}
		}
		fEnemyVisible = 0;
	}
	else
		m_vecLastSight = vecMidEnemy;

	UTIL_MakeAimVectors(m_vecCurAngles);	

	Vector vecLOS = vecDirToEnemy;
	vecLOS = vecLOS.Normalize();

	// Is the Gun looking at the target
	if (DotProduct(vecLOS, gpGlobals->v_forward) <= 0.996) // 5 degree slop
		fAttack = FALSE;
	else
		fAttack = TRUE;

	if (fAttack)
	{
		Vector vecSrc, vecAng;
		GetAttachment( 0, vecSrc, vecAng );
		Shoot(vecSrc + Vector(0,0,-7), gpGlobals->v_forward );
	}
	else
	{
		SetTurretAnim(TURRET_ANIM_SPIN);
	}

	if (fEnemyVisible)
	{
		if (vec.y > 360)
			vec.y -= 360;

		if (vec.y < 0)
			vec.y += 360;

		if (vec.x < -180)
			vec.x += 360;

		if (vec.x > 180)
			vec.x -= 360;

			if (vec.x > 90)
				vec.x = 90;
			else if (vec.x < m_iMinPitch)
				vec.x = m_iMinPitch;

		m_vecGoalAngles.y = vec.y;
		m_vecGoalAngles.x = vec.x;
	}
	MoveTurret();
}

void CBaseTurret::Deploy(void)
{
	pev->nextthink = gpGlobals->time + 0.1;
	StudioFrameAdvance( );

	if (pev->sequence != TURRET_ANIM_DEPLOY)
	{
		m_iOn = 1;
		SetTurretAnim(TURRET_ANIM_DEPLOY);
		EMIT_SOUND(ENT(pev), CHAN_BODY, "turret/tu_deploy.wav", 1, ATTN_NORM);
		SUB_UseTargets( this, USE_ON, 0 );
	}

	if (m_fSequenceFinished)
	{
		pev->maxs.z = m_iDeployHeight;
		pev->mins.z = -m_iDeployHeight;
		UTIL_SetSize(pev, pev->mins, pev->maxs);

		m_vecCurAngles.x = 0;

		SetTurretAnim(TURRET_ANIM_SPIN);
		pev->framerate = 0;
		SetThink(SearchThink);
	}
	m_flLastSight = gpGlobals->time + m_flMaxWait;
}

void CBaseTurret::SetTurretAnim(TURRET_ANIM anim)
{
	if (pev->sequence != anim)
	{
		switch(anim)
		{
		case TURRET_ANIM_FIRE:
		case TURRET_ANIM_SPIN:
			if (pev->sequence != TURRET_ANIM_FIRE && pev->sequence != TURRET_ANIM_SPIN)
			{
				pev->frame = 0;
			}
			break;
		default:
			pev->frame = 0;
			break;
		}

		pev->sequence = anim;
		ResetSequenceInfo( );

		switch(anim)
		{
		case TURRET_ANIM_RETIRE:
			pev->frame		= 255;
			pev->framerate		= -1.0;
			break;
		case TURRET_ANIM_DIE:
			pev->framerate		= 1.0;
			break;
		}
	}
}

void CBaseTurret::SearchThink(void)
{
    // ensure rethink
    SetTurretAnim(TURRET_ANIM_SPIN);
    StudioFrameAdvance( );
    pev->nextthink = gpGlobals->time + 0.1;

    Ping( );

    // If we have a target and we're still healthy
    if (m_hEnemy != NULL)
    {
        if (!m_hEnemy->IsAlive() )
            m_hEnemy = NULL;// Dead enemy forces a search for new one
    }

    // Acquire Target
    if (m_hEnemy == NULL)
    {
        Look(16384);
        m_hEnemy = BestVisibleEnemy();
    }

    if (m_hEnemy != NULL)
    {
        m_flLastSight = 0;
        SetThink(ActiveThink);
    }
    else
    {
        // generic hunt for new victims
        m_vecGoalAngles.y = (m_vecGoalAngles.y + 0.1 * m_fTurnRate);
        if (m_vecGoalAngles.y >= 360)
        m_vecGoalAngles.y -= 360;
        MoveTurret();
    }
}

void CBaseTurret::AutoSearchThink(void)
{
	// ensure rethink
	StudioFrameAdvance( );
	pev->nextthink = gpGlobals->time + 0.3;

	// If we have a target and we're still healthy
	if (m_hEnemy != NULL)
	{
		if (!m_hEnemy->IsAlive())
			m_hEnemy = NULL;// Dead enemy forces a search for new one
	}

	// Acquire Target
	if (m_hEnemy == NULL)
	{
		Look(16384);
		m_hEnemy = BestVisibleEnemy();
	}

	if (m_hEnemy != NULL)
		SetThink(Deploy);
}

int CBaseTurret::MoveTurret(void)
{
	int state = 0;
	if (m_vecCurAngles.x != m_vecGoalAngles.x)
	{
		float flDir = m_vecGoalAngles.x > m_vecCurAngles.x ? 1 : -1 ;

		m_vecCurAngles.x += 0.1 * m_fTurnRate * flDir;

		// if we started below the goal, and now we're past, peg to goal
		if (flDir == 1)
		{
			if (m_vecCurAngles.x > m_vecGoalAngles.x)
				m_vecCurAngles.x = m_vecGoalAngles.x;
		} 
		else
		{
			if (m_vecCurAngles.x < m_vecGoalAngles.x)
				m_vecCurAngles.x = m_vecGoalAngles.x;
		}

			SetBoneController(1, -m_vecCurAngles.x);
		state = 1;
	}

	if (m_vecCurAngles.y != m_vecGoalAngles.y)
	{
		float flDir = m_vecGoalAngles.y > m_vecCurAngles.y ? 1 : -1 ;
		float flDist = fabs(m_vecGoalAngles.y - m_vecCurAngles.y);
		
		if (flDist > 180)
		{
			flDist = 360 - flDist;
			flDir = -flDir;
		}
		if (flDist > 30)
		{
			if (m_fTurnRate < m_iBaseTurnRate * 10)
			{
				m_fTurnRate += m_iBaseTurnRate;
			}
		}
		else if (m_fTurnRate > 45)
		{
			m_fTurnRate -= m_iBaseTurnRate;
		}
		else
		{
			m_fTurnRate += m_iBaseTurnRate;
		}

		m_vecCurAngles.y += 0.1 * m_fTurnRate * flDir;

		if (m_vecCurAngles.y < 0)
			m_vecCurAngles.y += 360;
		else if (m_vecCurAngles.y >= 360)
			m_vecCurAngles.y -= 360;

		if (flDist < (0.05 * m_iBaseTurnRate))
			m_vecCurAngles.y = m_vecGoalAngles.y;

			SetBoneController(0, m_vecCurAngles.y - pev->angles.y );
		state = 1;
	}

	if (!state)
		m_fTurnRate = m_iBaseTurnRate;

	return state;
}

int	CBaseTurret::Classify ( void )
{
	return CLASS_MACHINE;
}

void CBaseTurret :: TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType)
{
	if (!pev->takedamage)
		return;

	AddMultiDamage( pevAttacker, this, flDamage, bitsDamageType );
}

int CBaseTurret::RealTakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType)
{
	if ( bitsDamageType & (DMG_RADIATION | DMG_BLIND | DMG_NERVEGAS | DMG_DROWN) )
		flDamage = 0;

	if ( (bitsDamageType & (DMG_REGENARMOR)) )
	{
		pev->health += min (flDamage, pev->max_health - pev->health);
		flDamage = 0;
	}

	if (!m_iOn && pev->rendermode == kRenderNormal)
	{
		SetThink( Deploy );
		SetUse( NULL );
		pev->nextthink = gpGlobals->time + 0.1;
	}

	if (pev->armorvalue && !(bitsDamageType &(DMG_RADIATION | DMG_DROWN | DMG_BLIND | DMG_NERVEGAS | DMG_IGNOREARMOR | DMG_REGENARMOR)))
	{
		float flNew = flDamage * 0.01;
		float flArmor = (flDamage - flNew) * 0.33;

		// Does this use more armor than we have?
		if (flArmor > pev->armorvalue)
		{
			flArmor = pev->armorvalue;
			flArmor *= (1/0.33);
			flNew = flDamage - flArmor;
			pev->armorvalue = 0;
			flDamage = flNew;
		}
		else
		{
			pev->armorvalue -= flArmor;
			flDamage = flNew;
		}
	}

	if (!pev->armorvalue && bitsDamageType &(DMG_ENERGYBEAM | DMG_GENERIC))
	flDamage = flDamage*0.75;

	if (!pev->armorvalue && bitsDamageType &(DMG_SHOCK | DMG_IGNITE | DMG_BURN | DMG_BULLETMAGNUM))
	flDamage = flDamage*0.6;

	if (!pev->armorvalue && bitsDamageType &(DMG_SLASH | DMG_BULLET | DMG_BULLETBUCKSHOT | DMG_FREEZE))
	flDamage = flDamage*0.45;

	if (!pev->armorvalue && bitsDamageType &(DMG_ACID | DMG_POISON))
	flDamage = flDamage*0.2;

	fTookDamage = CBaseMonster::RealTakeDamage(pevInflictor, pevAttacker, (int)flDamage, bitsDamageType);

	if (pev->health <= 0)
	{
		pev->health = 0;
		pev->takedamage = DAMAGE_NO;
		pev->dmgtime = gpGlobals->time;
		SetUse(NULL);
		SetThink(TurretDeath);
		SUB_UseTargets( this, USE_ON, 0 );
		pev->nextthink = gpGlobals->time + 0.1;
		UpdateInfoForBuilder();
		return 0;
	}
	UpdateInfoForBuilder();
return fTookDamage;
}

void CBaseTurret ::TurretDeath( void )
{
	BOOL iActive = FALSE;
	StudioFrameAdvance( );

	if (pev->deadflag != DEAD_DEAD)
	{
		pev->deadflag = DEAD_DEAD;
		SetTurretAnim(TURRET_ANIM_DIE); 
	}

	FX_Explosion( pev->origin + Vector(0,0,50), EXPLOSION_TURRET );
	RadiusDamage ( pev, pBuilder->pev, pev->dmg, CLASS_NONE, DMG_BLAST );
	
	pev->framerate = 0;
	SetThink( NULL );
	UTIL_Remove( this );
}

CBaseEntity *CBaseTurret :: BestVisibleEnemy ( void )
{
	CBaseEntity	*pReturn;
	CBaseEntity	*pNextEnt;
	int		iNearest;
	int		iDist;
	int		iBestRelationship;

	iNearest = 16384;// so first visible entity will become the closest.
	pNextEnt = m_pLink;
	pReturn = NULL;
	iBestRelationship = R_NO;

	while ( pNextEnt != NULL )
	{
		if ( pNextEnt->IsAlive() && !((g_pGameRules->IsTeamplay() && !strcmp(pNextEnt->TeamID(),pBuilder->TeamID())) || (g_pGameRules->IsDeathmatch() && pNextEnt == pBuilder)))
		{
			if ( IRelationship( pNextEnt) > iBestRelationship )
			{
				iBestRelationship = IRelationship ( pNextEnt );
				iNearest = ( pNextEnt->pev->origin - pev->origin ).Length();
				pReturn = pNextEnt;
			}
			else if ( IRelationship( pNextEnt) == iBestRelationship )
			{
				iDist = ( pNextEnt->pev->origin - pev->origin ).Length();
				
				if ( iDist <= iNearest )
				{
					iNearest = iDist;
					iBestRelationship = IRelationship ( pNextEnt );
					pReturn = pNextEnt;
				}
			}
		}
		pNextEnt = pNextEnt->m_pLink;
	}
	return pReturn;
}

//=========================================================
// Sentry Turret - 9MM Minigun 
//=========================================================
class CSentry : public CBaseTurret
{
public:
	void Spawn( );
	void Shoot(Vector &vecSrc, Vector &vecDirToEnemy);
	void UpdateInfoForBuilder ( void ) {if (pBuilder) pBuilder->m_fTurretSentry = pev->health;}
};
LINK_ENTITY_TO_CLASS(monster_turret_sentry, CSentry);

void CSentry::Spawn()
{ 
	CBaseTurret::Spawn();
	SET_MODEL(ENT(pev), "models/turret_sentry.mdl");
	pev->health		= 150;
	pev->max_health		= pev->health;
	pev->armorvalue		= 50;
	flMaxArmor	        = 50;
	pev->dmg 		= 100;
	m_HackedGunPos		= Vector(0,0,48);
	pev->view_ofs.z		= 48;
	m_flMaxWait 		= 1E6;
	m_iDeployHeight 	= 64;
	m_iMinPitch		= -60;
	UTIL_SetSize(pev, Vector(-16, -16, 0), Vector(16, 16, 32));
	SetThink(Initialize);	
	pev->nextthink = gpGlobals->time + 1; 
}

void CSentry::Shoot(Vector &vecSrc, Vector &vecDirToEnemy)
{
	if ( gpGlobals->time >= m_flShootTime )
	{
		SetTurretAnim(TURRET_ANIM_FIRE);
		FireMagnumBullets(vecSrc, vecDirToEnemy, VECTOR_CONE_6DEGREES, 16384, BULLET_9MM, pBuilder->pev);
		m_flShootTime = gpGlobals->time + 0.1;
	}
}

//=========================================================
// Missile Turret 
//=========================================================
class CMissileT : public CBaseTurret
{
public:
	void Spawn( );
	void Shoot(Vector &vecSrc, Vector &vecDirToEnemy);
	void UpdateInfoForBuilder ( void ) {if (pBuilder) pBuilder->m_fTurretMissile = pev->health;}
};
LINK_ENTITY_TO_CLASS(monster_turret_missile, CMissileT);

void CMissileT::Spawn()
{ 
	CBaseTurret::Spawn();
	SET_MODEL(ENT(pev), "models/turret_sentry.mdl");
	pev->body		= 1;
	pev->health		= 200;
	pev->max_health		= pev->health;
	pev->armorvalue		= 100;
	flMaxArmor	        = 100;
	pev->dmg 		= 200;
	m_HackedGunPos		= Vector(0,0,48);
	pev->view_ofs.z		= 48;
	m_flMaxWait 		= 1E6;
	m_iDeployHeight 	= 64;
	m_iMinPitch		= -60;
	UTIL_SetSize(pev, Vector(-16, -16, 0), Vector(16, 16, 32));
	SetThink(Initialize);	
	pev->nextthink = gpGlobals->time + 1; 
}

void CMissileT::Shoot(Vector &vecSrc, Vector &vecDirToEnemy)
{
	if ( gpGlobals->time >= m_flShootTime )
	{
		UTIL_MakeVectors ( m_vecCurAngles );
		CDumbfire::ShootDumbfire(pBuilder->pev, vecSrc, gpGlobals->v_forward * 1000 );
		SetTurretAnim(TURRET_ANIM_FIRE_MISSILE);
		m_flShootTime = gpGlobals->time + 1.5;
	}
}

//=========================================================
// Ion Turret 
//=========================================================

class CIonT : public CBaseTurret
{
public:
	void Spawn( );
	void Shoot(Vector &vecSrc, Vector &vecDirToEnemy);
	void UpdateInfoForBuilder ( void ) {if (pBuilder) pBuilder->m_fTurretIon = pev->health;}
private:
	CBeam	*m_pBeam;
};
LINK_ENTITY_TO_CLASS(monster_turret_ion, CIonT);

void CIonT::Spawn()
{ 
	CBaseTurret::Spawn();
	SET_MODEL(ENT(pev), "models/turret_sentry.mdl");
	pev->body		= 2;
	pev->health		= 250;
	pev->max_health		= pev->health;
	pev->dmg 		= 200;
	pev->armorvalue		= 150;
	flMaxArmor	        = 150;
	m_HackedGunPos		= Vector(0,0,48);
	pev->view_ofs.z		= 48;
	m_flMaxWait 		= 1E6;
	m_iDeployHeight 	= 64;
	m_iMinPitch		= -60;
	UTIL_SetSize(pev, Vector(-16, -16, 0), Vector(16, 16, 32));
	SetThink(Initialize);	
	pev->nextthink = gpGlobals->time + 1; 
}

void CIonT::Shoot(Vector &vecSrc, Vector &vecDirToEnemy)
{
	if ( gpGlobals->time >= m_flShootTime )
	{
		FX_FireGun(pev->v_angle, entindex(), 200, 0, FIREGUN_IONTURRET );
		FireBeam(vecSrc, vecDirToEnemy, BEAM_IONTURRET, 0, pBuilder->pev );
		SetTurretAnim(TURRET_ANIM_FIRE_ION);
		m_flShootTime = gpGlobals->time + 0.8;
	}
}

//========================== LA-LA-LA =============================

void DeactivateTurrets( CBasePlayer *pOwner )
{
	edict_t *pFind = FIND_ENTITY_BY_CLASSNAME( NULL, "monster_turret_sentry" );

	while ( !FNullEnt( pFind ) )
	{
		CBaseEntity *pEnt = CBaseEntity::Instance( pFind );

		if ( pEnt )
		{
			CBaseTurret *pTurret = (CBaseTurret*)pEnt;

			if ( pTurret->pBuilder == pOwner )
			{
				pTurret->RealTakeDamage(VARS(pFind), VARS(pFind), 10000, DMG_IGNOREARMOR);
			}
		}

		pFind = FIND_ENTITY_BY_CLASSNAME( pFind, "monster_turret_sentry" );
	}

	pFind = FIND_ENTITY_BY_CLASSNAME( NULL, "monster_turret_missile" );

	while ( !FNullEnt( pFind ) )
	{
		CBaseEntity *pEnt = CBaseEntity::Instance( pFind );

		if ( pEnt )
		{
			CBaseTurret *pTurret = (CBaseTurret*)pEnt;

			if ( pTurret->pBuilder == pOwner )
			{
				pTurret->RealTakeDamage(VARS(pFind), VARS(pFind), 10000, DMG_IGNOREARMOR);
			}
		}

		pFind = FIND_ENTITY_BY_CLASSNAME( pFind, "monster_turret_missile" );
	}

	pFind = FIND_ENTITY_BY_CLASSNAME( NULL, "monster_turret_ion" );

	while ( !FNullEnt( pFind ) )
	{
		CBaseEntity *pEnt = CBaseEntity::Instance( pFind );

		if ( pEnt )
		{
			CBaseTurret *pTurret = (CBaseTurret*)pEnt;

			if ( pTurret->pBuilder == pOwner )
			{
				pTurret->RealTakeDamage(VARS(pFind), VARS(pFind), 10000, DMG_IGNOREARMOR);
			}
		}

		pFind = FIND_ENTITY_BY_CLASSNAME( pFind, "monster_turret_ion" );
	}
}