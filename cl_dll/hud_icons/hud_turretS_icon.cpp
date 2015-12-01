#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include <string.h>
#include <stdio.h>

int CHudTurretSentry::Init(void) 
{
    gHUD.AddHudElem(this);
    return 1;
};

int CHudTurretSentry::VidInit(void)
{
	m_TURRET_S = gHUD.GetSpriteIndex( "item_turret_sentry" );
	return 1;
};

int CHudTurretSentry::MsgFunc_TurretS(const char *pszName,  int iSize, void *pbuf )
{
	TurretSentry = READ_BYTE();

	if (TurretSentry)
		m_iFlags |= HUD_ACTIVE;
	else
		m_iFlags &= ~HUD_ACTIVE;
	return 1;
}

int CHudTurretSentry::Draw(float flTime)
{
	if (gHUD.m_iHideHUDDisplay & ( HIDEHUD_ALL ))
	return 1;

	if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT)) ))
		return 1;

	int r, g, b, x, y;
	int TurretWidth = gHUD.GetSpriteRect(m_TURRET_S).right - gHUD.GetSpriteRect(m_TURRET_S).left;

	x = TurretWidth/3;
	y = TurretWidth/4;

	// Draw the icon
	SPR_Set(gHUD.GetSprite(m_TURRET_S), 0, 120, 0);
	SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(m_TURRET_S));

	if (TurretSentry <= 25) 
		UnpackRGB(r,g,b, RGB_REDISH);
	if (TurretSentry >= 26 && TurretSentry <= 49 )
		UnpackRGB(r,g,b, RGB_YELLOWISH);
	if (TurretSentry >= 50)
		UnpackRGB(r,g,b, RGB_GREENISH);

	x += TurretWidth;
	gHUD.DrawHudNumber(x, y+YRES(5), DHN_3DIGITS, TurretSentry, r, g, b);
    return 1;
}