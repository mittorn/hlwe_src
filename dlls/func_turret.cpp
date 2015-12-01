#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "effects.h"
#include "weapons.h"
#include "explode.h"
#include "monsters.h"
#include "animation.h"
#include "player.h"
#include "projectiles.h"

class CFuncMachinegun : public CBaseAnimating
{
public:
	void	Spawn( void );
	void	Precache( void );
	void	KeyValue( KeyValueData *pkvd );
	void	RealUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	virtual int	ObjectCaps( void ) { return FCAP_IMPULSE_USE; }
	BOOL	OnControls( entvars_t *pevTest );
	void	PostFrame( CBasePlayer *pActivator );
	void	UpdateClientData( CBasePlayer *client );

	bool IsRespawnable ( void ) { return true; }
	void Killed(entvars_t *pevAttacker, int iGib);
	void RealReSpawn( void );
	void EXPORT CheckSpawn( void );

	void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
	int RealTakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);
	virtual int Classify(void);

	virtual int	Save( CSave &save );
	virtual int	Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	TANK_PROJECTILE		m_projectileType;
	float	m_flPointHeight;
	float	m_flDistUp, m_flDistFwd;
	float	m_flNextAttack;
	float	m_fireRate;
	int	m_iShouldUpdate;
	int	m_iAmmo;
	int	m_spread;
	int 	i;
	BOOL 	m_fInZoom;
	BOOL 	m_AllowZoom;
};

LINK_ENTITY_TO_CLASS( func_turret, CFuncMachinegun );

enum turret_anim
{
	TURRET_IDLE,
	TURRET_SHOOT,
	TURRET_SHOOT_SOLID
};


TYPEDESCRIPTION	CFuncMachinegun::m_SaveData[] = 
{
	DEFINE_FIELD( CFuncMachinegun, m_flPointHeight, FIELD_FLOAT ),
	DEFINE_FIELD( CFuncMachinegun, m_flDistUp, FIELD_FLOAT ),
	DEFINE_FIELD( CFuncMachinegun, m_flDistFwd, FIELD_FLOAT ),
	DEFINE_FIELD( CFuncMachinegun, m_flNextAttack, FIELD_TIME ),
	DEFINE_FIELD( CFuncMachinegun, m_fireRate, FIELD_FLOAT ),
	DEFINE_FIELD( CFuncMachinegun, m_iAmmo, FIELD_INTEGER ),
	DEFINE_FIELD( CFuncMachinegun, m_AllowZoom, FIELD_INTEGER ),
	DEFINE_FIELD( CFuncMachinegun, m_projectileType, FIELD_INTEGER ),
	DEFINE_FIELD( CFuncMachinegun, m_spread, FIELD_INTEGER ),
};
IMPLEMENT_SAVERESTORE( CFuncMachinegun, CBaseAnimating );

void CFuncMachinegun::Precache()
{
	PRECACHE_MODEL( (char *)STRING(pev->model) );

	pev->sequence	= TURRET_IDLE;
	pev->frame	= 0;
	pev->framerate	= 1;
	ResetSequenceInfo();
	m_fSequenceLoops = TRUE;
	m_iShouldUpdate = 1;
}

void CFuncMachinegun::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "baseheight"))
	{
		m_flPointHeight = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "distfwd"))
	{
		m_flDistFwd = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "distup"))
	{
		m_flDistUp = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "firerate"))
	{
		m_fireRate = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "firespread"))
	{
		m_spread = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "projectile"))
	{
		m_projectileType = (TANK_PROJECTILE)atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "ammo"))
	{
		m_iAmmo = atoi(pkvd->szValue);
		if (m_iAmmo > 999) m_iAmmo = 999;
		if (m_iAmmo < -1) m_iAmmo = -1;
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "strength"))
	{
		pev->max_health = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "shield"))
	{
		pev->armortype = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "zoom"))
	{
		m_AllowZoom = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseAnimating::KeyValue( pkvd );
}

void CFuncMachinegun::Spawn()
{
	Precache();
	pev->movetype	= MOVETYPE_FLY;
	pev->solid	= SOLID_BBOX;
	pev->takedamage = DAMAGE_AIM;
	pev->angles.x	= 0;
	pev->renderfx 	= 51; // хак для рендерера, чтобы он не интерполировал контроллеры
	pev->health 	= pev->max_health;
	pev->armorvalue = pev->armortype;
	pev->weapons 	= m_iAmmo;//for respawn
	pev->origin.z  += 1;
	DROP_TO_FLOOR ( ENT(pev) );
	pev->oldorigin 	= pev->origin;

	if (pev->model)
		SET_MODEL( ENT(pev), STRING(pev->model) );
	else
		SET_MODEL(ENT(pev), "models/error.mdl");

	UTIL_SetSize( pev, Vector(-25,-25,0), Vector(25,25,35));

	ResetSequenceInfo();
	m_fSequenceLoops = TRUE;
	InitBoneControllers();
}

BOOL CFuncMachinegun::OnControls( entvars_t *pevTest )
{
	Vector vecToPlayer = pevTest->origin - pev->origin;
	vecToPlayer.z = 0;

	if ( vecToPlayer.Length() > 75 )
		return FALSE;

	if ( pev->health <=0 )
		return FALSE;

	UTIL_MakeVectors(pev->angles);
	vecToPlayer = vecToPlayer.Normalize();

	if (DotProduct(vecToPlayer, gpGlobals->v_forward) > -0.7)
		return FALSE;
	
	return TRUE;
}

void CFuncMachinegun::PostFrame( CBasePlayer *pActivator )
{
	Vector plAngles = pActivator->pev->angles;
	while (plAngles.y < 0) plAngles.y += 360;

	float yawAngle = plAngles.y - pev->angles.y;
	float pitchAngle = pActivator->pev->angles.x * -3;
	SetBoneController( 0, yawAngle );
	SetBoneController( 1, pitchAngle );
	StudioFrameAdvance();

	// return to idle after fire anim
	if (m_fSequenceFinished)
	{
		pev->sequence	= TURRET_IDLE;
		pev->frame	= 0;
		ResetSequenceInfo();
		m_fSequenceLoops = TRUE;
	}

	if ( gpGlobals->time < m_flNextAttack )
		return;

	if ( pActivator->pev->button & IN_ATTACK && (m_iAmmo > 0 || m_iAmmo == -1))
	{
	// fire
		Vector vecForward, vecSrc, vecAngles;
		vecAngles = pActivator->pev->angles;
		vecAngles.x = vecAngles.x * -3; // invert anf scale pitch
		UTIL_MakeVectorsPrivate( vecAngles, vecForward, NULL, NULL );
		GetAttachment(0, vecSrc, vecAngles);

		pev->sequence = TURRET_SHOOT_SOLID; // sounds, muzzleflashes, and shells will go by anim event
		pev->frame = 0;
		ResetSequenceInfo();
		m_fSequenceLoops = FALSE;
	
		switch( m_projectileType )
		{
			default:
			case TANK_BULLET_9MM:
				FireMagnumBullets(vecSrc, vecForward, VECTOR_CONE_6DEGREES, 16384, BULLET_9MM, pActivator->pev);
			break;

			case TANK_BULLET_12MM:
				FireMagnumBullets(vecSrc, vecForward, VECTOR_CONE_6DEGREES, 16384, BULLET_12MM, pActivator->pev);
			break;

			case TANK_BULLET_14MM:
				FireMagnumBullets(vecSrc, vecForward, VECTOR_CONE_7DEGREES, 16384, BULLET_14MM, pActivator->pev);
			break;

			case TANK_BULLET_BUCKSHOT:
				for ( i = 0; i < 8; i++ )
				FireMagnumBullets(vecSrc, vecForward, VECTOR_CONE_10DEGREES, 16384, BULLET_BUCKSHOT, pActivator->pev);
			break;

			case TANK_BULLET_12G:
				for ( i = 0; i < 4; i++ )
				FireMagnumBullets(vecSrc, vecForward, VECTOR_CONE_15DEGREES, 16384, BULLET_12G, pActivator->pev);
			break;

			case TANK_BULLET_57MM:
				FireMagnumBullets(vecSrc, vecForward, VECTOR_CONE_2DEGREES, 16384, BULLET_57mm, pActivator->pev);
			break;

			case TANK_BULLET_762NATO:
				FireMagnumBullets(vecSrc, vecForward, VECTOR_CONE_3DEGREES, 16384, BULLET_762Nato, pActivator->pev);
			break;

			case TANK_BULLET_338MAGNUM:
				FireMagnumBullets(vecSrc, vecForward, VECTOR_CONE_0DEGREES, 16384, BULLET_338Magnum, pActivator->pev);
			break;

			case TANK_BULLET_762X54:
				FireMagnumBullets(vecSrc, vecForward, VECTOR_CONE_4DEGREES, 16384, BULLET_762x54, pActivator->pev);
			break;

			case TANK_BULLET_86MM:
				FireMagnumBullets(vecSrc, vecForward, VECTOR_CONE_8DEGREES, 16384, BULLET_86mm, pActivator->pev);
			break;

			case TANK_BULLET_32MM:
				FireMagnumBullets(vecSrc, vecForward, VECTOR_CONE_5DEGREES, 16384, BULLET_32mm, pActivator->pev);
			break;

			case TANK_BULLET_127MM:
				FireMagnumBullets(vecSrc, vecForward, VECTOR_CONE_1DEGREES, 16384, BULLET_127MM, pActivator->pev);
			break;

			case TANK_BULLET_BFG:
				for ( i = 0; i < 8; i++ )
				FireMagnumBullets(vecSrc, vecForward, VECTOR_CONE_40DEGREES, 16384, BULLET_BFG, pActivator->pev);
			break;

			case TANK_BULLET_NAIL:
				FireMagnumBullets(vecSrc, vecForward, VECTOR_CONE_0DEGREES, 16384, BULLET_NAILGUN, pActivator->pev);
			break;

			case TANK_BULLET_BOLT:
				CBolt::ShootBolt(pActivator->pev, vecSrc + vecForward*16, vecForward * 5000, 0 );
			break;

			case TANK_ENERGY_HOMINGSHOCK:
				CShock::ShootShock(pActivator->pev, vecSrc + vecForward*16, vecForward * 400, FALSE );
			break;

			case TANK_ENERGY_SHOCK:
				CShock::ShootShock(pActivator->pev, vecSrc + vecForward*16, vecForward * 700, TRUE );
			break;

			case TANK_ENERGY_PLASMA:
				CPlasma::ShootPlasma(pActivator->pev, vecSrc + vecForward*16, vecForward * 20000 );
			break;

			case TANK_ENERGY_PLASMALARGE:
				CPlasma2::ShootPlasma2(pActivator->pev, vecSrc + vecForward*16, vecForward * 20000 );
			break;

			case TANK_ENERGY_FLAME:
				CFlame::ShootFlame(pActivator->pev, vecSrc + vecForward*16, vecForward * 900 );
			break;

			case TANK_ENERGY_ECHARGE:
				CEnergycharge::ShootEnergycharge(pActivator->pev, vecSrc + vecForward*16, vecForward * 8000);
			break;

			case TANK_ENERGY_FROSTBALL:
				CFrostball::ShootFrostball(pActivator->pev, vecSrc + vecForward*16, vecForward * 5000);
			break;

			case TANK_ENERGY_GLUON:
				CGluon::ShootGluon(pActivator->pev, vecSrc + vecForward*16, vecForward * 2500 );
			break;

			case TANK_ENERGY_GLUONLARGE:
				CGluon2::ShootGluon2(pActivator->pev, vecSrc + vecForward*16, vecForward * 6800, 10);
			break;
	
			case TANK_ENERGY_DISPBALL_HALF:
				CDispball::ShootDispball(pActivator->pev, vecSrc + vecForward*16, vecForward * 3000, 1);
			break;

			case TANK_ENERGY_DISPBALL_FULL:
				CDispball::ShootDispball(pActivator->pev, vecSrc + vecForward*16, vecForward * 1500, 2);
			break;

			case TANK_ENERGY_PBOLT:
				CPBolt::ShootPBolt(pActivator->pev, vecSrc + vecForward*16, vecForward * 15000 );
			break;

			case TANK_ROCKET_XBOLT:
				CBolt::ShootBolt(pActivator->pev, vecSrc + vecForward*16, vecForward * 7500, 2);
			break;

			case TANK_ROCKET_CLUSTER:
				CClMomma::ShootClusterMomma(pActivator->pev, vecSrc + vecForward*16, vecForward * 1250, 3 );
			break;

			case TANK_ROCKET_M203GREN:
				CM203grenade::ShootM203grenade(pActivator->pev, vecSrc + vecForward*16, vecForward * 1800);
			break;

			case TANK_ROCKET_AK74GREN:
				CAK74grenade::ShootAK74grenade(pActivator->pev, vecSrc + vecForward*16, vecForward * 4000);
			break;

			case TANK_ROCKET_HANDGREN:
				CGrenade::ShootGrenade(pActivator->pev, vecSrc + vecForward*16, vecForward * 1500, 3.5);
			break;

			case TANK_ROCKET_NGGREN:
				CNGgrenade::ShootNGgrenade(pActivator->pev, vecSrc + vecForward*16, vecForward * 2000, 3 );
			break;

			case TANK_ROCKET_U2:
				CU2Momma::ShootU2Momma(pActivator->pev, vecSrc + vecForward*16, vecForward * 900, 1 );
			break;

			case TANK_ROCKET_TESLAGREN:
				CTeslagren::ShootTeslagren(pActivator->pev, vecSrc + vecForward*16, vecForward * 3000);
			break;

			case TANK_ROCKET_MMISSILE:
				CMmissile::ShootMmissile(pActivator->pev, vecSrc + vecForward*16, vecForward * 5250 );
			break;

			case TANK_ROCKET_DEMOLITION:
				CWhlProjectile::ShootWhlProjectile(pActivator->pev, vecSrc + vecForward*16, vecForward * 2250, FALSE );
			break;

			case TANK_ROCKET_RPG:
				CRpgRocket::ShootRpgRocket(pActivator->pev, vecSrc + vecForward*16, vecForward * 2500, FALSE, NULL);
			break;
        
			case TANK_ROCKET_DUMBFIRE:
				CDumbfire::ShootDumbfire(pActivator->pev, vecSrc + vecForward*16, vecForward * 250);
			break;

			case TANK_ROCKET_HELLFIRE:
				Chellfire::ShootHellfire(pActivator->pev, vecSrc + vecForward*16, vecForward * 300, TRUE);
			break;

			case TANK_MORTAR:
				CTankProj::ShootTankProj(pActivator->pev, vecSrc + vecForward*16, vecForward * 100000);
			break;

			case TANK_ROCKET_NUKE:
				CNuke::ShootNuke(pActivator->pev, vecSrc + vecForward*16, vecForward * 1500, FALSE);
			break;

			case TANK_ROCKET_NUKE_CONTROLLED:
				CNuke::ShootNuke(pActivator->pev, vecSrc + vecForward*16, vecForward * 800, TRUE);
			break;

			case TANK_ROCKET_FLAKBOMB:
				CFlakBomb::ShootFlakBomb(pActivator->pev, vecSrc + vecForward*16, vecForward * 1800);
			break;

			case TANK_ROCKET_30MMGREN:
				C30mmgren::Shoot30mmgren(pActivator->pev, vecSrc + vecForward*16, vecForward * 4500);
			break;

			case TANK_ROCKET_BIOMASS:
				CBiomass::ShootBiomass(pActivator->pev, vecSrc + vecForward*16, vecForward * 3000, 200);
			break;

			case TANK_BEAM_BLASTER:
				FireBeam(vecSrc, vecForward, BEAM_BLASTER, 0, pActivator->pev);
			break;

			case TANK_BEAM_GAUSS:
				FireBeam(vecSrc, vecForward, BEAM_GAUSS, 0, pActivator->pev);
			break;

			case TANK_BEAM_GAUSSCHARGED:
				FireBeam(vecSrc, vecForward, BEAM_GAUSSCHARGED, 420, pActivator->pev);
			break;

			case TANK_BEAM_PHOTONGUN:
				FireBeam(vecSrc, vecForward, BEAM_PHOTONGUN, 0, pActivator->pev);
			break;

			case TANK_BEAM_PHOTONGUN_EXP:
				FireBeam(vecSrc, vecForward, BEAM_PHOTONGUN_EXP, 0, pActivator->pev);
			break;

			case TANK_BEAM_TAUCANNON:
				FireBeam(vecSrc, vecForward, BEAM_TAUCANNON, 0, pActivator->pev);
			break;

			case TANK_BEAM_PULSERIFLE:
				FireBeam(vecSrc, vecForward, BEAM_PULSERIFLE, 0, pActivator->pev);
			break;

			case TANK_BEAM_M72:
				FireMagnumBullets(vecSrc, vecForward, VECTOR_CONE_0DEGREES, 16384, BULLET_2MM, pActivator->pev);
			break;

			case TANK_BEAM_M72QUAD:
				FireMagnumBullets(vecSrc, vecForward, VECTOR_CONE_0DEGREES, 16384, BULLET_2MM_QUAD, pActivator->pev);
			break;

			case TANK_BEAM_IONTURRET:
				FireBeam(vecSrc, vecForward, BEAM_IONTURRET, 0, pActivator->pev);
			break;

			case TANK_BEAM_TESLAGUN:
				FireBeam(vecSrc, vecForward, BEAM_TESLAGUN, 0, pActivator->pev);
			break;
		}
		if (m_iAmmo > 0)
			m_iAmmo--;

		// update ammo counter
		MESSAGE_BEGIN( MSG_ONE, gmsgSpecTank, NULL, pActivator->pev );
			WRITE_BYTE( 2 ); // ammo update
			WRITE_LONG(m_iAmmo);
			WRITE_BYTE( m_spread ); // crosshair update
		MESSAGE_END();
		
		m_flNextAttack = gpGlobals->time + (1/m_fireRate);		
	}
	else if ( pActivator->pev->button & IN_ATTACK2 && m_AllowZoom)
	{
		if (m_fInZoom == 0) 
		{
			pActivator->m_iFOV = 25;
			m_fInZoom = 1;
		}
			else if (m_fInZoom == 1) 
		{
			pActivator->m_iFOV = 90;
			m_fInZoom = 0;
		}
		m_flNextAttack = gpGlobals->time + 0.3;
	}
}

void CFuncMachinegun::UpdateClientData( CBasePlayer *client )
{
	if (m_iShouldUpdate)
	{
		Vector vecPoint = pev->origin + Vector(0, 0, m_flPointHeight);
		float coneHor = GetControllerBound( 0 );
		float coneVer = GetControllerBound( 1 );

		MESSAGE_BEGIN( MSG_ONE, gmsgSpecTank, NULL, client->pev );
			WRITE_BYTE( 1 ); // tank is on
			WRITE_COORD(vecPoint.x);
			WRITE_COORD(vecPoint.y);
			WRITE_COORD(vecPoint.z);
			WRITE_COORD(pev->angles.y); // write default yaw
			WRITE_COORD(coneHor); // write cone
			WRITE_COORD(coneVer); //
			WRITE_COORD(m_flDistFwd);
			WRITE_COORD(m_flDistUp);
			WRITE_LONG(m_iAmmo);
			WRITE_BYTE( m_spread );
		MESSAGE_END();

		m_iShouldUpdate = 0;
	}
}

void CFuncMachinegun::RealUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if (!pActivator->IsPlayer())
	{
		ALERT(at_console, "non player activating func_machinegun!\n");
		return;
	}
	
	if (useType == USE_SET && value == 2)	// called by playerthink, update position
	{
		PostFrame( (CBasePlayer*)pActivator );
	}
	else if (useType == USE_SET && value == 3)	// update client data
	{
		UpdateClientData( (CBasePlayer*)pActivator );
	}
	else if (useType == USE_OFF) // turn off
	{
		CBasePlayer *player = (CBasePlayer *)pActivator;

		MESSAGE_BEGIN( MSG_ONE, gmsgSpecTank, NULL, player->pev );
			WRITE_BYTE( 0 ); // tank is off
		MESSAGE_END();

		pev->sequence	= TURRET_IDLE;
		pev->frame	= 0;
		ResetSequenceInfo();
		m_fSequenceLoops = TRUE;

		if ( player->m_pActiveItem )
			player->m_pActiveItem->Deploy();

		player->m_iHideHUD = 0;

		if (m_fInZoom == 1) 
		{
			player->m_iFOV = 90;
			m_fInZoom = 0;
		}
	}
	else // turn on
	{
		// is player in valid zone
		if ( OnControls(pActivator->pev) )
		{
			CBasePlayer *player = (CBasePlayer *)pActivator;
			player->m_pSpecTank = this;
			m_iShouldUpdate = 1;
			player->m_iHideHUD |= HIDEHUD_WEAPONS;

			if ( player->m_pActiveItem )
			{
				player->m_pActiveItem->Holster();
				player->pev->weaponmodel = 0;
				player->pev->viewmodel = 0; 
			}
		}
	}
}

void CFuncMachinegun::CheckSpawn( void )
{
	CBaseEntity *pFound = NULL;
	while ((pFound = UTIL_FindEntityInSphere(pFound, pev->oldorigin, 50)) != NULL)
	{
		if (pFound->IsPlayer() && pFound->IsAlive())
		{
			pev->nextthink = gpGlobals->time + 1;
			return;
		}
	ReSpawn();
	}
}

void CFuncMachinegun::RealReSpawn( void )
{
	pev->armorvalue = pev->armortype;
	pev->health 	= pev->max_health;
	pev->takedamage = DAMAGE_AIM;
	pev->movetype	= MOVETYPE_FLY;
	pev->solid	= SOLID_BBOX;
	pev->angles.x	= 0;
	pev->renderfx 	= 51;
	pev->origin 	= pev->oldorigin;
	pev->effects 	= 0;
	m_iAmmo         = pev->weapons;
	UTIL_SetSize( pev, Vector(-25,-25,0), Vector(25,25,35));

	ResetSequenceInfo();
	m_fSequenceLoops = TRUE;
	InitBoneControllers();
}

void CFuncMachinegun::Killed(entvars_t *pevAttacker, int iGib)
{
	pev->health = 0;
	pev->takedamage = DAMAGE_NO;
	pev->nextthink = gpGlobals->time + 45;//45 seconds till respawn
	pev->effects = EF_NODRAW;
	UTIL_SetSize(pev, g_vecZero, g_vecZero);
	SetThink (CheckSpawn); 
}

void CFuncMachinegun::TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType)
{
	if (!pev->takedamage) 
		return;

	AddMultiDamage( pevAttacker, this, flDamage, bitsDamageType );
}

int CFuncMachinegun:: RealTakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType)
{
	if ( bitsDamageType & (DMG_RADIATION | DMG_BLIND | DMG_NERVEGAS | DMG_DROWN) )
		flDamage = 0;

	if ( (bitsDamageType & (DMG_REGENARMOR)) )
	{
		pev->health += min (flDamage, pev->max_health - pev->health);
		flDamage = 0;
	}

	if (pev->armorvalue && !(bitsDamageType &(DMG_RADIATION | DMG_DROWN | DMG_BLIND | DMG_NERVEGAS | DMG_IGNOREARMOR | DMG_REGENARMOR)))
	{
		float flNew = flDamage * 0.01;
		float flArmor = (flDamage - flNew) * 0.33;

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

	float fTookDamage = CBaseEntity::RealTakeDamage(pevInflictor, pevAttacker, (int)flDamage, bitsDamageType);

	if (pev->health <= 0)
	{
		if(!(bitsDamageType & DMG_ANNIHILATION))
		{
			FX_Explosion( pev->origin + Vector(0,0,20), EXPLOSION_MACHINEGUN );
			::RadiusDamage (pev->origin+ Vector(0,0,5), pevInflictor, pevAttacker, 100, 250, CLASS_NONE, DMG_BLAST);
		}
		return 0;
	}
	return fTookDamage;
}

int CFuncMachinegun::Classify ( void )
{
	return CLASS_MACHINE;
}