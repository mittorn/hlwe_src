//==============================================================================================
//								HLWE Last man standing game rules.
//										(C) LLAPb, 2008
//==============================================================================================

#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"player.h"
#include	"gamerules.h"
#include	"game.h"
#include	"money.h"
#include	"weapons.h"

extern int gmsgVGUIMenu;
extern int gmsgManager;

extern void respawn(entvars_t *pev, BOOL fCopyCorpse);

bool CHLWELMS :: IsAllowedToBuy ( CBasePlayer *pPlayer )
{
	if (!pPlayer->IsAlive() || (m_fNextThinkTime && !m_bRoundStarted && !pPlayer->pev->waterlevel && !pPlayer->IsOnLadder()))
		return true;

	return false;
}

void CHLWELMS :: ShowMenu ( CBasePlayer* pPlayer )
{
	if (!pPlayer->IsAlive())
		CHalfLifeMultiplay::ShowMenu (pPlayer);

	else if (!m_bRoundStarted && m_fNextThinkTime)//can't call CHalfLifeTeamplay::ShowMenu
	{
		if (pPlayer->pev->waterlevel || pPlayer->IsOnLadder())
		return;

		MESSAGE_BEGIN(MSG_ONE, gmsgVGUIMenu, NULL, pPlayer->pev);
			WRITE_BYTE(11);
		MESSAGE_END();
	}
}

void CHLWELMS :: InterpretBuyCommand ( const char* pcmd, CBasePlayer* pPlayer )
{
	if (!pPlayer->IsAlive())
		CHalfLifeMultiplay::InterpretBuyCommand (pcmd, pPlayer);

	else if (!m_bRoundStarted && m_fNextThinkTime)//can't call CHalfLifeTeamplay::InterpretBuyCommand
	{
		if (pPlayer->pev->waterlevel || pPlayer->IsOnLadder())
		return;

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
}

BOOL CHLWELMS :: ClientConnected( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] )
{
	if (!m_iPlayers)//server starts on first player connection server
	{
		m_fNextThinkTime = gpGlobals->time + lms_waittime.value;
	}
	
	else if (m_bRoundStarted)//restart after rest of players connecting
	{
		m_fNextThinkTime = gpGlobals->time + 5;
		m_bRoundStarted = false;
		UTIL_ClientPrintAll (HUD_PRINTCENTER, "Round will be restarted within 5 seconds\n");
	}

	m_iPlayers++;

	return CHalfLifeMultiplay::ClientConnected (pEntity, pszName, pszAddress, szRejectReason);
}

void CHLWELMS :: ClientDisconnected( edict_t *pClient )
{
	CHalfLifeMultiplay :: ClientDisconnected (pClient);

	m_iPlayers--;

	int alivePlayers = 0;

	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *plr = UTIL_PlayerByIndex( i );

		if ( plr && plr->IsPlayer() )
		{
			if (plr->IsAlive())
			{
				++alivePlayers;
			}
		}
	}

	if (alivePlayers <= 1) 
	{
		EndRound();
	}
}

void CHLWELMS :: PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor )
{
	CHalfLifeMultiplay::PlayerKilled (pVictim, pKiller, pInflictor);

	int alivePlayers = 0;
	CBasePlayer *aliveLast = NULL;

	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *plr = UTIL_PlayerByIndex( i );

		if ( plr && plr->IsPlayer() )
		{
			if (plr->IsAlive())
			{
				++alivePlayers;
				aliveLast = (CBasePlayer *)plr;
			}
		}
	}

	if (alivePlayers <= 1) 
	{
		Congratulate (aliveLast);
		EndRound();
	}
}

BOOL CHLWELMS :: FPlayerCanTakeDamage( CBasePlayer *pPlayer, CBaseEntity *pAttacker )
{
	if (!m_bRoundStarted && m_fNextThinkTime)
		return false;

	return true;
}

void CHLWELMS :: Think( void )
{
	if (!m_bRoundStarted && m_fNextThinkTime)
	{
		if (gpGlobals->time < m_fNextThinkTime) return;

		StartRound();
	}

	if (m_bRoundStarted && m_fRoundTime)
	{
		if (gpGlobals->time < m_fRoundTime) return;

		EndRound();
		UTIL_ClientPrintAll (HUD_PRINTTALK, "Time is out!\n");
	}
}

void CHLWELMS :: StartRound ( void )
{
	m_bRoundStarted = true;
	m_fRoundTime = gpGlobals->time + lms_roundtime.value;

	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *plr = UTIL_PlayerByIndex( i );

		if ( plr && plr->IsPlayer() )
		{
			CBasePlayer *pPlayer = (CBasePlayer *)plr;

			if ( pPlayer->IsObserver() || !pPlayer->IsAlive() )
			{
				pPlayer->EndObserver();
				pPlayer->pev->nextthink = -1;//Is that necessary?
			}

			MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, pPlayer->pev);
				WRITE_BYTE(MSG_TIMER);
				WRITE_SHORT((short int)(lms_roundtime.value));
				WRITE_ELSE (3);
			MESSAGE_END();
		}
	}

	UTIL_ClientPrintAll (HUD_PRINTCENTER, "Round started!\n");
}

void CHLWELMS :: EndRound ( void )
{
	m_bRoundStarted = false;
	m_fNextThinkTime = gpGlobals->time + lms_waittime.value;

	MESSAGE_BEGIN(MSG_ALL, gmsgMSGManager);
		WRITE_BYTE(MSG_TIMER);
		WRITE_ELSE (1);
	MESSAGE_END();
}

void CHLWELMS :: Congratulate ( CBasePlayer *pPlayer )
{
	if (pPlayer && pPlayer->pev)
	{
		EMIT_SOUND(ENT(pPlayer->pev), CHAN_VOICE, "game/lms_congratulation.wav", VOL_NORM, ATTN_NORM);
		UTIL_ShowMessage("#LMS_CONGRATULATION", pPlayer);
		pPlayer->AddMoney ( GIVE_LMS_WIN );
	}
}