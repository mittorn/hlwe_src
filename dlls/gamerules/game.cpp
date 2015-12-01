#include "extdll.h"
#include "eiface.h"
#include "util.h"
#include "game.h"
#include "skill.h"

// multiplayer server rules
cvar_t	mp_fragsleft		= {"mp_fragsleft","0", FCVAR_SERVER | FCVAR_UNLOGGED };	  // Don't spam console/log files/users with this changing
cvar_t	mp_timeleft		= {"mp_timeleft","0" , FCVAR_SERVER | FCVAR_UNLOGGED };	  // "      "
cvar_t	mp_teamplay		= {"mp_teamplay","0", FCVAR_SERVER };
cvar_t	mp_fraglimit		= {"mp_fraglimit","0", FCVAR_SERVER };
cvar_t	mp_deadtime		= {"mp_deadtime","10", FCVAR_SERVER };
cvar_t	mp_wpnstaytime		= {"mp_wpnstaytime","120", FCVAR_SERVER };
cvar_t	mp_timelimit		= {"mp_timelimit","0", FCVAR_SERVER };
cvar_t	mp_weaponstay		= {"mp_weaponstay","0", FCVAR_SERVER };
cvar_t	mp_teamlist 		= {"mp_teamlist","swat;terror", FCVAR_SERVER };
cvar_t	mp_teamoverride 	= {"mp_teamoverride","1" };
cvar_t  mp_chattime 		= {"mp_chattime","10", FCVAR_SERVER };

cvar_t	mp_noteamdamage		= {"mp_noteamdamage","0", FCVAR_SERVER };
cvar_t	mp_noselfdamage		= {"mp_noselfdamage","0", FCVAR_SERVER };
cvar_t	mp_wpn_power		= {"mp_wpn_power","100", FCVAR_SERVER };

cvar_t	displaysoundlist 	= {"displaysoundlist","0"};
cvar_t	mp_startmoney 		= {"mp_startmoney","800", FCVAR_SERVER };
cvar_t	mp_nobuy 		= {"mp_nobuy","0", FCVAR_SERVER };

cvar_t	ctf_flagstay		= {"ctf_flagstay","45", FCVAR_SERVER };
cvar_t	lms_waittime		= {"lms_waittime","15", FCVAR_SERVER };
cvar_t	lms_roundtime 		= {"lms_roundtime","180", FCVAR_SERVER };
    
// Engine Cvars
cvar_t 	*g_psv_gravity = NULL;
cvar_t	*g_footsteps = NULL;
            
// Register your console variables here
// This gets called one time when the game is initialied
void GameDLLInit( void )
{
	g_psv_gravity = CVAR_GET_POINTER("sv_gravity");
	g_footsteps = CVAR_GET_POINTER("mp_footsteps");

	CVAR_REGISTER (&displaysoundlist);
	CVAR_REGISTER (&mp_teamplay);
	CVAR_REGISTER (&mp_noselfdamage);
	CVAR_REGISTER (&mp_noteamdamage);
	CVAR_REGISTER (&mp_fraglimit);
	CVAR_REGISTER (&mp_timelimit);
	CVAR_REGISTER (&mp_deadtime);
	CVAR_REGISTER (&mp_wpnstaytime);
	CVAR_REGISTER (&mp_fragsleft);
	CVAR_REGISTER (&mp_timeleft);
	CVAR_REGISTER (&mp_weaponstay);
	CVAR_REGISTER (&mp_teamlist);
	CVAR_REGISTER (&mp_teamoverride);
	CVAR_REGISTER (&mp_chattime);
	CVAR_REGISTER (&mp_startmoney);
	CVAR_REGISTER (&mp_nobuy);
	CVAR_REGISTER (&mp_wpn_power);
	CVAR_REGISTER (&ctf_flagstay);
	CVAR_REGISTER (&lms_waittime);
	CVAR_REGISTER (&lms_roundtime);

	SkillRegisterCvars();
}