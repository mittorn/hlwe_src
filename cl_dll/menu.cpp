#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"

#include <string.h>
#include <stdio.h>

//#include "vgui_TeamFortressViewport.h"

#define MAX_MENU_STRING	512
char g_szMenuString[MAX_MENU_STRING];
char g_szPrelocalisedMenuString[MAX_MENU_STRING];

int KB_ConvertString( char *in, char **ppout );

DECLARE_MESSAGE( m_Menu, ShowMenu );

int CHudMenu :: Init( void )
{
	gHUD.AddHudElem( this );

	HOOK_MESSAGE( ShowMenu );

	InitHUDData();

	return 1;
}

void CHudMenu :: InitHUDData( void )
{
	m_fMenuDisplayed = 0;
	m_bitsValidSlots = 0;
	Reset();
}

void CHudMenu :: Reset( void )
{
	g_szPrelocalisedMenuString[0] = 0;
	m_fWaitingForMore = FALSE;
}

int CHudMenu :: VidInit( void )
{
	return 1;
}

int CHudMenu :: Draw( float flTime )
{
	// check for if menu is set to disappear
	if ( m_flShutoffTime > 0 )
	{
		if ( m_flShutoffTime <= gHUD.m_flTime )
		{  // times up, shutoff
			m_fMenuDisplayed = 0;
			m_iFlags &= ~HUD_ACTIVE;
			return 1;
		}
	}

	// don't draw the menu if the scoreboard is being shown
//	if ( gViewPort && gViewPort->IsScoreBoardVisible() )
//		return 1;

	// draw the menu, along the left-hand side of the screen

	// count the number of newlines
	int nlc = 0;
	for ( int i = 0; i < MAX_MENU_STRING && g_szMenuString[i] != '\0'; i++ )
	{
		if ( g_szMenuString[i] == '\n' )
		{
			nlc++;
		}
	}

	// center it
	int y = (ScreenHeight/2) - ((nlc/2)*12) - 40; // make sure it is above the say text
	int x = 20;

	int def_r = 255;
	int def_g = 255;
	int def_b = 255;

	int r = def_r;
	int g = def_g;
	int b = def_b;

	int p = 0;

	for ( int j = 0; j < MAX_MENU_STRING; j++ )
	{
		if (g_szMenuString[j] == '@')
		{
		char curstring[MAX_MENU_STRING] = " ";						//stupid initialization
		strncpy(curstring, g_szMenuString + p, j - p);				//copying from p-th symbol to j-th symbol

		x = gHUD.DrawHudString( x, y, 320, curstring, r, g, b );	//writeout using old color settings, remember string end coords

		p = j + 2;

		switch (g_szMenuString[j + 1])
		{
			case 'r':
				r = 255;
				g = 0;
				b = 0;

				break;
			case 'g':
				r = 0;
				g = 200;
				b = 0;

				break;
			case 'b':
				r = 0;
				g = 0;
				b = 255;

				break;
			case 'y':
				r = 230;
				g = 155;
				b = 0;

				break;
			case 'v':
				r = 255;
				g = 0;
				b = 255;

				break;
			case 'c':
				r = 0;
				g = 255;
				b = 255;

				break;
			case 'w':
				r = 255;
				g = 255;
				b = 255;

				break;
		}
		}

		else if (g_szMenuString[j] == '\n')
		{
		char curstring[MAX_MENU_STRING] = " ";
		strncpy(curstring, g_szMenuString + p, j - p);

		gHUD.DrawHudString( x, y, 320, curstring, r, g, b );

		x = 20;		//reset x coords to 20
		y += 24;	//start new string
		p = j + 1;


		r = def_r;	//set color to default
		g = def_g;
		b = def_g;
		}

		else if (g_szMenuString[j] == '\0')
		{
		char curstring[MAX_MENU_STRING] = " ";
		strncpy(curstring, g_szMenuString + p, j - p);

		gHUD.DrawHudString( x, y, 320, curstring, r, g, b );

		break; // stop writting text
		}
	}

	return 1;
}
// selects an item from the menu
void CHudMenu :: SelectMenuItem( int menu_item )
{
	// if menu_item is in a valid slot,  send a menuselect command to the server
	if ( (menu_item > 0) && (m_bitsValidSlots & (1 << (menu_item-1))) )
	{
		char szbuf[32];
		sprintf( szbuf, "menuselect %d\n", menu_item );
		ClientCmd( szbuf );

		// remove the menu
		m_fMenuDisplayed = 0;
		m_iFlags &= ~HUD_ACTIVE;
	}
}


// Message handler for ShowMenu message
// takes four values:
//		short: a bitfield of keys that are valid input
//		char : the duration, in seconds, the menu should stay up. -1 means is stays until something is chosen.
//		byte : a boolean, TRUE if there is more string yet to be received before displaying the menu, FALSE if it's the last string
//		string: menu string to display
// if this message is never received, then scores will simply be the combined totals of the players.
int CHudMenu :: MsgFunc_ShowMenu( const char *pszName, int iSize, void *pbuf )
{
	char *temp = NULL;

	BEGIN_READ( pbuf, iSize );

	m_bitsValidSlots = READ_SHORT();
	int DisplayTime = READ_CHAR();
	int NeedMore = READ_BYTE();

	if ( DisplayTime > 0 )
		m_flShutoffTime = DisplayTime + gHUD.m_flTime;
	else
		m_flShutoffTime = -1;

	if ( m_bitsValidSlots )
	{
		if ( !m_fWaitingForMore ) // this is the start of a new menu
		{
			strncpy( g_szPrelocalisedMenuString, READ_STRING(), MAX_MENU_STRING );
		}
		else
		{  // append to the current menu string
			strncat( g_szPrelocalisedMenuString, READ_STRING(), MAX_MENU_STRING - strlen(g_szPrelocalisedMenuString) );
		}
		g_szPrelocalisedMenuString[MAX_MENU_STRING-1] = 0;  // ensure null termination (strncat/strncpy does not)

		if ( !NeedMore )
		{  // we have the whole string, so we can localise it now
			strcpy( g_szMenuString, gHUD.m_TextMessage.BufferedLocaliseTextString( g_szPrelocalisedMenuString ) );

			// Swap in characters
			if ( KB_ConvertString( g_szMenuString, &temp ) )
			{
				strcpy( g_szMenuString, temp );
				free( temp );
			}
		}

		m_fMenuDisplayed = 1;
		m_iFlags |= HUD_ACTIVE;
	}
	else
	{
		m_fMenuDisplayed = 0; // no valid slots means that the menu should be turned off
		m_iFlags &= ~HUD_ACTIVE;
	}

	m_fWaitingForMore = NeedMore;

	return 1;
}
