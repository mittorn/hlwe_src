#define NUM_DMG_TYPES		16 // number of drawn damage images
#define DMG_IMAGE_LIFE		2 // seconds that image is up

// instant damage
#define DMG_GENERIC		0		// generic damage was done
#define DMG_CRUSH		(1 << 0)	// crushed by falling or moving object
#define DMG_BULLET		(1 << 1)	// shot
#define DMG_SLASH		(1 << 2)	// cut, clawed, stabbed
#define DMG_BURN		(1 << 3)	// heat burned
#define DMG_BULLETMAGNUM	(1 << 4)	// Hit by high-velocity projectile
#define DMG_REGENARMOR		(1 << 5)	// repares "physical" armor
#define DMG_BLAST		(1 << 6)	// explosive blast damage
#define DMG_ANNIHILATION	(1 << 7)	// black hole "annihilation" effect
#define DMG_SHOCK		(1 << 8)	// electric shock
#define DMG_SONIC		(1 << 9)	// sound pulse shockwave
#define DMG_ENERGYBEAM		(1 << 10)	// laser or other high energy beam 
#define DMG_ENERGYBLAST		(1 << 11)	// energy explosive radial damage
#define DMG_NEVERGIB		(1 << 12)	// with this bit OR'd in, no damage type will be able to gib victims upon death
#define DMG_ALWAYSGIB		(1 << 13)	// with this bit OR'd in, any damage type can be made to gib victims upon death.
#define DMG_DROWN		(1 << 14)	// Drowning
#define DMG_BLIND		(1 << 15)	// blind damage
#define DMG_KNOCKBACK		(1 << 16)	// knocks back
#define DMG_PLASMA		(1 << 17)	// extremally heating
#define DMG_NUKE		(1 << 18)	// nuclear explosion
#define DMG_BULLETBUCKSHOT	(1 << 19)	// shotgun pellets
#define DMG_IGNOREARMOR		(1 << 20)	// ignores armor
#define DMG_HEADSHOT		(1 << 21)	//no comment :)
#define DMG_PARALYZE		(1 << 22)	// slows affected creature down
#define DMG_NERVEGAS		(1 << 23)	// nerve toxins, very bad
#define DMG_POISON		(1 << 24)	// blood poisioning
#define DMG_RADIATION		(1 << 25)	// radiation exposure
#define DMG_DROWNRECOVER	(1 << 26)	// drowning recovery
#define DMG_ACID		(1 << 27)	// toxic chemicals or acid burns
#define DMG_IGNITE		(1 << 28)	// makes player burn
#define DMG_FREEZE		(1 << 29)	// freeze player
#define DMG_CONCUSSION		(1 << 30)	// concussion

typedef struct
{
	float fExpire;
	float fBaseline;
	int	x, y;
} DAMAGE_IMAGE;
	
//
//-----------------------------------------------------
//
class CHudHealth: public CHudBase
{
public:
	virtual int Init( void );
	virtual int VidInit( void );
	virtual int Draw(float fTime);
	virtual void Reset( void );
	int MsgFunc_Damage(const char *pszName,  int iSize, void *pbuf);
	int MsgFunc_Health(const char *pszName,  int iSize, void *pbuf);
	int m_iHealth;
	int m_HUD_dmg_bio;
	int m_HUD_cross;
	float m_fAttackFront, m_fAttackRear, m_fAttackLeft, m_fAttackRight;
	void GetPainColor( int &r, int &g, int &b );
	float m_fFade;

private:
	HSPRITE m_hSprite;
	HSPRITE m_hDamage;
	
	DAMAGE_IMAGE m_dmg[NUM_DMG_TYPES];
	int	m_bitsDamage;
	int DrawPain(float fTime);
	int DrawDamage(float fTime);
	void CalcDamageDirection(vec3_t vecFrom);
	void UpdateTiles(float fTime, long bits);
};