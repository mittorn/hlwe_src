#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include <string.h>
#include <stdio.h>

int CHudPHK::Init(void) 
{
    gHUD.AddHudElem(this);
    return 1;
};

int CHudPHK::VidInit(void)
{
	int HUD_PHK = gHUD.GetSpriteIndex( "PHK_icon" );

	m_hSprite1 = gHUD.GetSprite(HUD_PHK);
	m_prc1 = &gHUD.GetSpriteRect(HUD_PHK);

	m_iWidth = m_prc1->right - m_prc1->left;
return 1;
};

int CHudPHK::MsgFunc_PHK(const char *pszName,  int iSize, void *pbuf )
{
	PHK = READ_BYTE();

	if (PHK)
		m_iFlags |= HUD_ACTIVE;
	else
		m_iFlags &= ~HUD_ACTIVE;
return 1;
}

int CHudPHK::Draw(float flTime)
{
	if ( gHUD.m_iHideHUDDisplay & ( HIDEHUD_ALL ) )
		return 1;

	if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT)) ))
		return 1;

	int r,g,b,x,y;
	r = 0;
	g = 230;
	b = 0;
      
	y = (m_prc1->bottom - m_prc1->top)+15;
	x = ScreenWidth - m_iWidth - m_iWidth/2;

	// Draw the icon
	SPR_Set(m_hSprite1, r, g, b );
	SPR_DrawAdditive( 0,  x, y, m_prc1);
return 1;
}
