#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include <string.h>
#include <stdio.h>

int CHudKevlar::Init(void)
{
	m_iBat = 0;
	m_fFade = 0;
	m_iFlags = 0;

	gHUD.AddHudElem(this);
	return 1;
};

int CHudKevlar::VidInit(void)
{
	int HUD_kevlar_empty = gHUD.GetSpriteIndex( "kevlar_empty" );
	int HUD_kevlar_full = gHUD.GetSpriteIndex( "kevlar_full" );

	m_hSprite1 = m_hSprite2 = 0;
	m_prc1 = &gHUD.GetSpriteRect( HUD_kevlar_empty );
	m_prc2 = &gHUD.GetSpriteRect( HUD_kevlar_full );
	m_iHeight = m_prc2->bottom - m_prc1->top;
	m_fFade = 0;
	return 1;
};

int CHudKevlar:: MsgFunc_Kevlar(const char *pszName,  int iSize, void *pbuf )
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


int CHudKevlar::Draw(float flTime)
{
	if ( gHUD.m_iHideHUDDisplay & HIDEHUD_HEALTH )
		return 1;

	if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT)) ))
		return 1;

	if (m_iBat <= 0) 
		return 1;

	int r, g, b, x, y, a;
	wrect_t rc;

	rc = *m_prc2;
	rc.top  += m_iHeight * ((float)(100-(min(100,m_iBat))) * 0.01);	// battery can go from 0 to 100 so * 0.01 goes from 0 to 1

	if (m_iBat <= 25) 			// if suit power is less than 25... 
	UnpackRGB(r,g,b, RGB_REDISH); 		// ...make the hud colour red 
	if (m_iBat >= 26 && m_iBat <= 49 ) 	// if suit power is above 25 and below 50... 
	UnpackRGB(r,g,b, RGB_YELLOWISH); 	// ...make the hud colour default orange 
	if (m_iBat >= 50) 			// if suit power is above 100 and below 201... 
	UnpackRGB(r,g,b, RGB_GREENISH); 	// ...make the hud colour green 

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
	
	int iOffset = (m_prc1->bottom - m_prc1->top)/4;

	y = ScreenHeight - gHUD.m_iFontHeight - gHUD.m_iFontHeight / 2;
	x = ScreenWidth/3;

	if ( !m_hSprite1 )
		m_hSprite1 = gHUD.GetSprite( gHUD.GetSpriteIndex( "kevlar_empty" ) );
	if ( !m_hSprite2 )
		m_hSprite2 = gHUD.GetSprite( gHUD.GetSpriteIndex( "kevlar_full" ) );

	SPR_Set(m_hSprite1, r, g, b );
	SPR_DrawAdditive( 0,  x, y - iOffset, m_prc1);

	if (rc.bottom > rc.top)
	{
		SPR_Set(m_hSprite2, r, g, b );
		SPR_DrawAdditive( 0, x, y - iOffset + (rc.top - m_prc2->top), &rc);
	}

	x = gHUD.DrawHudNumberLarge(x, y, DHN_3DIGITS | DHN_DRAWZERO, m_iBat, r, g, b);
	return 1;
}