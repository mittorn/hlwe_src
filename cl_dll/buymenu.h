#ifndef BUYMENU_H
#define BUYMENU_H

using namespace vgui;

typedef enum
{
	TYPE_DEFAULT = 0,	//weapons & powerups
	TYPE_FULL,			//default | onlyammo
	TYPE_ONLYAMMO,		//player can only buy ammo and items or sell weapon
	TYPE_WEAPONS,
	TYPE_MELEE,
	TYPE_HANDGUN,
	TYPE_ASSAULT,
	TYPE_SNIPER,
	TYPE_HEAVY,
	TYPE_LAUNCHER,
	TYPE_ENERGY,
	TYPE_EXPLOSIVE,
	TYPE_EXPERIMENTAL,
	TYPE_ITEMS,
	TYPE_POWERUPS,
	TYPE_NO				//hide menu
} menu_state;

typedef enum
{
	CLASS_TEAM = 0,		//select team
	CLASS_UNLOCK,		//unlock better weapons
	CLASS_SELECTCLASS,	//change class
	CLASS_NO			//hide menu
} class_state;


#define	TYPE_MEDKITS	(1<<1)
#define	TYPE_ARMOR		(1<<2)


//========================================================================
//
//					HLWE-STYLED BUTTONS AND SLIDERS
//
//========================================================================

class CHLWEButton : public Button
{
public:
    CHLWEButton(const char* text, int x, int y) : Button(text, x, y, XRES(110), YRES(20)) 
	{
		setFgColor(255, 170, 0, 0);
		setContentAlignment( a_west );
	};

    virtual void paintBackground()
    {
        if (isSelected())
        {
			//Background
			drawSetColor(0, 0, 0, 105);
			drawFilledRect(0, 0, getWide(), getTall());

			//Dark borderlines
			drawSetColor(112, 112, 112, 0);
			drawFilledRect(0, 0, XRES(1), getTall());
			drawFilledRect(0, 0, getWide(), YRES(1));

			//Bright borderlines
			drawSetColor(236, 236, 236, 0);
			drawFilledRect(0, getTall() - YRES(1), getWide(), getTall());
			drawFilledRect(getWide() - XRES(1), 0, getWide(), getTall());
        }
        else
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
        }
    }

    void internalCursorExited()
    {
        setSelected(false);// fixing one old bug...
    }
};


class CHLWEScrollButton : public CommandButton
{
public:
	CHLWEScrollButton(int x,int y,int wide,int tall) : CommandButton( "", x, y, wide, tall)
	{
		setFgColor(0, 0, 0, 0);

		InputSignal *pISignal = new CHandler_CommandButtonHighlight(this);
		addInputSignal(pISignal);
	}

	void paintBackground( void )
	{
		if (isSelected())
		{
			drawSetColor( 236, 236, 236, 0 );
			drawFilledRect(0,0,_size[0]-1,_size[1]);

			drawSetColor( 112, 112, 112, 0 );
			drawOutlinedRect(0,0,_size[0]-1,_size[1]);
		}
		else
		{
			drawSetColor( 236, 236, 236, 0 );
			drawOutlinedRect(0,0,_size[0]-1,_size[1]);
		}
	}
};


class CHLWESlider : public Slider
{
public:
	CHLWESlider(int x,int y,int wide,int tall,bool vertical) : Slider(x,y,wide,tall,vertical)
	{
	};

	void paintBackground( void )
	{
		int wide,tall,nobx,noby;
		getPaintSize(wide,tall);
		getNobPos(nobx,noby);

		// Border
		drawSetColor( 0, 0, 0, 0 );
		drawOutlinedRect( 0,0,wide,tall );

		// Nob Outline
		drawSetColor( 112, 112, 112, 0 );
		drawOutlinedRect( 0,nobx,wide,noby );
	}
};

class CHLWEScrollPanel : public ScrollPanel
{
public:
	CHLWEScrollPanel(int x,int y,int wide,int tall) : ScrollPanel (x, y, wide, tall)
	{
		ScrollBar *pScrollBar = getVerticalScrollBar();
		pScrollBar->setButton( new CHLWEScrollButton( 0,0,16,16 ), 0 );
		pScrollBar->setButton( new CHLWEScrollButton( 0,0,16,16 ), 1 );
		pScrollBar->setSlider( new CHLWESlider(0,wide-1,wide,(tall-(wide*2))+2,true) ); 
		pScrollBar->setPaintBorderEnabled(false);
		pScrollBar->setPaintBackgroundEnabled(false);
		pScrollBar->setPaintEnabled(false);

		//Dont't draw horizontal slider anyway
		pScrollBar->setPaintBorderEnabled(false);
		pScrollBar->setPaintBackgroundEnabled(false);
		pScrollBar->setPaintEnabled(false);
	}
};

//==============================================================================
//
//			  BUY MENU AND CLASS MENU. JUST A CONTAINERS FOR ALL STUFF
//
//==============================================================================

class CBuyMenu : public CMenuPanel
{
public:
    CBuyMenu();
	~CBuyMenu();

    void paint();

	int ProcessKey(int down, int keynum);

	void UpdateState(menu_state);
	void SetAppearance (const char*); //load image and text file by argument string

	void SetMoneyAmount(int);

private:

	CHLWEButton*		m_pCancelButton;

	CHLWEButton*		m_pBuyButtons[6];
	CHLWEButton*		m_pBuyWeaponButtons[9];
	CHLWEButton*		m_pBuyMeleeButtons[5];
	CHLWEButton*		m_pBuyHandgunButtons[6];
	CHLWEButton*		m_pBuyAssaultButtons[7];
	CHLWEButton*		m_pBuySniperButtons[6];
	CHLWEButton*		m_pBuyHeavyButtons[8];
	CHLWEButton*		m_pBuyLauncherButtons[5];
	CHLWEButton*		m_pBuyEnergyButtons[9];
	CHLWEButton*		m_pBuyExplosiveButtons[5];
	CHLWEButton*		m_pBuyExperimentalButtons[7];
	CHLWEButton*		m_pBuyItemButtons[5];
	CHLWEButton*		m_pBuyPowerupButtons[7];

	CImageLabel*		m_pImage;
	CHLWEScrollPanel*	m_pText;
	TextPanel*			text;

	menu_state	state;
	int			availableItems;//hacking var
	char		moneyValue[96];
	byte		moneyStringLength;//fast
};

class CClassMenu : public CMenuPanel
{
public:

    CClassMenu();
	~CClassMenu();

	bool teamOnly;		//it could be only team menu or what?
	int	m_iPlayerClass; //to know, what images of unlocks to show
	int m_iPlayerKit;	//ÁËßÒÜ!!!

    void paint();

	int ProcessKey(int down, int keynum);

	void UpdateState(class_state);
	void SetAppearance (const char*); //load image and text file by argument string

	void SetMoneyAmount(int);

private:

	CHLWEButton*		m_pClassButtons[10];
	CHLWEButton*		m_pUnlockButtons[6];// 4 unlocks, "change class" button & cancel
	CHLWEButton*		m_pTeamButtons[2];

	CImageLabel*		m_pImage;
	CHLWEScrollPanel*	m_pText;
	TextPanel*			text;

	class_state	state;
	char		moneyValue[96];
	byte		moneyStringLength;//fast
};

//==============================================================================
//
//				Action/Input Signal - Obstraction, that handling
//							key-pressing events etc.
//
//==============================================================================

class CCancelButtonHandler : public ActionSignal
{
public:
    void actionPerformed(Panel* panel)
    {
        gViewPort->HideVGUIMenu();
    }
};

class CButtonHandler : public CDefaultInputSignal
{
private:
    CBuyMenu* pOwner;
	char   _cmd[64];
	menu_state _state;

public:
    CButtonHandler(CBuyMenu* owner, menu_state NewState, char* cmd)
    {
        pOwner = owner;
		_state = NewState;

		//low
		if (cmd != NULL) strcpy(_cmd, cmd);
		else _cmd[0] = '\0';
    }

    void mouseReleased(MouseCode code,Panel* panel)
    {
		if (code != MOUSE_LEFT) return;

		pOwner->UpdateState(_state);

		if (!strcmp(_cmd, "weapons") || !strcmp(_cmd, "items") || !strcmp(_cmd, "powerups")
			|| !strcmp(_cmd, "melee") || !strcmp(_cmd, "handguns") || !strcmp(_cmd, "assault")
			|| !strcmp(_cmd, "sniper") || !strcmp(_cmd, "heavy") || !strcmp(_cmd, "launchers")
			|| !strcmp(_cmd, "energy") || !strcmp(_cmd, "explosives") || !strcmp(_cmd, "experimental"))
		{
			return;//hack.
		}

		//low
		if (strlen(_cmd)) ClientCmd(_cmd);
    }

	void cursorEntered(Panel* panel)
	{
		if (strlen(_cmd)) pOwner->SetAppearance (_cmd); //veeeeeery low...
	}
};

class CClassHandler : public CDefaultInputSignal
{
private:
    CClassMenu* pOwner;
	char		_cmd[64];
	class_state _state;

public:
    CClassHandler(CClassMenu* owner, class_state NewState, char* cmd)
    {
        pOwner = owner;
		_state = NewState;

		//low
		if (cmd != NULL) strcpy(_cmd, cmd);
		else _cmd[0] = '\0';
    }

    void mouseReleased(MouseCode code,Panel* panel)
    {
		if (code != MOUSE_LEFT) return;

		pOwner->UpdateState(_state);

		//low
		if (strlen(_cmd)) ClientCmd(_cmd);
    }

	void cursorEntered(Panel* panel)
	{
		if (strlen(_cmd)) pOwner->SetAppearance (_cmd); //veeeeeery low...
	}
};


#endif