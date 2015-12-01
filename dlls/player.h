#ifndef PLAYER_H
#define PLAYER_H
#include "pm_materials.h"
#include "game.h"

extern int gmsgHudText;
extern BOOL gInitHUD;

#define ITEM_RESPAWN_TIME	45
#define WEAPON_RESPAWN_TIME	60
#define AMMO_RESPAWN_TIME	45
#define CHARGER_REACTIVATE_TIME	45
#define BREAKABLE_RESPAWN_TIME	60

#define PLAYER_FATAL_FALL_SPEED		1024// approx 60 feet
#define PLAYER_MAX_SAFE_FALL_SPEED	580// approx 20 feet
#define DAMAGE_FOR_FALL_SPEED		(float) 100 / ( PLAYER_FATAL_FALL_SPEED - PLAYER_MAX_SAFE_FALL_SPEED )// damage per unit per second.
#define PLAYER_MIN_BOUNCE_SPEED		200
#define PLAYER_FALL_PUNCH_THRESHHOLD (float)350 // won't punch player's screen/make scrape noise unless player falling at least this fast.

// Observer code
#define OBS_CHASE_LOCKED	1
#define OBS_CHASE_FREE	2
#define OBS_ROAMING	3

//
// Player PHYSICS FLAGS bits
//
#define		PFLAG_ONLADDER		( 1<<0 )
#define		PFLAG_ONSWING		( 1<<0 )
#define		PFLAG_ONTRAIN		( 1<<1 )
#define		PFLAG_ONBARNACLE	( 1<<2 )
#define		PFLAG_DUCKING		( 1<<3 )		// In the process of ducking, but totally squatted yet
#define		PFLAG_USING		( 1<<4 )		// Using a continuous entity
#define		PFLAG_OBSERVER		( 1<<5 )		// player is locked in stationary cam mode. Spectators can move, observers can't.

//-----------------------------------------------------
//This is Half-Life player entity
//-----------------------------------------------------

#define TEAM_NAME_LENGTH	16

typedef enum
{
	PLAYER_IDLE,
	PLAYER_WALK,
	PLAYER_JUMP,
	PLAYER_SUPERJUMP,
	PLAYER_DIE,
	PLAYER_ATTACK1,
} PLAYER_ANIM;

#define MAX_ID_RANGE 8192
#define SBAR_STRING_SIZE 128

enum sbar_data
{
	SBAR_ID_TARGETNAME = 1,
	SBAR_ID_TARGETHEALTH,
	SBAR_ID_TARGETARMOR,
	SBAR_END,
};

#define MSGMGRMAXSIZE  5 //MSGManager

#define	MK_DELAY_TIME	2	//in what time range multikill happends.
#define CHAT_INTERVAL 1.0f

extern edict_t *EntSelectSpawnPoint( CBaseEntity *pPlayer );
void		WRITE_ELSE(int);//MSGManager

class CBasePlayer : public CBaseMonster
{
public:
	CBaseEntity	*m_pCarryingObject;//flag

	float	m_fVodkaShots;

	int		m_iClass;	//pev->playerclass is already occupied, fucking bots...
	int		m_iKit;		//random PC carrying different kits, but it's class is always 10
	int		m_iUnlock[9];//for each class

	BOOL		DeadItems[70];
	void		BuyDeadItems( void );

	edict_t	   *m_LastAttacker1;	// last attacker (for poison frags)
	edict_t	   *m_LastAttacker2;	// last attacker (for bleed-to-death frags)
	edict_t	   *m_LastAttacker3;	// last attacker (for acid frags)

	int lastDamageType;

	float	lastKillTime;	//used for MULTIKILL checking
	int	KillsAmount;

	int	m_flMoneyAmount;
	void	AddMoney(int Money); 
	void	NoMoney(void); 
	void	HaveItem(void); 
	void	HaveWeapon(void); 
	BOOL	m_iInBuyZone;

	BOOL m_fShieldProtection; //shieldgun
	BOOL m_fTorch; //torch
	char	oldmodel [TEAM_NAME_LENGTH];

	Vector 	v_LastAngles;  // spinning corpses fix
	int					random_seed;    // See that is shared between client & server for shared weapons code

	int			m_iPlayerSound;// the index of the sound list slot reserved for this player
	int			m_iTargetVolume;// ideal sound volume. 
	int			m_iExtraSoundTypes;// additional classification for this weapon's sound
	float			m_flStopExtraSoundTime;
	
	float			m_flFlashLightTime;	// Time until next battery draw/Recharge
	int			m_iFlashBattery;		// Flashlight Battery Draw
	int			m_iKevlarBattery;
	int			m_iHarmorBattery;
	int			m_iWeapons2;

	int			m_afButtonLast;
	int			m_afButtonPressed;
	int			m_afButtonReleased;
	
	edict_t			*m_pentSndLast;			// last sound entity to modify player room type
	float			m_flSndRoomtype;		// last roomtype set by sound entity
	float			m_flSndRange;			// dist from player to sound entity

	float			m_flFallVelocity;
	
	int			m_rgItems[MAX_ITEMS];
	int			m_fKnownItem;		// True when a new item needs to be added
	int			m_fNewAmmo;			// True when a new item has been added

	unsigned int		m_afPhysicsFlags;	// physics flags - set when 'normal' physics should be revisited or overriden
	float			m_fNextSuicideTime; // the time after which the player can next use the suicide command


// these are time-sensitive things that we keep track of

	BOOL	 m_fCloak;
	BOOL	 m_fCloakActivated;
	void	 CloakToggle(BOOL activate);
	void	 CloakUpdate();
	float	 CloakDrainCounter;

	BOOL	 m_fAntigrav;
	BOOL	 m_fAntigravActivated;
	void	 AntigravToggle(BOOL activate);
	void	 AntigravUpdate();
	float	 AntigravDrainCounter;

	float				m_flTimeStepSound;	// when the last stepping sound was made
	float				m_flTimeWeaponIdle; // when to play another weapon idle animation.
	float				m_flSwimTime;		// how long player has been underwater
	float				m_flDuckTime;		// how long we've been ducking
	float				m_flWallJumpTime;	// how long until next walljump

	int				m_lastDamageAmount;		// Last damage taken
	float				m_tbdPrev;				// Time-based damage timer

	int				m_iStepLeft;			// alternate left/right foot stepping sound
	char				m_szTextureName[CBTEXTURENAMEMAX];	// current texture name we're standing on
	char				m_chTextureType;		// current texture type

	int				m_idrowndmg;			// track drowning damage taken
	int				m_idrownrestored;		// track drowning damage restored

	int				m_bitsHUDDamage;		// Damage bits for the current fame. These get sent to 
												// the hude via the DAMAGE message
	BOOL				m_fInitHUD;				// True when deferred HUD restart msg needs to be sent
	BOOL				m_fGameHUDInitialized;
	int				m_iTrain;				// Train control position
	BOOL				m_fWeapon;				// Set this to FALSE to force a reset of the current weapon HUD info
	BOOL				m_fPowerShield;	
	BOOL				m_fKevlar;	
	BOOL				m_fHeavyArmor;	

	BOOL				m_fAnnihilated;	//for chronosceptor special death

	BOOL				m_fPortableHEV;
	BOOL				m_fpt;

	BYTE				m_fTurretSentry;
	BYTE				m_fTurretMissile;
	BYTE				m_fTurretIon;

	BOOL				m_fPortableHealthkit;
	float				regencounter;
	float				armorregencounter;
	float				armorregencounterPS;
	float				Antigravdraincounter;

	float			m_fDeadTime;			// the time at which the player died  (used in PlayerDeathThink())

	BOOL			m_fLongJump; // does this player have the longjump module?

	int			m_iUpdateTime;		// stores the number of frame ticks before sending HUD update messages
	int			m_iClientHealth;	// the health currently known by the client.  If this changes, send a new
	int			m_iClientBattery;	// the Battery currently known by the client.  If this changes, send a new

// new item-icons update messages
	int			m_iClientMoney;	
	int			m_iClientBuyZone;	
	int			m_iClientKevlar;	
	int			m_iClientHarmor;	
	int			m_iClientPShield;
	int			m_iClientCloak;
	int			m_iClientLongjump;
	int			m_iClientAntigrav;
	int			m_iClientPt;
	int			m_iClientPhealth;
	int			m_iClientPHEV;
	int			m_iClientTurretS;
	int			m_iClientTurretM;
	int			m_iClientTurretI;
// new item-icons update messages

	int			m_iHideHUD;		// the players hud weapon info is to be hidden
	int			m_iClientHideHUD;
	int			m_iFOV;			// field of view
	int			m_iClientFOV;	// client's known FOV
	// usable player items 
	CBasePlayerItem	*m_rgpPlayerItems[MAX_ITEM_TYPES];
	CBasePlayerItem *m_pActiveItem;
	CBasePlayerItem *m_pClientActiveItem;  // client version of the active item
	CBasePlayerItem *m_pLastItem;
	CBasePlayerItem *m_pNextItem;

	// buz: special tank
	CBaseEntity	*m_pSpecTank;

	// shared ammo slots
	int	m_rgAmmo[MAX_AMMO_SLOTS];
	int	m_rgAmmoLast[MAX_AMMO_SLOTS];

	int		m_iDeaths;   
	float		m_iRespawnFrames;// used in PlayerDeathThink() to make sure players can always respawn

	int m_lastx, m_lasty;  // These are the previous update's crosshair angles, DON"T SAVE/RESTORE

	int m_nCustomSprayFrames;// Custom clan logo frames for this player
	float	m_flNextDecalTime;// next time this player can spray a decal

	char m_szTeamName[TEAM_NAME_LENGTH];

	virtual void Spawn( void );
	void EntsSendClientData(void);
	virtual void Jump( void );
	virtual void Duck( void );
	virtual void PreThink( void );
	virtual void PostThink( void );
	virtual Vector GetGunPosition( void );
	virtual int TakeHealth( float flHealth, int bitsDamageType );
	virtual void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
	virtual int RealTakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);
	virtual void	Killed( entvars_t *pevAttacker, int iGib);
	virtual Vector BodyTarget( const Vector &posSrc ) { return Center( ) + pev->view_ofs * RANDOM_FLOAT( 0.5, 1.1 ); };		// position to shoot at
	virtual BOOL IsAlive( void ) { return ((pev->deadflag == DEAD_NO) && (pev->health > 0) && (!pev->iuser1)); }//observer is DEAD
	virtual BOOL ShouldFadeOnDeath( void ) { return FALSE; }
	virtual	BOOL IsPlayer( void ) { return TRUE; }			// Spectators should return FALSE for this, they aren't "players" as far as game logic is concerned
	virtual	BOOL IsBot (void) { return (pev->flags & FL_FAKECLIENT)?TRUE:FALSE; }
	virtual BOOL IsNetClient(void)	{ return TRUE; }// Bots should return FALSE for this, they can't receive NET messages Spectators should return TRUE for this
															// Spectators should return TRUE for this
	virtual const char *TeamID( void );

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	void PackDeadPlayerItems( void );
	void RemoveAllItems( BOOL removeSuit );
	BOOL SwitchWeapon( CBasePlayerItem *pWeapon );

	// JOHN:  sends custom messages if player HUD data has changed  (eg health, ammo)
	virtual void UpdateClientData( void );
	
	static	TYPEDESCRIPTION m_playerSaveData[];

	// Player is moved across the transition by other means
	virtual int		ObjectCaps( void ) { return CBaseMonster :: ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
	virtual void	Precache( void );
	BOOL			IsOnLadder( void );
	BOOL			FlashlightIsOn( void );
	void			FlashlightTurnOn( void );
	void			FlashlightTurnOff( void );
	
	void UpdatePlayerSound ( void );
	void DeathSound ( void );

	int Classify ( void );
	void SetAnimation( PLAYER_ANIM playerAnim );
	void SetWeaponAnimType( const char *szExtention );
	char m_szAnimExtention[32];

	// custom player functions
	virtual void ImpulseCommands( void );
	void CheatImpulseCommands( int iImpulse );

	void StartDeathCam( void );
	void StartObserver( Vector vecPosition, Vector vecViewAngle );
	void EndObserver( void );
	void	Observer_FindNextPlayer();
	void	Observer_HandleButtons();
	void	Observer_SetMode( int iMode );
	EHANDLE	m_hObserverTarget;
	float	m_flNextObserverInput;
	int	IsObserver() { return pev->iuser1; };

	void AddPoints( int score, BOOL bAllowNegativeScore );
	void AddPointsToTeam( int score, BOOL bAllowNegativeScore );
	BOOL AddPlayerItem( CBasePlayerItem *pItem );
	BOOL RemovePlayerItem( CBasePlayerItem *pItem );
	BOOL RemoveNamedPlayerItem ( const char *pszItemName );
	void DropPlayerItem ( char *pszItemName );
	void SellCurrentWeapon ( );
	BOOL HasPlayerItem( CBasePlayerItem *pCheckItem );
	BOOL HasNamedPlayerItem( const char *pszItemName );
	BOOL HasWeapons( void );// do I have ANY weapons?
	void SelectPrevItem( int iItem );
	void SelectNextItem( int iItem );
	void SelectLastItem(void);
	void SelectItem(const char *pstr);
	void ItemPreFrame( void );
	void QueueItem(CBasePlayerItem *pItem);
	void ItemPostFrame( void );
	void GiveNamedItem( const char *szName );
	void EnableControl(BOOL fControl);

	int  GiveAmmo( int iAmount, char *szName, int iMax );
	void SendAmmoUpdate(void);

	void WaterMove( void );
	void EXPORT PlayerDeathThink( void );
	void EXPORT PlayerBurn( void );

	void PlayerUse( void );

	void CheckTimeBasedDamage( void );

	static int GetAmmoIndex(const char *psz);
	int AmmoInventory( int iAmmoIndex );

	void ForceClientDllUpdate( void );  // Forces all client .dll specific data to be resent to client.
	void DeathMessage( entvars_t *pevKiller );

	void SetCustomDecalFrames( int nFrames );
	int GetCustomDecalFrames( void );

	void CBasePlayer::TabulateAmmo( void );

	float m_flStartCharge;
	float m_flAmmoStartCharge;
	float m_flPlayAftershock;
	float m_flNextAmmoBurn;// while charging, when to absorb another unit of player's ammo?
	
//Ignite and freeze effects
	virtual void FrozenStart(void);
	virtual void FrozenEnd(void);
	BOOL m_fFrozen;
	float m_fFrFadeTime;
	float m_fRemFreezeTime;
	BOOL m_fIgnite;
	BOOL m_fLastIgnite;
	BOOL m_fLastFreeze;
	float m_fConcussLevel;
	float m_fConcussLevelChange;

	BOOL m_fDropped;  //to allow dodydrop sound

	virtual void IgniteStart(void);
	virtual void IgniteEnd(void);
	float	m_flScreamTime;
	float	m_flBleedTime;

	//Player ID
	void InitStatusBar( void );
	void UpdateStatusBar( void );
	int m_izSBarState[ SBAR_END ];
	float m_flNextSBarUpdateTime;
	float m_flStatusBarDisappearDelay;
	char m_SbarString0[ SBAR_STRING_SIZE ];
	char m_SbarString1[ SBAR_STRING_SIZE ];
	
	float m_flNextChatTime;

	// rain tutorial
	int	Rain_dripsPerSecond, Rain_ideal_dripsPerSecond, Rain_needsUpdate;
	float	Rain_randX, Rain_randY, Rain_windX, Rain_windY, Rain_ideal_windX, Rain_ideal_windY, Rain_ideal_randX, Rain_ideal_randY, Rain_endFade, Rain_nextFadeUpdate;
};

#endif // PLAYER_H
