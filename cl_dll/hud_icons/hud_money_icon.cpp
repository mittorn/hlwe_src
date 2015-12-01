#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include <string.h>
#include <stdio.h>
//#include "vgui_TeamFortressViewport.h"

int CHudMoney::Init(void)
{
	m_iMoney = 0;
	m_fFade = 0;
	m_iFlags = 0;
	m_iPlus = 0;

	gHUD.AddHudElem(this);
	return 1;
};

int CHudMoney::VidInit(void)
{
	int HUD_dollar = gHUD.GetSpriteIndex( "dollar" );
	int HUD_plus = gHUD.GetSpriteIndex( "plus" );
	int HUD_minus = gHUD.GetSpriteIndex( "minus" );

	m_hSprite1 = 0;
	m_hSprite_plus = 0;
	m_hSprite_minus = 0;
	m_prc_dollar = &gHUD.GetSpriteRect( HUD_dollar );
	m_prc_plus = &gHUD.GetSpriteRect( HUD_plus );
	m_prc_minus = &gHUD.GetSpriteRect( HUD_minus );
	m_iHeight = m_prc_dollar->top;
	m_fFade = 0;
	return 1;
};

int CHudMoney:: MsgFunc_Money(const char *pszName,  int iSize, void *pbuf )
{
	m_iNewMoney = READ_LONG();
	m_iFlags |= HUD_ACTIVE;

	if (m_iNewMoney != m_iMoney)
	{
		m_fFade = FADE_TIME;
		m_iPlus = (m_iNewMoney > m_iMoney);

		m_iPlusMoney = (m_iNewMoney - m_iMoney);
		m_iMinusMoney = (m_iMoney - m_iNewMoney);

//		gViewPort->SetMoneyAmount (m_iNewMoney);
	}
	m_iMoney = m_iNewMoney;
	return 1;
}


int CHudMoney::Draw(float flTime)
{
	if ( gHUD.m_iHideHUDDisplay & HIDEHUD_HEALTH )
		return 1;

	if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT)) ))
		return 1;

	int r,g,b,x,y,a,x2,y2;
	r = 150;
	g = 250;
	b = 0;

	// Has money changed? Flash the money #
	if (m_fFade)
	{
		if (m_fFade > FADE_TIME)
			m_fFade = FADE_TIME;

		m_fFade -= (gHUD.m_flTimeDelta * 80);
		if (m_fFade <= 0)
		{
			a = 256;
			m_fFade = 0;
		}

		// Fade the health number back to dim
		a = 150 +  (m_fFade/FADE_TIME) * 256;

		if (m_iPlus)
		{
			UnpackRGB(r,g,b, RGB_GREENISH);

			y2 = ScreenHeight - gHUD.m_iFontHeight - gHUD.m_iFontHeight/0.25;
			x2 = gHUD.GetSpriteRect(gHUD.m_HUD_number_0).right - gHUD.GetSpriteRect(gHUD.m_HUD_number_0).left;

			m_hSprite_plus = gHUD.GetSprite( gHUD.GetSpriteIndex( "plus" ) );
			SPR_Set(m_hSprite_plus, 0, 120, 0 );
			SPR_DrawAdditive( 0, x2, y2, m_prc_plus);

			x2 += (m_prc_dollar->right - m_prc_dollar->left);
			x2 = gHUD.DrawHudNumberLarge(x2, y2, DHN_DRAWZERO, m_iPlusMoney, 0, 120, 0);
		}
		else
		{
			UnpackRGB(r,g,b, RGB_REDISH);

			y2 = ScreenHeight - gHUD.m_iFontHeight - gHUD.m_iFontHeight/0.25;
			x2 = gHUD.GetSpriteRect(gHUD.m_HUD_number_0).right - gHUD.GetSpriteRect(gHUD.m_HUD_number_0).left;

			m_hSprite_plus = gHUD.GetSprite( gHUD.GetSpriteIndex( "minus" ) );
			SPR_Set(m_hSprite_plus, 255, 0, 0 );
			SPR_DrawAdditive( 0, x2, y2, m_prc_minus);

			x2 += (m_prc_dollar->right - m_prc_dollar->left);
			x2 = gHUD.DrawHudNumberLarge(x2, y2, DHN_DRAWZERO, m_iMinusMoney, 255, 0, 0);
		}
	}
	else
		a = 150;

	ScaleColors(r, g, b, a );
	
	y = ScreenHeight - gHUD.m_iFontHeight - gHUD.m_iFontHeight / 0.4;
	x = gHUD.GetSpriteRect(gHUD.m_HUD_number_0).right - gHUD.GetSpriteRect(gHUD.m_HUD_number_0).left;

	// make sure we have the right sprite handles
	if ( !m_hSprite1 )
		m_hSprite1 = gHUD.GetSprite( gHUD.GetSpriteIndex( "dollar" ) );

	SPR_Set(m_hSprite1, 0, 220, 0 );
	SPR_DrawAdditive( 0,  x, y, m_prc_dollar);

	x += (m_prc_dollar->right - m_prc_dollar->left);
	x = gHUD.DrawHudNumberLarge(x, y, DHN_DRAWZERO, m_iMoney, r, g, b);
return 1;
}