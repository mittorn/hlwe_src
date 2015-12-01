#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include <string.h>
#include <stdio.h>

int CHudTurretIon::Init(void) 
{
    gHUD.AddHudElem(this);
    return 1;
};

int CHudTurretIon::VidInit(void)
{
	m_TURRET_I = gHUD.GetSpriteIndex( "item_turret_ion" );
	return 1;
};

int CHudTurretIon::MsgFunc_TurretI(const char *pszName,  int iSize, void *pbuf )
{
	TurretIon = READ_BYTE();

	if (TurretIon)
		m_iFlags |= HUD_ACTIVE;
	else
		m_iFlags &= ~HUD_ACTIVE;
	return 1;
}

int CHudTurretIon::Draw(float flTime)
{
	if (gHUD.m_iHideHUDDisplay & ( HIDEHUD_ALL ))
	return 1;

	if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT)) ))
		return 1;

	int r, g, b, x, y;
	int TurretWidth = gHUD.GetSpriteRect(m_TURRET_I).right - gHUD.GetSpriteRect(m_TURRET_I).left;

	x = TurretWidth/3;
	y = TurretWidth/0.6;

	// Draw the icon
	SPR_Set(gHUD.GetSprite(m_TURRET_I), 0, 120, 0);
	SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(m_TURRET_I));

	if (TurretIon <= 50) 
		UnpackRGB(r,g,b, RGB_REDISH);
	if (TurretIon >= 49 && TurretIon <= 99)
		UnpackRGB(r,g,b, RGB_YELLOWISH);
	if (TurretIon >= 100)
		UnpackRGB(r,g,b, RGB_GREENISH);

	x += TurretWidth;
	gHUD.DrawHudNumber(x, y+YRES(5), DHN_3DIGITS, TurretIon, r, g, b);

    return 1;
}