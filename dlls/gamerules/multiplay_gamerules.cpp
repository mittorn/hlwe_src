#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"player.h"
#include	"weapons.h"
#include	"gamerules.h"
#include	"skill.h"
#include	"game.h"
#include	"voice_gamemgr.h"
#include	"hltv.h"
#include	"items.h"

// START BOT
#include "bot.h"
extern respawn_t bot_respawn[32];
// END BOT

extern DLL_GLOBAL CGameRules	*g_pGameRules;
extern DLL_GLOBAL BOOL	g_fGameOver;
extern int gmsgDeathMsg;	// client dll messages
extern int gmsgScoreInfo;
extern int gmsgMOTD;
extern int gmsgServerName;

extern int g_teamplay;
extern int  gmsgVGUIMenu;
extern int gmsgManager;
float g_flIntermissionStartTime = 0;
CVoiceGameMgr	g_VoiceGameMgr;

class CMultiplayGameMgrHelper : public IVoiceGameMgrHelper
{
public:
	virtual bool		CanPlayerHearPlayer(CBasePlayer *pListener, CBasePlayer *pTalker)
	{
		if ( g_teamplay )
		{
			if ( g_pGameRules->PlayerRelationship( pListener, pTalker ) != GR_TEAMMATE )
			{
				return false;
			}
		}

		return true;
	}
};
static CMultiplayGameMgrHelper g_GameMgrHelper;

//*********************************************************
// Rules for the half-life multiplayer game.
//*********************************************************

//===========================================================
//Interpretation of buy-commands and menu calling moved here, 
//because it is game rule dependant
//===========================================================

bool CHalfLifeMultiplay :: IsAllowedToBuy ( CBasePlayer *pPlayer )
{
	if ( mp_nobuy.value > 0 )
	    return false;

	return (!pPlayer->IsAlive() || (!pPlayer->pev->waterlevel && !pPlayer->IsOnLadder()));
}

void CHalfLifeMultiplay :: ShowMenu ( CBasePlayer* pPlayer )
{
	if (!IsAllowedToBuy (pPlayer))
		return;

	MESSAGE_BEGIN(MSG_ONE, gmsgVGUIMenu, NULL, pPlayer->pev);
		WRITE_BYTE(pPlayer->IsAlive() ? 10 : 0);
	MESSAGE_END();
}

void CHalfLifeMultiplay :: InterpretBuyCommand ( const char* pcmd, CBasePlayer* pPlayer )
{
	if (!IsAllowedToBuy (pPlayer))
		return;

	//STUFF...
	if (FStrEq(pcmd, "buy_sell"))
	{
		if (pPlayer->IsAlive())
			pPlayer->SellCurrentWeapon( );
	}
	else if (FStrEq(pcmd, "buy_ammo1"))
	{
		if (pPlayer->m_pActiveItem)
		{
		    CBasePlayerItem *pGun = pPlayer->m_pActiveItem->GetWeaponPtr();
		    if (pGun)
		        ((CBasePlayerWeapon*)pGun)->BuyPrimaryAmmo();
		}
	}
	else if (FStrEq(pcmd, "buy_ammo2"))
	{
		if (pPlayer->m_pActiveItem)
		{
		    CBasePlayerItem *pGun = pPlayer->m_pActiveItem->GetWeaponPtr();
		    if (pGun)
		        ((CBasePlayerWeapon*)pGun)->BuySecondaryAmmo();
		}
	}
	//MELEE
	else if (FStrEq(pcmd, "buy_medkit"))
	{
		if (pPlayer->m_flMoneyAmount < COST_MEDKIT)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_MEDKIT] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_MEDKIT] = 1;
			pPlayer->m_flMoneyAmount -= COST_MEDKIT;
		}
	}
	else if (FStrEq(pcmd, "buy_crowbar"))
	{
		if (pPlayer->m_flMoneyAmount < COST_CROWBAR)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_CROWBAR] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_CROWBAR] = 1;
			pPlayer->m_flMoneyAmount -= COST_CROWBAR;
		}
	}
	else if (FStrEq(pcmd, "buy_bandsaw"))
	{
		if (pPlayer->m_flMoneyAmount < COST_BANDSAW)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_BANDSAW] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_BANDSAW] = 1;
			pPlayer->m_flMoneyAmount -= COST_BANDSAW;
		}
	}
	else if (FStrEq(pcmd, "buy_torch"))
	{
		if (pPlayer->m_flMoneyAmount < COST_TORCH)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_TORCH] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_TORCH] = 1;
			pPlayer->m_flMoneyAmount -= COST_TORCH;
		}
	}
	else if (FStrEq(pcmd, "buy_lightsaber"))
	{
		if (pPlayer->m_flMoneyAmount < COST_LIGHTSABER)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_LIGHTSABER] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_LIGHTSABER] = 1;
			pPlayer->m_flMoneyAmount -= COST_LIGHTSABER;
		}
	}
	//HANDGUNS
	else if (FStrEq(pcmd, "buy_glock"))
	{
		if (pPlayer->m_flMoneyAmount < COST_GLOCK)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_GLOCK] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_GLOCK] = 1;
			pPlayer->m_flMoneyAmount -= COST_GLOCK;
		}
	}
	else if (FStrEq(pcmd, "buy_usp"))
	{
		if (pPlayer->m_flMoneyAmount < COST_USP)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_USP] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_USP] = 1;
			pPlayer->m_flMoneyAmount -= COST_USP;
		}
	}
	else if (FStrEq(pcmd, "buy_deagle"))
	{
		if (pPlayer->m_flMoneyAmount < COST_DEAGLE)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_DEAGLE] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_DEAGLE] = 1;
			pPlayer->m_flMoneyAmount -= COST_DEAGLE;
		}
	}
	else if (FStrEq(pcmd, "buy_python"))
	{
		if (pPlayer->m_flMoneyAmount < COST_PYTHON)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_PYTHON] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}
		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_PYTHON] = 1;
			pPlayer->m_flMoneyAmount -= COST_PYTHON;
		}
	}
	else if (FStrEq(pcmd, "buy_uzi"))
	{
		if (pPlayer->m_flMoneyAmount < COST_UZI)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_UZI] == 2)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_UZI] += 1;
			pPlayer->m_flMoneyAmount -= COST_UZI;
		}
	}
	else if (FStrEq(pcmd, "buy_shieldgun"))
	{
		if (pPlayer->m_flMoneyAmount < COST_SHIELDGUN)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_SHIELDGUN] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_SHIELDGUN] = 1;
			pPlayer->m_flMoneyAmount -= COST_SHIELDGUN;
		}
	}
	//ASSAULT
	else if (FStrEq(pcmd, "buy_shotgun"))
	{
		if (pPlayer->m_flMoneyAmount < COST_SHOTGUN)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_SHOTGUN] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_SHOTGUN] = 1;
			pPlayer->m_flMoneyAmount -= COST_SHOTGUN;
		}
	}
	else if (FStrEq(pcmd, "buy_autoshotgun"))
	{
		if (pPlayer->m_flMoneyAmount < COST_AUTOSHOTGUN)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_AUTOSHOTGUN] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_AUTOSHOTGUN] = 1;
			pPlayer->m_flMoneyAmount -= COST_AUTOSHOTGUN;
		}
	}
	else if (FStrEq(pcmd, "buy_30mmsg"))
	{
		if (pPlayer->m_flMoneyAmount < COST_SG30MM)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_30MMSG] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_30MMSG] = 1;
			pPlayer->m_flMoneyAmount -= COST_SG30MM;
		}
	}
	else if (FStrEq(pcmd, "buy_mp5"))
	{
		if (pPlayer->m_flMoneyAmount < COST_MP5)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_MP5] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_MP5] = 1;
			pPlayer->m_flMoneyAmount -= COST_MP5;
		}
	}
	else if (FStrEq(pcmd, "buy_m16"))
	{
		if (pPlayer->m_flMoneyAmount < COST_M16)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_M16] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_M16] = 1;
			pPlayer->m_flMoneyAmount -= COST_M16;
		}
	}
	else if (FStrEq(pcmd, "buy_akimbogun"))
	{
		if (pPlayer->m_flMoneyAmount < COST_AKIMBOGUN)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_AKIMBOGUN] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_AKIMBOGUN] = 1;
			pPlayer->m_flMoneyAmount -= COST_AKIMBOGUN;
		}
	}
	else if (FStrEq(pcmd, "buy_ak74"))
	{
		if (pPlayer->m_flMoneyAmount < COST_AK74)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_AK74] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_AK74] = 1;
			pPlayer->m_flMoneyAmount -= COST_AK74;
		}
	}
	//SNIPER
	else if (FStrEq(pcmd, "buy_g11"))
	{
		if (pPlayer->m_flMoneyAmount < COST_G11)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_G11] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_G11] = 1;
			pPlayer->m_flMoneyAmount -= COST_G11;
		}
	}
	else if (FStrEq(pcmd, "buy_u2"))
	{
		if (pPlayer->m_flMoneyAmount < COST_U2)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_U2] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_U2] = 1;
			pPlayer->m_flMoneyAmount -= COST_U2;
		}
	}
	else if (FStrEq(pcmd, "buy_crossbow"))
	{
		if (pPlayer->m_flMoneyAmount < COST_CROSSBOW)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_CROSSBOW] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_CROSSBOW] = 1;
			pPlayer->m_flMoneyAmount -= COST_CROSSBOW;
		}
	}
	else if (FStrEq(pcmd, "buy_svd"))
	{
		if (pPlayer->m_flMoneyAmount < COST_SVD)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_SVD] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_SVD] = 1;
			pPlayer->m_flMoneyAmount -= COST_SVD;
		}
	}
	else if (FStrEq(pcmd, "buy_awp"))
	{
		if (pPlayer->m_flMoneyAmount < COST_AWP)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_AWP] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_AWP] = 1;
			pPlayer->m_flMoneyAmount -= COST_AWP;
		}
	}
	else if (FStrEq(pcmd, "buy_barett"))
	{
		if (pPlayer->m_flMoneyAmount < COST_BARETT)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_BARETT] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_BARETT] = 1;
			pPlayer->m_flMoneyAmount -= COST_BARETT;
		}
	}
	//HEAVY
	else if (FStrEq(pcmd, "buy_m249"))
	{
		if (pPlayer->m_flMoneyAmount < COST_M249)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_M249] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_M249] = 1;
			pPlayer->m_flMoneyAmount -= COST_M249;
		}
	}
	else if (FStrEq(pcmd, "buy_minigun"))
	{
		if (pPlayer->m_flMoneyAmount < COST_MINIGUN)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_MINIGUN] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_MINIGUN] = 1;
			pPlayer->m_flMoneyAmount -= COST_MINIGUN;
		}
	}
	else if (FStrEq(pcmd, "buy_nailgun"))
	{
		if (pPlayer->m_flMoneyAmount < COST_NAILGUN)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_NAILGUN] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_NAILGUN] = 1;
			pPlayer->m_flMoneyAmount -= COST_NAILGUN;
		}
	}
	else if (FStrEq(pcmd, "buy_froster"))
	{
		if (pPlayer->m_flMoneyAmount < COST_FROSTER)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_FROSTER] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_FROSTER] = 1;
			pPlayer->m_flMoneyAmount -= COST_FROSTER;
		}
	}
	else if (FStrEq(pcmd, "buy_flamethrower"))
	{
		if (pPlayer->m_flMoneyAmount < COST_FLAMETHROWER)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_FLAMETHROWER] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_FLAMETHROWER] = 1;
			pPlayer->m_flMoneyAmount -= COST_FLAMETHROWER;
		}
	}
	else if (FStrEq(pcmd, "buy_flakcannon"))
	{
		if (pPlayer->m_flMoneyAmount < COST_FLAKCANNON)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_FLAKCANNON] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_FLAKCANNON] = 1;
			pPlayer->m_flMoneyAmount -= COST_FLAKCANNON;
		}
	}
	else if (FStrEq(pcmd, "buy_machinegun"))
	{
		if (pPlayer->m_flMoneyAmount < COST_MACHINEGUN)
		{
			pPlayer->NoMoney();
			return;
		}
		if(pPlayer->DeadItems[DEAD_BUY_HARMOR] != 1)
		{
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("Can't use without Heavy Armor!"));
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_MACHINEGUN] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_MACHINEGUN] = 1;
			pPlayer->m_flMoneyAmount -= COST_MACHINEGUN;
		}
	}
	else if (FStrEq(pcmd, "buy_bfg"))
	{
		if (pPlayer->m_flMoneyAmount < COST_BFG)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_BFG] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_BFG] = 1;
			pPlayer->m_flMoneyAmount -= COST_BFG;
		}
	}
	//LAUNCHERZ
	else if (FStrEq(pcmd, "buy_rpg"))
	{
		if (pPlayer->m_flMoneyAmount < COST_RPG)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_RPG] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_RPG] = 1;
			pPlayer->m_flMoneyAmount -= COST_RPG;
		}
	}
	else if (FStrEq(pcmd, "buy_incendiary"))
	{
		if (pPlayer->m_flMoneyAmount < COST_INCENDIARY)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_INCENDIARY] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_INCENDIARY] = 1;
			pPlayer->m_flMoneyAmount -= COST_INCENDIARY;
		}
	}
	else if (FStrEq(pcmd, "buy_whl"))
	{
		if (pPlayer->m_flMoneyAmount < COST_WHL)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_WHL] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_WHL] = 1;
			pPlayer->m_flMoneyAmount -= COST_WHL;
		}
	}
	else if (FStrEq(pcmd, "buy_devastator"))
	{
		if (pPlayer->m_flMoneyAmount < COST_DEVASTATOR)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_DEVASTATOR] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_DEVASTATOR] = 1;
			pPlayer->m_flMoneyAmount -= COST_DEVASTATOR;
		}
	}
	else if (FStrEq(pcmd, "buy_redeemer"))
	{
		if (pPlayer->m_flMoneyAmount < COST_REDEEMER)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_REDEEMER] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}
		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_REDEEMER] = 1;
			pPlayer->m_flMoneyAmount -= COST_REDEEMER;
		}
	}
	//ENERGY
	else if (FStrEq(pcmd, "buy_smartgun"))
	{
		if (pPlayer->m_flMoneyAmount < COST_SMARTGUN)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_SMARTGUN] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_SMARTGUN] = 1;
			pPlayer->m_flMoneyAmount -= COST_SMARTGUN;

			float flMaxArmor;
			if (pPlayer->DeadItems[DEAD_BUY_PORTABLEHEV] == 1)
				flMaxArmor = MAX_PHEV_BATTERY;
			else
				flMaxArmor = MAX_NORMAL_BATTERY;

			if (pPlayer->DeadItems[DEAD_BUY_BATTERY] < flMaxArmor/give_battery.value)
			{
				pPlayer->DeadItems[DEAD_BUY_BATTERY] += 1;
			}
		}
	}
	else if (FStrEq(pcmd, "buy_teslagun"))
	{
		if (pPlayer->m_flMoneyAmount < COST_TESLAGUN)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_TESLAGUN] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_TESLAGUN] = 1;
			pPlayer->m_flMoneyAmount -= COST_TESLAGUN;
		}
	}
	else if (FStrEq(pcmd, "buy_egon"))
	{
		if (pPlayer->m_flMoneyAmount < COST_EGON)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_EGON] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_EGON] = 1;
			pPlayer->m_flMoneyAmount -= COST_EGON;
		}
	}
	else if (FStrEq(pcmd, "buy_plasmarifle"))
	{
		if (pPlayer->m_flMoneyAmount < COST_PLASMARIFLE)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_PLASMARIFLE] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_PLASMARIFLE] = 1;
			pPlayer->m_flMoneyAmount -= COST_PLASMARIFLE;
		}
	}
	else if (FStrEq(pcmd, "buy_photongun"))
	{
		if (pPlayer->m_flMoneyAmount < COST_PHOTONGUN)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_PHOTONGUN] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_PHOTONGUN] = 1;
			pPlayer->m_flMoneyAmount -= COST_PHOTONGUN;
		}
	}
	else if (FStrEq(pcmd, "buy_gauss"))
	{
		if (pPlayer->m_flMoneyAmount < COST_GAUSS)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_GAUSS] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_GAUSS] = 1;
			pPlayer->m_flMoneyAmount -= COST_GAUSS;
		}
	}
	else if (FStrEq(pcmd, "buy_taucannon"))
	{
		if (pPlayer->m_flMoneyAmount < COST_TAUCANNON)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_TAUCANNON] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_TAUCANNON] = 1;
			pPlayer->m_flMoneyAmount -= COST_TAUCANNON;
		}
	}
	else if (FStrEq(pcmd, "buy_gluongun"))
	{
		if (pPlayer->m_flMoneyAmount < COST_GLUONGUN)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_GLUONGUN] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_GLUONGUN] = 1;
			pPlayer->m_flMoneyAmount -= COST_GLUONGUN;
		}
	}
	else if (FStrEq(pcmd, "buy_displacer"))
	{
		if (pPlayer->m_flMoneyAmount < COST_DISPLACER)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_DISPLACER] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_DISPLACER] = 1;
			pPlayer->m_flMoneyAmount -= COST_DISPLACER;
		}
	}
	//EXPLOSIVES
	else if (FStrEq(pcmd, "buy_flashbang"))
	{
		if (pPlayer->m_flMoneyAmount < COST_FLASHBANG)
		{
			pPlayer->NoMoney();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_FLASHBANG] += 1;
			pPlayer->m_flMoneyAmount -= COST_FLASHBANG;
		}
	}
	else if (FStrEq(pcmd, "buy_handgrenade"))
	{
		if (pPlayer->m_flMoneyAmount < COST_HANDGRENADE)
		{
			pPlayer->NoMoney();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_HANDGRENADE] += 1;
			pPlayer->m_flMoneyAmount -= COST_HANDGRENADE;
		}
	}
	else if (FStrEq(pcmd, "buy_tripmine"))
	{
		if (pPlayer->m_flMoneyAmount < COST_TRIPMINE)
		{
			pPlayer->NoMoney();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_TRIPMINE] += 1;
			pPlayer->m_flMoneyAmount -= COST_TRIPMINE;
		}
	}
	else if (FStrEq(pcmd, "buy_satchel"))
	{
		if (pPlayer->m_flMoneyAmount < COST_SATCHEL)
		{
			pPlayer->NoMoney();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_SATCHEL] += 1;
			pPlayer->m_flMoneyAmount -= COST_SATCHEL;
		}
	}
	else if (FStrEq(pcmd, "buy_c4"))
	{
		if (pPlayer->m_flMoneyAmount < COST_C4)
		{
			pPlayer->NoMoney();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_C4] += 1;
			pPlayer->m_flMoneyAmount -= COST_C4;
		}
	}
	//EXPERIMENTAL WEAPONS
	else if (FStrEq(pcmd, "buy_blaster"))
	{
		if (pPlayer->m_flMoneyAmount < COST_BLASTER)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_BLASTER] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_BLASTER] = 1;
			pPlayer->m_flMoneyAmount -= COST_BLASTER;
		}
	}
	else if (FStrEq(pcmd, "buy_biorifle"))
	{
		if (pPlayer->m_flMoneyAmount < COST_BIORIFLE)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_BIORIFLE] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_BIORIFLE] = 1;
			pPlayer->m_flMoneyAmount -= COST_BIORIFLE;
		}
	}
	else if (FStrEq(pcmd, "buy_pulserifle"))
	{
		if (pPlayer->m_flMoneyAmount < COST_PULSERIFLE)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_PULSERIFLE] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_PULSERIFLE] = 1;
			pPlayer->m_flMoneyAmount -= COST_PULSERIFLE;
		}
	}
	else if (FStrEq(pcmd, "buy_m72"))
	{
		if (pPlayer->m_flMoneyAmount < COST_M72)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_M72] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_M72] = 1;
			pPlayer->m_flMoneyAmount -= COST_M72;
		}
	}
	else if (FStrEq(pcmd, "buy_satellite"))
	{
		if (pPlayer->m_flMoneyAmount < COST_SATELLITE)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_SATELLITE] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_SATELLITE] = 1;
			pPlayer->m_flMoneyAmount -= COST_SATELLITE;
		}
	}
	else if (FStrEq(pcmd, "buy_turretkit"))
	{
		if (pPlayer->m_flMoneyAmount < COST_TURRETKIT)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_TURRETKIT] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_TURRETKIT] = 1;
			pPlayer->m_flMoneyAmount -= COST_TURRETKIT;
		}
	}
	else if (FStrEq(pcmd, "buy_chronosceptor"))
	{
		if (pPlayer->m_flMoneyAmount < COST_CHRONOSCEPTOR)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_CHRONOSCEPTOR] == 1)
		{
			pPlayer->HaveWeapon();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_CHRONOSCEPTOR] = 1;
			pPlayer->m_flMoneyAmount -= COST_CHRONOSCEPTOR;
		}
	}
	//EQUIPMENT ITEMS
	else if (FStrEq(pcmd, "buy_healthkit"))
	{
		if (pPlayer->m_flMoneyAmount < COST_HEALTHKIT)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->pev->health >= 100)
		{
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("You already have MAXIMUM HEALTH!"));
			return;
		}

		pPlayer->m_flMoneyAmount -= COST_HEALTHKIT;
		pPlayer->GiveNamedItem( "item_healthkit" );
	}
	else if (FStrEq(pcmd, "buy_battery"))
	{
		if (pPlayer->m_flMoneyAmount < COST_BATTERY)
		{
			pPlayer->NoMoney();
			return;
		}
			float flMaxArmor;
			if (pPlayer->DeadItems[DEAD_BUY_PORTABLEHEV] == 1)
				flMaxArmor = MAX_PHEV_BATTERY;
			else
				flMaxArmor = MAX_NORMAL_BATTERY;

		if (pPlayer->DeadItems[DEAD_BUY_BATTERY] >= flMaxArmor/give_battery.value)
		{
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("You already have MAXIMUM ARMOR!"));
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_BATTERY] += 1;
			pPlayer->m_flMoneyAmount -= COST_BATTERY;
		}
	}
	else if (FStrEq(pcmd, "buy_flashbattery"))
	{
		if (pPlayer->m_flMoneyAmount < COST_FLASHBATTERY)
		{
			pPlayer->NoMoney();
			return;
		}
		if (!pPlayer->IsAlive() || pPlayer->m_iFlashBattery >= 99)
		{
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("You already have MAXIMUM ENERGY!"));
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_FLASHBATTERY;
		pPlayer->GiveNamedItem( "item_flashbattery" );
	}
	else if (FStrEq(pcmd, "buy_kevlar"))
	{
		if (pPlayer->m_flMoneyAmount < COST_KEVLAR)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_KEVLAR] == 1)
		{
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("You already have KEVLAR ARMOR!"));
			return;
		}
		if(pPlayer->DeadItems[DEAD_BUY_HARMOR] == 1)
		{
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("You already have the best ARMOR!"));
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_KEVLAR] = 1;
			pPlayer->m_flMoneyAmount -= COST_KEVLAR;
		}
	}
	else if (FStrEq(pcmd, "buy_harmor"))
	{
		if (pPlayer->m_flMoneyAmount < COST_HARMOR)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_HARMOR] == 1)
		{
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("You already have HEAVY ARMOR!"));
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_HARMOR] = 1;
			pPlayer->m_flMoneyAmount -= COST_HARMOR;
		}
	}
	//SUIT POWERUPS
	else if (FStrEq(pcmd, "buy_longjump"))
	{
		if (pPlayer->m_flMoneyAmount < COST_LONGJUMP)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_LONGJUMP] == 1)
		{
			pPlayer->HaveItem();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_HARMOR] == 1)
		{
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("Can't use with HEAVY ARMOR!"));
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_LONGJUMP] = 1;
			pPlayer->m_flMoneyAmount -= COST_LONGJUMP;
		}
	}
	else if (FStrEq(pcmd, "buy_antigrav"))
	{
		if (pPlayer->m_flMoneyAmount < COST_ANTIGRAV)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_ANTIGRAV] == 1)
		{
			pPlayer->HaveItem();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_HARMOR] == 1)
		{
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("Can't use with HEAVY ARMOR!"));
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_ANTIGRAV] = 1;
			pPlayer->m_flMoneyAmount -= COST_ANTIGRAV;
		}
	}
	else if (FStrEq(pcmd, "buy_cloak"))
	{
		if (pPlayer->m_flMoneyAmount < COST_CLOAK)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_CLOAK] == 1)
		{
			pPlayer->HaveItem();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_CLOAK] = 1;
			pPlayer->m_flMoneyAmount -= COST_CLOAK;
		}
	}
	else if (FStrEq(pcmd, "buy_teleport"))
	{
		if (pPlayer->m_flMoneyAmount < COST_PT)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_PT] == 1)
		{
			pPlayer->HaveItem();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_PT] = 1;
			pPlayer->m_flMoneyAmount -= COST_PT;
		}
	}
	else if (FStrEq(pcmd, "buy_powershield"))
	{
		if (pPlayer->m_flMoneyAmount < COST_PSHIELD)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_POWERSHIELD] == 1)
		{
			pPlayer->HaveItem();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_POWERSHIELD] = 1;
			pPlayer->m_flMoneyAmount -= COST_PSHIELD;
		}
	}
	else if (FStrEq(pcmd, "buy_phealthkit"))
	{
		if (pPlayer->m_flMoneyAmount < COST_PHK)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_PORTABLEHEALTHKIT] == 1)
		{
			pPlayer->HaveItem();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_PORTABLEHEALTHKIT] = 1;
			pPlayer->m_flMoneyAmount -= COST_PHK;
		}
	}
	else if (FStrEq(pcmd, "buy_portHEV"))
	{
		if (pPlayer->m_flMoneyAmount < COST_PHEV)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->DeadItems[DEAD_BUY_PORTABLEHEV] == 1)
		{
			pPlayer->HaveItem();
			return;
		}

		if (!pPlayer->IsAlive())
		{
			pPlayer->DeadItems[DEAD_BUY_PORTABLEHEV] = 1;
			pPlayer->m_flMoneyAmount -= COST_PHEV;
		}
	}
}

//===================================================================
//
//							BUY MENU END
//
//===================================================================

CHalfLifeMultiplay :: CHalfLifeMultiplay()
{
	g_VoiceGameMgr.Init(&g_GameMgrHelper, gpGlobals->maxClients);
	m_flIntermissionEndTime = 0;
	g_flIntermissionStartTime = 0;
	
	// 11/8/98
	// Modified by YWB:  Server .cfg file is now a cvar, so that 
	//  server ops can run multiple game servers, with different server .cfg files,
	//  from a single installed directory.
	// Mapcyclefile is already a cvar.

	// 3/31/99
	// Added lservercfg file cvar, since listen and dedicated servers should not
	// share a single config file. (sjb)
	if ( IS_DEDICATED_SERVER() )
	{
		// dedicated server
		char *servercfgfile = (char *)CVAR_GET_STRING( "servercfgfile" );

		if ( servercfgfile && servercfgfile[0] )
		{
			char szCommand[256];
			
			ALERT( at_console, "Executing dedicated server config file\n" );
			sprintf( szCommand, "exec %s\n", servercfgfile );
			SERVER_COMMAND( szCommand );
		}
	}
	else
	{
		// listen server
		char *lservercfgfile = (char *)CVAR_GET_STRING( "lservercfgfile" );

		if ( lservercfgfile && lservercfgfile[0] )
		{
			char szCommand[256];
			
			ALERT( at_console, "Executing listen server config file\n" );
			sprintf( szCommand, "exec %s\n", lservercfgfile );
			SERVER_COMMAND( szCommand );
		}
	}
}

// longest the intermission can last, in seconds
#define MAX_INTERMISSION_TIME		120

//=========================================================
//=========================================================
void CHalfLifeMultiplay :: Think ( void )
{
	g_VoiceGameMgr.Update(gpGlobals->frametime);

	///// Check game rules /////
	static int last_frags;
	static int last_time;

	int frags_remaining = 0;
	int time_remaining = 0;

	if ( g_fGameOver )   // someone else quit the game already
	{
		// bounds check
		int time = (int)CVAR_GET_FLOAT( "mp_chattime" );
		if ( time < 1 )
			CVAR_SET_STRING( "mp_chattime", "1" );
		else if ( time > MAX_INTERMISSION_TIME )
			CVAR_SET_STRING( "mp_chattime", UTIL_dtos1( MAX_INTERMISSION_TIME ) );

		m_flIntermissionEndTime = g_flIntermissionStartTime + mp_chattime.value;

		// check to see if we should change levels now
		if ( m_flIntermissionEndTime < gpGlobals->time )
		{
			if ( m_iEndIntermissionButtonHit  // check that someone has pressed a key, or the max intermission time is over
				|| ( ( g_flIntermissionStartTime + MAX_INTERMISSION_TIME ) < gpGlobals->time) ) 
				ChangeLevel(); // intermission is over
		}

		return;
	}

	float flTimeLimit = mp_timelimit.value * 60;
	float flFragLimit = mp_fraglimit.value;

	time_remaining = (int)(flTimeLimit ? ( flTimeLimit - gpGlobals->time ) : 0);
	
	if ( flTimeLimit != 0 && gpGlobals->time >= flTimeLimit )
	{
		GoToIntermission();
		return;
	}

	if ( flFragLimit )
	{
		int bestfrags = 9999;
		int remain;

		// check if any player is over the frag limit
		for ( int i = 1; i <= gpGlobals->maxClients; i++ )
		{
			CBaseEntity *pPlayer = UTIL_PlayerByIndex( i );

			if ( pPlayer && pPlayer->pev->frags >= flFragLimit )
			{
				GoToIntermission();
				return;
			}


			if ( pPlayer )
			{
				remain = flFragLimit - pPlayer->pev->frags;
				if ( remain < bestfrags )
				{
					bestfrags = remain;
				}
			}

		}
		frags_remaining = bestfrags;
	}

	// Updates when frags change
	if ( frags_remaining != last_frags )
	{
		g_engfuncs.pfnCvar_DirectSet( &mp_fragsleft, UTIL_VarArgs( "%i", frags_remaining ) );
	}

	// Updates once per second
	if ( mp_timeleft.value != last_time )
	{
		g_engfuncs.pfnCvar_DirectSet( &mp_timeleft, UTIL_VarArgs( "%i", time_remaining ) );
	}

	last_frags = frags_remaining;
	last_time  = time_remaining;
}


//=========================================================
//=========================================================
BOOL CHalfLifeMultiplay::IsMultiplayer( void )
{
	return TRUE;
}

//=========================================================
//=========================================================
BOOL CHalfLifeMultiplay::IsDeathmatch( void )
{
	return TRUE;
}

//=========================================================
//=========================================================
BOOL CHalfLifeMultiplay::IsCoOp( void )
{
	return FALSE;
}

//=========================================================
//=========================================================
BOOL CHalfLifeMultiplay::FShouldSwitchWeapon( CBasePlayer *pPlayer, CBasePlayerItem *pWeapon )
{
	if (!pWeapon->CanDeploy())
		return FALSE;// that weapon can't deploy anyway.

	if (!pPlayer->m_pActiveItem)
		return TRUE;// player doesn't have an active item!

	if (!pPlayer->m_pActiveItem->CanHolster())
		return FALSE;// can't put away the active item.

	if (pWeapon->iWeight() > pPlayer->m_pActiveItem->iWeight())
		return TRUE;

	return FALSE;
}

BOOL CHalfLifeMultiplay :: GetNextBestWeapon( CBasePlayer *pPlayer, CBasePlayerItem *pCurrentWeapon )
{

	CBasePlayerItem *pCheck;
	CBasePlayerItem *pBest;// this will be used in the event that we don't find a weapon in the same category.
	int iBestWeight;
	int i;

	iBestWeight = -1;// no weapon lower than -1 can be autoswitched to
	pBest = NULL;

	if ( !pCurrentWeapon->CanHolster() )
	{
		// can't put this gun away right now, so can't switch.
		return FALSE;
	}

	for ( i = 0 ; i < MAX_ITEM_TYPES ; i++ )
	{
		pCheck = pPlayer->m_rgpPlayerItems[ i ];

		while ( pCheck )
		{
			if ( pCheck->iWeight() > -1 && pCheck->iWeight() == pCurrentWeapon->iWeight() && pCheck != pCurrentWeapon )
			{
				// this weapon is from the same category. 
				if ( pCheck->CanDeploy() )
				{
					if ( pPlayer->SwitchWeapon( pCheck ) )
					{
						return TRUE;
					}
				}
			}
			else if ( pCheck->iWeight() > iBestWeight && pCheck != pCurrentWeapon )// don't reselect the weapon we're trying to get rid of
			{
				//ALERT ( at_console, "Considering %s\n", STRING( pCheck->pev->classname ) );
				// we keep updating the 'best' weapon just in case we can't find a weapon of the same weight
				// that the player was using. This will end up leaving the player with his heaviest-weighted 
				// weapon. 
				if ( pCheck->CanDeploy() )
				{
					// if this weapon is useable, flag it as the best
					iBestWeight = pCheck->iWeight();
					pBest = pCheck;
				}
			}

			pCheck = pCheck->m_pNext;
		}
	}

	// if we make it here, we've checked all the weapons and found no useable 
	// weapon in the same catagory as the current weapon. 
	
	// if pBest is null, we didn't find ANYTHING. Shouldn't be possible- should always 
	// at least get the crowbar, but ya never know.
	if ( !pBest )
	{
		return FALSE;
	}

	pPlayer->SwitchWeapon( pBest );

	return TRUE;
}

//=========================================================
//=========================================================
BOOL CHalfLifeMultiplay :: ClientConnected( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] )
{
	g_VoiceGameMgr.ClientConnected(pEntity);
	return TRUE;
}

extern int gmsgSayText;
extern int gmsgGameMode;

void CHalfLifeMultiplay :: UpdateGameMode( CBasePlayer *pPlayer )
{
	MESSAGE_BEGIN( MSG_ONE, gmsgGameMode, NULL, pPlayer->edict() );
		WRITE_BYTE( 0 );  // game mode none
	MESSAGE_END();
}

void CHalfLifeMultiplay :: InitHUD( CBasePlayer *pl )
{
	//start game as observer, if you're not a BOT
	if (!pl->IsBot())  //don't send for bots
		pl->StartObserver (pl->pev->origin, pl->pev->angles);

	pl->AddMoney (mp_startmoney.value);

	// notify other clients of player joining the game
	UTIL_ClientPrintAll( HUD_PRINTNOTIFY, UTIL_VarArgs( "%s has joined the game\n", 
		( pl->pev->netname && STRING(pl->pev->netname)[0] != 0 ) ? STRING(pl->pev->netname) : "unconnected" ) );

	// team match?
	if ( g_teamplay )
	{
		UTIL_LogPrintf( "\"%s<%i><%s><%s>\" entered the game\n",  
			STRING( pl->pev->netname ), 
			GETPLAYERUSERID( pl->edict() ),
			GETPLAYERAUTHID( pl->edict() ),
			g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer( pl->edict() ), "model" ) );
	}
	else
	{
		UTIL_LogPrintf( "\"%s<%i><%s><%i>\" entered the game\n",  
			STRING( pl->pev->netname ), 
			GETPLAYERUSERID( pl->edict() ),
			GETPLAYERAUTHID( pl->edict() ),
			GETPLAYERUSERID( pl->edict() ) );
	}

	UpdateGameMode( pl );

	// sending just one score makes the hud scoreboard active;  otherwise
	// it is just disabled for single play
	MESSAGE_BEGIN( MSG_ONE, gmsgScoreInfo, NULL, pl->edict() );
		WRITE_BYTE( ENTINDEX(pl->edict()) );
		WRITE_SHORT( 0 );
		WRITE_SHORT( 0 );
		WRITE_SHORT( 0 );
		WRITE_SHORT( 0 );
	MESSAGE_END();

	if (!pl->IsBot())  //don't send for bots
		SendMOTDToClient( pl->edict() );

	// loop through all active players and send their score info to the new client
	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		// FIXME:  Probably don't need to cast this just to read m_iDeaths
		CBasePlayer *plr = (CBasePlayer *)UTIL_PlayerByIndex( i );

		if ( plr )
		{
			MESSAGE_BEGIN( MSG_ONE, gmsgScoreInfo, NULL, pl->edict() );
				WRITE_BYTE( i );	// client number
				WRITE_SHORT( plr->pev->frags );
				WRITE_SHORT( plr->m_iDeaths );
				WRITE_SHORT( 0 );
				WRITE_SHORT( GetTeamIndex( plr->m_szTeamName ) + 1 );
			MESSAGE_END();
		}
	}

	if ( g_fGameOver )
	{
		MESSAGE_BEGIN( MSG_ONE, SVC_INTERMISSION, NULL, pl->edict() );
		MESSAGE_END();
	}
}

//=========================================================
//=========================================================
void CHalfLifeMultiplay :: ClientDisconnected( edict_t *pClient )
{
	if ( pClient )
	{
		CBasePlayer *pPlayer = (CBasePlayer *)CBaseEntity::Instance( pClient );

		if ( pPlayer )
		{
			FireTargets( "game_playerleave", pPlayer, pPlayer, USE_TOGGLE, 0 );

			// team match?
			if ( g_teamplay )
			{
				UTIL_LogPrintf( "\"%s<%i><%s><%s>\" disconnected\n",  
					STRING( pPlayer->pev->netname ), 
					GETPLAYERUSERID( pPlayer->edict() ),
					GETPLAYERAUTHID( pPlayer->edict() ),
					g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model" ) );
			}
			else
			{
				UTIL_LogPrintf( "\"%s<%i><%s><%i>\" disconnected\n",  
					STRING( pPlayer->pev->netname ), 
					GETPLAYERUSERID( pPlayer->edict() ),
					GETPLAYERAUTHID( pPlayer->edict() ),
					GETPLAYERUSERID( pPlayer->edict() ) );
			}

			pPlayer->RemoveAllItems( TRUE );// destroy all of the players weapons and items
		}
	}
}

//=========================================================
//=========================================================
float CHalfLifeMultiplay :: FlPlayerFallDamage( CBasePlayer *pPlayer )
{
	pPlayer->m_flFallVelocity -= PLAYER_MAX_SAFE_FALL_SPEED;
	return pPlayer->m_flFallVelocity * DAMAGE_FOR_FALL_SPEED;
} 

//=========================================================
//=========================================================
BOOL CHalfLifeMultiplay::FPlayerCanTakeDamage( CBasePlayer *pPlayer, CBaseEntity *pAttacker )
{
	return TRUE;
}

//=========================================================
//=========================================================
void CHalfLifeMultiplay :: PlayerThink( CBasePlayer *pPlayer )
{
	if ( g_fGameOver )
	{
		// check for button presses
		if ( pPlayer->m_afButtonPressed & ( IN_DUCK | IN_ATTACK | IN_ATTACK2 | IN_USE | IN_JUMP ) )
			m_iEndIntermissionButtonHit = TRUE;

		// clear attack/use commands from player
		pPlayer->m_afButtonPressed = 0;
		pPlayer->pev->button = 0;
		pPlayer->m_afButtonReleased = 0;
	}
}

//=========================================================
//=========================================================
void CHalfLifeMultiplay :: PlayerSpawn( CBasePlayer *pPlayer )
{
	BOOL		addDefault;
	CBaseEntity	*pWeaponEntity = NULL;
	pPlayer->pev->weapons |= (1<<WEAPON_SUIT);
	
	addDefault = TRUE;

	while ( pWeaponEntity = UTIL_FindEntityByClassname( pWeaponEntity, "game_player_equip" ))
	{
		pWeaponEntity->Touch( pPlayer );
		addDefault = FALSE;
	}

	if (addDefault)
	{
		pPlayer->GiveNamedItem( "weapon_flashbang" );
		pPlayer->GiveNamedItem( "weapon_crowbar" );
		pPlayer->GiveNamedItem( "weapon_glock" );
	}
}

//=========================================================
//=========================================================
BOOL CHalfLifeMultiplay :: FPlayerCanRespawn( CBasePlayer *pPlayer )
{
	return TRUE;
}

//=========================================================
//=========================================================
float CHalfLifeMultiplay :: FlPlayerSpawnTime( CBasePlayer *pPlayer )
{
	return gpGlobals->time;//now!
}

//=========================================================
// IPointsForKill - how many points awarded to anyone
// that kills this player?
//=========================================================
int CHalfLifeMultiplay :: IPointsForKill( CBasePlayer *pAttacker, CBasePlayer *pKilled )
{
	pAttacker->AddMoney(GIVE_KILL);
	return 1;
}


//=========================================================
// PlayerKilled - someone/something killed this player
//=========================================================
void CHalfLifeMultiplay :: PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor )
{
	DeathNotice( pVictim, pKiller, pInflictor );

	pVictim->m_iDeaths += 1;


	FireTargets( "game_playerdie", pVictim, pVictim, USE_TOGGLE, 0 );
	CBasePlayer *peKiller = NULL;
	CBaseEntity *ktmp = CBaseEntity::Instance( pKiller );
	if ( ktmp && (ktmp->Classify() == CLASS_PLAYER) )
		peKiller = (CBasePlayer*)ktmp;

	if ( pVictim->pev == pKiller )  
	{  // killed self
		pKiller->frags -= 1;
	}
	else if ( ktmp && ktmp->IsPlayer() )
	{
		// if a player dies in a deathmatch game and the killer is a client, award the killer some points
		pKiller->frags += IPointsForKill( peKiller, pVictim );
		
		FireTargets( "game_playerkill", ktmp, ktmp, USE_TOGGLE, 0 );
	}
	else
	{  // killed by the world
		pKiller->frags -= 1;
	}

	// update the scores
	// killed scores
	MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
		WRITE_BYTE( ENTINDEX(pVictim->edict()) );
		WRITE_SHORT( pVictim->pev->frags );
		WRITE_SHORT( pVictim->m_iDeaths );
		WRITE_SHORT( 0 );
		WRITE_SHORT( GetTeamIndex( pVictim->m_szTeamName ) + 1 );
	MESSAGE_END();

	// killers score, if it's a player
	CBaseEntity *ep = CBaseEntity::Instance( pKiller );
	if ( ep && ep->Classify() == CLASS_PLAYER )
	{
		CBasePlayer *PK = (CBasePlayer*)ep;

		MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
			WRITE_BYTE( ENTINDEX(PK->edict()) );
			WRITE_SHORT( PK->pev->frags );
			WRITE_SHORT( PK->m_iDeaths );
			WRITE_SHORT( 0 );
			WRITE_SHORT( GetTeamIndex( PK->m_szTeamName) + 1 );
		MESSAGE_END();

		// let the killer paint another decal as soon as he'd like.
		PK->m_flNextDecalTime = gpGlobals->time;

		DeactivateSatchels( pVictim );
		DeactivateBiomass( pVictim );
		DeactivateTurrets( pVictim );
	}
}

//=========================================================
// Deathnotice. 
//=========================================================
void CHalfLifeMultiplay::DeathNotice( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pevInflictor )
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
				pPlayer->pev->health = 200;
				sprintf(str, "#ULTRA_KILL");
				break;
			case 5:
				EMIT_SOUND(pPlayer->edict(), CHAN_ITEM, "announce/kill_unstopable.wav", 1, ATTN_NORM);
				pPlayer->AddMoney (GIVE_KILL_UNSTOPABLE);
				pPlayer->GiveNamedItem("weapon_devastator");
				if (pPlayer->m_fCloak = FALSE)
					pPlayer->GiveNamedItem("item_cloak");
				sprintf(str, "#UNSTOPABLE_KILL");
				break;
			case 6:
				EMIT_SOUND(pPlayer->edict(), CHAN_ITEM, "announce/kill_godlike.wav", 1, ATTN_NORM);
				pPlayer->AddMoney (GIVE_KILL_GODLIKE);
				pPlayer->pev->armorvalue = 666;
				pPlayer->GiveNamedItem("weapon_satellite");
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
		// killed self

		// team match?
		if ( g_teamplay )
		{
			UTIL_LogPrintf( "\"%s<%i><%s><%s>\" committed suicide with \"%s\"\n",  
				STRING( pVictim->pev->netname ), 
				GETPLAYERUSERID( pVictim->edict() ),
				GETPLAYERAUTHID( pVictim->edict() ),
				g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer( pVictim->edict() ), "model" ),
				killer_weapon_name );		
		}
		else
		{
			UTIL_LogPrintf( "\"%s<%i><%s><%i>\" committed suicide with \"%s\"\n",  
				STRING( pVictim->pev->netname ), 
				GETPLAYERUSERID( pVictim->edict() ),
				GETPLAYERAUTHID( pVictim->edict() ),
				GETPLAYERUSERID( pVictim->edict() ),
				killer_weapon_name );		
		}
	}
	else if ( pKiller->flags & FL_CLIENT )
	{
		// team match?
		if ( g_teamplay )
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
			UTIL_LogPrintf( "\"%s<%i><%s><%i>\" killed \"%s<%i><%s><%i>\" with \"%s\"\n",  
				STRING( pKiller->netname ),
				GETPLAYERUSERID( ENT(pKiller) ),
				GETPLAYERAUTHID( ENT(pKiller) ),
				GETPLAYERUSERID( ENT(pKiller) ),
				STRING( pVictim->pev->netname ),
				GETPLAYERUSERID( pVictim->edict() ),
				GETPLAYERAUTHID( pVictim->edict() ),
				GETPLAYERUSERID( pVictim->edict() ),
				killer_weapon_name );
		}
	}
	else
	{ 
		// killed by the world

		// team match?
		if ( g_teamplay )
		{
			UTIL_LogPrintf( "\"%s<%i><%s><%s>\" committed suicide with \"%s\" (world)\n",
				STRING( pVictim->pev->netname ), 
				GETPLAYERUSERID( pVictim->edict() ), 
				GETPLAYERAUTHID( pVictim->edict() ),
				g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer( pVictim->edict() ), "model" ),
				killer_weapon_name );		
		}
		else
		{
			UTIL_LogPrintf( "\"%s<%i><%s><%i>\" committed suicide with \"%s\" (world)\n",
				STRING( pVictim->pev->netname ), 
				GETPLAYERUSERID( pVictim->edict() ), 
				GETPLAYERAUTHID( pVictim->edict() ),
				GETPLAYERUSERID( pVictim->edict() ),
				killer_weapon_name );		
		}
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

//  Print a standard message
	// TODO: make this go direct to console
	return; // just remove for now
}

//=========================================================
// PlayerGotWeapon - player has grabbed a weapon that was
// sitting in the world
//=========================================================
void CHalfLifeMultiplay :: PlayerGotWeapon( CBasePlayer *pPlayer, CBasePlayerItem *pWeapon )
{
}

//=========================================================
// FlWeaponRespawnTime - what is the time in the future
// at which this weapon may spawn?
//=========================================================
float CHalfLifeMultiplay :: FlWeaponRespawnTime( CBasePlayerItem *pWeapon )
{
	if ( mp_weaponstay.value > 0 )
	{
		// make sure it's only certain weapons
		if ( !(pWeapon->iFlags() & ITEM_FLAG_LIMITINWORLD) )
		{
			return gpGlobals->time + 0;		// weapon respawns almost instantly
		}
	}

	return gpGlobals->time + WEAPON_RESPAWN_TIME;
}

// when we are within this close to running out of entities,  items 
// marked with the ITEM_FLAG_LIMITINWORLD will delay their respawn
#define ENTITY_INTOLERANCE	100

//=========================================================
// FlWeaponRespawnTime - Returns 0 if the weapon can respawn 
// now,  otherwise it returns the time at which it can try
// to spawn again.
//=========================================================
float CHalfLifeMultiplay :: FlWeaponTryRespawn( CBasePlayerItem *pWeapon )
{
	if ( pWeapon && pWeapon->m_iId && (pWeapon->iFlags() & ITEM_FLAG_LIMITINWORLD) )
	{
		if ( NUMBER_OF_ENTITIES() < (gpGlobals->maxEntities - ENTITY_INTOLERANCE) )
			return 0;

		// we're past the entity tolerance level,  so delay the respawn
		return FlWeaponRespawnTime( pWeapon );
	}

	return 0;
}

//=========================================================
// VecWeaponRespawnSpot - where should this weapon spawn?
// Some game variations may choose to randomize spawn locations
//=========================================================
Vector CHalfLifeMultiplay :: VecWeaponRespawnSpot( CBasePlayerItem *pWeapon )
{
	return pWeapon->pev->origin;
}

//=========================================================
// WeaponShouldRespawn - any conditions inhibiting the
// respawning of this weapon?
//=========================================================
int CHalfLifeMultiplay :: WeaponShouldRespawn( CBasePlayerItem *pWeapon )
{
	if ( pWeapon->pev->spawnflags & SF_NORESPAWN )
	{
		return GR_WEAPON_RESPAWN_NO;
	}

	return GR_WEAPON_RESPAWN_YES;
}

//=========================================================
// CanHaveWeapon - returns FALSE if the player is not allowed
// to pick up this weapon
//=========================================================
BOOL CHalfLifeMultiplay::CanHavePlayerItem( CBasePlayer *pPlayer, CBasePlayerItem *pItem )
{
	if ( mp_weaponstay.value > 0 )
	{
		if ( pItem->iFlags() & ITEM_FLAG_LIMITINWORLD )
			return CGameRules::CanHavePlayerItem( pPlayer, pItem );

		// check if the player already has this weapon
		for ( int i = 0 ; i < MAX_ITEM_TYPES ; i++ )
		{
			CBasePlayerItem *it = pPlayer->m_rgpPlayerItems[i];

			while ( it != NULL )
			{
				if ( it->m_iId == pItem->m_iId )
				{
					return FALSE;
				}

				it = it->m_pNext;
			}
		}
	}

	return CGameRules::CanHavePlayerItem( pPlayer, pItem );
}

//=========================================================
//=========================================================
BOOL CHalfLifeMultiplay::CanHaveItem( CBasePlayer *pPlayer, CItem *pItem )
{
	return TRUE;
}

//=========================================================
//=========================================================
void CHalfLifeMultiplay::PlayerGotItem( CBasePlayer *pPlayer, CItem *pItem )
{
}

//=========================================================
//=========================================================
int CHalfLifeMultiplay::ItemShouldRespawn( CItem *pItem )
{
	if ( pItem->pev->spawnflags & SF_NORESPAWN )
	{
		return GR_ITEM_RESPAWN_NO;
	}

	return GR_ITEM_RESPAWN_YES;
}


//=========================================================
// At what time in the future may this Item respawn?
//=========================================================
float CHalfLifeMultiplay::FlItemRespawnTime( CItem *pItem )
{
	return gpGlobals->time + ITEM_RESPAWN_TIME;
}

//=========================================================
//=========================================================
void CHalfLifeMultiplay::PlayerGotAmmo( CBasePlayer *pPlayer, char *szName, int iCount )
{
}

//=========================================================
//=========================================================
BOOL CHalfLifeMultiplay::IsAllowedToSpawn( CBaseEntity *pEntity )
{
	return TRUE;
}

//=========================================================
//=========================================================
int CHalfLifeMultiplay::AmmoShouldRespawn( CBasePlayerAmmo *pAmmo )
{
	if ( pAmmo->pev->spawnflags & SF_NORESPAWN )
	{
		return GR_AMMO_RESPAWN_NO;
	}

	return GR_AMMO_RESPAWN_YES;
}

//=========================================================
//=========================================================
float CHalfLifeMultiplay::FlAmmoRespawnTime( CBasePlayerAmmo *pAmmo )
{
	return gpGlobals->time + AMMO_RESPAWN_TIME;
}

//=========================================================
//=========================================================
Vector CHalfLifeMultiplay::VecAmmoRespawnSpot( CBasePlayerAmmo *pAmmo )
{
	return pAmmo->pev->origin;
}

//=========================================================
//=========================================================
int CHalfLifeMultiplay::DeadPlayerWeapons( CBasePlayer *pPlayer )
{
	return GR_PLR_DROP_GUN_ACTIVE;
}

//=========================================================
//=========================================================
int CHalfLifeMultiplay::DeadPlayerAmmo( CBasePlayer *pPlayer )
{
	return GR_PLR_DROP_AMMO_ACTIVE;
}

edict_t *CHalfLifeMultiplay::GetPlayerSpawnSpot( CBasePlayer *pPlayer )
{
	edict_t *pentSpawnSpot = CGameRules::GetPlayerSpawnSpot( pPlayer );	
	if ( IsMultiplayer() && pentSpawnSpot->v.target )
	{
		FireTargets( STRING(pentSpawnSpot->v.target), pPlayer, pPlayer, USE_TOGGLE, 0 );
	}

	return pentSpawnSpot;
}


//=========================================================
//=========================================================
int CHalfLifeMultiplay::PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget )
{
	// half life deathmatch has only enemies
	return GR_NOTTEAMMATE;
}

BOOL CHalfLifeMultiplay :: PlayFootstepSounds( CBasePlayer *pl, float fvol )
{
	if ( g_footsteps && g_footsteps->value == 0 )
		return FALSE;

	if ( pl->IsOnLadder() || pl->pev->velocity.Length2D() > 80 )
		return TRUE;  // only make step sounds in multiplayer if the player is moving fast enough

	return FALSE;
}

//=========================================================
//======== CHalfLifeMultiplay private functions ===========
#define INTERMISSION_TIME		6

void CHalfLifeMultiplay :: GoToIntermission( void )
{
	if ( g_fGameOver )
		return;  // intermission has already been triggered, so ignore.

	MESSAGE_BEGIN(MSG_ALL, SVC_INTERMISSION);
	MESSAGE_END();

	// bounds check
	int time = (int)CVAR_GET_FLOAT( "mp_chattime" );
	if ( time < 1 )
		CVAR_SET_STRING( "mp_chattime", "1" );
	else if ( time > MAX_INTERMISSION_TIME )
		CVAR_SET_STRING( "mp_chattime", UTIL_dtos1( MAX_INTERMISSION_TIME ) );

	m_flIntermissionEndTime = gpGlobals->time + ( (int)mp_chattime.value );
	g_flIntermissionStartTime = gpGlobals->time;

	g_fGameOver = TRUE;
	m_iEndIntermissionButtonHit = FALSE;
}

#define MAX_RULE_BUFFER 1024

typedef struct mapcycle_item_s
{
	struct mapcycle_item_s *next;

	char mapname[ 32 ];
	int  minplayers, maxplayers;
	char rulebuffer[ MAX_RULE_BUFFER ];
} mapcycle_item_t;

typedef struct mapcycle_s
{
	struct mapcycle_item_s *items;
	struct mapcycle_item_s *next_item;
} mapcycle_t;

/*
==============
DestroyMapCycle

Clean up memory used by mapcycle when switching it
==============
*/
void DestroyMapCycle( mapcycle_t *cycle )
{
	mapcycle_item_t *p, *n, *start;
	p = cycle->items;
	if ( p )
	{
		start = p;
		p = p->next;
		while ( p != start )
		{
			n = p->next;
			delete p;
			p = n;
		}
		
		delete cycle->items;
	}
	cycle->items = NULL;
	cycle->next_item = NULL;
}

static char com_token[ 1500 ];

/*
==============
COM_Parse

Parse a token out of a string
==============
*/
char *COM_Parse (char *data)
{
	int             c;
	int             len;
	
	len = 0;
	com_token[0] = 0;
	
	if (!data)
		return NULL;
		
// skip whitespace
skipwhite:
	while ( (c = *data) <= ' ')
	{
		if (c == 0)
			return NULL;                    // end of file;
		data++;
	}
	
// skip // comments
	if (c=='/' && data[1] == '/')
	{
		while (*data && *data != '\n')
			data++;
		goto skipwhite;
	}
	

// handle quoted strings specially
	if (c == '\"')
	{
		data++;
		while (1)
		{
			c = *data++;
			if (c=='\"' || !c)
			{
				com_token[len] = 0;
				return data;
			}
			com_token[len] = c;
			len++;
		}
	}

// parse single characters
	if (c=='{' || c=='}'|| c==')'|| c=='(' || c=='\'' || c == ',' )
	{
		com_token[len] = c;
		len++;
		com_token[len] = 0;
		return data+1;
	}

// parse a regular word
	do
	{
		com_token[len] = c;
		data++;
		len++;
		c = *data;
	if (c=='{' || c=='}'|| c==')'|| c=='(' || c=='\'' || c == ',' )
			break;
	} while (c>32);
	
	com_token[len] = 0;
	return data;
}

/*
==============
COM_TokenWaiting

Returns 1 if additional data is waiting to be processed on this line
==============
*/
int COM_TokenWaiting( char *buffer )
{
	char *p;

	p = buffer;
	while ( *p && *p!='\n')
	{
		if ( !isspace( *p ) || isalnum( *p ) )
			return 1;

		p++;
	}

	return 0;
}



/*
==============
ReloadMapCycleFile


Parses mapcycle.txt file into mapcycle_t structure
==============
*/
int ReloadMapCycleFile( char *filename, mapcycle_t *cycle )
{
	char szBuffer[ MAX_RULE_BUFFER ];
	char szMap[ 32 ];
	int length;
	char *pFileList;
	char *aFileList = pFileList = (char*)LOAD_FILE_FOR_ME( filename, &length );
	int hasbuffer;
	mapcycle_item_s *item, *newlist = NULL, *next;

	if ( pFileList && length )
	{
		// the first map name in the file becomes the default
		while ( 1 )
		{
			hasbuffer = 0;
			memset( szBuffer, 0, MAX_RULE_BUFFER );

			pFileList = COM_Parse( pFileList );
			if ( strlen( com_token ) <= 0 )
				break;

			strcpy( szMap, com_token );

			// Any more tokens on this line?
			if ( COM_TokenWaiting( pFileList ) )
			{
				pFileList = COM_Parse( pFileList );
				if ( strlen( com_token ) > 0 )
				{
					hasbuffer = 1;
					strcpy( szBuffer, com_token );
				}
			}

			// Check map
			if ( IS_MAP_VALID( szMap ) )
			{
				// Create entry
				char *s;

				item = new mapcycle_item_s;

				strcpy( item->mapname, szMap );

				item->minplayers = 0;
				item->maxplayers = 0;

				memset( item->rulebuffer, 0, MAX_RULE_BUFFER );

				if ( hasbuffer )
				{
					s = g_engfuncs.pfnInfoKeyValue( szBuffer, "minplayers" );
					if ( s && s[0] )
					{
						item->minplayers = atoi( s );
						item->minplayers = max( item->minplayers, 0 );
						item->minplayers = min( item->minplayers, gpGlobals->maxClients );
					}
					s = g_engfuncs.pfnInfoKeyValue( szBuffer, "maxplayers" );
					if ( s && s[0] )
					{
						item->maxplayers = atoi( s );
						item->maxplayers = max( item->maxplayers, 0 );
						item->maxplayers = min( item->maxplayers, gpGlobals->maxClients );
					}

					// Remove keys
					//
					g_engfuncs.pfnInfo_RemoveKey( szBuffer, "minplayers" );
					g_engfuncs.pfnInfo_RemoveKey( szBuffer, "maxplayers" );

					strcpy( item->rulebuffer, szBuffer );
				}

				item->next = cycle->items;
				cycle->items = item;
			}
			else
			{
				ALERT( at_console, "Skipping %s from mapcycle, not a valid map\n", szMap );
			}

		}

		FREE_FILE( aFileList );
	}

	// Fixup circular list pointer
	item = cycle->items;

	// Reverse it to get original order
	while ( item )
	{
		next = item->next;
		item->next = newlist;
		newlist = item;
		item = next;
	}
	cycle->items = newlist;
	item = cycle->items;

	// Didn't parse anything
	if ( !item )
	{
		return 0;
	}

	while ( item->next )
	{
		item = item->next;
	}
	item->next = cycle->items;
	
	cycle->next_item = item->next;

	return 1;
}

/*
==============
CountPlayers

Determine the current # of active players on the server for map cycling logic
==============
*/
int CountPlayers( void )
{
	int	num = 0;

	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *pEnt = UTIL_PlayerByIndex( i );

		if ( pEnt )
		{
			num = num + 1;
		}
	}

	return num;
}

/*
==============
ExtractCommandString

Parse commands/key value pairs to issue right after map xxx command is issued on server
 level transition
==============
*/
void ExtractCommandString( char *s, char *szCommand )
{
	// Now make rules happen
	char	pkey[512];
	char	value[512];	// use two buffers so compares
								// work without stomping on each other
	char	*o;
	
	if ( *s == '\\' )
		s++;

	while (1)
	{
		o = pkey;
		while ( *s != '\\' )
		{
			if ( !*s )
				return;
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value;

		while (*s != '\\' && *s)
		{
			if (!*s)
				return;
			*o++ = *s++;
		}
		*o = 0;

		strcat( szCommand, pkey );
		if ( strlen( value ) > 0 )
		{
			strcat( szCommand, " " );
			strcat( szCommand, value );
		}
		strcat( szCommand, "\n" );

		if (!*s)
			return;
		s++;
	}
}

/*
==============
ChangeLevel

Server is changing to a new level, check mapcycle.txt for map name and setup info
==============
*/
void CHalfLifeMultiplay :: ChangeLevel( void )
{
	static char szPreviousMapCycleFile[ 256 ];
	static mapcycle_t mapcycle;

	char szNextMap[32];
	char szFirstMapInList[32];
	char szCommands[ 1500 ];
	char szRules[ 1500 ];
	int minplayers = 0, maxplayers = 0;
	strcpy( szFirstMapInList, "hldm1" );  // the absolute default level is hldm1

	int	curplayers;
	BOOL do_cycle = TRUE;

	// find the map to change to
	char *mapcfile = (char*)CVAR_GET_STRING( "mapcyclefile" );
	ASSERT( mapcfile != NULL );

	szCommands[ 0 ] = '\0';
	szRules[ 0 ] = '\0';

	curplayers = CountPlayers();

	// Has the map cycle filename changed?
	if ( stricmp( mapcfile, szPreviousMapCycleFile ) )
	{
		strcpy( szPreviousMapCycleFile, mapcfile );

		DestroyMapCycle( &mapcycle );

		if ( !ReloadMapCycleFile( mapcfile, &mapcycle ) || ( !mapcycle.items ) )
		{
			ALERT( at_console, "Unable to load map cycle file %s\n", mapcfile );
			do_cycle = FALSE;
		}
	}

	if ( do_cycle && mapcycle.items )
	{
		BOOL keeplooking = FALSE;
		BOOL found = FALSE;
		mapcycle_item_s *item;

		// Assume current map
		strcpy( szNextMap, STRING(gpGlobals->mapname) );
		strcpy( szFirstMapInList, STRING(gpGlobals->mapname) );

		// Traverse list
		for ( item = mapcycle.next_item; item->next != mapcycle.next_item; item = item->next )
		{
			keeplooking = FALSE;

			ASSERT( item != NULL );

			if ( item->minplayers != 0 )
			{
				if ( curplayers >= item->minplayers )
				{
					found = TRUE;
					minplayers = item->minplayers;
				}
				else
				{
					keeplooking = TRUE;
				}
			}

			if ( item->maxplayers != 0 )
			{
				if ( curplayers <= item->maxplayers )
				{
					found = TRUE;
					maxplayers = item->maxplayers;
				}
				else
				{
					keeplooking = TRUE;
				}
			}

			if ( keeplooking )
				continue;

			found = TRUE;
			break;
		}

		if ( !found )
		{
			item = mapcycle.next_item;
		}			
		
		// Increment next item pointer
		mapcycle.next_item = item->next;

		// Perform logic on current item
		strcpy( szNextMap, item->mapname );

		ExtractCommandString( item->rulebuffer, szCommands );
		strcpy( szRules, item->rulebuffer );
	}

	if ( !IS_MAP_VALID(szNextMap) )
	{
		strcpy( szNextMap, szFirstMapInList );
	}

	g_fGameOver = TRUE;

	ALERT( at_console, "CHANGE LEVEL: %s\n", szNextMap );

// START BOT

   // loop through all the players...
	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
      CBaseEntity *pEntity;

      pEntity = UTIL_PlayerByIndex( i );

      if (!pEntity)  // if invalid then continue with next index...
         continue;

      CBasePlayer *pPlayer = (CBasePlayer *)pEntity;
   }

   // kick any bot off of the server after time/frag limit...
   for (int index = 0; index < 32; index++)
   {
      if (bot_respawn[index].is_used)  // is this slot used?
      {
         char cmd[40];

         sprintf(cmd, "kick \"%s\"\n", bot_respawn[index].name);

         bot_respawn[index].state = BOT_NEED_TO_RESPAWN;

         SERVER_COMMAND(cmd);  // kick the bot using (kick "name")
      }
   }

// END BOT


	if ( minplayers || maxplayers )
	{
		ALERT( at_console, "PLAYER COUNT:  min %i max %i current %i\n", minplayers, maxplayers, curplayers );
	}
	if ( strlen( szRules ) > 0 )
	{
		ALERT( at_console, "RULES:  %s\n", szRules );
	}
	
	CHANGE_LEVEL( szNextMap, NULL );
	if ( strlen( szCommands ) > 0 )
	{
		SERVER_COMMAND( szCommands );
	}
}

#define MAX_MOTD_CHUNK	  60
#define MAX_MOTD_LENGTH   1536 // (MAX_MOTD_CHUNK * 4)

void CHalfLifeMultiplay :: SendMOTDToClient( edict_t *client )
{
	int length, char_count = 0;
	char *pFileList;
	char szFilename[100]="";
	sprintf(szFilename, "maps/map_info/%s.txt", STRING(gpGlobals->mapname));
	char *aFileList = pFileList = (char*)LOAD_FILE_FOR_ME(szFilename, &length );

	if (length <= 0 || pFileList == NULL)
	{
		sprintf(szFilename, "maps/map_info/default_MOTD.txt", STRING(gpGlobals->mapname));
		aFileList = pFileList = (char*)LOAD_FILE_FOR_ME(szFilename, &length );
	}

	// send the server name
	MESSAGE_BEGIN( MSG_ONE, gmsgServerName, NULL, client );
		WRITE_STRING( CVAR_GET_STRING("hostname") );
	MESSAGE_END();

	// Send the message of the day
	// read it chunk-by-chunk,  and send it in parts

	while ( pFileList && *pFileList && char_count < MAX_MOTD_LENGTH )
	{
		char chunk[MAX_MOTD_CHUNK+1];
		
		if ( strlen( pFileList ) < MAX_MOTD_CHUNK )
		{
			strcpy( chunk, pFileList );
		}
		else
		{
			strncpy( chunk, pFileList, MAX_MOTD_CHUNK );
			chunk[MAX_MOTD_CHUNK] = 0;		// strncpy doesn't always append the null terminator
		}

		char_count += strlen( chunk );
		if ( char_count < MAX_MOTD_LENGTH )
			pFileList = aFileList + char_count; 
		else
			*pFileList = 0;

		MESSAGE_BEGIN( MSG_ONE, gmsgMOTD, NULL, client );
			WRITE_BYTE( *pFileList ? FALSE : TRUE );	// FALSE means there is still more message to come
			WRITE_STRING( chunk );
		MESSAGE_END();
	}

	FREE_FILE( aFileList );
}

BOOL CHalfLifeMultiplay::ClientCommand( CBasePlayer *pPlayer, const char *pcmd ) {
  if(g_VoiceGameMgr.ClientCommand(pPlayer, pcmd))
    return TRUE;

	return 1;

  return CGameRules::ClientCommand(pPlayer, pcmd);
}

//=========================================================
// ClientUserInfoChanged
//=========================================================
void CHalfLifeMultiplay::ClientUserInfoChanged( CBasePlayer *pPlayer, char *infobuffer )
{
	if (pPlayer->m_fHeavyArmor && pPlayer->IsAlive() == TRUE)
	{
		g_engfuncs.pfnSetClientKeyValue(pPlayer->entindex(), g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model", "player_harmor" );
		return;
	}
}