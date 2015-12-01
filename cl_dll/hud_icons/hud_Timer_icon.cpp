#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include <string.h>
#include <stdio.h>

int CHudTimer::Init(void) 
{
    gHUD.AddHudElem(this);

    return 1;
};

int CHudTimer::VidInit(void)
{
	int HUD_TIMER = gHUD.GetSpriteIndex( "Timer_icon" );
	int HUD_SEPARATOR = gHUD.GetSpriteIndex( "Timer_separator" );
	Time = 0;
	Inited = false;

	m_hSprite1 = gHUD.GetSprite(HUD_TIMER);
	m_prc1 = &gHUD.GetSpriteRect(HUD_TIMER);

	m_hSprite2 = gHUD.GetSprite(HUD_SEPARATOR);
	m_prc2 = &gHUD.GetSpriteRect(HUD_SEPARATOR);

	m_iWidth = m_prc1->right - m_prc1->left;

	return 1;
};

int CHudTimer::MsgFunc_Timer(const char *pszName,  int iSize, void *pbuf )
{
	Time = READ_SHORT();
	Inited = false;

	if (Time > 0)
		m_iFlags |= HUD_ACTIVE;
	else
		m_iFlags &= ~HUD_ACTIVE;

	return 1;
}

int CHudTimer::Draw(float flTime)
{
	if (!Inited)
	{
		Inited = true;
		Time += flTime;
	}

	if ( gHUD.m_iHideHUDDisplay & ( HIDEHUD_ALL ) )
		return 1;

	if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT)) ))
		return 1;

	int diff = Time - flTime;

	if (diff < 0)
	{
		diff = 0;
		return 1;
	}

	int r,g,b,x,y;
	r = (diff > 10) ? 255 : 200;
	g = (diff > 10) ? 140 : 0;
	b = 0;
      
	y = (m_prc1->bottom - m_prc1->top);
	x = ScreenWidth/2 - 80;

	// Draw the icon
	SPR_Set(m_hSprite1, r, g, b );
	SPR_DrawAdditive( 0,  x, y, m_prc1);

	x += 35;
	y -= 2;

	int min = diff /60;
	int sec = diff %60;

	int sec_exp1 = sec/10;
	int sec_exp0 = sec%10;

	x = gHUD.DrawHudNumberLarge(x, y, DHN_2DIGITS | DHN_DRAWZERO, min, r, g, b);

	SPR_Set(m_hSprite2, r, g, b );
	SPR_DrawAdditive( 0,  x + 7, y, m_prc2);

	x = gHUD.DrawHudNumberLarge(x, y, DHN_2DIGITS | DHN_DRAWZERO, sec_exp1, r, g, b);
	gHUD.DrawHudNumberLarge(x, y, DHN_DRAWZERO, sec_exp0, r, g, b);

	return 1;
}