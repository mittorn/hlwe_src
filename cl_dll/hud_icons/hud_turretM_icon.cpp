#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include <string.h>
#include <stdio.h>

int CHudTurretMissile::Init(void) 
{
    gHUD.AddHudElem(this);
    return 1;
};

int CHudTurretMissile::VidInit(void)
{
	m_TURRET_M = gHUD.GetSpriteIndex( "item_turret_missile" );
	return 1;
};

int CHudTurretMissile::MsgFunc_TurretM(const char *pszName,  int iSize, void *pbuf )
{
	TurretMissile = READ_BYTE();

	if (TurretMissile)
		m_iFlags |= HUD_ACTIVE;
	else
		m_iFlags &= ~HUD_ACTIVE;
	return 1;
}

int CHudTurretMissile::Draw(float flTime)
{
	if (gHUD.m_iHideHUDDisplay & ( HIDEHUD_ALL ))
	return 1;

	if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT)) ))
		return 1;

	int r, g, b, x, y;
	int TurretWidth = gHUD.GetSpriteRect(m_TURRET_M).right - gHUD.GetSpriteRect(m_TURRET_M).left;

	x = TurretWidth/3;
	y = TurretWidth;

	// Draw the icon
	SPR_Set(gHUD.GetSprite(m_TURRET_M), 0, 120, 0);
	SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(m_TURRET_M));

	if (TurretMissile <= 40) 
		UnpackRGB(r,g,b, RGB_REDISH);
	if (TurretMissile >= 41 && TurretMissile <= 74 )
		UnpackRGB(r,g,b, RGB_YELLOWISH);
	if (TurretMissile >= 75)
		UnpackRGB(r,g,b, RGB_GREENISH);

	x += TurretWidth;
	gHUD.DrawHudNumber(x, y+YRES(5), DHN_3DIGITS, TurretMissile, r, g, b);

    return 1;
}