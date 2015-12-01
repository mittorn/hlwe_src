#include "hud.h"
#include "cl_util.h"
#include "vgui_TeamFortressViewport.h"
#include "buymenu.h"
#include "VGUI_TextImage.h"
#include "..\game_shared\vgui_LoadTGA.h"

CClassMenu::CClassMenu() : CMenuPanel(0, XRES(55), YRES(55), ScreenWidth - XRES(110), ScreenHeight - YRES(110))
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

    char *pfile = (char*)gEngfuncs.COM_LoadFile( "gfx/desc/CLASSMENU.txt", 5, NULL );
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

	//=========================================================================================
	//	  creating all buttons. process is NOT iterative - arrays has different lengths etc
	//=========================================================================================

	//primary menu
	m_pTeamButtons[0] = new CHLWEButton("  Team 1", XRES(15), YRES(35));
	m_pTeamButtons[0]->setParent(this);
	m_pTeamButtons[0]->addInputSignal( new CClassHandler(this, CLASS_SELECTCLASS, "team1") );

	m_pTeamButtons[1] = new CHLWEButton("  Team 2", XRES(15), YRES(60));
	m_pTeamButtons[1]->setParent(this);
	m_pTeamButtons[1]->addInputSignal( new CClassHandler(this, CLASS_SELECTCLASS, "team2") );

	m_pClassButtons[0] = new CHLWEButton("  1.Intelligence", XRES(15), YRES(35));
	m_pClassButtons[0]->setParent(this);
	m_pClassButtons[0]->addInputSignal( new CClassHandler(this, CLASS_NO, "class1") );

	m_pClassButtons[1] = new CHLWEButton("  2.Marksmanship", XRES(15), YRES(60));
	m_pClassButtons[1]->setParent(this);
	m_pClassButtons[1]->addInputSignal( new CClassHandler(this, CLASS_NO, "class2") );

	m_pClassButtons[2] = new CHLWEButton("  3.Infantry", XRES(15), YRES(85));
	m_pClassButtons[2]->setParent(this);
	m_pClassButtons[2]->addInputSignal( new CClassHandler(this, CLASS_NO, "class3") );

	m_pClassButtons[3] = new CHLWEButton("  4.Field medicine", XRES(15), YRES(110));
	m_pClassButtons[3]->setParent(this);
	m_pClassButtons[3]->addInputSignal( new CClassHandler(this, CLASS_NO, "class4") );

	m_pClassButtons[4] = new CHLWEButton("  5.Heavy infantry", XRES(15), YRES(135));
	m_pClassButtons[4]->setParent(this);
	m_pClassButtons[4]->addInputSignal( new CClassHandler(this, CLASS_NO, "class5") );

	m_pClassButtons[5] = new CHLWEButton("  6.Landing", XRES(15), YRES(160));
	m_pClassButtons[5]->setParent(this);
	m_pClassButtons[5]->addInputSignal( new CClassHandler(this, CLASS_NO, "class6") );

	m_pClassButtons[6] = new CHLWEButton("  7.Maintenance", XRES(15), YRES(185));
	m_pClassButtons[6]->setParent(this);
	m_pClassButtons[6]->addInputSignal( new CClassHandler(this, CLASS_NO, "class7") );

	m_pClassButtons[7] = new CHLWEButton("  8.Strategic arms", XRES(15), YRES(210));
	m_pClassButtons[7]->setParent(this);
	m_pClassButtons[7]->addInputSignal( new CClassHandler(this, CLASS_NO, "class8") );

	m_pClassButtons[8] = new CHLWEButton("  9.Fortification", XRES(15), YRES(235));
	m_pClassButtons[8]->setParent(this);
	m_pClassButtons[8]->addInputSignal( new CClassHandler(this, CLASS_NO, "class9") );

	m_pClassButtons[9] = new CHLWEButton("  0.Random branch", XRES(15), getTall() - YRES(40));
	m_pClassButtons[9]->setParent(this);
	m_pClassButtons[9]->addInputSignal( new CClassHandler(this, CLASS_NO, "class_random") );

	m_pUnlockButtons[0] = new CHLWEButton("  1.Change branch", XRES(15), YRES(35));
	m_pUnlockButtons[0]->setParent(this);
	m_pUnlockButtons[0]->addInputSignal( new CClassHandler(this, CLASS_SELECTCLASS, NULL) );

	m_pUnlockButtons[1] = new CHLWEButton("  2.Unlock 1", XRES(15), YRES(85));
	m_pUnlockButtons[1]->setParent(this);
	m_pUnlockButtons[1]->addInputSignal( new CClassHandler(this, CLASS_NO, "unlock1") );

	m_pUnlockButtons[2] = new CHLWEButton("  3.Unlock 2", XRES(15), YRES(110));
	m_pUnlockButtons[2]->setParent(this);
	m_pUnlockButtons[2]->addInputSignal( new CClassHandler(this, CLASS_NO, "unlock2") );

	m_pUnlockButtons[3] = new CHLWEButton("  4.Unlock 3", XRES(15), YRES(135));
	m_pUnlockButtons[3]->setParent(this);
	m_pUnlockButtons[3]->addInputSignal( new CClassHandler(this, CLASS_NO, "unlock3") );

	m_pUnlockButtons[4] = new CHLWEButton("  5.Unlock 4", XRES(15), YRES(160));
	m_pUnlockButtons[4]->setParent(this);
	m_pUnlockButtons[4]->addInputSignal( new CClassHandler(this, CLASS_NO, "unlock4") );

	m_pUnlockButtons[5] = new CHLWEButton("  0.Cancel", XRES(15), getTall() - YRES(40));
	m_pUnlockButtons[5]->setParent(this);
	m_pUnlockButtons[5]->addActionSignal( new CCancelButtonHandler() );
}

CClassMenu::~CClassMenu()
{
	delete m_pImage;
}

void CClassMenu::paint()
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
 

void CClassMenu::UpdateState (class_state NewState)
{
	state = NewState;

	if (state == CLASS_NO || (teamOnly && state != CLASS_TEAM) || (gHUD.m_Teamplay < 2 && state != CLASS_TEAM))//hacks, hacks... enough of this bullshit!!!
	{
		gViewPort->HideVGUIMenu();
		return;
	}

	SetAppearance ("CLASSMENU");

	gViewPort->UpdateCursorState();

	if (teamOnly || gHUD.m_Teamplay < 2)//the worst hack!
	{
		switch (m_iPlayerKit)
		{
		case 0:
			SetAppearance ("TEAMMENU");
			break;
		case 1:
			SetAppearance ("CLASSMENU");
			break;
		case 2:
			SetAppearance ("ASSAULTMENU");
			break;
		}
	}

	int i;

	switch (state)
	{
	case CLASS_TEAM:
		{
			m_pTeamButtons[0]->setVisible (true);
			m_pTeamButtons[1]->setVisible (true);


			for (i = 0; i <6; i++)
				m_pUnlockButtons[i]->setVisible (false);

			for (i = 0; i <10; i++)
					m_pClassButtons[i]->setVisible (false);

			if (teamOnly) m_pUnlockButtons[5]->setVisible(true);
		}
		break;

	case CLASS_UNLOCK:
		{
			m_pTeamButtons[0]->setVisible (false);
			m_pTeamButtons[1]->setVisible (false);


			for (i = 0; i <6; i++)
				m_pUnlockButtons[i]->setVisible (true);

			for (i = 0; i <10; i++)
					m_pClassButtons[i]->setVisible (false);
		}
		break;

	case CLASS_SELECTCLASS:
		{
			m_pTeamButtons[0]->setVisible (false);
			m_pTeamButtons[1]->setVisible (false);


			for (i = 0; i <6; i++)
				m_pUnlockButtons[i]->setVisible (false);

			for (i = 0; i <10; i++)
				if (m_iPlayerClass - 1 != i)
					m_pClassButtons[i]->setVisible (true);
				else
					m_pClassButtons[i]->setVisible (false);
		}
		break;
	}
}

//===========================================================================================
//
// ATTENTION! This function must send the same client commands, as action signals of buttons
//
//===========================================================================================

int CClassMenu::ProcessKey (int down, int keynum)
{
	if (keynum == '0' || keynum == '1' || keynum == '2' || keynum == '3' || keynum == '4' ||
		keynum == '5' || keynum == '6' || keynum == '7' || keynum == '8' || keynum == '9')
		//codepage independant ;)
	{
		if (!down) return 0;

		switch (state)
		{
		default:
			return 0;

		case TYPE_NO:
			return 1;//it's impossible...

		case CLASS_TEAM:
			{
				switch (keynum)
				{
				case '1':
					UpdateState (CLASS_SELECTCLASS);
					ClientCmd("team1");
					break;

				case '2':
					UpdateState (CLASS_SELECTCLASS);
					ClientCmd("team2");
					break;

				case '0'://cancel
					if (teamOnly)
						UpdateState (CLASS_NO);
					break;
				}
			}
			break;

		case CLASS_UNLOCK:
			{
				switch (keynum)
				{
				case '1':
					UpdateState (CLASS_SELECTCLASS);
					break;

				case '2':
					UpdateState (CLASS_NO);
						ClientCmd ("unlock1");
					break;

				case '3':
					UpdateState (CLASS_NO);
						ClientCmd ("unlock2");
					break;

				case '4':
					UpdateState (CLASS_NO);
						ClientCmd ("unlock3");
					break;

				case '5':
					UpdateState (CLASS_NO);
						ClientCmd ("unlock4");
					break;

				case '0'://cancel
					UpdateState (CLASS_NO);
					break;
				}
			}
			break;

		case CLASS_SELECTCLASS:
			{
				switch (keynum)
				{
				case '1':
					UpdateState (CLASS_NO);

					if (m_iPlayerClass != 1)
						ClientCmd ("class1");
					break;

				case '2':
					UpdateState (CLASS_NO);

					if (m_iPlayerClass != 2)
						ClientCmd ("class2");
					break;

				case '3':
					UpdateState (CLASS_NO);

					if (m_iPlayerClass != 3)
						ClientCmd ("class3");
					break;

				case '4':
					UpdateState (CLASS_NO);

					if (m_iPlayerClass != 4)
						ClientCmd ("class4");
					break;

				case '5':
					UpdateState (CLASS_NO);

					if (m_iPlayerClass != 5)
						ClientCmd ("class5");
					break;

				case '6':
					UpdateState (CLASS_NO);

					if (m_iPlayerClass != 6)
						ClientCmd ("class6");
					break;

				case '7':
					UpdateState (CLASS_NO);

					if (m_iPlayerClass != 7)
						ClientCmd ("class7");
					break;

				case '8':
					UpdateState (CLASS_NO);

					if (m_iPlayerClass != 8)
						ClientCmd ("class8");
					break;

				case '9':
					UpdateState (CLASS_NO);

					if (m_iPlayerClass != 9)
						ClientCmd ("class9");
					break;

				case '0':
					UpdateState (CLASS_NO);

					if (m_iPlayerClass != 10)
						ClientCmd ("class_random");
					break;
				}
			}
			break;
		}

		return 0;
	}

	return 1;
}

void CClassMenu::SetAppearance ( const char* arg )
{
	char buf[128];

	//Change image
	if (!strncmp(arg, "unlock", 6))
		sprintf (buf, "unlock"/*%s%i",, arg, (m_iPlayerClass == 10)? m_iPlayerKit + 1 : m_iPlayerClass*/);
	else if (!strncmp(arg, "team", 4))
		sprintf (buf, "%s%i", arg, gHUD.m_Teamplay);
	else
		strcpy (buf, arg);

	m_pImage->LoadImage(buf);

	int x = (getWide() + XRES(125) - m_pImage->getImageWide())/2;

	int y = (getTall() - YRES(100) + YRES(35) - m_pImage->getImageTall())/2;

	m_pImage->setPos(x, y);
	m_pImage->setVisible (true);

	//Change text
	if (!strncmp(arg, "unlock", 6))
		sprintf (buf, "gfx/desc/%s%i.txt", arg, (m_iPlayerClass == 10)? m_iPlayerKit + 1 : m_iPlayerClass);
	else if (!strncmp(arg, "team", 4))
		sprintf (buf, "gfx/desc/%s%i.txt", arg, gHUD.m_Teamplay);
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

void CClassMenu::SetMoneyAmount ( int money )
{
	sprintf ( moneyValue, "Half-Life : Weapon Edition 1.5   [You have %i rub. 0 kop.]", money );
	moneyStringLength = strlen (moneyValue);
}
