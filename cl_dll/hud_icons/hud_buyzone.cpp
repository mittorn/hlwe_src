#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include <string.h>
#include <stdio.h>

int CHudBuyZone::Init(void) 
{
    gHUD.AddHudElem(this);
    return 1;
};

int CHudBuyZone::VidInit(void)
{
	HUD_BuyZone = gHUD.GetSpriteIndex("buyzone");
	buyzoneW = gHUD.GetSpriteRect(HUD_BuyZone).right - gHUD.GetSpriteRect(HUD_BuyZone).left;
	buyzoneH = gHUD.GetSpriteRect(HUD_BuyZone).bottom - gHUD.GetSpriteRect(HUD_BuyZone).top;

	ScreenX = buyzoneW/2;
	ScreenY = (ScreenHeight - buyzoneH)/1.2;
	return 1;
};

int CHudBuyZone::MsgFunc_BuyZone(const char *pszName,  int iSize, void *pbuf )
{
	int BuyZone = READ_BYTE();

	if (BuyZone)
		m_iFlags |= HUD_ACTIVE;
	else
		m_iFlags &= ~HUD_ACTIVE;
return 1;
}

int CHudBuyZone::Draw(float flTime)
{
	if ( gHUD.m_iHideHUDDisplay & ( HIDEHUD_ALL ) )
		return 1;

	SPR_Set(gHUD.GetSprite(HUD_BuyZone), 0, 200, 0);
	SPR_DrawAdditive(0, ScreenX, ScreenY, &gHUD.GetSpriteRect(HUD_BuyZone));
return 1;
}