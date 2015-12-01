#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include <string.h>
#include <stdio.h>

int CHudAntigrav::Init(void) 
{
    gHUD.AddHudElem(this);
    return 1;
};

int CHudAntigrav::VidInit(void)
{
	int HUD_Antigrav = gHUD.GetSpriteIndex( "antigrav_icon" );

	m_hSprite1 = gHUD.GetSprite(HUD_Antigrav);
	m_prc1 = &gHUD.GetSpriteRect(HUD_Antigrav);

	m_iWidth = m_prc1->right - m_prc1->left;
return 1;
};

int CHudAntigrav::MsgFunc_Antigrav(const char *pszName,  int iSize, void *pbuf )
{
	Antigrav = READ_BYTE();

	if (Antigrav)
		m_iFlags |= HUD_ACTIVE;
	else
		m_iFlags &= ~HUD_ACTIVE;
return 1;
}

int CHudAntigrav::Draw(float flTime)
{
	if ( gHUD.m_iHideHUDDisplay & ( HIDEHUD_ALL ) )
		return 1;

	if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT)) ))
		return 1;

	int r,g,b,x,y;
	r = 128;
	g = 128;
	b = 128;
      
	y = (m_prc1->bottom - m_prc1->top)+190;
	x = ScreenWidth - m_iWidth - m_iWidth/2;

	// Draw the icon
	SPR_Set(m_hSprite1, r, g, b );
	SPR_DrawAdditive( 0,  x, y, m_prc1);
return 1;
}
