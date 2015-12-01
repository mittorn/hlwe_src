#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"player.h"
#include	"weapons.h"
#include	"gamerules.h"
#include	"teamplay_gamerules.h"
#include	"game.h"

extern DLL_GLOBAL BOOL		g_fGameOver;
extern int gmsgVGUIMenu;
extern int gmsgJoinMenu;

//===========================================================
//Interpretation of buy-commands and menu calling moved here, 
//because it is game rule dependant
//===========================================================

bool CHalfLifeTeamplay :: IsAllowedToBuy ( CBasePlayer *pPlayer )
{
	CBaseEntity *pSpot = NULL;
	BOOL found = FALSE;

	while ((pSpot = UTIL_FindEntityInSphere (pSpot, pPlayer->pev->origin, 250)) != NULL)
	{
		if (g_pGameRules->GetTeamIndex( pPlayer->m_szTeamName ) == 0 && FClassnameIs(pSpot->pev, "info_player_start"))
		{
			found = true;
			break;
		}
		else if (g_pGameRules->GetTeamIndex( pPlayer->m_szTeamName ) == 1 && FClassnameIs(pSpot->pev, "info_player_deathmatch"))
		{
			found = true;
			break;
		}
	}
	return (found && !pPlayer->pev->waterlevel && !pPlayer->IsOnLadder() && pPlayer->IsAlive());
}

void CHalfLifeTeamplay :: ShowMenu ( CBasePlayer* pPlayer )
{
	if (!IsAllowedToBuy (pPlayer))
		return;

	MESSAGE_BEGIN(MSG_ONE, gmsgVGUIMenu, NULL, pPlayer->pev);
		WRITE_BYTE(11);
	MESSAGE_END();
}

void CHalfLifeTeamplay :: InterpretBuyCommand ( const char* pcmd, CBasePlayer* pPlayer )
{
	if (!pPlayer->m_iInBuyZone)
	{
		ClientPrint(pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("You aren't in buy zone!"));
		return;
	}

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
		if (pPlayer->m_iWeapons2 & (1<<(WEAPON_MEDKIT - 32)))
		{
			pPlayer->HaveWeapon();
			return;
		}

		pPlayer->GiveNamedItem( "weapon_medkit" );
		pPlayer->m_flMoneyAmount -= COST_MEDKIT;
	}
	else if (FStrEq(pcmd, "buy_crowbar"))
	{
		if (pPlayer->m_flMoneyAmount < COST_CROWBAR)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->m_iWeapons2 & (1<<(WEAPON_CROWBAR - 32)))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_CROWBAR;
		pPlayer->GiveNamedItem( "weapon_crowbar" );
	}
	else if (FStrEq(pcmd, "buy_bandsaw"))
	{
		if (pPlayer->m_flMoneyAmount < COST_BANDSAW)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->m_iWeapons2 & (1<<(WEAPON_BANDSAW - 32)))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_BANDSAW;
		pPlayer->GiveNamedItem( "weapon_bandsaw" );
	}
	else if (FStrEq(pcmd, "buy_torch"))
	{
		if (pPlayer->m_flMoneyAmount < COST_TORCH)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->m_iWeapons2 & (1<<(WEAPON_TORCH - 32)))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_TORCH;
		pPlayer->GiveNamedItem( "weapon_torch" );
	}
	else if (FStrEq(pcmd, "buy_lightsaber"))
	{
		if (pPlayer->m_flMoneyAmount < COST_LIGHTSABER)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->m_iWeapons2 & (1<<(WEAPON_LIGHTSABER - 32)))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_LIGHTSABER;
		pPlayer->GiveNamedItem( "weapon_lightsaber" );
	}
	//HANDGUNS
	else if (FStrEq(pcmd, "buy_glock"))
	{
		if (pPlayer->m_flMoneyAmount < COST_GLOCK)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->pev->weapons & (1<<WEAPON_GLOCK_AKIMBO))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_GLOCK;
		pPlayer->GiveNamedItem( "weapon_glock" );
	}
	else if (FStrEq(pcmd, "buy_usp"))
	{
		if (pPlayer->m_flMoneyAmount < COST_USP)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->pev->weapons & (1<<WEAPON_USP))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_USP;
		pPlayer->GiveNamedItem( "weapon_usp" );
	}
	else if (FStrEq(pcmd, "buy_deagle"))
	{
		if (pPlayer->m_flMoneyAmount < COST_DEAGLE)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->pev->weapons & (1<<WEAPON_DEAGLE))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_DEAGLE;
		pPlayer->GiveNamedItem( "weapon_deagle" );
	}
	else if (FStrEq(pcmd, "buy_python"))
	{
		if (pPlayer->m_flMoneyAmount < COST_PYTHON)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->pev->weapons & (1<<(WEAPON_PYTHON)))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_PYTHON;
		pPlayer->GiveNamedItem( "weapon_python" );
	}
	else if (FStrEq(pcmd, "buy_uzi"))
	{
		if (pPlayer->m_flMoneyAmount < COST_UZI)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->pev->weapons & (1<<WEAPON_UZI_AKIMBO))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_UZI;
		pPlayer->GiveNamedItem( "weapon_uzi" );
	}
	else if (FStrEq(pcmd, "buy_shieldgun"))
	{
		if (pPlayer->m_flMoneyAmount < COST_SHIELDGUN)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->pev->weapons & (1<<WEAPON_SHIELDGUN))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_SHIELDGUN;
		pPlayer->GiveNamedItem( "weapon_shieldgun" );
	}
	//ASSAULT
	else if (FStrEq(pcmd, "buy_shotgun"))
	{
		if (pPlayer->m_flMoneyAmount < COST_SHOTGUN)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->pev->weapons & (1<<WEAPON_SHOTGUN))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_SHOTGUN;
		pPlayer->GiveNamedItem( "weapon_shotgun" );
	}
	else if (FStrEq(pcmd, "buy_autoshotgun"))
	{
		if (pPlayer->m_flMoneyAmount < COST_AUTOSHOTGUN)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->pev->weapons & (1<<WEAPON_AUTOSHOTGUN))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_AUTOSHOTGUN;
		pPlayer->GiveNamedItem( "weapon_autoshotgun" );
	}
	else if (FStrEq(pcmd, "buy_30mmsg"))
	{
		if (pPlayer->m_flMoneyAmount < COST_SG30MM)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->pev->weapons & (1<<WEAPON_30MMSG))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_SG30MM;
		pPlayer->GiveNamedItem( "weapon_30mmsg" );
	}
	else if (FStrEq(pcmd, "buy_mp5"))
	{
		if (pPlayer->m_flMoneyAmount < COST_MP5)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->pev->weapons & (1<<WEAPON_MP5))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_MP5;
		pPlayer->GiveNamedItem( "weapon_mp5" );
	}
	else if (FStrEq(pcmd, "buy_m16"))
	{
		if (pPlayer->m_flMoneyAmount < COST_M16)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->pev->weapons & (1<<WEAPON_M16))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_M16;
		pPlayer->GiveNamedItem( "weapon_m16" );
	}
	else if (FStrEq(pcmd, "buy_akimbogun"))
	{
		if (pPlayer->m_flMoneyAmount < COST_AKIMBOGUN)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->pev->weapons & (1<<WEAPON_AKIMBOGUN))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_AKIMBOGUN;
		pPlayer->GiveNamedItem( "weapon_akimbogun" );
	}
	else if (FStrEq(pcmd, "buy_ak74"))
	{
		if (pPlayer->m_flMoneyAmount < COST_AK74)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->m_iWeapons2 & (1<<(WEAPON_AK74 - 32)))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_AK74;
		pPlayer->GiveNamedItem( "weapon_ak74" );
	}
	//SNIPER
	else if (FStrEq(pcmd, "buy_g11"))
	{
		if (pPlayer->m_flMoneyAmount < COST_G11)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->pev->weapons & (1<<WEAPON_G11))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_G11;
		pPlayer->GiveNamedItem( "weapon_g11" );
	}
	else if (FStrEq(pcmd, "buy_u2"))
	{
		if (pPlayer->m_flMoneyAmount < COST_U2)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->pev->weapons & (1<<WEAPON_U2))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_U2;
		pPlayer->GiveNamedItem( "weapon_u2" );
	}
	else if (FStrEq(pcmd, "buy_crossbow"))
	{
		if (pPlayer->m_flMoneyAmount < COST_CROSSBOW)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->pev->weapons & (1<<WEAPON_CROSSBOW))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_CROSSBOW;
		pPlayer->GiveNamedItem( "weapon_crossbow" );
	}
	else if (FStrEq(pcmd, "buy_svd"))
	{
		if (pPlayer->m_flMoneyAmount < COST_SVD)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->pev->weapons & (1<<WEAPON_SVD))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_SVD;
		pPlayer->GiveNamedItem( "weapon_svd" );
	}
	else if (FStrEq(pcmd, "buy_awp"))
	{
		if (pPlayer->m_flMoneyAmount < COST_AWP)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->pev->weapons & (1<<WEAPON_AWP))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_AWP;
		pPlayer->GiveNamedItem( "weapon_awp" );
	}
	else if (FStrEq(pcmd, "buy_barett"))
	{
		if (pPlayer->m_flMoneyAmount < COST_BARETT)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->pev->weapons & (1<<WEAPON_BARETT))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_BARETT;
		pPlayer->GiveNamedItem( "weapon_barett" );
	}
	//HEAVY
	else if (FStrEq(pcmd, "buy_m249"))
	{
		if (pPlayer->m_flMoneyAmount < COST_M249)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->pev->weapons & (1<<WEAPON_M249))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_M249;
		pPlayer->GiveNamedItem( "weapon_m249" );
	}
	else if (FStrEq(pcmd, "buy_minigun"))
	{
		if (pPlayer->m_flMoneyAmount < COST_MINIGUN)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->pev->weapons & (1<<WEAPON_MINIGUN))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_MINIGUN;
		pPlayer->GiveNamedItem( "weapon_minigun" );
	}
	else if (FStrEq(pcmd, "buy_nailgun"))
	{
		if (pPlayer->m_flMoneyAmount < COST_NAILGUN)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->m_iWeapons2 & (1<<(WEAPON_NAILGUN - 32)))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_NAILGUN;
		pPlayer->GiveNamedItem( "weapon_nailgun" );
	}
	else if (FStrEq(pcmd, "buy_froster"))
	{
		if (pPlayer->m_flMoneyAmount < COST_FROSTER)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->m_iWeapons2 & (1<<(WEAPON_FROSTER - 32)))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_FROSTER;
		pPlayer->GiveNamedItem( "weapon_froster" );
	}
	else if (FStrEq(pcmd, "buy_flamethrower"))
	{
		if (pPlayer->m_flMoneyAmount < COST_FLAMETHROWER)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->m_iWeapons2 & (1<<(WEAPON_FLAMETHROWER - 32)))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_FLAMETHROWER;
		pPlayer->GiveNamedItem( "weapon_flamethrower" );
	}
	else if (FStrEq(pcmd, "buy_machinegun"))
	{
		if (pPlayer->m_flMoneyAmount < COST_MACHINEGUN)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->m_iWeapons2 & (1<<(WEAPON_MACHINEGUN - 32)))
		{
			pPlayer->HaveWeapon();
			return;
		}
		if (!pPlayer->m_fHeavyArmor)
		{
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("Can't use without Heavy Armor!"));
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_MACHINEGUN;
		pPlayer->GiveNamedItem( "weapon_machinegun" );
	}
	else if (FStrEq(pcmd, "buy_flakcannon"))
	{
		if (pPlayer->m_flMoneyAmount < COST_FLAKCANNON)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->pev->weapons & (1<<WEAPON_FLAKCANNON))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_FLAKCANNON;
		pPlayer->GiveNamedItem( "weapon_flakcannon" );
	}
	else if (FStrEq(pcmd, "buy_bfg"))
	{
		if (pPlayer->m_flMoneyAmount < COST_BFG)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->pev->weapons & (1<<WEAPON_BFG))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_BFG;
		pPlayer->GiveNamedItem( "weapon_bfg" );
	}
	//LAUNCHERZ
	else if (FStrEq(pcmd, "buy_rpg"))
	{
		if (pPlayer->m_flMoneyAmount < COST_RPG)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->pev->weapons & (1<<WEAPON_RPG))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_RPG;
		pPlayer->GiveNamedItem( "weapon_rpg" );
	}
	else if (FStrEq(pcmd, "buy_incendiary"))
	{
		if (pPlayer->m_flMoneyAmount < COST_INCENDIARY)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->m_iWeapons2 & (1<<(WEAPON_INCENDIARY - 32)))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_INCENDIARY;
		pPlayer->GiveNamedItem( "weapon_incendiary" );
	}
	else if (FStrEq(pcmd, "buy_whl"))
	{
		if (pPlayer->m_flMoneyAmount < COST_WHL)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->m_iWeapons2 & (1<<(WEAPON_WHL - 32)))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_WHL;
		pPlayer->GiveNamedItem( "weapon_whl" );
	}
	else if (FStrEq(pcmd, "buy_devastator"))
	{
		if (pPlayer->m_flMoneyAmount < COST_DEVASTATOR)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->m_iWeapons2 & (1<<(WEAPON_DEVASTATOR - 32)))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_DEVASTATOR;
		pPlayer->GiveNamedItem( "weapon_devastator" );
	}
	else if (FStrEq(pcmd, "buy_redeemer"))
	{
		if (pPlayer->m_flMoneyAmount < COST_REDEEMER)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->m_iWeapons2 & (1<<(WEAPON_REDEEMER - 32)))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_REDEEMER;
		pPlayer->GiveNamedItem( "weapon_redeemer" );
	}
	//ENERGY
	else if (FStrEq(pcmd, "buy_smartgun"))
	{
		if (pPlayer->m_flMoneyAmount < COST_SMARTGUN)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->m_iWeapons2 & (1<<(WEAPON_SMARTGUN - 32)))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_SMARTGUN;
		pPlayer->GiveNamedItem( "weapon_smartgun" );
		pPlayer->GiveNamedItem( "item_battery" );
	}
	else if (FStrEq(pcmd, "buy_teslagun"))
	{
		if (pPlayer->m_flMoneyAmount < COST_TESLAGUN)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->m_iWeapons2 & (1<<(WEAPON_TESLAGUN - 32)))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_TESLAGUN;
		pPlayer->GiveNamedItem( "weapon_teslagun" );
	}
	else if (FStrEq(pcmd, "buy_egon"))
	{
		if (pPlayer->m_flMoneyAmount < COST_EGON)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->pev->weapons & (1<<WEAPON_EGON))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_EGON;
		pPlayer->GiveNamedItem( "weapon_egon" );
	}
	else if (FStrEq(pcmd, "buy_plasmarifle"))
	{
		if (pPlayer->m_flMoneyAmount < COST_PLASMARIFLE)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->m_iWeapons2 & (1<<(WEAPON_PLASMARIFLE - 32)))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_PLASMARIFLE;
		pPlayer->GiveNamedItem( "weapon_plasmarifle" );
	}
	else if (FStrEq(pcmd, "buy_photongun"))
	{
		if (pPlayer->m_flMoneyAmount < COST_PHOTONGUN)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->m_iWeapons2 & (1<<(WEAPON_PHOTONGUN - 32)))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_PHOTONGUN;
		pPlayer->GiveNamedItem( "weapon_photongun" );
	}
	else if (FStrEq(pcmd, "buy_gauss"))
	{
		if (pPlayer->m_flMoneyAmount < COST_GAUSS)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->pev->weapons & (1<<WEAPON_GAUSS))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_GAUSS;
		pPlayer->GiveNamedItem( "weapon_gauss" );
	}
	else if (FStrEq(pcmd, "buy_taucannon"))
	{
		if (pPlayer->m_flMoneyAmount < COST_TAUCANNON)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->pev->weapons & (1<<WEAPON_TAUCANNON))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_TAUCANNON;
		pPlayer->GiveNamedItem( "weapon_taucannon" );
	}
	else if (FStrEq(pcmd, "buy_gluongun"))
	{
		if (pPlayer->m_flMoneyAmount < COST_GLUONGUN)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->m_iWeapons2 & (1<<(WEAPON_GLUONGUN - 32)))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_GLUONGUN;
		pPlayer->GiveNamedItem( "weapon_gluongun" );
	}
	else if (FStrEq(pcmd, "buy_displacer"))
	{
		if (pPlayer->m_flMoneyAmount < COST_DISPLACER)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->m_iWeapons2 & (1<<(WEAPON_DISPLACER - 32)))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_DISPLACER;
		pPlayer->GiveNamedItem( "weapon_displacer" );
	}
	//EXPLOSIVES
	else if (FStrEq(pcmd, "buy_flashbang"))
	{
		if (pPlayer->m_flMoneyAmount < COST_FLASHBANG)
		{
			pPlayer->NoMoney();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_FLASHBANG;
		pPlayer->GiveNamedItem( "weapon_flashbang" );
	}
	else if (FStrEq(pcmd, "buy_handgrenade"))
	{
		if (pPlayer->m_flMoneyAmount < COST_HANDGRENADE)
		{
			pPlayer->NoMoney();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_HANDGRENADE;
		pPlayer->GiveNamedItem( "weapon_handgrenade" );
	}
	else if (FStrEq(pcmd, "buy_tripmine"))
	{
		if (pPlayer->m_flMoneyAmount < COST_TRIPMINE)
		{
			pPlayer->NoMoney();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_TRIPMINE;
		pPlayer->GiveNamedItem( "weapon_tripmine" );
	}
	else if (FStrEq(pcmd, "buy_satchel"))
	{
		if (pPlayer->m_flMoneyAmount < COST_SATCHEL)
		{
			pPlayer->NoMoney();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_SATCHEL;
		pPlayer->GiveNamedItem( "weapon_satchel" );
	}
	else if (FStrEq(pcmd, "buy_c4"))
	{
		if (pPlayer->m_flMoneyAmount < COST_C4)
		{
			pPlayer->NoMoney();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_C4;
		pPlayer->GiveNamedItem( "weapon_c4" );
	}
	//EXPERIMENTAL WEAPONS
	else if (FStrEq(pcmd, "buy_blaster"))
	{
		if (pPlayer->m_flMoneyAmount < COST_BLASTER)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->m_iWeapons2 & (1<<(WEAPON_BLASTER - 32)))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_BLASTER;
		pPlayer->GiveNamedItem( "weapon_blaster" );
	}
	else if (FStrEq(pcmd, "buy_biorifle"))
	{
		if (pPlayer->m_flMoneyAmount < COST_BIORIFLE)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->pev->weapons & (1<<WEAPON_BIORIFLE))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_BIORIFLE;
		pPlayer->GiveNamedItem( "weapon_biorifle" );
	}
	else if (FStrEq(pcmd, "buy_pulserifle"))
	{
		if (pPlayer->m_flMoneyAmount < COST_PULSERIFLE)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->m_iWeapons2 & (1<<(WEAPON_PULSERIFLE - 32)))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_PULSERIFLE;
		pPlayer->GiveNamedItem( "weapon_pulserifle" );
	}
	else if (FStrEq(pcmd, "buy_m72"))
	{
		if (pPlayer->m_flMoneyAmount < COST_M72)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->m_iWeapons2 & (1<<(WEAPON_M72 - 32)))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_M72;
		pPlayer->GiveNamedItem( "weapon_m72" );
	}
	else if (FStrEq(pcmd, "buy_satellite"))
	{
		if (pPlayer->m_flMoneyAmount < COST_SATELLITE)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->m_iWeapons2 & (1<<(WEAPON_SATELLITE - 32)))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_SATELLITE;
		pPlayer->GiveNamedItem( "weapon_satellite" );
	}
	else if (FStrEq(pcmd, "buy_turretkit"))
	{
		if (pPlayer->m_flMoneyAmount < COST_TURRETKIT)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->m_iWeapons2 & (1<<(WEAPON_TURRETKIT - 32)))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_TURRETKIT;
		pPlayer->GiveNamedItem( "weapon_turretkit" );
	}
	else if (FStrEq(pcmd, "buy_chronosceptor"))
	{
		if (pPlayer->m_flMoneyAmount < COST_CHRONOSCEPTOR)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->m_iWeapons2 & (1<<(WEAPON_CHRONOSCEPTOR - 32)))
		{
			pPlayer->HaveWeapon();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_CHRONOSCEPTOR;
		pPlayer->GiveNamedItem( "weapon_chronosceptor" );
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
		if (pPlayer->pev->health <= 0)
		{
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("It won't help!"));
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
		if (pPlayer->m_fPowerShield)
			flMaxArmor = MAX_SHIELD_BATTERY;
		else if (pPlayer->m_fPortableHEV)
			flMaxArmor = MAX_PHEV_BATTERY;
		else
			flMaxArmor = MAX_NORMAL_BATTERY;

		if (pPlayer->pev->armorvalue >= flMaxArmor)
		{
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("You already have MAXIMUM ARMOR!"));
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_BATTERY;
		pPlayer->GiveNamedItem( "item_battery" );
	}
	else if (FStrEq(pcmd, "buy_flashbattery"))
	{
		if (pPlayer->m_flMoneyAmount < COST_FLASHBATTERY)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->m_iFlashBattery >= 99)
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
		if (pPlayer->m_iKevlarBattery >= 25)
		{
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("You already have KEVLAR ARMOR!"));
			return;
		}
		if(pPlayer->m_fHeavyArmor)
		{
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("You already have the best ARMOR!"));
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_KEVLAR;
		pPlayer->GiveNamedItem( "item_kevlar" );
	}
	else if (FStrEq(pcmd, "buy_harmor"))
	{
		if (pPlayer->m_flMoneyAmount < COST_HARMOR)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->m_iHarmorBattery >= 50)
		{
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("You already have HEAVY ARMOR!"));
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_HARMOR;
		pPlayer->GiveNamedItem( "item_harmor" );
	}
	//SUIT POWERUPS
	else if (FStrEq(pcmd, "buy_longjump"))
	{
		if (pPlayer->m_flMoneyAmount < COST_LONGJUMP)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->m_fLongJump)
		{
			pPlayer->HaveItem();
			return;
		}
		if (pPlayer->m_fHeavyArmor)
		{
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("Can't use with HEAVY ARMOR!"));
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_LONGJUMP;
		pPlayer->GiveNamedItem( "item_longjump" );
	}
	else if (FStrEq(pcmd, "buy_antigrav"))
	{
		if (pPlayer->m_flMoneyAmount < COST_ANTIGRAV)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->m_fAntigrav)
		{
			pPlayer->HaveItem();
			return;
		}
		if (pPlayer->m_fHeavyArmor)
		{
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, UTIL_VarArgs("Can't use with HEAVY ARMOR!"));
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_ANTIGRAV;
		pPlayer->GiveNamedItem( "item_antigrav" );
	}
	else if (FStrEq(pcmd, "buy_cloak"))
	{
		if (pPlayer->m_flMoneyAmount < COST_CLOAK)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->m_fCloak)
		{
			pPlayer->HaveItem();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_CLOAK;
		pPlayer->GiveNamedItem( "item_cloak" );
	}
	else if (FStrEq(pcmd, "buy_teleport"))
	{
		if (pPlayer->m_flMoneyAmount < COST_PT)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->m_fpt)
		{
			pPlayer->HaveItem();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_PT;
		pPlayer->GiveNamedItem( "item_pt" );
	}
	else if (FStrEq(pcmd, "buy_powershield"))
	{
		if (pPlayer->m_flMoneyAmount < COST_PSHIELD)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->m_fPowerShield)
		{
			pPlayer->HaveItem();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_PSHIELD;
		pPlayer->GiveNamedItem( "item_PowerShield" );
	}
	else if (FStrEq(pcmd, "buy_phealthkit"))
	{
		if (pPlayer->m_flMoneyAmount < COST_PHK)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->m_fPortableHealthkit)
		{
			pPlayer->HaveItem();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_PHK;
		pPlayer->GiveNamedItem( "item_portableHealthkit" );
	}
	else if (FStrEq(pcmd, "buy_portHEV"))
	{
		if (pPlayer->m_flMoneyAmount < COST_PHEV)
		{
			pPlayer->NoMoney();
			return;
		}
		if (pPlayer->m_fPortableHEV)
		{
			pPlayer->HaveItem();
			return;
		}
		pPlayer->m_flMoneyAmount -= COST_PHEV;
		pPlayer->GiveNamedItem( "item_portableHEV" );
	}
}

//===================================================================
//
//							BUY MENU END
//
//===================================================================


CHalfLifeTeamplay :: CHalfLifeTeamplay()
{
	m_DisableDeathMessages = FALSE;
	m_DisableDeathPenalty = FALSE;

	memset( team_names, 0, sizeof(team_names) );
	memset( team_scores, 0, sizeof(team_scores) );
	num_teams = 0;

	// Copy over the team from the server config
	m_szTeamList[0] = 0;

	// Cache this because the team code doesn't want to deal with changing this in the middle of a game
	strncpy( m_szTeamList, mp_teamlist.string, TEAMPLAY_TEAMLISTLENGTH );

	edict_t *pWorld = INDEXENT(0);
	if ( pWorld && pWorld->v.team )
	{
		if ( mp_teamoverride.value )
		{
			const char *pTeamList = STRING(pWorld->v.team);
			if ( pTeamList && strlen(pTeamList) )
			{
				strncpy( m_szTeamList, pTeamList, TEAMPLAY_TEAMLISTLENGTH );
			}
		}
	}
	// Has the server set teams
	if ( strlen( m_szTeamList ) )
		m_teamLimit = TRUE;
	else
		m_teamLimit = FALSE;

	RecountTeams();
}

#include "voice_gamemgr.h"
extern CVoiceGameMgr	g_VoiceGameMgr;

void CHalfLifeTeamplay :: Think ( void )
{
	///// Check game rules /////
	static int last_frags;
	static int last_time;

	int frags_remaining = 0;
	int time_remaining = 0;

	g_VoiceGameMgr.Update(gpGlobals->frametime);

	if ( g_fGameOver )   // someone else quit the game already
	{
		CHalfLifeMultiplay::Think();
		return;
	}

	float flTimeLimit = CVAR_GET_FLOAT("mp_timelimit") * 60;
	
	time_remaining = (int)(flTimeLimit ? ( flTimeLimit - gpGlobals->time ) : 0);

	if ( flTimeLimit != 0 && gpGlobals->time >= flTimeLimit )
	{
		GoToIntermission();
		return;
	}

	float flFragLimit = mp_fraglimit.value;
	if ( flFragLimit )
	{
		int bestfrags = 9999;
		int remain;

		// check if any team is over the frag limit
		for ( int i = 0; i < num_teams; i++ )
		{
			if ( team_scores[i] >= flFragLimit )
			{
				GoToIntermission();
				return;
			}

			remain = flFragLimit - team_scores[i];
			if ( remain < bestfrags )
			{
				bestfrags = remain;
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
// ClientCommand
// the user has typed a command which is unrecognized by everything else;
// this check to see if the gamerules knows anything about the command
//=========================================================
BOOL CHalfLifeTeamplay :: ClientCommand( CBasePlayer *pPlayer, const char *pcmd )
{
	if(g_VoiceGameMgr.ClientCommand(pPlayer, pcmd))
		return TRUE;

	else if (!strcmp(pcmd, "teammenu"))
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgJoinMenu, NULL, pPlayer->pev);
			WRITE_BYTE(12);
			WRITE_BYTE(0);//doesn't matter
			WRITE_BYTE(0);//show teamplay splash
		MESSAGE_END();

		return 1;
	}

	else if (!strncmp (pcmd, "team", 4))
	{
		if (pcmd[4] - '1' != GetTeamIndex(pPlayer->m_szTeamName))
			ChangePlayerTeam (pPlayer, GetIndexedTeamName(pcmd[4] - '1'), true, false);//team numeration starts with 0

		return 1;
	}

	return CGameRules::ClientCommand(pPlayer, pcmd);
}

void CHalfLifeTeamplay :: UpdateGameMode( CBasePlayer *pPlayer )
{
	MESSAGE_BEGIN( MSG_ONE, gmsgGameMode, NULL, pPlayer->edict() );
		WRITE_BYTE( 1 );  // game mode teamplay
	MESSAGE_END();
}


const char *CHalfLifeTeamplay::SetDefaultPlayerTeam( CBasePlayer *pPlayer )
{
	// copy out the team name from the model
	char *mdls = g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model" );

	const char *pTeamName = NULL;

	if (!strncmp(mdls, "player_harmor", TEAM_NAME_LENGTH))
	{
		if (!IsValidTeam( pPlayer->oldmodel ))
		{
			pTeamName = TeamWithFewestPlayers();
			strncpy( pPlayer->m_szTeamName, pTeamName, TEAM_NAME_LENGTH );
		}
		else
			strncpy( pPlayer->m_szTeamName, pPlayer->oldmodel, TEAM_NAME_LENGTH );
	}
	else
		strncpy( pPlayer->m_szTeamName, mdls, TEAM_NAME_LENGTH );

	// update the current player of the team he is joining
	if ( pPlayer->m_szTeamName[0] == '\0' || !IsValidTeam( pPlayer->m_szTeamName ) )
	{
		pTeamName = TeamWithFewestPlayers();
		strncpy( pPlayer->m_szTeamName, pTeamName, TEAM_NAME_LENGTH );
	}

	RecountTeams();

	return pPlayer->m_szTeamName;
}


//=========================================================
// InitHUD
//=========================================================
void CHalfLifeTeamplay::InitHUD( CBasePlayer *pPlayer )
{
	int i;

	SetDefaultPlayerTeam( pPlayer );
	CHalfLifeMultiplay::InitHUD( pPlayer );

	MESSAGE_BEGIN(MSG_ONE, gmsgJoinMenu, NULL, pPlayer->pev);
		WRITE_BYTE(13);
		WRITE_BYTE(0);//doesn't matter
		WRITE_BYTE(0);//show teamplay splash
	MESSAGE_END();

	// Send down the team names
	MESSAGE_BEGIN( MSG_ONE, gmsgTeamNames, NULL, pPlayer->edict() );  
		WRITE_BYTE( num_teams );
		for ( i = 0; i < num_teams; i++ )
		{
			WRITE_STRING( team_names[ i ] );
		}
	MESSAGE_END();

	RecountTeams();

	char *mdls = g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model" );
	// update the current player of the team he is joining
	char text[1024];
	if ( !strcmp( mdls, pPlayer->m_szTeamName ) )
	{
		sprintf( text, "* you are on team \'%s\'\n", pPlayer->m_szTeamName );
	}
	else
	{
		sprintf( text, "* assigned to team %s\n", pPlayer->m_szTeamName );
	}

	ChangePlayerTeam( pPlayer, pPlayer->m_szTeamName, FALSE, FALSE );
	UTIL_SayText( text, pPlayer );
	int clientIndex = pPlayer->entindex();
	RecountTeams();
	// update this player with all the other players team info
	// loop through all active players and send their team info to the new client
	for ( i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *plr = UTIL_PlayerByIndex( i );
		if ( plr && IsValidTeam( plr->TeamID() ) )
		{
			MESSAGE_BEGIN( MSG_ONE, gmsgTeamInfo, NULL, pPlayer->edict() );
				WRITE_BYTE( plr->entindex() );
				WRITE_STRING( plr->TeamID() );
			MESSAGE_END();
		}
	}
}


void CHalfLifeTeamplay::ChangePlayerTeam( CBasePlayer *pPlayer, const char *pTeamName, BOOL bKill, BOOL bGib )
{
	int damageFlags = DMG_GENERIC;
	int clientIndex = pPlayer->entindex();

	if ( !bGib )
	{
		damageFlags |= DMG_NEVERGIB;
	}
	else
	{
		damageFlags |= DMG_ALWAYSGIB;
	}

	if ( bKill )
	{
		// kill the player,  remove a death,  and let them start on the new team
		m_DisableDeathMessages = TRUE;
		m_DisableDeathPenalty = TRUE;

		entvars_t *pevWorld = VARS( INDEXENT(0) );
		pPlayer->TakeDamage( pevWorld, pevWorld, 9000, damageFlags );

		m_DisableDeathMessages = FALSE;
		m_DisableDeathPenalty = FALSE;
	}

	if (!strncmp(pTeamName, "player_harmor", TEAM_NAME_LENGTH))
		strncpy( pPlayer->m_szTeamName, pPlayer->oldmodel, TEAM_NAME_LENGTH );
	else
	{
		strncpy( pPlayer->m_szTeamName, pTeamName, TEAM_NAME_LENGTH );
		strncpy( pPlayer->oldmodel, pTeamName, TEAM_NAME_LENGTH );
	}

	if ( pPlayer->m_fHeavyArmor && pPlayer->IsAlive() )
		g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model", "player_harmor" );
	else
		g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model", pPlayer->m_szTeamName );

	g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "team", pPlayer->m_szTeamName );

	// notify everyone's HUD of the team change
	MESSAGE_BEGIN( MSG_ALL, gmsgTeamInfo );
		WRITE_BYTE( clientIndex );
		WRITE_STRING( pPlayer->m_szTeamName );
	MESSAGE_END();

	MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
		WRITE_BYTE( clientIndex );
		WRITE_SHORT( pPlayer->pev->frags );
		WRITE_SHORT( pPlayer->m_iDeaths );
		WRITE_SHORT( 0 );
		WRITE_SHORT( g_pGameRules->GetTeamIndex( pPlayer->m_szTeamName ) + 1 );
	MESSAGE_END();

	pPlayer->pev->team = g_pGameRules->GetTeamIndex( pPlayer->m_szTeamName );
}


//=========================================================
// ClientUserInfoChanged
//=========================================================
void CHalfLifeTeamplay::ClientUserInfoChanged( CBasePlayer *pPlayer, char *infobuffer )
{
	char text[1024];

	// prevent skin/color/model changes
	char *mdls = g_engfuncs.pfnInfoKeyValue( infobuffer, "model" );

	if ( !stricmp( mdls, pPlayer->m_szTeamName ) || !strcmp( mdls, "player_harmor"))
		return;

	if ( !IsValidTeam( mdls ) )
	{
		int clientIndex = pPlayer->entindex();

		if (pPlayer->m_fHeavyArmor && pPlayer->IsAlive() )
		{
			g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model", "player_harmor" );
			strncpy (pPlayer->oldmodel, pPlayer->m_szTeamName, TEAM_NAME_LENGTH);
		}
		else
			g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model", pPlayer->m_szTeamName );

		sprintf( text, "* Can't change team to \'%s\'\n", mdls );
		UTIL_SayText( text, pPlayer );
		sprintf( text, "* Server limits teams to \'%s\'\n", m_szTeamList );
		UTIL_SayText( text, pPlayer );
		return;
	}
	// notify everyone of the team change
	sprintf( text, "* %s has changed to team \'%s\'\n", STRING(pPlayer->pev->netname), mdls );
	UTIL_SayTextAll( text, pPlayer );

	UTIL_LogPrintf( "\"%s<%i><%s><%s>\" joined team \"%s\"\n", 
		STRING(pPlayer->pev->netname),
		GETPLAYERUSERID( pPlayer->edict() ),
		GETPLAYERAUTHID( pPlayer->edict() ),
		pPlayer->m_szTeamName,
		mdls );

	ChangePlayerTeam( pPlayer, mdls, TRUE, TRUE );
	// recound stuff
	RecountTeams( TRUE );
}

extern int gmsgDeathMsg;

//=========================================================
// Deathnotice. 
//=========================================================
void CHalfLifeTeamplay::DeathNotice( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pevInflictor )
{
	if ( m_DisableDeathMessages )
		return;
	
	if ( pVictim && pKiller && pKiller->flags & FL_CLIENT )
	{
		CBasePlayer *pk = (CBasePlayer*) CBaseEntity::Instance( pKiller );

		if ( pk )
		{
			if ( (pk != pVictim) && (PlayerRelationship( pVictim, pk ) == GR_TEAMMATE) )
			{
				MESSAGE_BEGIN( MSG_ALL, gmsgDeathMsg );
					WRITE_BYTE( ENTINDEX(ENT(pKiller)) );		// the killer
					WRITE_BYTE( ENTINDEX(pVictim->edict()) );	// the victim
					WRITE_STRING( "teammate" );		// flag this as a teammate kill
				MESSAGE_END();
				return;
			}
		}
	}

	CHalfLifeMultiplay::DeathNotice( pVictim, pKiller, pevInflictor );
}

//=========================================================
//=========================================================
void CHalfLifeTeamplay :: PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor )
{
	if ( !m_DisableDeathPenalty )
	{
		CHalfLifeMultiplay::PlayerKilled( pVictim, pKiller, pInflictor );
		RecountTeams();
	}
}


//=========================================================
// IsTeamplay
//=========================================================
BOOL CHalfLifeTeamplay::IsTeamplay( void )
{
	return TRUE;
}

BOOL CHalfLifeTeamplay::FPlayerCanTakeDamage( CBasePlayer *pPlayer, CBaseEntity *pAttacker )
{
	return TRUE;
}

//=========================================================
//=========================================================
int CHalfLifeTeamplay::PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget )
{
	// half life multiplay has a simple concept of Player Relationships.
	// you are either on another player's team, or you are not.
	if ( !pPlayer || !pTarget || !pTarget->IsPlayer() )
		return GR_NOTTEAMMATE;

	if ( (*GetTeamID(pPlayer) != '\0') && (*GetTeamID(pTarget) != '\0') && !stricmp( GetTeamID(pPlayer), GetTeamID(pTarget) ) )
	{
		return GR_TEAMMATE;
	}

	return GR_NOTTEAMMATE;
}

//=========================================================
//=========================================================
int CHalfLifeTeamplay::IPointsForKill( CBasePlayer *pAttacker, CBasePlayer *pKilled )
{
	if ( !pKilled )
		return 0;

	if ( !pAttacker )
		return 1;

	if ( pAttacker != pKilled && PlayerRelationship( pAttacker, pKilled ) == GR_TEAMMATE )
	{
		pAttacker->AddMoney(GIVE_KILL_TEAMMATE);
		return -5;
	}

	pAttacker->AddMoney(GIVE_KILL);
	return 1;
}

//=========================================================
//=========================================================
const char *CHalfLifeTeamplay::GetTeamID( CBaseEntity *pEntity )
{
	if ( pEntity == NULL || pEntity->pev == NULL )
		return "";

	// return their team name
	return pEntity->TeamID();
}


int CHalfLifeTeamplay::GetTeamIndex( const char *pTeamName )
{
	if ( pTeamName && *pTeamName != 0 )
	{
		// try to find existing team
		for ( int tm = 0; tm < num_teams; tm++ )
		{
			if ( !stricmp( team_names[tm], pTeamName ) )
				return tm;
		}
	}
	
	return -1;	// No match
}


const char *CHalfLifeTeamplay::GetIndexedTeamName( int teamIndex )
{
	if ( teamIndex < 0 || teamIndex >= num_teams )
		return "";

	return team_names[ teamIndex ];
}


BOOL CHalfLifeTeamplay::IsValidTeam( const char *pTeamName ) 
{
	if ( !m_teamLimit )	// Any team is valid if the teamlist isn't set
		return TRUE;

	return ( GetTeamIndex( pTeamName ) != -1 ) ? TRUE : FALSE;
}

const char *CHalfLifeTeamplay::TeamWithFewestPlayers( void )
{
	int i;
	int minPlayers = MAX_TEAMS;
	int teamCount[ MAX_TEAMS ];
	char *pTeamName = NULL;

	memset( teamCount, 0, MAX_TEAMS * sizeof(int) );
	
	// loop through all clients, count number of players on each team
	for ( i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *plr = UTIL_PlayerByIndex( i );

		if ( plr )
		{
			int team = GetTeamIndex( plr->TeamID() );
			if ( team >= 0 )
				teamCount[team] ++;
		}
	}

	// Find team with least players
	for ( i = 0; i < num_teams; i++ )
	{
		if ( teamCount[i] < minPlayers )
		{
			minPlayers = teamCount[i];
			pTeamName = team_names[i];
		}
	}

	return pTeamName;
}


//=========================================================
//=========================================================
void CHalfLifeTeamplay::RecountTeams( bool bResendInfo )
{
	char	*pName;
	char	teamlist[TEAMPLAY_TEAMLISTLENGTH];

	// loop through all teams, recounting everything
	num_teams = 0;

	// Copy all of the teams from the teamlist
	// make a copy because strtok is destructive
	strcpy( teamlist, m_szTeamList );
	pName = teamlist;
	pName = strtok( pName, ";" );
	while ( pName != NULL && *pName )
	{
		if ( GetTeamIndex( pName ) < 0 )
		{
			strcpy( team_names[num_teams], pName );
			num_teams++;
		}
		pName = strtok( NULL, ";" );
	}

	if ( num_teams < 2 )
	{
		num_teams = 0;
		m_teamLimit = FALSE;
	}

	// Sanity check
	memset( team_scores, 0, sizeof(team_scores) );

	// loop through all clients
	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *plr = UTIL_PlayerByIndex( i );

		if ( plr )
		{
			const char *pTeamName = plr->TeamID();
			// try add to existing team
			int tm = GetTeamIndex( pTeamName );
			
			if ( tm < 0 ) // no team match found
			{ 
				if ( !m_teamLimit )
				{
					// add to new team
					tm = num_teams;
					num_teams++;
					team_scores[tm] = 0;
					strncpy( team_names[tm], pTeamName, MAX_TEAMNAME_LENGTH );
				}
			}

			if ( tm >= 0 && !IsCTF())
			{
				team_scores[tm] += plr->pev->frags;
			}

			if ( bResendInfo ) //Someone's info changed, let's send the team info again.
			{
				if ( plr && IsValidTeam( plr->TeamID() ) )
				{
					MESSAGE_BEGIN( MSG_ALL, gmsgTeamInfo, NULL );
						WRITE_BYTE( plr->entindex() );
						WRITE_STRING( plr->TeamID() );
					MESSAGE_END();
				}
			}
		}
	}
}
