#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include <string.h>
#include <stdio.h>

int CHudLongjump::Init(void) 
{
    gHUD.AddHudElem(this);
    return 1;
};

int CHudLongjump::VidInit(void)
{
	int HUD_Longjump = gHUD.GetSpriteIndex( "longjump_icon" );

	m_hSprite1 = gHUD.GetSprite(HUD_Longjump);
	m_prc1 = &gHUD.GetSpriteRect(HUD_Longjump);

	m_iWidth = m_prc1->right - m_prc1->left;
return 1;
};

int CHudLongjump::MsgFunc_Longjump(const char *pszName,  int iSize, void *pbuf )
{
	Longjump = READ_BYTE();

	if (Longjump)
		m_iFlags |= HUD_ACTIVE;
	else
		m_iFlags &= ~HUD_ACTIVE;
return 1;
}

int CHudLongjump::Draw(float flTime)
{
	if ( gHUD.m_iHideHUDDisplay & ( HIDEHUD_ALL ) )
	return 1;

      int r,g,b,x,y;
      r = 240;
      g = 180;
      b = 0;

	if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT)) ))
		return 1;
      
	y = (m_prc1->bottom - m_prc1->top)+120;
	x = ScreenWidth - m_iWidth - m_iWidth/2;

	// Draw the icon
	SPR_Set(m_hSprite1, r, g, b );
	SPR_DrawAdditive( 0,  x, y, m_prc1);

      return 1;
}