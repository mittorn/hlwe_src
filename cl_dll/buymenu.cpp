#include "hud.h"
#include "cl_util.h"
#include "vgui_TeamFortressViewport.h"
#include "buymenu.h"
#include "VGUI_TextImage.h"
#include "..\game_shared\vgui_LoadTGA.h"

CBuyMenu::CBuyMenu() : CMenuPanel(0, XRES(55), YRES(55), ScreenWidth - XRES(110), ScreenHeight - YRES(110))
{
	//stuff...
    setPaintBackgroundEnabled(false);

	//image
	m_pImage = new CImageLabel("NULL", 0, 0);
    m_pImage->setParent(this);

	//text descriptions
	int x, y;
	getSize (x, y);

    m_pText = new CHLWEScrollPanel(XRES(160), getTall() - YRES(90), x - XRES(195), y - YRES(300));
    m_pText->setParent(this);

	m_pText->setBgColor(0, 0, 0, 55);
	m_pText->setPaintBackgroundEnabled(true);
	m_pText->setBorder( new LineBorder( Color(236,236,236,0) ) );
    m_pText->setScrollBarAutoVisible(false, true);
    m_pText->setScrollBarVisible(false, false);

    m_pText->validate();

    char *pfile = (char*)gEngfuncs.COM_LoadFile( "gfx/desc/HLWE.txt", 5, NULL );
    if (pfile)
    {
        text = new TextPanel(pfile, 0, 0, m_pText->getClientClip()->getWide(), 64 );
        text->setParent( m_pText->getClient() );
        text->setPaintBackgroundEnabled(false);

        int iScrollSizeX, iScrollSizeY;
        text->getTextImage()->getTextSizeWrapped( iScrollSizeX, iScrollSizeY );

        text->setSize( m_pText->getClientClip()->getWide(), iScrollSizeY );
		text->setFgColor ( 255, 170, 0, 35 );
        m_pText->validate();

        gEngfuncs.COM_FreeFile(pfile);
	}

	//add cancel button
    m_pCancelButton = new CHLWEButton("  0.Cancel", XRES(15), getTall() - YRES(40));
    m_pCancelButton->setParent(this);
    m_pCancelButton->addActionSignal(new CCancelButtonHandler());

	//=========================================================================================
	//	  creating all buttons. process is NOT iterative - arrays has different lengths etc
	//=========================================================================================

	//primary menu
	m_pBuyButtons[0] = new CHLWEButton("  1.Weapons", XRES(15), YRES(35));
	m_pBuyButtons[0]->setParent(this);
	m_pBuyButtons[0]->addInputSignal( new CButtonHandler(this, TYPE_WEAPONS, "weapons") );

	m_pBuyButtons[1] = new CHLWEButton("  2.Items", XRES(15), YRES(60));
	m_pBuyButtons[1]->setParent(this);
	m_pBuyButtons[1]->addInputSignal( new CButtonHandler(this, TYPE_ITEMS, "items") );

	m_pBuyButtons[2] = new CHLWEButton("  3.Powerups", XRES(15), YRES(85));
	m_pBuyButtons[2]->setParent(this);
	m_pBuyButtons[2]->addInputSignal( new CButtonHandler(this, TYPE_POWERUPS, "powerups") );

	m_pBuyButtons[3] = new CHLWEButton("  6.Primary ammo", XRES(15), YRES(125));
	m_pBuyButtons[3]->setParent(this);
	m_pBuyButtons[3]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_ammo1") );

	m_pBuyButtons[4] = new CHLWEButton("  7.Secondary ammo", XRES(15), YRES(150));
	m_pBuyButtons[4]->setParent(this);
	m_pBuyButtons[4]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_ammo2") );

	m_pBuyButtons[5] = new CHLWEButton("  8.Sell Weapon", XRES(15), YRES(175));
	m_pBuyButtons[5]->setParent(this);
	m_pBuyButtons[5]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_sell") );

	//weapon classes
	m_pBuyWeaponButtons[0] = new CHLWEButton("  1.Melee", XRES(15), YRES(35));
	m_pBuyWeaponButtons[0]->setParent(this);
	m_pBuyWeaponButtons[0]->addInputSignal( new CButtonHandler(this, TYPE_MELEE, "melee") );

	m_pBuyWeaponButtons[1] = new CHLWEButton("  2.Handguns", XRES(15), YRES(60));
	m_pBuyWeaponButtons[1]->setParent(this);
	m_pBuyWeaponButtons[1]->addInputSignal( new CButtonHandler(this, TYPE_HANDGUN, "handguns") );

	m_pBuyWeaponButtons[2] = new CHLWEButton("  3.Assault", XRES(15), YRES(85));
	m_pBuyWeaponButtons[2]->setParent(this);
	m_pBuyWeaponButtons[2]->addInputSignal( new CButtonHandler(this, TYPE_ASSAULT, "assault") );

	m_pBuyWeaponButtons[3] = new CHLWEButton("  4.Sniper", XRES(15), YRES(110));
	m_pBuyWeaponButtons[3]->setParent(this);
	m_pBuyWeaponButtons[3]->addInputSignal( new CButtonHandler(this, TYPE_SNIPER, "sniper") );

	m_pBuyWeaponButtons[4] = new CHLWEButton("  5.Heavy", XRES(15), YRES(135));
	m_pBuyWeaponButtons[4]->setParent(this);
	m_pBuyWeaponButtons[4]->addInputSignal( new CButtonHandler(this, TYPE_HEAVY, "heavy") );

	m_pBuyWeaponButtons[5] = new CHLWEButton("  6.Launchers", XRES(15), YRES(160));
	m_pBuyWeaponButtons[5]->setParent(this);
	m_pBuyWeaponButtons[5]->addInputSignal( new CButtonHandler(this, TYPE_LAUNCHER, "launchers") );

	m_pBuyWeaponButtons[6] = new CHLWEButton("  7.Energy", XRES(15), YRES(185));
	m_pBuyWeaponButtons[6]->setParent(this);
	m_pBuyWeaponButtons[6]->addInputSignal( new CButtonHandler(this, TYPE_ENERGY, "energy") );

	m_pBuyWeaponButtons[7] = new CHLWEButton("  8.Explosives", XRES(15), YRES(210));
	m_pBuyWeaponButtons[7]->setParent(this);
	m_pBuyWeaponButtons[7]->addInputSignal( new CButtonHandler(this, TYPE_EXPLOSIVE, "explosives") );

	m_pBuyWeaponButtons[8] = new CHLWEButton("  9.Experimental", XRES(15), YRES(235));
	m_pBuyWeaponButtons[8]->setParent(this);
	m_pBuyWeaponButtons[8]->addInputSignal( new CButtonHandler(this, TYPE_EXPERIMENTAL, "experimental") );

	//melee weapons
	m_pBuyMeleeButtons[0] = new CHLWEButton("  1.Medkit", XRES(15), YRES(35));
	m_pBuyMeleeButtons[0]->setParent(this);
	m_pBuyMeleeButtons[0]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_medkit") );

	m_pBuyMeleeButtons[1] = new CHLWEButton("  2.Crowbar", XRES(15), YRES(60));
	m_pBuyMeleeButtons[1]->setParent(this);
	m_pBuyMeleeButtons[1]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_crowbar") );

	m_pBuyMeleeButtons[2] = new CHLWEButton("  3.Bandsaw", XRES(15), YRES(85));
	m_pBuyMeleeButtons[2]->setParent(this);
	m_pBuyMeleeButtons[2]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_bandsaw") );

	m_pBuyMeleeButtons[3] = new CHLWEButton("  4.Torch", XRES(15), YRES(110));
	m_pBuyMeleeButtons[3]->setParent(this);
	m_pBuyMeleeButtons[3]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_torch") );

	m_pBuyMeleeButtons[4] = new CHLWEButton("  5.Lightsaber", XRES(15), YRES(135));
	m_pBuyMeleeButtons[4]->setParent(this);
	m_pBuyMeleeButtons[4]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_lightsaber") );

	//handguns
	m_pBuyHandgunButtons[0] = new CHLWEButton("  1.Glock 18", XRES(15), YRES(35));
	m_pBuyHandgunButtons[0]->setParent(this);
	m_pBuyHandgunButtons[0]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_glock") );

	m_pBuyHandgunButtons[1] = new CHLWEButton("  2.USP", XRES(15), YRES(60));
	m_pBuyHandgunButtons[1]->setParent(this);
	m_pBuyHandgunButtons[1]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_usp") );

	m_pBuyHandgunButtons[2] = new CHLWEButton("  3.Desert Eagle", XRES(15), YRES(85));
	m_pBuyHandgunButtons[2]->setParent(this);
	m_pBuyHandgunButtons[2]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_deagle") );

	m_pBuyHandgunButtons[3] = new CHLWEButton("  4.Python", XRES(15), YRES(110));
	m_pBuyHandgunButtons[3]->setParent(this);
	m_pBuyHandgunButtons[3]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_python") );

	m_pBuyHandgunButtons[4] = new CHLWEButton("  5.Mac 10", XRES(15), YRES(135));
	m_pBuyHandgunButtons[4]->setParent(this);
	m_pBuyHandgunButtons[4]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_uzi") );

	m_pBuyHandgunButtons[5] = new CHLWEButton("  6.Chendler pistol", XRES(15), YRES(160));
	m_pBuyHandgunButtons[5]->setParent(this);
	m_pBuyHandgunButtons[5]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_shieldgun") );

	//assault weapons
	m_pBuyAssaultButtons[0] = new CHLWEButton("  1.SPAS 14", XRES(15), YRES(35));
	m_pBuyAssaultButtons[0]->setParent(this);
	m_pBuyAssaultButtons[0]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_shotgun") );

	m_pBuyAssaultButtons[1] = new CHLWEButton("  2.Combat SPAS 12", XRES(15), YRES(60));
	m_pBuyAssaultButtons[1]->setParent(this);
	m_pBuyAssaultButtons[1]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_autoshotgun") );

	m_pBuyAssaultButtons[2] = new CHLWEButton("  3.Assault shotgun", XRES(15), YRES(85));
	m_pBuyAssaultButtons[2]->setParent(this);
	m_pBuyAssaultButtons[2]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_30mmsg") );

	m_pBuyAssaultButtons[3] = new CHLWEButton("  4.MP5 NAVY [GL]", XRES(15), YRES(110));
	m_pBuyAssaultButtons[3]->setParent(this);
	m_pBuyAssaultButtons[3]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_mp5") );

	m_pBuyAssaultButtons[4] = new CHLWEButton("  5.M16", XRES(15), YRES(135));
	m_pBuyAssaultButtons[4]->setParent(this);
	m_pBuyAssaultButtons[4]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_m16") );

	m_pBuyAssaultButtons[5] = new CHLWEButton("  6.SG552 + SteyrAUG", XRES(15), YRES(160));
	m_pBuyAssaultButtons[5]->setParent(this);
	m_pBuyAssaultButtons[5]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_akimbogun") );

	m_pBuyAssaultButtons[6] = new CHLWEButton("  7.AK74", XRES(15), YRES(185));
	m_pBuyAssaultButtons[6]->setParent(this);
	m_pBuyAssaultButtons[6]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_ak74") );

	//shit, sniper weapons
	m_pBuySniperButtons[0] = new CHLWEButton("  1.H&K G11", XRES(15), YRES(35));
	m_pBuySniperButtons[0]->setParent(this);
	m_pBuySniperButtons[0]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_g11") );

	m_pBuySniperButtons[1] = new CHLWEButton("  2.U2 Assault Rifle", XRES(15), YRES(60));
	m_pBuySniperButtons[1]->setParent(this);
	m_pBuySniperButtons[1]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_u2") );

	m_pBuySniperButtons[2] = new CHLWEButton("  3.Crossbow", XRES(15), YRES(85));
	m_pBuySniperButtons[2]->setParent(this);
	m_pBuySniperButtons[2]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_crossbow") );

	m_pBuySniperButtons[3] = new CHLWEButton("  4.SVD", XRES(15), YRES(110));
	m_pBuySniperButtons[3]->setParent(this);
	m_pBuySniperButtons[3]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_svd") );

	m_pBuySniperButtons[4] = new CHLWEButton("  5.AWP", XRES(15), YRES(135));
	m_pBuySniperButtons[4]->setParent(this);
	m_pBuySniperButtons[4]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_awp") );

	m_pBuySniperButtons[5] = new CHLWEButton("  6.Barett", XRES(15), YRES(160));
	m_pBuySniperButtons[5]->setParent(this);
	m_pBuySniperButtons[5]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_barett") );

	//heavy weapons
	m_pBuyHeavyButtons[0] = new CHLWEButton("  1.M249 SAW", XRES(15), YRES(35));
	m_pBuyHeavyButtons[0]->setParent(this);
	m_pBuyHeavyButtons[0]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_m249") );

	m_pBuyHeavyButtons[1] = new CHLWEButton("  2.Minigun", XRES(15), YRES(60));
	m_pBuyHeavyButtons[1]->setParent(this);
	m_pBuyHeavyButtons[1]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_minigun") );

	m_pBuyHeavyButtons[2] = new CHLWEButton("  3.Nail gun", XRES(15), YRES(85));
	m_pBuyHeavyButtons[2]->setParent(this);
	m_pBuyHeavyButtons[2]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_nailgun") );

	m_pBuyHeavyButtons[3] = new CHLWEButton("  4.Froster", XRES(15), YRES(110));
	m_pBuyHeavyButtons[3]->setParent(this);
	m_pBuyHeavyButtons[3]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_froster") );

	m_pBuyHeavyButtons[4] = new CHLWEButton("  5.Flamethrower", XRES(15), YRES(135));
	m_pBuyHeavyButtons[4]->setParent(this);
	m_pBuyHeavyButtons[4]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_flamethrower") );

	m_pBuyHeavyButtons[5] = new CHLWEButton("  6.Flak cannon", XRES(15), YRES(160));
	m_pBuyHeavyButtons[5]->setParent(this);
	m_pBuyHeavyButtons[5]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_flakcannon") );

	m_pBuyHeavyButtons[6] = new CHLWEButton("  7.Machinegun", XRES(15), YRES(185));
	m_pBuyHeavyButtons[6]->setParent(this);
	m_pBuyHeavyButtons[6]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_machinegun") );

	m_pBuyHeavyButtons[7] = new CHLWEButton("  8.BFG", XRES(15), YRES(210));
	m_pBuyHeavyButtons[7]->setParent(this);
	m_pBuyHeavyButtons[7]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_bfg") );

	//launchers
	m_pBuyLauncherButtons[0] = new CHLWEButton("  1.RPG", XRES(15), YRES(35));
	m_pBuyLauncherButtons[0]->setParent(this);
	m_pBuyLauncherButtons[0]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_rpg") );

	m_pBuyLauncherButtons[1] = new CHLWEButton("  2.Incendiary", XRES(15), YRES(60));
	m_pBuyLauncherButtons[1]->setParent(this);
	m_pBuyLauncherButtons[1]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_incendiary") );

	m_pBuyLauncherButtons[2] = new CHLWEButton("  3.WHL", XRES(15), YRES(85));
	m_pBuyLauncherButtons[2]->setParent(this);
	m_pBuyLauncherButtons[2]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_whl") );

	m_pBuyLauncherButtons[3] = new CHLWEButton("  4.Devastator", XRES(15), YRES(110));
	m_pBuyLauncherButtons[3]->setParent(this);
	m_pBuyLauncherButtons[3]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_devastator") );

	m_pBuyLauncherButtons[4] = new CHLWEButton("  5.Redeemer", XRES(15), YRES(135));
	m_pBuyLauncherButtons[4]->setParent(this);
	m_pBuyLauncherButtons[4]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_redeemer") );

	//energy
	m_pBuyEnergyButtons[0] = new CHLWEButton("  1.Smart gun", XRES(15), YRES(35));
	m_pBuyEnergyButtons[0]->setParent(this);
	m_pBuyEnergyButtons[0]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_smartgun") );

	m_pBuyEnergyButtons[1] = new CHLWEButton("  2.Tesla gun", XRES(15), YRES(60));
	m_pBuyEnergyButtons[1]->setParent(this);
	m_pBuyEnergyButtons[1]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_teslagun") );

	m_pBuyEnergyButtons[2] = new CHLWEButton("  3.Egon gun", XRES(15), YRES(85));
	m_pBuyEnergyButtons[2]->setParent(this);
	m_pBuyEnergyButtons[2]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_egon") );

	m_pBuyEnergyButtons[3] = new CHLWEButton("  4.Plasma rifle", XRES(15), YRES(110));
	m_pBuyEnergyButtons[3]->setParent(this);
	m_pBuyEnergyButtons[3]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_plasmarifle") );

	m_pBuyEnergyButtons[4] = new CHLWEButton("  5.Photon gun", XRES(15), YRES(135));
	m_pBuyEnergyButtons[4]->setParent(this);
	m_pBuyEnergyButtons[4]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_photongun") );

	m_pBuyEnergyButtons[5] = new CHLWEButton("  6.Gauss gun", XRES(15), YRES(160));
	m_pBuyEnergyButtons[5]->setParent(this);
	m_pBuyEnergyButtons[5]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_gauss") );

	m_pBuyEnergyButtons[6] = new CHLWEButton("  7.TAU cannon", XRES(15), YRES(185));
	m_pBuyEnergyButtons[6]->setParent(this);
	m_pBuyEnergyButtons[6]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_taucannon") );

	m_pBuyEnergyButtons[7] = new CHLWEButton("  8.Gluon gun", XRES(15), YRES(210));
	m_pBuyEnergyButtons[7]->setParent(this);
	m_pBuyEnergyButtons[7]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_gluongun") );

	m_pBuyEnergyButtons[8] = new CHLWEButton("  9.Displacer", XRES(15), YRES(235));
	m_pBuyEnergyButtons[8]->setParent(this);
	m_pBuyEnergyButtons[8]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_displacer") );

	//explosives
	m_pBuyExplosiveButtons[0] = new CHLWEButton("  1.Flashbang", XRES(15), YRES(35));
	m_pBuyExplosiveButtons[0]->setParent(this);
	m_pBuyExplosiveButtons[0]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_flashbang") );

	m_pBuyExplosiveButtons[1] = new CHLWEButton("  2.Hand grenade", XRES(15), YRES(60));
	m_pBuyExplosiveButtons[1]->setParent(this);
	m_pBuyExplosiveButtons[1]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_handgrenade") );

	m_pBuyExplosiveButtons[2] = new CHLWEButton("  3.Trip mine", XRES(15), YRES(85));
	m_pBuyExplosiveButtons[2]->setParent(this);
	m_pBuyExplosiveButtons[2]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_tripmine") );

	m_pBuyExplosiveButtons[3] = new CHLWEButton("  4.Satchel charge", XRES(15), YRES(110));
	m_pBuyExplosiveButtons[3]->setParent(this);
	m_pBuyExplosiveButtons[3]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_satchel") );

	m_pBuyExplosiveButtons[4] = new CHLWEButton("  5.C4 detpack", XRES(15), YRES(135));
	m_pBuyExplosiveButtons[4]->setParent(this);
	m_pBuyExplosiveButtons[4]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_c4") );

	//experimental weapons
	m_pBuyExperimentalButtons[0] = new CHLWEButton("  1.E11 blaster", XRES(15), YRES(35));
	m_pBuyExperimentalButtons[0]->setParent(this);
	m_pBuyExperimentalButtons[0]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_blaster") );

	m_pBuyExperimentalButtons[1] = new CHLWEButton("  2.Bio rifle", XRES(15), YRES(60));
	m_pBuyExperimentalButtons[1]->setParent(this);
	m_pBuyExperimentalButtons[1]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_biorifle") );

	m_pBuyExperimentalButtons[2] = new CHLWEButton("  3.Pulse rifle", XRES(15), YRES(85));
	m_pBuyExperimentalButtons[2]->setParent(this);
	m_pBuyExperimentalButtons[2]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_pulserifle") );

	m_pBuyExperimentalButtons[3] = new CHLWEButton("  4.M72", XRES(15), YRES(110));
	m_pBuyExperimentalButtons[3]->setParent(this);
	m_pBuyExperimentalButtons[3]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_m72") );

	m_pBuyExperimentalButtons[4] = new CHLWEButton("  5.Satellite cannon", XRES(15), YRES(135));
	m_pBuyExperimentalButtons[4]->setParent(this);
	m_pBuyExperimentalButtons[4]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_satellite") );

	m_pBuyExperimentalButtons[5] = new CHLWEButton("  6.PDS", XRES(15), YRES(160));
	m_pBuyExperimentalButtons[5]->setParent(this);
	m_pBuyExperimentalButtons[5]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_turretkit") );

	m_pBuyExperimentalButtons[6] = new CHLWEButton("  7.Chronosceptor", XRES(15), YRES(185));
	m_pBuyExperimentalButtons[6]->setParent(this);
	m_pBuyExperimentalButtons[6]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_chronosceptor") );

	//equipment items
	m_pBuyItemButtons[0] = new CHLWEButton("  1.Healthkit", XRES(15), YRES(35));
	m_pBuyItemButtons[0]->setParent(this);
	m_pBuyItemButtons[0]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_healthkit") );

	m_pBuyItemButtons[1] = new CHLWEButton("  2.Battery", XRES(15), YRES(60));
	m_pBuyItemButtons[1]->setParent(this);
	m_pBuyItemButtons[1]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_battery") );

	m_pBuyItemButtons[2] = new CHLWEButton("  3.Flash battery", XRES(15), YRES(85));
	m_pBuyItemButtons[2]->setParent(this);
	m_pBuyItemButtons[2]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_flashbattery") );

	m_pBuyItemButtons[3] = new CHLWEButton("  4.Kevlar armor", XRES(15), YRES(110));
	m_pBuyItemButtons[3]->setParent(this);
	m_pBuyItemButtons[3]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_kevlar") );

	m_pBuyItemButtons[4] = new CHLWEButton("  5.Heavy armor", XRES(15), YRES(135));
	m_pBuyItemButtons[4]->setParent(this);
	m_pBuyItemButtons[4]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_harmor") );

	//power up items
	m_pBuyPowerupButtons[0] = new CHLWEButton("  1.Long jump", XRES(15), YRES(35));
	m_pBuyPowerupButtons[0]->setParent(this);
	m_pBuyPowerupButtons[0]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_longjump") );

	m_pBuyPowerupButtons[1] = new CHLWEButton("  2.Antigrav", XRES(15), YRES(60));
	m_pBuyPowerupButtons[1]->setParent(this);
	m_pBuyPowerupButtons[1]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_antigrav") );

	m_pBuyPowerupButtons[2] = new CHLWEButton("  3.Personal cloak", XRES(15), YRES(85));
	m_pBuyPowerupButtons[2]->setParent(this);
	m_pBuyPowerupButtons[2]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_cloak") );

	m_pBuyPowerupButtons[3] = new CHLWEButton("  4.Personal teleport", XRES(15), YRES(110));
	m_pBuyPowerupButtons[3]->setParent(this);
	m_pBuyPowerupButtons[3]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_teleport") );

	m_pBuyPowerupButtons[4] = new CHLWEButton("  5.Power shield", XRES(15), YRES(135));
	m_pBuyPowerupButtons[4]->setParent(this);
	m_pBuyPowerupButtons[4]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_powershield") );

	m_pBuyPowerupButtons[5] = new CHLWEButton("  6.Port. healthkit", XRES(15), YRES(160));
	m_pBuyPowerupButtons[5]->setParent(this);
	m_pBuyPowerupButtons[5]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_phealthkit") );

	m_pBuyPowerupButtons[6] = new CHLWEButton("  7.Portable HEV", XRES(15), YRES(185));
	m_pBuyPowerupButtons[6]->setParent(this);
	m_pBuyPowerupButtons[6]->addInputSignal( new CButtonHandler(this, TYPE_NO, "buy_portHEV") );
}

CBuyMenu::~CBuyMenu()
{
	delete m_pImage;
}

void CBuyMenu::paint()
{
	//Background
    drawSetColor(0, 0, 0, 127);
    drawFilledRect(0, 0, getWide(), getTall());

	//Bright borderlines
	drawSetColor(236, 236, 236, 0);
	drawFilledRect(0, 0, XRES(1), getTall());
	drawFilledRect(0, 0, getWide(), YRES(1));

	//Dark borderlines
	drawSetColor(112, 112, 112, 0);
	drawFilledRect(0, getTall() - YRES(1), getWide(), getTall());
	drawFilledRect(getWide() - XRES(1), 0, getWide(), getTall());

	//Text
	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
    SchemeHandle_t hTitleScheme = pSchemes->getSchemeHandle( "Title Font" );
    Font *pTitleFont = pSchemes->getFont( hTitleScheme );

	drawSetTextFont(pTitleFont);
    drawSetTextColor(255, 255, 255, 0);
    drawSetTextPos(XRES(15), YRES(7));
    drawPrintText(moneyValue, moneyStringLength);

	//Image background
    drawSetColor(0, 0, 0, 55);
    drawFilledRect(XRES(160), YRES(35), getWide() - XRES(35), getTall() - YRES(100));
}
 

void CBuyMenu::UpdateState (menu_state NewState)
{
	state = NewState;

	if (state == TYPE_NO)
	{
		gViewPort->HideVGUIMenu();
		return;
	}

	switch (state)
	{
	case TYPE_FULL:
		availableItems = TYPE_MEDKITS|TYPE_ARMOR;
		break;

	case TYPE_ONLYAMMO:
		availableItems = TYPE_MEDKITS;
		break;

	case TYPE_DEFAULT:
		availableItems = TYPE_ARMOR;
		break;
	}

	SetAppearance ("HLWE");

	switch (gHUD.m_Teamplay)
	{
	case 0:
		SetAppearance ("HLWE");
		break;
	case 1:
		SetAppearance ("TEAMMENU");
		break;
	case 2:
		SetAppearance ("ASSAULTMENU");
		break;
	}

	int i;

	switch (state)
	{
	case TYPE_DEFAULT:
		{
			m_pBuyButtons[0]->setVisible (true);
			m_pBuyButtons[1]->setVisible (true);
			m_pBuyButtons[2]->setVisible (true);
			m_pBuyButtons[3]->setVisible (false);
			m_pBuyButtons[4]->setVisible (false);
			m_pBuyButtons[5]->setVisible (false);


			for (i = 0; i <9; i++)
				m_pBuyWeaponButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyMeleeButtons[i]->setVisible (false);

			for (i = 0; i <6; i++)
				m_pBuyHandgunButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyAssaultButtons[i]->setVisible (false);

			for (i = 0; i <6; i++)
				m_pBuySniperButtons[i]->setVisible (false);

			for (i = 0; i <8; i++)
				m_pBuyHeavyButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyLauncherButtons[i]->setVisible (false);

			for (i = 0; i <9; i++)
				m_pBuyEnergyButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyExplosiveButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyExperimentalButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyItemButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyPowerupButtons[i]->setVisible (false);
		}
		break;

	case TYPE_ONLYAMMO:
		{
			m_pBuyButtons[0]->setVisible (false);
			m_pBuyButtons[1]->setVisible (true);
			m_pBuyButtons[2]->setVisible (false);
			m_pBuyButtons[3]->setVisible (true);
			m_pBuyButtons[4]->setVisible (true);
			m_pBuyButtons[5]->setVisible (true);

			for (i = 0; i <9; i++)
				m_pBuyWeaponButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyMeleeButtons[i]->setVisible (false);

			for (i = 0; i <6; i++)
				m_pBuyHandgunButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyAssaultButtons[i]->setVisible (false);

			for (i = 0; i <6; i++)
				m_pBuySniperButtons[i]->setVisible (false);

			for (i = 0; i <8; i++)
				m_pBuyHeavyButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyLauncherButtons[i]->setVisible (false);

			for (i = 0; i <9; i++)
				m_pBuyEnergyButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyExplosiveButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyExperimentalButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyItemButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyPowerupButtons[i]->setVisible (false);
		}
		break;

	case TYPE_FULL:
		{
			for (i = 0; i <6; i++)
				m_pBuyButtons[i]->setVisible (true);

			for (i = 0; i <9; i++)
				m_pBuyWeaponButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyMeleeButtons[i]->setVisible (false);

			for (i = 0; i <6; i++)
				m_pBuyHandgunButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyAssaultButtons[i]->setVisible (false);

			for (i = 0; i <6; i++)
				m_pBuySniperButtons[i]->setVisible (false);

			for (i = 0; i <8; i++)
				m_pBuyHeavyButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyLauncherButtons[i]->setVisible (false);

			for (i = 0; i <9; i++)
				m_pBuyEnergyButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyExplosiveButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyExperimentalButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyItemButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyPowerupButtons[i]->setVisible (false);
		}
		break;

	case TYPE_WEAPONS:
		{
			for (i = 0; i <6; i++)
				m_pBuyButtons[i]->setVisible (false);

			for (i = 0; i <9; i++)
				m_pBuyWeaponButtons[i]->setVisible (true);

			for (i = 0; i <5; i++)
				m_pBuyMeleeButtons[i]->setVisible (false);

			for (i = 0; i <6; i++)
				m_pBuyHandgunButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyAssaultButtons[i]->setVisible (false);

			for (i = 0; i <6; i++)
				m_pBuySniperButtons[i]->setVisible (false);

			for (i = 0; i <8; i++)
				m_pBuyHeavyButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyLauncherButtons[i]->setVisible (false);

			for (i = 0; i <9; i++)
				m_pBuyEnergyButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyExplosiveButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyExperimentalButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyItemButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyPowerupButtons[i]->setVisible (false);
		}
		break;

	case TYPE_MELEE:
		{
			for (i = 0; i <6; i++)
				m_pBuyButtons[i]->setVisible (false);

			for (i = 0; i <9; i++)
				m_pBuyWeaponButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyMeleeButtons[i]->setVisible (true);

			for (i = 0; i <6; i++)
				m_pBuyHandgunButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyAssaultButtons[i]->setVisible (false);

			for (i = 0; i <6; i++)
				m_pBuySniperButtons[i]->setVisible (false);

			for (i = 0; i <8; i++)
				m_pBuyHeavyButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyLauncherButtons[i]->setVisible (false);

			for (i = 0; i <9; i++)
				m_pBuyEnergyButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyExplosiveButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyExperimentalButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyItemButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyPowerupButtons[i]->setVisible (false);
		}
		break;

	case TYPE_HANDGUN:
		{
			for (i = 0; i <6; i++)
				m_pBuyButtons[i]->setVisible (false);

			for (i = 0; i <9; i++)
				m_pBuyWeaponButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyMeleeButtons[i]->setVisible (false);

			for (i = 0; i <6; i++)
				m_pBuyHandgunButtons[i]->setVisible (true);

			for (i = 0; i <7; i++)
				m_pBuyAssaultButtons[i]->setVisible (false);

			for (i = 0; i <6; i++)
				m_pBuySniperButtons[i]->setVisible (false);

			for (i = 0; i <8; i++)
				m_pBuyHeavyButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyLauncherButtons[i]->setVisible (false);

			for (i = 0; i <9; i++)
				m_pBuyEnergyButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyExplosiveButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyExperimentalButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyItemButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyPowerupButtons[i]->setVisible (false);
		}
		break;

	case TYPE_ASSAULT:
		{
			for (i = 0; i <6; i++)
				m_pBuyButtons[i]->setVisible (false);

			for (i = 0; i <9; i++)
				m_pBuyWeaponButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyMeleeButtons[i]->setVisible (false);

			for (i = 0; i <6; i++)
				m_pBuyHandgunButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyAssaultButtons[i]->setVisible (true);

			for (i = 0; i <6; i++)
				m_pBuySniperButtons[i]->setVisible (false);

			for (i = 0; i <8; i++)
				m_pBuyHeavyButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyLauncherButtons[i]->setVisible (false);

			for (i = 0; i <9; i++)
				m_pBuyEnergyButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyExplosiveButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyExperimentalButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyItemButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyPowerupButtons[i]->setVisible (false);
		}
		break;

	case TYPE_SNIPER:
		{
			for (i = 0; i <6; i++)
				m_pBuyButtons[i]->setVisible (false);

			for (i = 0; i <9; i++)
				m_pBuyWeaponButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyMeleeButtons[i]->setVisible (false);

			for (i = 0; i <6; i++)
				m_pBuyHandgunButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyAssaultButtons[i]->setVisible (false);

			for (i = 0; i <6; i++)
				m_pBuySniperButtons[i]->setVisible (true);

			for (i = 0; i <8; i++)
				m_pBuyHeavyButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyLauncherButtons[i]->setVisible (false);

			for (i = 0; i <9; i++)
				m_pBuyEnergyButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyExplosiveButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyExperimentalButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyItemButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyPowerupButtons[i]->setVisible (false);
		}
		break;

	case TYPE_HEAVY:
		{
			for (i = 0; i <6; i++)
				m_pBuyButtons[i]->setVisible (false);

			for (i = 0; i <9; i++)
				m_pBuyWeaponButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyMeleeButtons[i]->setVisible (false);

			for (i = 0; i <6; i++)
				m_pBuyHandgunButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyAssaultButtons[i]->setVisible (false);

			for (i = 0; i <6; i++)
				m_pBuySniperButtons[i]->setVisible (false);

			for (i = 0; i <8; i++)
				m_pBuyHeavyButtons[i]->setVisible (true);

			for (i = 0; i <5; i++)
				m_pBuyLauncherButtons[i]->setVisible (false);

			for (i = 0; i <9; i++)
				m_pBuyEnergyButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyExplosiveButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyExperimentalButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyItemButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyPowerupButtons[i]->setVisible (false);
		}
		break;

	case TYPE_LAUNCHER:
		{
			for (i = 0; i <6; i++)
				m_pBuyButtons[i]->setVisible (false);

			for (i = 0; i <9; i++)
				m_pBuyWeaponButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyMeleeButtons[i]->setVisible (false);

			for (i = 0; i <6; i++)
				m_pBuyHandgunButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyAssaultButtons[i]->setVisible (false);

			for (i = 0; i <6; i++)
				m_pBuySniperButtons[i]->setVisible (false);

			for (i = 0; i <8; i++)
				m_pBuyHeavyButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyLauncherButtons[i]->setVisible (true);

			for (i = 0; i <9; i++)
				m_pBuyEnergyButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyExplosiveButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyExperimentalButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyItemButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyPowerupButtons[i]->setVisible (false);
		}
		break;

	case TYPE_ENERGY:
		{
			for (i = 0; i <6; i++)
				m_pBuyButtons[i]->setVisible (false);

			for (i = 0; i <9; i++)
				m_pBuyWeaponButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyMeleeButtons[i]->setVisible (false);

			for (i = 0; i <6; i++)
				m_pBuyHandgunButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyAssaultButtons[i]->setVisible (false);

			for (i = 0; i <6; i++)
				m_pBuySniperButtons[i]->setVisible (false);

			for (i = 0; i <8; i++)
				m_pBuyHeavyButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyLauncherButtons[i]->setVisible (false);

			for (i = 0; i <9; i++)
				m_pBuyEnergyButtons[i]->setVisible (true);

			for (i = 0; i <5; i++)
				m_pBuyExplosiveButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyExperimentalButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyItemButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyPowerupButtons[i]->setVisible (false);
		}
		break;

	case TYPE_EXPLOSIVE:
		{
			for (i = 0; i <6; i++)
				m_pBuyButtons[i]->setVisible (false);

			for (i = 0; i <9; i++)
				m_pBuyWeaponButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyMeleeButtons[i]->setVisible (false);

			for (i = 0; i <6; i++)
				m_pBuyHandgunButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyAssaultButtons[i]->setVisible (false);

			for (i = 0; i <6; i++)
				m_pBuySniperButtons[i]->setVisible (false);

			for (i = 0; i <8; i++)
				m_pBuyHeavyButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyLauncherButtons[i]->setVisible (false);

			for (i = 0; i <9; i++)
				m_pBuyEnergyButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyExplosiveButtons[i]->setVisible (true);

			for (i = 0; i <7; i++)
				m_pBuyExperimentalButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyItemButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyPowerupButtons[i]->setVisible (false);
		}
		break;

	case TYPE_EXPERIMENTAL:
		{
			for (i = 0; i <6; i++)
				m_pBuyButtons[i]->setVisible (false);

			for (i = 0; i <9; i++)
				m_pBuyWeaponButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyMeleeButtons[i]->setVisible (false);

			for (i = 0; i <6; i++)
				m_pBuyHandgunButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyAssaultButtons[i]->setVisible (false);

			for (i = 0; i <6; i++)
				m_pBuySniperButtons[i]->setVisible (false);

			for (i = 0; i <8; i++)
				m_pBuyHeavyButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyLauncherButtons[i]->setVisible (false);

			for (i = 0; i <9; i++)
				m_pBuyEnergyButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyExplosiveButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyExperimentalButtons[i]->setVisible (true);

			for (i = 0; i <5; i++)
				m_pBuyItemButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyPowerupButtons[i]->setVisible (false);
		}
		break;

	case TYPE_ITEMS:
		{
			for (i = 0; i <6; i++)
				m_pBuyButtons[i]->setVisible (false);

			for (i = 0; i <9; i++)
				m_pBuyWeaponButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyMeleeButtons[i]->setVisible (false);

			for (i = 0; i <6; i++)
				m_pBuyHandgunButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyAssaultButtons[i]->setVisible (false);

			for (i = 0; i <6; i++)
				m_pBuySniperButtons[i]->setVisible (false);

			for (i = 0; i <8; i++)
				m_pBuyHeavyButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyLauncherButtons[i]->setVisible (false);

			for (i = 0; i <9; i++)
				m_pBuyEnergyButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyExplosiveButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyExperimentalButtons[i]->setVisible (false);

			m_pBuyItemButtons[0]->setVisible ((availableItems & TYPE_MEDKITS) ? true : false);
			m_pBuyItemButtons[1]->setVisible ((availableItems & TYPE_ARMOR) ? true : false);
			m_pBuyItemButtons[2]->setVisible ((availableItems & TYPE_MEDKITS) ? true : false);
			m_pBuyItemButtons[3]->setVisible ((availableItems & TYPE_ARMOR) ? true : false);
			m_pBuyItemButtons[4]->setVisible ((availableItems & TYPE_ARMOR) ? true : false);

			for (i = 0; i <7; i++)
				m_pBuyPowerupButtons[i]->setVisible (false);
		}
		break;

	case TYPE_POWERUPS:
		{
			for (i = 0; i <6; i++)
				m_pBuyButtons[i]->setVisible (false);

			for (i = 0; i <9; i++)
				m_pBuyWeaponButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyMeleeButtons[i]->setVisible (false);

			for (i = 0; i <6; i++)
				m_pBuyHandgunButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyAssaultButtons[i]->setVisible (false);

			for (i = 0; i <6; i++)
				m_pBuySniperButtons[i]->setVisible (false);

			for (i = 0; i <8; i++)
				m_pBuyHeavyButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyLauncherButtons[i]->setVisible (false);

			for (i = 0; i <9; i++)
				m_pBuyEnergyButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyExplosiveButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyExperimentalButtons[i]->setVisible (false);

			for (i = 0; i <5; i++)
				m_pBuyItemButtons[i]->setVisible (false);

			for (i = 0; i <7; i++)
				m_pBuyPowerupButtons[i]->setVisible (true);
		}
		break;
	}
}

//===========================================================================================
//
// ATTENTION! This function must send the same client commands, as action signals of buttons
//
//===========================================================================================

int CBuyMenu::ProcessKey (int down, int keynum)
{
	if (keynum == '0' || keynum == '1' || keynum == '2' || keynum == '3' || keynum == '4' ||
		keynum == '5' || keynum == '6' || keynum == '7' || keynum == '8' || keynum == '9')
		//codepage independant ;)
	{
		if (!down) return 0;

		if (keynum == '0')
		{
			UpdateState (TYPE_NO); // cancel button always shown
			return 0;
		}

		switch (state)
		{
		default:
			return 0;

		case TYPE_NO:
			return 1;//it's impossible...

		case TYPE_DEFAULT:
			{
				switch (keynum)
				{
				case '1':
					UpdateState (TYPE_WEAPONS);
					break;

				case '2':
					UpdateState (TYPE_ITEMS);
					break;

				case '3':
					UpdateState (TYPE_POWERUPS);
					break;
				}
			}
			break;

		case TYPE_ONLYAMMO:
			{
				switch (keynum)
				{
				case '2':
					UpdateState (TYPE_ITEMS);
					break;

				case '6':
					UpdateState (TYPE_NO);
					ClientCmd ("buy_ammo1");
					break;

				case '7':
					UpdateState (TYPE_NO);
					ClientCmd ("buy_ammo2");
					break;

				case '8':
					UpdateState (TYPE_NO);
					ClientCmd ("buy_sell");
					break;
				}
			}
			break;

		case TYPE_FULL:
			{
				switch (keynum)
				{
				case '1':
					UpdateState (TYPE_WEAPONS);
					break;

				case '2':
					UpdateState (TYPE_ITEMS);
					break;

				case '3':
					UpdateState (TYPE_POWERUPS);
					break;

				case '6':
					UpdateState (TYPE_NO);
					ClientCmd ("buy_ammo1");
					break;

				case '7':
					UpdateState (TYPE_NO);
					ClientCmd ("buy_ammo2");
					break;

				case '8':
					UpdateState (TYPE_NO);
					ClientCmd ("buy_sell");
					break;
				}
			}
			break;

		case TYPE_WEAPONS:
			{
				switch (keynum)
				{
				case '1':
					UpdateState (TYPE_MELEE);
					break;

				case '2':
					UpdateState (TYPE_HANDGUN);
					break;

				case '3':
					UpdateState (TYPE_ASSAULT);
					break;

				case '4':
					UpdateState (TYPE_SNIPER);
					break;

				case '5':
					UpdateState (TYPE_HEAVY);
					break;

				case '6':
					UpdateState (TYPE_LAUNCHER);
					break;

				case '7':
					UpdateState (TYPE_ENERGY);
					break;

				case '8':
					UpdateState (TYPE_EXPLOSIVE);
					break;

				case '9':
					UpdateState (TYPE_EXPERIMENTAL);
					break;
				}
			}
			break;

		case TYPE_MELEE:
			{
				switch (keynum)
				{
				case '1':
					UpdateState (TYPE_NO);
					ClientCmd("buy_medkit");
					break;

				case '2':
					UpdateState (TYPE_NO);
					ClientCmd("buy_crowbar");
					break;

				case '3':
					UpdateState (TYPE_NO);
					ClientCmd("buy_bandsaw");
					break;

				case '4':
					UpdateState (TYPE_NO);
					ClientCmd("buy_torch");
					break;

				case '5':
					UpdateState (TYPE_NO);
					ClientCmd("buy_lightsaber");
					break;
				}
			}
			break;

		case TYPE_HANDGUN:
			{
				switch (keynum)
				{
				case '1':
					UpdateState (TYPE_NO);
					ClientCmd("buy_glock");
					break;

				case '2':
					UpdateState (TYPE_NO);
					ClientCmd("buy_usp");
					break;

				case '3':
					UpdateState (TYPE_NO);
					ClientCmd("buy_deagle");
					break;

				case '4':
					UpdateState (TYPE_NO);
					ClientCmd("buy_python");
					break;

				case '5':
					UpdateState (TYPE_NO);
					ClientCmd("buy_uzi");
					break;

				case '6':
					UpdateState (TYPE_NO);
					ClientCmd("buy_shieldgun");
					break;
				}
			}
			break;

		case TYPE_ASSAULT:
			{
				switch (keynum)
				{
				case '1':
					UpdateState (TYPE_NO);
					ClientCmd("buy_shotgun");
					break;

				case '2':
					UpdateState (TYPE_NO);
					ClientCmd("buy_autoshotgun");
					break;

				case '3':
					UpdateState (TYPE_NO);
					ClientCmd("buy_30mmsg");
					break;

				case '4':
					UpdateState (TYPE_NO);
					ClientCmd("buy_mp5");
					break;

				case '5':
					UpdateState (TYPE_NO);
					ClientCmd("buy_m16");
					break;

				case '6':
					UpdateState (TYPE_NO);
					ClientCmd("buy_akimbogun");
					break;

				case '7':
					UpdateState (TYPE_NO);
					ClientCmd("buy_ak74");
					break;
				}
			}
			break;

		case TYPE_SNIPER:
			{
				switch (keynum)
				{
				case '1':
					UpdateState (TYPE_NO);
					ClientCmd("buy_g11");
					break;

				case '2':
					UpdateState (TYPE_NO);
					ClientCmd("buy_u2");
					break;

				case '3':
					UpdateState (TYPE_NO);
					ClientCmd("buy_crossbow");
					break;

				case '4':
					UpdateState (TYPE_NO);
					ClientCmd("buy_svd");
					break;

				case '5':
					UpdateState (TYPE_NO);
					ClientCmd("buy_awp");
					break;

				case '6':
					UpdateState (TYPE_NO);
					ClientCmd("buy_barett");
					break;
				}
			}
			break;

		case TYPE_HEAVY:
			{
				switch (keynum)
				{
				case '1':
					UpdateState (TYPE_NO);
					ClientCmd("buy_m249");
					break;

				case '2':
					UpdateState (TYPE_NO);
					ClientCmd("buy_minigun");
					break;

				case '3':
					UpdateState (TYPE_NO);
					ClientCmd("buy_nailgun");
					break;

				case '4':
					UpdateState (TYPE_NO);
					ClientCmd("buy_froster");
					break;

				case '5':
					UpdateState (TYPE_NO);
					ClientCmd("buy_flamethrower");
					break;

				case '6':
					UpdateState (TYPE_NO);
					ClientCmd("buy_flakcannon");
					break;

				case '7':
					UpdateState (TYPE_NO);
					ClientCmd("buy_machinegun");
					break;

				case '8':
					UpdateState (TYPE_NO);
					ClientCmd("buy_bfg");
					break;
				}
			}
			break;

		case TYPE_LAUNCHER:
			{
				switch (keynum)
				{
				case '1':
					UpdateState (TYPE_NO);
					ClientCmd("buy_rpg");
					break;

				case '2':
					UpdateState (TYPE_NO);
					ClientCmd("buy_incendiary");
					break;

				case '3':
					UpdateState (TYPE_NO);
					ClientCmd("buy_whl");
					break;

				case '4':
					UpdateState (TYPE_NO);
					ClientCmd("buy_devastator");
					break;

				case '5':
					UpdateState (TYPE_NO);
					ClientCmd("buy_redeemer");
					break;
				}
			}
			break;

		case TYPE_ENERGY:
			{
				switch (keynum)
				{
				case '1':
					UpdateState (TYPE_NO);
					ClientCmd("buy_smartgun");
					break;

				case '2':
					UpdateState (TYPE_NO);
					ClientCmd("buy_teslagun");
					break;

				case '3':
					UpdateState (TYPE_NO);
					ClientCmd("buy_egon");
					break;

				case '4':
					UpdateState (TYPE_NO);
					ClientCmd("buy_plasmarifle");
					break;

				case '5':
					UpdateState (TYPE_NO);
					ClientCmd("buy_photongun");
					break;

				case '6':
					UpdateState (TYPE_NO);
					ClientCmd("buy_gauss");
					break;

				case '7':
					UpdateState (TYPE_NO);
					ClientCmd("buy_taucannon");
					break;

				case '8':
					UpdateState (TYPE_NO);
					ClientCmd("buy_gluongun");
					break;

				case '9':
					UpdateState (TYPE_NO);
					ClientCmd("buy_displacer");
					break;
				}
			}
			break;

		case TYPE_EXPLOSIVE:
			{
				switch (keynum)
				{
				case '1':
					UpdateState (TYPE_NO);
					ClientCmd("buy_flashbang");
					break;

				case '2':
					UpdateState (TYPE_NO);
					ClientCmd("buy_handgrenade");
					break;

				case '3':
					UpdateState (TYPE_NO);
					ClientCmd("buy_tripmine");
					break;

				case '4':
					UpdateState (TYPE_NO);
					ClientCmd("buy_satchel");
					break;

				case '5':
					UpdateState (TYPE_NO);
					ClientCmd("buy_c4");
					break;
				}
			}
			break;

		case TYPE_EXPERIMENTAL:
			{
				switch (keynum)
				{
				case '1':
					UpdateState (TYPE_NO);
					ClientCmd("buy_blaster");
					break;

				case '2':
					UpdateState (TYPE_NO);
					ClientCmd("buy_biorifle");
					break;

				case '3':
					UpdateState (TYPE_NO);
					ClientCmd("buy_pulserifle");
					break;

				case '4':
					UpdateState (TYPE_NO);
					ClientCmd("buy_m72");
					break;

				case '5':
					UpdateState (TYPE_NO);
					ClientCmd("buy_satellite");
					break;

				case '6':
					UpdateState (TYPE_NO);
					ClientCmd("buy_turretkit");
					break;

				case '7':
					UpdateState (TYPE_NO);
					ClientCmd("buy_chronosceptor");
					break;
				}
			}
			break;

		case TYPE_ITEMS:
			{
				switch (keynum)
				{
				case '1':
					if (availableItems & TYPE_MEDKITS)
					{
						UpdateState (TYPE_NO);
						ClientCmd("buy_healthkit");
					}
					break;

				case '2':
					if (availableItems & TYPE_ARMOR)
					{
						UpdateState (TYPE_NO);
						ClientCmd("buy_battery");
					}
					break;

				case '3':
					if (availableItems & TYPE_MEDKITS)
					{
						UpdateState (TYPE_NO);
						ClientCmd("buy_flashbattery");
					}
					break;

				case '4':
					if (availableItems & TYPE_ARMOR)
					{
						UpdateState (TYPE_NO);
						ClientCmd("buy_kevlar");
					}
					break;

				case '5':
					if (availableItems & TYPE_ARMOR)
					{
						UpdateState (TYPE_NO);
						ClientCmd("buy_harmor");
					}
					break;
				}
			}
			break;

		case TYPE_POWERUPS:
			{
				switch (keynum)
				{
				case '1':
					UpdateState (TYPE_NO);
					ClientCmd("buy_longjump");
					break;

				case '2':
					UpdateState (TYPE_NO);
					ClientCmd("buy_antigrav");
					break;

				case '3':
					UpdateState (TYPE_NO);
					ClientCmd("buy_cloak");
					break;

				case '4':
					UpdateState (TYPE_NO);
					ClientCmd("buy_teleport");
					break;

				case '5':
					UpdateState (TYPE_NO);
					ClientCmd("buy_powershield");
					break;

				case '6':
					UpdateState (TYPE_NO);
					ClientCmd("buy_phealthkit");
					break;

				case '7':
					UpdateState (TYPE_NO);
					ClientCmd("buy_portHEV");
					break;
				}
			}
			break;
		}

		return 0;
	}

	return 1;
}

void CBuyMenu::SetAppearance ( const char* arg )
{
	//Change image
	m_pImage->LoadImage(arg);

	int x = (getWide() + XRES(125) - m_pImage->getImageWide())/2;

	int y = (getTall() - YRES(100) + YRES(35) - m_pImage->getImageTall())/2;

	m_pImage->setPos(x, y);
	m_pImage->setVisible (true);

	//Change text
	char buf[64];

	if (!strcmp(arg, "TEAMMENU") || !strcmp(arg, "ASSAULTMENU"))
		sprintf (buf, "gfx/desc/HLWE.txt");
	else
		sprintf (buf, "gfx/desc/%s.txt", arg);

    char *pfile = (char*)gEngfuncs.COM_LoadFile( buf, 5, NULL );
    if (pfile)
    {
        text->setText(pfile);

        int iScrollSizeX, iScrollSizeY;
        text->getTextImage()->getTextSizeWrapped( iScrollSizeX, iScrollSizeY );

        text->setSize( m_pText->getClientClip()->getWide(), iScrollSizeY );
        m_pText->validate();

        gEngfuncs.COM_FreeFile(pfile);
	}

	m_pText->setScrollValue(0,0);
}

void CBuyMenu::SetMoneyAmount ( int money )
{
	sprintf ( moneyValue, "Half-Life : Weapon Edition 1.5.0.7. You have %i rub. 0 kop.", money );
	moneyStringLength = strlen (moneyValue);
}
