#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include <string.h>
#include <stdio.h>

int CHudPt::Init(void) 
{
    gHUD.AddHudElem(this);
    return 1;
};

int CHudPt::VidInit(void)
{
	int HUD_Pt = gHUD.GetSpriteIndex( "pt_icon" );

	m_hSprite1 = gHUD.GetSprite(HUD_Pt);
	m_prc1 = &gHUD.GetSpriteRect(HUD_Pt);

	m_iWidth = m_prc1->right - m_prc1->left;
return 1;
};

int CHudPt::MsgFunc_Pt(const char *pszName,  int iSize, void *pbuf )
{
	Pt = READ_BYTE();

	if (Pt)
		m_iFlags |= HUD_ACTIVE;
	else
		m_iFlags &= ~HUD_ACTIVE;
return 1;
}

int CHudPt::Draw(float flTime)
{
	if ( gHUD.m_iHideHUDDisplay & ( HIDEHUD_ALL ) )
	return 1;

      int r,g,b,x,y;
      r = 220;
      g = 240;
      b = 0;

	if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT)) ))
		return 1;
      
	y = (m_prc1->bottom - m_prc1->top)+225;
	x = ScreenWidth - m_iWidth - m_iWidth/2;

	// Draw the icon
	SPR_Set(m_hSprite1, r, g, b );
	SPR_DrawAdditive( 0,  x, y, m_prc1);

      return 1;
}