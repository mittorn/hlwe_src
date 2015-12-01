#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include <string.h>
#include <stdio.h>

int CHudHarmor::Init(void)
{
	m_iBat = 0;
	m_fFade = 0;
	m_iFlags = 0;

	gHUD.AddHudElem(this);
	return 1;
};

int CHudHarmor::VidInit(void)
{
	int HUD_Harmor_empty = gHUD.GetSpriteIndex( "harmor_empty" );
	int HUD_Harmor_full = gHUD.GetSpriteIndex( "harmor_full" );

	m_hSprite1 = m_hSprite2 = 0;
	m_prc1 = &gHUD.GetSpriteRect( HUD_Harmor_empty );
	m_prc2 = &gHUD.GetSpriteRect( HUD_Harmor_full );
	m_iHeight = m_prc2->bottom - m_prc1->top;
	m_fFade = 0;
	return 1;
};

int CHudHarmor:: MsgFunc_Harmor(const char *pszName,  int iSize, void *pbuf )
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

int CHudHarmor::Draw(float flTime)
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
	rc.top  += m_iHeight * ((float)(500-(min(500,m_iBat))) * 0.002);

	if (m_iBat <= 50)
	UnpackRGB(r,g,b, RGB_REDISH);
	if (m_iBat >= 51 && m_iBat <= 249 )
	UnpackRGB(r,g,b, RGB_YELLOWISH);
	if (m_iBat >= 250 )
	UnpackRGB(r,g,b, RGB_GREENISH);

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
	
	int iOffset = (m_prc1->bottom - m_prc1->top)/3;

	y = ScreenHeight - gHUD.m_iFontHeight - gHUD.m_iFontHeight/2;
	x = ScreenWidth/3;

	if ( !m_hSprite1 )
		m_hSprite1 = gHUD.GetSprite( gHUD.GetSpriteIndex( "harmor_empty" ) );
	if ( !m_hSprite2 )
		m_hSprite2 = gHUD.GetSprite( gHUD.GetSpriteIndex( "harmor_full" ) );

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