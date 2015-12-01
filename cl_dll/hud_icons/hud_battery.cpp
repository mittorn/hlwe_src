#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include <string.h>
#include <stdio.h>

int CHudBattery::Init(void)
{
	m_iBat = 0;
	m_fFade = 0;
	m_iFlags = 0;
	gHUD.AddHudElem(this);

	return 1;
};


int CHudBattery::VidInit(void)
{
	int HUD_suit_empty = gHUD.GetSpriteIndex( "suit_empty" );
	int HUD_suit_full = gHUD.GetSpriteIndex( "suit_full" );

	m_hSprite1 = m_hSprite2 = 0;  // delaying get sprite handles until we know the sprites are loaded
	m_prc1 = &gHUD.GetSpriteRect( HUD_suit_empty );
	m_prc2 = &gHUD.GetSpriteRect( HUD_suit_full );
	m_iHeight = m_prc2->bottom - m_prc1->top;
	m_fFade = 0;
	return 1;
};

int CHudBattery:: MsgFunc_Battery(const char *pszName,  int iSize, void *pbuf )
{
	m_iFlags |= HUD_ACTIVE;
	int x = READ_SHORT();

	if (x != m_iBat)
	{
		m_fFade = FADE_TIME;
		m_iBat = x;
	}

	return 1;
}


int CHudBattery::Draw(float flTime)
{
	if ( gHUD.m_iHideHUDDisplay & HIDEHUD_HEALTH )
		return 1;

	int r, g, b, x, y, a;
	wrect_t rc;

	rc = *m_prc2;
	rc.top  += m_iHeight * ((float)(200-(min(200,m_iBat))) * 0.005);	// battery can go from 0 to 200 so * 0.01 goes from 0 to 1

	UnpackRGB(r,g,b, RGB_YELLOWISH);

	// BKJ - Different colour hud START 
	if (m_iBat < 25) // if suit power is less than 25... 
	UnpackRGB(r,g,b, RGB_REDISH); // ...make the hud colour red 
	if (m_iBat > 25 && m_iBat < 50 ) // if suit power is above 25 and below 50... 
	UnpackRGB(r,g,b, RGB_YELLOWISH); // ...make the hud colour default orange 
	if (m_iBat > 100 && m_iBat < 201 ) // if suit power is above 100 and below 201... 
	UnpackRGB(r,g,b, RGB_GREENISH); // ...make the hud colour green 
	if (m_iBat > 200 && m_iBat < 301 ) // if suit power is above 200 and below 301... 
	UnpackRGB(r,g,b, RGB_BLUEISH); // ...make the hud colour blue 
	if (m_iBat > 300) // if suit power is above 300... 
	UnpackRGB(r,g,b, RGB_STRONGBLUEISH); // ...make the hud colour strong blue 
	// BKJ - Different colour hud END 

	if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT)) ))
		return 1;

	// Has health changed? Flash the health #
	if (m_fFade)
	{
		if (m_fFade > FADE_TIME)
			m_fFade = FADE_TIME;

		m_fFade -= (gHUD.m_flTimeDelta * 20);
		if (m_fFade <= 0)
		{
			a = 128;
			m_fFade = 0;
		}

		// Fade the health number back to dim

		a = MIN_ALPHA +  (m_fFade/FADE_TIME) * 128;

	}
	else
		a = MIN_ALPHA;

	ScaleColors(r, g, b, a );
	
	int iOffset = (m_prc1->bottom - m_prc1->top)/6;

	y = ScreenHeight - gHUD.m_iFontHeight - gHUD.m_iFontHeight / 2;
	x = ScreenWidth/5;

	// make sure we have the right sprite handles
	if ( !m_hSprite1 )
		m_hSprite1 = gHUD.GetSprite( gHUD.GetSpriteIndex( "suit_empty" ) );
	if ( !m_hSprite2 )
		m_hSprite2 = gHUD.GetSprite( gHUD.GetSpriteIndex( "suit_full" ) );

	SPR_Set(m_hSprite1, r, g, b );
	SPR_DrawAdditive( 0,  x, y - iOffset, m_prc1);

	if (rc.bottom > rc.top)
	{
		SPR_Set(m_hSprite2, r, g, b );
		SPR_DrawAdditive( 0, x, y - iOffset + (rc.top - m_prc2->top), &rc);
	}

	x += (m_prc1->right - m_prc1->left);
	x = gHUD.DrawHudNumber(x, y, DHN_3DIGITS | DHN_DRAWZERO, m_iBat, r, g, b);

	return 1;
}