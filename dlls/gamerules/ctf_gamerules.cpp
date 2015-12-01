#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"player.h"
#include	"weapons.h"
#include	"gamerules.h"
#include	"teamplay_gamerules.h"
#include	"game.h"
#include	"hltv.h"

extern int gmsgTeamScore;
extern int gmsgJoinMenu;
extern int gmsgDeathMsg;

//===============================CLASSES' WEAPON KITS================================

typedef struct {

	int  amount;
	char name [64];

} ammo;

typedef struct {

	int armorvalue;
	int cost;

	char weapons[12][64];//max 10 names of 64 characters
	ammo equip[32];//no more than 32 ammo types

} weapon_defs;

weapon_defs class_kits[9][5] = //first index - class, second index - unlock
{
	{//scout - intelligence, terrorism etc
		{ 20, 0, 		{"weapon_usp", "weapon_lightsaber", "weapon_flashbang", "item_kevlar", "\0"}, { {60, "45ACP"}, {4, "Flashbang"}, {0, "\0"} } },
		{ 40, COST_SCOUT_1, 	{"weapon_usp", "weapon_lightsaber", "weapon_flashbang", "item_kevlar", "item_longjump", "weapon_crossbow", "\0"}, { {84, "45ACP"}, {6, "Flashbang"}, {10, "bolts"}, {0, "\0"} } },
		{ 60, COST_SCOUT_2, 	{"weapon_usp", "weapon_lightsaber", "weapon_flashbang", "item_kevlar", "item_longjump", "item_cloak", "weapon_crossbow", "weapon_froster", "\0"}, { {108, "45ACP"}, {8, "Flashbang"}, {20, "bolts"}, {10, "accelpower"}, {50, "cell"}, {2, "Cgrenade"}, {0, "\0"} } },
		{ 80, COST_SCOUT_3, 	{"weapon_usp", "weapon_lightsaber", "weapon_flashbang", "item_kevlar", "item_longjump", "item_cloak", "weapon_crossbow", "weapon_froster", "weapon_gauss", "\0"}, { {120, "45ACP"}, {10, "Flashbang"}, {30, "bolts"}, {20, "accelpower"}, {100, "cell"}, {5, "Cgrenade"}, {80, "uranium"}, {0, "\0"} } },
		{ 110, COST_SCOUT_4, 	{"weapon_usp", "weapon_lightsaber", "weapon_flashbang", "item_kevlar", "item_longjump", "item_cloak", "weapon_crossbow", "weapon_froster", "weapon_gauss", "weapon_satellite", "\0"}, { {132, "45ACP"}, {15, "Flashbang"}, {50, "bolts"}, {50, "accelpower"}, {200, "cell"}, {12, "Cgrenade"}, {160, "uranium"}, {0, "\0"} } },
	},

	{//sniper - camping
		{ 75, 0, {"weapon_deagle", "weapon_crowbar", "weapon_awp", "\0"}, { {28, "50AE"}, {20, "338Magnum"}, {0, "\0"} } },
		{ 90, COST_SNIPER_1, {"weapon_deagle", "weapon_crowbar", "weapon_awp", "weapon_svd", "\0"}, { {42, "50AE"}, {40, "338Magnum"}, {36, "762x54"}, {0, "\0"} } },
		{ 105, COST_SNIPER_2, {"weapon_deagle", "weapon_crowbar", "weapon_awp", "weapon_svd", "weapon_barett", "item_pt", "\0"}, { {56, "50AE"}, {60, "762x54"}, {50, "338Magnum"}, {15, "127mm"}, {0, "\0"} } },
		{ 125, COST_SNIPER_3, {"weapon_deagle", "weapon_crowbar", "weapon_awp", "weapon_svd", "weapon_barett", "weapon_taucannon", "item_pt", "\0"}, { {70, "50AE"}, {72, "762x54"}, {60, "338Magnum"}, {25, "127mm"}, {50, "EnergyCells"}, {0, "\0"} } },
		{ 150, COST_SNIPER_4, {"weapon_deagle", "weapon_crowbar", "weapon_awp", "weapon_svd", "weapon_barett", "weapon_taucannon", "weapon_m72", "item_pt", "\0"}, { {84, "50AE"}, {96, "762x54"}, {80, "338Magnum"}, {40, "127mm"}, {150, "EnergyCells"}, {60, "2mm"}, {30, "quad"}, {0, "\0"} } }
	},

	{//soldier - assault, no step back
		{ 100, 0, {"weapon_glock_akimbo", "weapon_crowbar", "weapon_handgrenade", "weapon_mp5", "\0"}, { {3, "Hand Grenade"}, {68, "9mmP"}, {120, "9mm"}, {3, "ARgrenades"}, {0, "\0"} } },
		{ 130, COST_SOLDIER_1, {"weapon_glock_akimbo", "weapon_crowbar", "weapon_handgrenade", "weapon_mp5", "weapon_u2", "item_kevlar", "\0"}, { {5, "Hand Grenade"}, {102, "9mmP"}, {160, "9mm"}, {4, "ARgrenades"}, {60, "556"}, {3, "U2grenades"}, {0, "\0"} } },
		{ 160, COST_SOLDIER_2, {"weapon_glock_akimbo", "weapon_crowbar", "weapon_handgrenade", "weapon_mp5", "weapon_u2", "weapon_ak74", "item_kevlar", "\0"}, { {7, "Hand Grenade"}, {136, "9mmP"}, {210, "9mm"}, {6, "ARgrenades"}, {120, "556"}, {6, "U2grenades"}, {90, "762"}, {5, "AK74grenades"}, {0, "\0"} } },
		{ 200, COST_SOLDIER_3, {"weapon_glock_akimbo", "weapon_crowbar", "weapon_handgrenade", "weapon_mp5", "weapon_u2", "weapon_ak74", "weapon_30mmsg", "item_kevlar", "\0"}, { {8, "Hand Grenade"}, {170, "9mmP"}, {240, "9mm"}, {8, "ARgrenades"}, {150, "556"}, {8, "U2grenades"}, {150, "762"}, {8, "AK74grenades"}, {16, "30mm"}, {0, "\0"} } },
		{ 250, COST_SOLDIER_4, {"weapon_glock_akimbo", "weapon_crowbar", "weapon_handgrenade", "weapon_mp5", "weapon_u2", "weapon_ak74", "weapon_30mmsg", "weapon_teslagun", "item_kevlar", "\0"}, { {10, "Hand Grenade"}, {204, "9mmP"}, {300, "9mm"}, {12, "ARgrenades"}, {210, "556"}, {12, "U2grenades"}, {210, "762"}, {10, "AK74grenades"}, {28, "30mm"}, {240, "ShockCore"}, {20, "ShockGrenade"}, {0, "\0"} } }
	},

	{//medic - medicine
		{ 80, 0, {"weapon_uzi", "weapon_handgrenade", "weapon_medkit", "item_kevlar", "\0"}, { {96, "8mm"}, {2, "Hand Grenade"}, {200, "Health"}, {0, "\0"} } },
		{ 110, COST_MEDIC_1, {"weapon_uzi_akimbo", "weapon_handgrenade", "weapon_medkit", "item_portableHealthkit", "item_kevlar", "\0"}, { {192, "8mm"}, {4, "Hand Grenade"}, {400, "Health"}, {0, "\0"} } },
		{ 140, COST_MEDIC_2, {"weapon_uzi_akimbo", "weapon_handgrenade", "weapon_medkit", "weapon_akimbogun", "item_portableHealthkit", "item_kevlar", "\0"}, { {192, "8mm"}, {6, "Hand Grenade"}, {600, "Health"}, {120, "762Magnum"}, {0, "\0"} } },
		{ 170, COST_MEDIC_3, {"weapon_uzi_akimbo", "weapon_handgrenade", "weapon_medkit", "weapon_akimbogun", "weapon_m16", "item_portableHealthkit", "item_kevlar", "item_longjump", "\0"}, { {320, "8mm"}, {7, "Hand Grenade"}, {800, "Health"}, {90, "762Nato"}, {8, "NGgrenades"}, {180, "762Magnum"}, {0, "\0"} } },
		{ 210, COST_MEDIC_4, {"weapon_uzi_akimbo", "weapon_handgrenade", "weapon_medkit", "weapon_akimbogun", "weapon_m16", "weapon_biorifle", "item_portableHealthkit", "item_kevlar", "item_longjump", "\0"}, { {384, "8mm"}, {8, "Hand Grenade"}, {999, "Health"}, {180, "762Nato"}, {15, "NGgrenades"}, {90, "biocharge"}, {240, "762Magnum"}, {0, "\0"} } }
	},

	{//machinegunner - universal heavy class
		{ 125, 0, {"weapon_python", "weapon_bandsaw", "weapon_m249", "\0"}, { {24, "357"}, {200, "556Nato"}, {0, "\0"} } },
		{ 150, COST_MACHINEGUNNER_1, {"weapon_python", "weapon_bandsaw", "weapon_m249", "weapon_minigun", "\0"}, { {36, "357"}, {400, "556Nato"}, {240, "86mm"}, {0, "\0"} } },
		{ 185, COST_MACHINEGUNNER_2, {"weapon_python", "weapon_bandsaw", "weapon_m249", "weapon_minigun", "weapon_nailgun", "\0"}, { {48, "357"}, {500, "556Nato"}, {480, "86mm"}, {120, "nails"}, {0, "\0"} } },
		{ 240, COST_MACHINEGUNNER_3, {"weapon_python", "weapon_bandsaw", "weapon_m249", "weapon_minigun", "weapon_nailgun", "weapon_photongun", "\0"}, { {60, "357"}, {700, "556Nato"}, {720, "86mm"}, {180, "nails"}, {120, "uranium235"}, {0, "\0"} } },
		{ 300, COST_MACHINEGUNNER_4, {"weapon_python", "weapon_bandsaw", "weapon_m249", "weapon_minigun", "weapon_nailgun", "weapon_photongun", "weapon_machinegun", "item_harmor", "\0"}, { {72, "357"}, {900, "556Nato"}, {960, "86mm"}, {240, "nails"}, {240, "uranium235"}, {180, "32mm"}, {0, "\0"} } },
	},

	{//lander - air assault, artillery, cleanup
		{ 130, 0, {"weapon_lightsaber", "weapon_smartgun", "weapon_handgrenade", "item_antigrav", "\0"}, { {4, "Hand Grenade"}, {0, "\0"} } },
		{ 150, COST_LANDER_1, {"weapon_lightsaber", "weapon_smartgun", "weapon_handgrenade", "weapon_blaster", "item_antigrav", "weapon_plasmarifle", "\0"}, { {5, "Hand Grenade"}, {60, "MicroFusionCells"}, {0, "\0"} } },
		{ 175, COST_LANDER_2, {"weapon_lightsaber", "weapon_smartgun", "weapon_handgrenade", "weapon_blaster", "item_antigrav", "item_kevlar", "weapon_plasmarifle", "weapon_flakcannon", "\0"}, { {6, "Hand Grenade"}, {120, "MicroFusionCells"}, {15, "shrapnel"}, {0, "\0"} } },
		{ 200, COST_LANDER_3, {"weapon_lightsaber", "weapon_smartgun", "weapon_handgrenade", "weapon_blaster", "item_antigrav", "item_kevlar", "item_portableHEV", "weapon_plasmarifle", "weapon_flakcannon", "\0"}, { {8, "Hand Grenade"}, {180, "MicroFusionCells"}, {40, "shrapnel"}, {0, "\0"} } },
		{ 200, COST_LANDER_4, {"weapon_lightsaber", "weapon_smartgun", "weapon_handgrenade", "weapon_blaster", "item_antigrav", "item_kevlar", "item_portableHEV", "weapon_plasmarifle", "weapon_flakcannon", "weapon_gluongun", "\0"}, { {10, "Hand Grenade"}, {300, "MicroFusionCells"}, {50, "shrapnel"}, {80, "GluonCells"}, {0, "\0"} } },
	},

	{//engineer - battle maintenance, like medic, but for machines
		{ 60, 0, {"weapon_shieldgun", "weapon_torch", "item_kevlar", "\0"}, { {30, "12g"}, {200, "propane"}, {0, "\0"} } },
		{ 80, COST_ENGINER_1, {"weapon_shieldgun", "weapon_torch", "weapon_flamethrower", "item_kevlar", "\0"}, { {42, "12g"}, {400, "propane"}, {100, "fuel"}, {0, "\0"} } },
		{ 120, COST_ENGINER_2, {"weapon_flamethrower", "weapon_shieldgun", "weapon_torch", "weapon_g11", "item_kevlar", "\0"}, { {54, "12g"}, {600, "propane"}, {300, "fuel"}, {150, "57mm"}, {0, "\0"} } },
		{ 140, COST_ENGINER_3, {"weapon_flamethrower", "weapon_shieldgun", "weapon_torch", "weapon_pulserifle", "weapon_g11", "item_kevlar", "\0"}, { {66, "12g"}, {800, "propane"}, {400, "fuel"}, {30, "stormcell"}, {250, "57mm"}, {0, "\0"} } },
		{ 180, COST_ENGINER_4, {"weapon_flamethrower", "weapon_shieldgun", "weapon_torch", "weapon_pulserifle", "weapon_g11", "weapon_c4", "item_kevlar", "\0"}, { {78, "12g"}, {999, "propane"}, {500, "fuel"}, {50, "stormcell"}, {400, "57mm"}, {5, "C4"}, {0, "\0"} } }
	},

	{//counter-armor - universal rocket class
		{ 90, 0, {"weapon_python", "weapon_crowbar", "weapon_rpg", "\0"}, { {24, "357"}, {6, "rockets"}, {0, "\0"} } },
		{ 120, COST_COUNTERARMOR_1, {"weapon_python", "weapon_crowbar", "weapon_rpg", "weapon_incendiary", "\0"}, { {36, "357"}, {8, "rockets"}, {9, "hellfire"}, {0, "\0"} } },
		{ 150, COST_COUNTERARMOR_2, {"weapon_python", "weapon_crowbar", "weapon_rpg", "weapon_incendiary", "weapon_whl", "\0"}, { {54, "357"}, {10, "rockets"}, {12, "hellfire"}, {4, "Dmissile"}, {0, "\0"} } },
		{ 180, COST_COUNTERARMOR_3, {"weapon_python", "weapon_crowbar", "weapon_rpg", "weapon_incendiary", "weapon_whl", "weapon_devastator", "\0"}, { {70, "357"}, {12, "rockets"}, {15, "hellfire"}, {6, "Dmissile"}, {16, "Mmissiles"}, {0, "\0"} } },
		{ 250, COST_COUNTERARMOR_4, {"weapon_python", "weapon_crowbar", "weapon_rpg", "weapon_incendiary", "weapon_whl", "weapon_devastator", "weapon_displacer", "\0"}, { {84, "357"}, {15, "rockets"}, {21, "hellfire"}, {9, "Dmissile"}, {32, "Mmissiles"}, {120, "isotope"}, {0, "\0"} } }
	},

	{//fortificator - protection
		{ 0, 0, {"item_PowerShield", "weapon_glock_akimbo", "weapon_bandsaw", "weapon_shotgun", "weapon_tripmine", "\0"}, { {68, "9mmP"}, {32, "buckshot"}, {5, "Trip Mine"}, {0, "\0"} } },
		{ 15, COST_FORTIFICATOR_1, {"item_PowerShield", "weapon_glock_akimbo", "weapon_bandsaw", "weapon_shotgun", "weapon_autoshotgun", "weapon_tripmine", "\0"}, { {102, "9mmP"}, {48, "buckshot"}, {30, "10mmbuckshot"}, {10, "Trip Mine"}, {0, "\0"} } },
		{ 30, COST_FORTIFICATOR_2, {"item_PowerShield", "weapon_glock_akimbo", "weapon_bandsaw", "weapon_shotgun", "weapon_autoshotgun", "weapon_tripmine", "weapon_satchel", "\0"}, { {50, "10mmbuckshot"}, {64, "buckshot"}, {136, "9mmP"}, {15, "Trip Mine"}, {5, "Satchel Charge"}, {0, "\0"} } },
		{ 60, COST_FORTIFICATOR_3, {"item_PowerShield", "weapon_glock_akimbo", "weapon_bandsaw", "weapon_shotgun", "weapon_autoshotgun", "weapon_tripmine", "weapon_satchel", "weapon_turretkit", "\0"}, { {70, "10mmbuckshot"}, {80, "buckshot"}, {170, "9mmP"}, {20, "Trip Mine"}, {10, "Satchel Charge"}, {70, "energypack"}, {0, "\0"} } },
		{ 90, COST_FORTIFICATOR_4, {"item_PowerShield", "weapon_glock_akimbo", "weapon_bandsaw", "weapon_shotgun", "weapon_autoshotgun", "weapon_tripmine", "weapon_satchel", "weapon_turretkit", "weapon_egon", "\0"}, { {100, "10mmbuckshot"}, {96, "buckshot"}, {204, "9mmP"}, {25, "Trip Mine"}, {15, "Satchel Charge"}, {150, "energypack"}, {180, "uranium238"}, {0, "\0"} } }
	}
};

//=======================================END=========================================

CHLWECTF::CHLWECTF () : CHalfLifeTeamplay()
{
	memset (team_scores, 0, sizeof team_scores);

	m_flNextThinkTime = gpGlobals->time + 0.2;
}

CHLWECTF::~CHLWECTF ()
{
	memset (team_scores, 0, sizeof team_scores);
}

//================================CLASS SYSTEM START=================================

BOOL CHLWECTF :: ClientCommand( CBasePlayer *pPlayer, const char *pcmd )
{
	if (!strncmp (pcmd, "unlock", 6))
	{
		if (pPlayer->IsAlive()) return 1;

		int iClass;

		if (pPlayer->m_iClass == 0)
			return 1;
		else if (pPlayer->m_iClass == 10)
			iClass = pPlayer->m_iKit;
		else
			iClass = pPlayer->m_iClass - 1;

		int iUnlock = pcmd[6] - '0';

		if (iUnlock > 4) return 0;

		if (iUnlock > pPlayer->m_iUnlock[iClass] + 1)
		{
			char message[64];
			sprintf (message, "Only Unlock %d is available for now!", pPlayer->m_iUnlock[iClass] + 1);
			ClientPrint (pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs(message));
		}
		else if (iUnlock > pPlayer->m_iUnlock[iClass])
		{
			if (pPlayer->m_flMoneyAmount >= class_kits[iClass][iUnlock].cost)
			{
				pPlayer->m_flMoneyAmount -= class_kits[iClass][iUnlock].cost;//zero unlock is default
				pPlayer->m_iUnlock[iClass] = iUnlock;
			}
			else
				ClientPrint (pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("Not enough money!"));
		}
		else if (iUnlock == pPlayer->m_iUnlock[iClass])
			ClientPrint (pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("You already have this weapons!"));
		else
			ClientPrint (pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("You already have better weapons!"));

		return 1;
	}

	else if (!strcmp(pcmd, "teammenu"))
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgJoinMenu, NULL, pPlayer->pev);
			WRITE_BYTE(12);
			WRITE_BYTE(0);//doesn't matter
			WRITE_BYTE(1);//show CTF splash
		MESSAGE_END();

		return 1;
	}

	else if (!strncmp (pcmd, "class", 5))
	{
		if (!pPlayer->IsAlive())
		{
			if (pPlayer->m_iClass != pcmd[5] - '0')//same class
			{
				if (!strcmp(pcmd, "class_random"))
				{
					pPlayer->m_iClass = 10;
					pPlayer->m_iKit = RANDOM_LONG(0, 8);//can cause kit changing, if type command "class_random" in console
				}
				else
					pPlayer->m_iClass = pcmd[5] - '0';//zero class means, that we should show join menu, I suppose; Random class number is 10
			}
		}
		return 1;
	}

	return CHalfLifeTeamplay::ClientCommand(pPlayer, pcmd);
}


void CHLWECTF :: ShowMenu ( CBasePlayer* pPlayer )
{
	if (pPlayer->IsAlive())
		return;

	if (!pPlayer->m_iClass)
		return;

	MESSAGE_BEGIN(MSG_ONE, gmsgJoinMenu, NULL, pPlayer->pev);
		WRITE_BYTE(14);//unlock menu, 12 - only team menu, 13 - join menu
		WRITE_BYTE(pPlayer->m_iClass);
		WRITE_BYTE(pPlayer->m_iKit);
	MESSAGE_END();
}

void CHLWECTF :: InterpretBuyCommand ( const char* cmd, CBasePlayer* pPlayer )
{
	return;//don't allow to buy weapons
}

//=================================CLASS SYSTEM END===================================

void CHLWECTF :: UpdateGameMode( CBasePlayer *pPlayer )
{
	MESSAGE_BEGIN( MSG_ONE, gmsgGameMode, NULL, pPlayer->edict() );
		WRITE_BYTE( 2 );  // game mode - "CTF"
	MESSAGE_END();
}

void CHLWECTF :: Think ( void )
{
	CHalfLifeTeamplay::Think();
}

void CHLWECTF :: PlayerSpawn( CBasePlayer *pPlayer )
{
	pPlayer->pev->weapons |= (1<<WEAPON_SUIT);

	int iClass;

	if (pPlayer->m_iClass == 0)
		return;
	else if (pPlayer->m_iClass == 10)
		iClass = pPlayer->m_iKit;
	else
		iClass = pPlayer->m_iClass - 1;

	pPlayer->pev->armorvalue = class_kits[iClass][pPlayer->m_iUnlock[iClass]].armorvalue;

	int i = 0;

	while (class_kits[iClass][pPlayer->m_iUnlock[iClass]].weapons[i][0] != '\0')
	{
		pPlayer->GiveNamedItem (class_kits[iClass][pPlayer->m_iUnlock[iClass]].weapons[i]);
		++i;
	}

	i = 0;

	while (class_kits[iClass][pPlayer->m_iUnlock[iClass]].equip[i].name[0] != '\0')
	{
		pPlayer->GiveAmmo (class_kits[iClass][pPlayer->m_iUnlock[iClass]].equip[i].amount, class_kits[iClass][pPlayer->m_iUnlock[iClass]].equip[i].name, class_kits[iClass][pPlayer->m_iUnlock[iClass]].equip[i].amount);
		++i;
	}

	//note: try to assign to some squad or recruit squad members;
	//if before death bot was squad leader, wait for new recruits;
	//in other wise - follow old squad leader
}

void CHLWECTF :: PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor )
{
	//note: remove from squad leader's team list;
	//if bot was squad leader, declare new commander and put squad under his control

	if (pVictim->m_pCarryingObject != NULL)
		pVictim->m_pCarryingObject->Use(pVictim, pVictim, USE_TOGGLE, 2);// COU_DROPPED = 2

	if (pVictim->m_iClass == 10)
		pVictim->m_iKit = RANDOM_LONG (0,8);//apriory randomize
	
	CHalfLifeTeamplay::PlayerKilled(pVictim, pKiller, pInflictor);
}

void CHLWECTF :: InitHUD( CBasePlayer *pPlayer )
{
	CHalfLifeTeamplay::InitHUD (pPlayer);

	MESSAGE_BEGIN(MSG_ONE, gmsgJoinMenu, NULL, pPlayer->pev);
		WRITE_BYTE(13);//join menu, 12 - only team menu, 14 - unlock menu
		WRITE_BYTE(pPlayer->m_iClass);
		WRITE_BYTE(pPlayer->m_iKit);
	MESSAGE_END();
}

int CHLWECTF :: AddScoreToTeam(int teamIndex, int score)
{
	int old = team_scores[teamIndex];
	team_scores[teamIndex] += score;

	if (score)
	{
		MESSAGE_BEGIN(MSG_ALL, gmsgTeamScore, NULL);
			WRITE_STRING(GetIndexedTeamName(teamIndex));
			WRITE_SHORT(team_scores[teamIndex]);
			WRITE_SHORT(0);//shit, this team_scores[!teamIndex] based on 2 teams teamplay
		MESSAGE_END();
	}

	return old;
}

void CHLWECTF :: ClientDisconnected (edict_t* pClient)
{
	CBaseEntity *pEnt = CBaseEntity::Instance (pClient);

	if (!pEnt->IsPlayer())
		return;

	CBasePlayer *pPlayer = (CBasePlayer*)pEnt;

	if (pPlayer->m_pCarryingObject != NULL)
		pPlayer->m_pCarryingObject->Use(pPlayer, pPlayer, USE_TOGGLE, 2);// COU_DROPPED = 2
}

//all the same, except extra health, armor and weapons for multikills
void CHLWECTF::DeathNotice( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pevInflictor )
{
	// Work out what killed the player, and send a message to all clients about it
	CBaseEntity *Killer = CBaseEntity::Instance( pKiller );

	const char *killer_weapon_name = "world";		// by default, the player is killed by the world
	int killer_index = 0;
	
	if ( pKiller->flags & FL_CLIENT )
	{
		killer_index = ENTINDEX(ENT(pKiller));
		
		if ( pevInflictor )
		{
			if ( pevInflictor == pKiller )
			{
				// If the inflictor is the killer,  then it must be their current weapon doing the damage
				CBasePlayer *pPlayer = (CBasePlayer*)CBaseEntity::Instance( pKiller );
				
				if ( pPlayer->m_pActiveItem )
				{
					killer_weapon_name = pPlayer->m_pActiveItem->pszName();
				}
			}
			else
			{
				killer_weapon_name = STRING( pevInflictor->classname );  // it's just that easy
			}
		}
	}
	else
	{
		killer_weapon_name = STRING( pevInflictor->classname );
	}

	// strip the monster_* or weapon_* from the inflictor's classname
	if ( strncmp( killer_weapon_name, "weapon_", 7 ) == 0 )
		killer_weapon_name += 7;
	else if ( strncmp( killer_weapon_name, "monster_", 8 ) == 0 )
		killer_weapon_name += 8;
	else if ( strncmp( killer_weapon_name, "func_", 5 ) == 0 )
		killer_weapon_name += 5;

	if      (pVictim->lastDamageType & DMG_HEADSHOT)
		killer_weapon_name = "headshot";
	else if (pVictim->lastDamageType & DMG_IGNITE)
		killer_weapon_name = "x_flame";
	else if (pVictim->lastDamageType & DMG_RADIATION)
		killer_weapon_name = "x_radiation";
	else if (pVictim->lastDamageType & DMG_NERVEGAS)
		killer_weapon_name = "x_nervegas";
	else if (pVictim->lastDamageType & DMG_POISON)
		killer_weapon_name = "x_poison";
	else if (pVictim->lastDamageType & DMG_ACID)
		killer_weapon_name = "x_acid";
	else if (pVictim->lastDamageType & DMG_FREEZE)
		killer_weapon_name = "x_freeze";
	else if (pVictim->lastDamageType & DMG_IGNOREARMOR)
		killer_weapon_name = "x_bleeding";

	MESSAGE_BEGIN( MSG_ALL, gmsgDeathMsg );
		WRITE_BYTE( killer_index );						// the killer
		WRITE_BYTE( ENTINDEX(pVictim->edict()) );		// the victim
		WRITE_STRING( killer_weapon_name );		// what they were killed by (should this be a string?)
	MESSAGE_END();

//===============
//MULTIKILL START
//===============
	if ( (pVictim->pev != pKiller) && Killer->IsPlayer() )
	{
		CBasePlayer *pPlayer = (CBasePlayer *)Killer;

		if ( pPlayer->lastKillTime > gpGlobals->time - MK_DELAY_TIME )
		{
			pPlayer->KillsAmount ++;

			char str[25];
			switch (pPlayer->KillsAmount)
			{
			case 1:
				EMIT_SOUND(pPlayer->edict(), CHAN_ITEM, "announce/kill_double.wav", 1, ATTN_NORM);
				pPlayer->AddMoney (GIVE_KILL_DOUBLE);
				sprintf(str, "#DOUBLE_KILL");
				break;
			case 2:
				EMIT_SOUND(pPlayer->edict(), CHAN_ITEM, "announce/kill_multi.wav", 1, ATTN_NORM);
				pPlayer->AddMoney (GIVE_KILL_MULTI);
				sprintf(str, "#MULTI_KILL");
				break;
			case 3:
				EMIT_SOUND(pPlayer->edict(), CHAN_ITEM, "announce/kill_monster.wav", 1, ATTN_NORM);
				pPlayer->AddMoney (GIVE_KILL_MONSTER);
				sprintf(str, "#MONSTER_KILL");
				break;
			case 4:
				EMIT_SOUND(pPlayer->edict(), CHAN_ITEM, "announce/kill_ultra.wav", 1, ATTN_NORM);
				pPlayer->AddMoney (GIVE_KILL_ULTRA);
				sprintf(str, "#ULTRA_KILL");
				break;
			case 5:
				EMIT_SOUND(pPlayer->edict(), CHAN_ITEM, "announce/kill_unstopable.wav", 1, ATTN_NORM);
				pPlayer->AddMoney (GIVE_KILL_UNSTOPABLE);
				sprintf(str, "#UNSTOPABLE_KILL");
				break;
			case 6:
				EMIT_SOUND(pPlayer->edict(), CHAN_ITEM, "announce/kill_godlike.wav", 1, ATTN_NORM);
				pPlayer->AddMoney (GIVE_KILL_GODLIKE);
				sprintf(str, "#GODLIKE_KILL");
				break;
			default:
				sprintf(str, "#TERMINATOR_KILL");
				pPlayer->AddMoney (GIVE_KILL_GODLIKE);
				break;
			}

			UTIL_ShowMessage(str, pPlayer);
		}
		else
			pPlayer->KillsAmount = 0;
		pPlayer->lastKillTime = gpGlobals->time;
	}
//=============
//MULTIKILL END
//=============

	if ( pVictim->pev == pKiller )  
	{
			UTIL_LogPrintf( "\"%s<%i><%s><%s>\" committed suicide with \"%s\"\n",  
				STRING( pVictim->pev->netname ), 
				GETPLAYERUSERID( pVictim->edict() ),
				GETPLAYERAUTHID( pVictim->edict() ),
				g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer( pVictim->edict() ), "model" ),
				killer_weapon_name );
	}
	else if ( pKiller->flags & FL_CLIENT )
	{
			UTIL_LogPrintf( "\"%s<%i><%s><%s>\" killed \"%s<%i><%s><%s>\" with \"%s\"\n",  
				STRING( pKiller->netname ),
				GETPLAYERUSERID( ENT(pKiller) ),
				GETPLAYERAUTHID( ENT(pKiller) ),
				g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer( ENT(pKiller) ), "model" ),
				STRING( pVictim->pev->netname ),
				GETPLAYERUSERID( pVictim->edict() ),
				GETPLAYERAUTHID( pVictim->edict() ),
				g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer( pVictim->edict() ), "model" ),
				killer_weapon_name );
	}
	else
	{ 
			UTIL_LogPrintf( "\"%s<%i><%s><%s>\" committed suicide with \"%s\" (world)\n",
				STRING( pVictim->pev->netname ), 
				GETPLAYERUSERID( pVictim->edict() ), 
				GETPLAYERAUTHID( pVictim->edict() ),
				g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer( pVictim->edict() ), "model" ),
				killer_weapon_name );
	}

	MESSAGE_BEGIN( MSG_SPEC, SVC_DIRECTOR );
		WRITE_BYTE ( 9 );	// command length in bytes
		WRITE_BYTE ( DRC_CMD_EVENT );	// player killed
		WRITE_SHORT( ENTINDEX(pVictim->edict()) );	// index number of primary entity
		if (pevInflictor)
			WRITE_SHORT( ENTINDEX(ENT(pevInflictor)) );	// index number of secondary entity
		else
			WRITE_SHORT( ENTINDEX(ENT(pKiller)) );	// index number of secondary entity
		WRITE_LONG( 7 | DRC_FLAG_DRAMATIC);   // eventflags (priority and flags)
	MESSAGE_END();
}