
#define	ROCKET_YAW_SPEED	0.3


// Because we use ActiveRockets, RPG must be here!
class CRpg : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	void Reload( void );
	int GetItemInfo(ItemInfo *p);

	BOOL Deploy( void );
	void Holster( void );

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	void WeaponIdle( void );
	BOOL ShouldWeaponIdle( void ) {return TRUE; };

	void UpdateSpot( void );

	void BuyPrimaryAmmo( void );
	void SellWeapon( void );
	CLaserSpot *m_pSpot;
	int m_cActiveRockets;
};

// Power Shield Detonation
class CPshieldDet : public CBaseEntity
{
public:
	void Spawn( void );
	void EXPORT DamageThink( void );
};

// BlackHole
class CBlackHole : public CBaseEntity
{
public:
	static CBlackHole *ShootBlackHole( entvars_t *pevOwner, Vector vecStart );
	void RadiusDamage ();
	void Spawn( void );
	void EXPORT DesintegrateThink( void );
};

// Pulse (pulse rifle time-based radius damage entity)
class CPulse : public CBaseEntity
{
public:
	void Spawn( void );
	void EXPORT DamageThink( void );
	float PulseStayTime;
};

// Teleport Enter (disp effect)
class CTeleenter : public CBaseEntity
{
public:
	static CTeleenter *ShootTeleenter( entvars_t *pevOwner, Vector vecStart );
	void Spawn( void );
	void EXPORT MakeBlast( void );
};

// Hand grenade
class CGrenade : public CBaseMonster
{
public:
	void Spawn( void );
	static CGrenade *ShootGrenade( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time );
	void EXPORT BounceTouch( CBaseEntity *pOther );
	void EXPORT Detonate( void );
	void EXPORT TumbleThink( void );
	void Killed(entvars_t *pevAttacker, int iGib) {UTIL_Remove (this);};
	int Classify(void) { return CLASS_PROJECTILE; };
};

// Satchel Charge
class CSCharge : public CBaseMonster
{
public:
	void Spawn( void );
	typedef enum { SATCHEL_DETONATE = 0, SATCHEL_RELEASE } SATCHELCODE;

	static CSCharge *ShootSCharge( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity );
	static void UseSatchelCharges( entvars_t *pevOwner, SATCHELCODE code );
	void EXPORT SlideTouch( CBaseEntity *pOther );
	void EXPORT Detonate( void );
	void EXPORT DetonateUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void EXPORT SatchelThink( void );
	void Deactivate( void );
	void Killed(entvars_t *pevAttacker, int iGib) {UTIL_Remove (this);};
	int Classify(void) { return CLASS_PROJECTILE; };
};

// BioRifle biomass
class CBiomass : public CBaseMonster
{
public:
	void Spawn( void );
	typedef enum { BIOMASS_DETONATE = 0, BIOMASS_RELEASE } BIOMASSCODE;

	static CBiomass *ShootBiomass( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float Time);
	static void UseBiomass( entvars_t *pevOwner, BIOMASSCODE code );
	void EXPORT SlideTouch( CBaseEntity *pOther );
	void EXPORT Detonate( void );
	void EXPORT DetonateUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void EXPORT StayInWorld( void );
	void Deactivate( void );
	Vector	dist;
	float	angl_y, angl_x;
	BOOL	b_attached;
	void Killed(entvars_t *pevAttacker, int iGib) {UTIL_Remove (this);};
	int Classify(void) { return CLASS_PROJECTILE; };
};

class CTrip : public CBaseMonster
{
public:
	void Explode( TraceResult *pTrace, int bitsDamageType );
	void Killed(entvars_t *pevAttacker, int iGib) {UTIL_Remove (this);};
	int Classify(void) { return CLASS_PROJECTILE; };
};

// Detpack
class CDetpack : public CBaseMonster
{
public:
	void Spawn( void );
	static CDetpack *ShootTimedDetpack( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time );
	void EXPORT BounceTouch( CBaseEntity *pOther );
	void EXPORT Detonate( void );
	void EXPORT DrawFX( void );
	void Explode( entvars_t *pevAttacker );
	void EXPORT BeepThink( void );

	void Killed(entvars_t *pevAttacker, int iGib)
	{
		UTIL_Remove (this);
	};
	float	m_flSoundTime;
	float	m_flBeepTime;
	void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
	int RealTakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);
	int Classify(void) { return CLASS_PROJECTILE; };
};

//main U2 rocket
class CU2Momma : public CBaseMonster
{
public:
	void Spawn( void );
	static CU2Momma *ShootU2Momma( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time );
	void EXPORT ExplodeTouch( CBaseEntity *pOther );
	void EXPORT ShootShards( void );
	void EXPORT TumbleThink( void );
	void Killed(entvars_t *pevAttacker, int iGib);
	int Classify(void) { return CLASS_PROJECTILE; };
};

// U2 shrapnel
class CU2Baby : public CBaseMonster
{
public:
	void Spawn( void );
	static CU2Baby *ShootU2Baby( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity );
	void EXPORT ExplodeTouch( CBaseEntity *pOther );
	void Killed(entvars_t *pevAttacker, int iGib);
	int Classify(void) { return CLASS_PROJECTILE; };
};

// Flak Cannon shrapnel
class CShrapnel : public CBaseMonster
{
public:
	CBasePlayer *pLauncher;

	void Spawn( void );
	static CShrapnel *ShootShrapnel( entvars_t *pevOwner, Vector vecOrigin, Vector vecVelocity);
	static CShrapnel *ShootBombShrap( entvars_t *pevOwner, Vector vecOrigin);
	void EXPORT ShrapnelTouch( CBaseEntity *pOther );
	void EXPORT ShrapnelThink( void );
	void Killed(entvars_t *pevAttacker, int iGib) {UTIL_Remove (this);};
};

class CFlakBomb : public CBaseMonster
{
public:
	void Spawn( void );
	static CFlakBomb *ShootFlakBomb( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity);
	void EXPORT ExplodeTouch( CBaseEntity *pOther );
	void Killed(entvars_t *pevAttacker, int iGib);
	int Classify(void) { return CLASS_PROJECTILE; };
};

// Cluster Grenade (momma)
class CClMomma : public CBaseMonster
{
public:
	void Spawn( void );
	static CClMomma *ShootClusterMomma( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time );
	void EXPORT BounceTouch( CBaseEntity *pOther );
	void EXPORT Detonate( void );
	void EXPORT TumbleThink( void );
	void Killed(entvars_t *pevAttacker, int iGib);
	int Classify(void) { return CLASS_PROJECTILE; };
};

class CClBaby : public CBaseMonster
{
public:
	void Spawn( void );
	static CClBaby *ShootClBaby( entvars_t *pevOwner, Vector vecStart);
	void EXPORT ExplodeTouch( CBaseEntity *pOther );
	void EXPORT RemoveMe( void );
	void Killed(entvars_t *pevAttacker, int iGib) {UTIL_Remove (this);};
	int Classify(void) { return CLASS_PROJECTILE; };
};

// Satellite Strike
class CSatelliteStrike : public CBaseMonster
{
public:
	void Spawn( void );
	static CSatelliteStrike *ShootSatelliteStrike( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time );
	void Explode( TraceResult *pTrace, int bitsDamageType );
	void EXPORT Irradiate( void );
	float RadiationStayTime;
	void EXPORT Detonate( void );
	void EXPORT TumbleThink( void );
	void EXPORT BounceTouch( CBaseEntity *pOther );
};

// Chronosceptor clip (disarmed)
class CChronoClip : public CBaseMonster
{
public:
	void Spawn( void );
	static CChronoClip *ShootChronoClip( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time );
	void Explode( TraceResult *pTrace, int bitsDamageType );
	void EXPORT Irradiate( void );
	float IrradiateStayTime;
	void EXPORT BounceTouch( CBaseEntity *pOther );
	void EXPORT Detonate( void );
	void EXPORT TumbleThink( void );
	void Killed(entvars_t *pevAttacker, int iGib) {UTIL_Remove (this);};
	int Classify(void) { return CLASS_PROJECTILE; };
};

// Flashbang
class CFlashBang : public CBaseMonster
{
public:
	void Spawn( void );
	static CFlashBang *ShootFlashbang_projectile( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time );
	void EXPORT BounceTouch( CBaseEntity *pOther );
	void EXPORT Detonate( void );
	void EXPORT TumbleThink( void );
	void Killed(entvars_t *pevAttacker, int iGib) {UTIL_Remove (this);};
	int Classify(void) { return CLASS_PROJECTILE; };
};

// Nerve Gas Grenade (c) Ghoul [BB]
class CNGgrenade : public CBaseMonster
{
public:
	void Spawn( void );
	static CNGgrenade *ShootNGgrenade( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time );
	void Explode( TraceResult *pTrace, int bitsDamageType );
	void EXPORT BounceTouch( CBaseEntity *pOther );
	void EXPORT Detonate( void );
	void EXPORT TumbleThink( void );
	void EXPORT Smoke( void );
	float GasStayTime;
	float	m_flSoundTime;
	void Killed(entvars_t *pevAttacker, int iGib);
	int Classify(void) { return CLASS_PROJECTILE; };
};

// IncendiaryProjectile
class Chellfire : public CBaseMonster
{
public:
	void Spawn( void );
	void EXPORT ExplodeTouch( CBaseEntity *pOther );
	static Chellfire *ShootHellfire( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, BOOL Homing );
	void EXPORT Follow( void );
	void EXPORT Ignite( void );
	void EXPORT IgniteFollow( void );
	void EXPORT Burn( void );
	float FireStayTime;
	float	m_flSoundTime;
	void Killed(entvars_t *pevAttacker, int iGib);
	int Classify(void) { return CLASS_PROJECTILE; };
};

// AK74 contact grenade
class CAK74grenade : public CBaseMonster
{
public:
	void Spawn( void );
	static CAK74grenade *ShootAK74grenade( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity );
	void EXPORT ExplodeTouch( CBaseEntity *pOther );
	void Killed(entvars_t *pevAttacker, int iGib);
	int Classify(void) { return CLASS_PROJECTILE; };
};

// Mini Missile
class CMmissile : public CBaseMonster
{
public:
	void Spawn( void );
	static CMmissile *ShootMmissile( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity );
	void EXPORT ExplodeTouch( CBaseEntity *pOther );
	void Killed(entvars_t *pevAttacker, int iGib);
	int Classify(void) { return CLASS_PROJECTILE; };
};

// RpgRocket
class CRpgRocket : public CBaseMonster
{
public:
	CRpg *m_pLauncher; 
	void Spawn( void );
	void EXPORT ExplodeTouch( CBaseEntity *pOther );
	void EXPORT Follow( void );
	void EXPORT Ignite( void );
	void EXPORT IgniteFollow( void );
	static CRpgRocket *ShootRpgRocket( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, BOOL LaserFollow, CRpg *pLauncher );
	float m_flIgniteTime;
	void Killed(entvars_t *pevAttacker, int iGib);
	int Classify(void) { return CLASS_PROJECTILE; };
};

//Tank cannon projectile
class CTankProj : public CBaseMonster
{
public:
	void Spawn( void );
	static CTankProj *ShootTankProj( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity);
	void EXPORT ExplodeTouch( CBaseEntity *pOther );
	void Killed(entvars_t *pevAttacker, int iGib);
};

// WHL Rocket
class CWhlProjectile : public CBaseMonster
{
public:
	void Spawn( void );
	static CWhlProjectile *ShootWhlProjectile( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, BOOL Camera );
	void Explode( TraceResult *pTrace, int bitsDamageType );
	void EXPORT ExplodeTouch( CBaseEntity *pOther );
	void EXPORT ShootShards( void );
	void EXPORT Ignite( void );
	float ExplodeStayTime;
	void Killed(entvars_t *pevAttacker, int iGib);
	int Classify(void) { return CLASS_PROJECTILE; };
};

// Redeemer Nuke
class CNuke : public CBaseMonster
{
public:
	void Spawn( void );
	void Explode( TraceResult *pTrace, int bitsDamageType );
	void EXPORT Irradiate( void );
	void EXPORT ExplodeTouch( CBaseEntity *pOther );
	static CNuke *ShootNuke( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, BOOL Camera );
	void EXPORT Follow( void );
	void EXPORT Ignite( void );
	void EXPORT IgniteFollow( void );

	float m_yawCenter;
	float m_pitchCenter;
	float RadiationStayTime;
	void Killed(entvars_t *pevAttacker, int iGib);
	int Classify(void) { return CLASS_PROJECTILE; };
};

// M203 contact grenade
class CM203grenade : public CBaseMonster
{
public:
	void Spawn( void );
	static CM203grenade *ShootM203grenade( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity );
	void EXPORT ExplodeTouch( CBaseEntity *pOther );
	void Killed(entvars_t *pevAttacker, int iGib);
	int Classify(void) { return CLASS_PROJECTILE; };
};

// Tesla gun Contact Grenade 
class CTeslagren : public CBaseMonster
{
public:
	void Spawn( void );
	static CTeslagren *ShootTeslagren( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity );
	void EXPORT ExplodeTouch( CBaseEntity *pOther );
	void Killed(entvars_t *pevAttacker, int iGib);
	int Classify(void) { return CLASS_PROJECTILE; };
};

// Crossbow Explosive Bolt 
class CBolt : public CBaseMonster
{
public:
	void Spawn( void );
	static CBolt *ShootBolt( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, int type );
	void EXPORT ExplodeTouch( CBaseEntity *pOther );
	void Killed(entvars_t *pevAttacker, int iGib) {UTIL_Remove (this);};
	int Classify(void) { return CLASS_PROJECTILE; };
};

// 30mm Explosive grenade 
class C30mmgren : public CBaseMonster
{
public:
	void Spawn( void );
	static C30mmgren *Shoot30mmgren( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity );
	void EXPORT ExplodeTouch( CBaseEntity *pOther );
	void Killed(entvars_t *pevAttacker, int iGib);
	int Classify(void) { return CLASS_PROJECTILE; };
};

// Dumbfire missile 
class CDumbfire : public CBaseMonster
{
public:
	void Spawn( void );
	static CDumbfire *ShootDumbfire( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity );
	void EXPORT ExplodeTouch( CBaseEntity *pOther );
	void EXPORT Follow( void );
	void Killed(entvars_t *pevAttacker, int iGib);
	int Classify(void) { return CLASS_PROJECTILE; };
};

class CPBolt : public CBaseMonster
{
public:
	static CPBolt *ShootPBolt( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity );
	void EXPORT Nova( void );
	void Spawn( void );
	void EXPORT ExplodeTouch( CBaseEntity *pOther );
	void EXPORT Fly( void );
	void Killed(entvars_t *pevAttacker, int iGib) {UTIL_Remove (this);};
	int Classify(void) { return CLASS_PROJECTILE; };
};

class CPlasma : public CBaseMonster
{
public:
	static CPlasma *ShootPlasma( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity );
	void EXPORT Irradiate( void );
	void Spawn( void );
	void EXPORT ExplodeTouch( CBaseEntity *pOther );
	void EXPORT Fly( void );
	float RadiationStayTime;
	void Killed(entvars_t *pevAttacker, int iGib) {UTIL_Remove (this);};
	int Classify(void) { return CLASS_PROJECTILE; };
};

class CPlasma2 : public CBaseMonster
{
public:
	static CPlasma2 *ShootPlasma2( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity );
	void EXPORT Irradiate( void );
	void Spawn( void );
	void EXPORT ExplodeTouch( CBaseEntity *pOther );
	void EXPORT Fly( void );
	float RadiationStayTime;
	void Killed(entvars_t *pevAttacker, int iGib) {UTIL_Remove (this);};
	int Classify(void) { return CLASS_PROJECTILE; };
};

class CShock : public CBaseMonster
{
public:
	static CShock *ShootShock( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, BOOL LightningBall );
	void Spawn( void );
	void EXPORT ExplodeTouch( CBaseEntity *pOther );
	void EXPORT Lightning( void );
	void EXPORT Follow( void );

	float	m_flBuzzTime;
	float	m_flStrikeTime;
	int  m_maxFrame;

	void Killed(entvars_t *pevAttacker, int iGib);
	int Classify(void) { return CLASS_PROJECTILE; };
};

class CGluon2 : public CBaseMonster
{
public:
	static CGluon2 *ShootGluon2( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float flWastedAmmo);
	void Spawn( void );
	void EXPORT Fly( void );
	void EXPORT ExplodeTouch( CBaseEntity *pOther );
	int  m_maxFrame;

	void Killed(entvars_t *pevAttacker, int iGib);
	int Classify(void) { return CLASS_PROJECTILE; };
};

class CFrostball : public CBaseMonster
{
public:
	static CFrostball *ShootFrostball( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity);
	void Spawn( void );
	void EXPORT Fly( void );
	void EXPORT ExplodeTouch( CBaseEntity *pOther );
	int  m_maxFrame;

	void Killed(entvars_t *pevAttacker, int iGib);
	int Classify(void) { return CLASS_PROJECTILE; };
};

class CGluon : public CBaseMonster
{
public:
	static CGluon *ShootGluon( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity );
	void Spawn( void );
	void EXPORT Fly( void );
	void EXPORT ExplodeTouch( CBaseEntity *pOther );
	int  m_maxFrame;

	void Killed(entvars_t *pevAttacker, int iGib);
	int Classify(void) { return CLASS_PROJECTILE; };
};

class CFlame : public CBaseMonster
{
public:
	static CFlame *ShootFlame( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity );
	void Spawn( void );
	void EXPORT Burn( void );
	void EXPORT Fly( void );
	void EXPORT ExplodeTouch( CBaseEntity *pOther );
	float FireStayTime;

	void Killed(entvars_t *pevAttacker, int iGib);
	int Classify(void) { return CLASS_PROJECTILE; };
};

class CDispball : public CBaseMonster
{
public:
	static CDispball *ShootDispball( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float flWastedAmmo);
	void Spawn( void );
	void EXPORT Fly( void );
	void EXPORT ExplodeTouch( CBaseEntity *pOther );
	void Killed(entvars_t *pevAttacker, int iGib);
	int Classify(void) { return CLASS_PROJECTILE; };
	int  m_maxFrame;
};

// Sun of God: Charge Radiance Emitter projectile
class CSunOfGod : public CBaseMonster
{
public:
	static CSunOfGod *ShootSunOfGod( entvars_t *pevOwner, Vector vecStart );
	void Spawn( void );
	void Explode( TraceResult *pTrace, int bitsDamageType );
	void EXPORT Animate( void );
	void EXPORT ExplodeTouch( CBaseEntity *pOther );
	void EXPORT Detonate( void );
	void EXPORT Irradiate( void );
	float RadiationStayTime;
	int Classify(void) { return CLASS_PROJECTILE; };
};

class CEnergycharge : public CBaseMonster
{
public:
	void Spawn( void );
	void EXPORT ExplodeTouch( CBaseEntity *pOther );
	static CEnergycharge *ShootEnergycharge( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity);
	void EXPORT BlastOn( void );
	void EXPORT BlastOff( void );
	int Classify(void) { return CLASS_PROJECTILE; };

	CBeam *m_pBeam;
	CBeam *m_pNoise;
};

class CFXSpawner : public CBaseEntity
{
public:
	void Spawn( void );
	void Destroy( void );
	void EXPORT FollowThink( void );
};

class CFXSpawner2 : public CBaseEntity
{
public:
	void Spawn( void );
	void Destroy( void );
	void EXPORT FollowThink( void );
};
