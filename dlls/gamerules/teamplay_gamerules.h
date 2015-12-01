#ifndef TEAMPLAY_H
#define TEAMPLAY_H

#define MAX_TEAMNAME_LENGTH	16
#define MAX_TEAMS		32

#define TEAMPLAY_TEAMLISTLENGTH		MAX_TEAMS*MAX_TEAMNAME_LENGTH

static char team_names[MAX_TEAMS][MAX_TEAMNAME_LENGTH];
static int team_scores[MAX_TEAMS];
static int num_teams = 0;

extern int gmsgGameMode;
extern int gmsgSayText;
extern int gmsgTeamInfo;
extern int gmsgTeamNames;
extern int gmsgScoreInfo;

class CHalfLifeTeamplay : public CHalfLifeMultiplay
{
public:
	virtual bool IsAllowedToBuy ( CBasePlayer *pPlayer );

	CHalfLifeTeamplay();

	virtual void ShowMenu ( CBasePlayer* );
	virtual void InterpretBuyCommand ( const char*, CBasePlayer* );

	virtual BOOL ClientCommand( CBasePlayer *pPlayer, const char *pcmd );
	virtual void ClientUserInfoChanged( CBasePlayer *pPlayer, char *infobuffer );
	virtual BOOL IsTeamplay( void );
	virtual BOOL FPlayerCanTakeDamage( CBasePlayer *pPlayer, CBaseEntity *pAttacker );
	virtual int PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget );
	virtual const char *GetTeamID( CBaseEntity *pEntity );
	virtual int IPointsForKill( CBasePlayer *pAttacker, CBasePlayer *pKilled );
	virtual void InitHUD( CBasePlayer *pl );
	virtual void DeathNotice( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pevInflictor );
	virtual const char *GetGameDescription( void ) { return "RFTD Teamplay"; }  // this is the game name that gets seen in the server browser
	virtual void UpdateGameMode( CBasePlayer *pPlayer );  // the client needs to be informed of the current game mode
	virtual void PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor );
	virtual void Think ( void );
	virtual int GetTeamIndex( const char *pTeamName );
	virtual const char *GetIndexedTeamName( int teamIndex );
	BOOL IsValidTeam( const char *pTeamName );
	virtual const char *SetDefaultPlayerTeam( CBasePlayer *pPlayer );
	virtual void ChangePlayerTeam( CBasePlayer *pPlayer, const char *pTeamName, BOOL bKill, BOOL bGib );
	void RecountTeams( bool bResendInfo = false );

private:
	BOOL m_DisableDeathMessages;
	BOOL m_DisableDeathPenalty;

protected://LLAPb
	virtual const char *TeamWithFewestPlayers( void );

	BOOL m_teamLimit;				// This means the server set only some teams as valid
	char m_szTeamList[TEAMPLAY_TEAMLISTLENGTH];
};

//===========================================================================================
//								HLWE CTF GAME RULES
//===========================================================================================

class CHLWECTF : public CHalfLifeTeamplay
{
public:
	/*===============================CONSTRUCTION/DESTRUCTION==============================*/
	CHLWECTF ();
	~CHLWECTF ();

	/*==================================NEW MENU COMMANDS==================================*/
	void ShowMenu ( CBasePlayer* );
	void InterpretBuyCommand ( const char*, CBasePlayer* );
	BOOL ClientCommand( CBasePlayer *pPlayer, const char *pcmd );//new class necessary commands
	virtual bool IsAllowedToBuy ( CBasePlayer * )
	{
		return false;
	}

	/*====================================CLASS SPECIFIC===================================*/
	void UpdateGameMode( CBasePlayer *pPlayer );//"team points for flags"
	void Think ( void );//Schtrategy think!!!
	void PlayerSpawn( CBasePlayer *pPlayer );//give all stuff
	void PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor );//drop flag
	void ClientDisconnected( edict_t *pClient );//drop flag if disconnected
	void InitHUD( CBasePlayer* );//class vars init
	int AddScoreToTeam(int teamIndex, int score);
	void DeathNotice( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pevInflictor );//don't give stuff for multikills


	/*=====================================CTF SPECIFIC====================================*/
	virtual BOOL IsCTF ( void ) { return TRUE; };//assault is CTF or not CTF???
	virtual const char *GetGameDescription( void ) { return "RFTD CTF"; }


	/*==============================DON'T ALLOW TO DROP WEAPONS============================*/
	int DeadPlayerWeapons( CBasePlayer *pPlayer ) {return GR_PLR_DROP_GUN_NO;};
	int DeadPlayerAmmo( CBasePlayer *pPlayer ) {return GR_PLR_DROP_AMMO_ALL;};

private:
	float m_flNextThinkTime;
};

#endif