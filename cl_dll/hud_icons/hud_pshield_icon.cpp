#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include <string.h>
#include <stdio.h>

int CHudPshield::Init(void) 
{
    gHUD.AddHudElem(this);
    return 1;
};

int CHudPshield::VidInit(void)
{
	int HUD_Pshield = gHUD.GetSpriteIndex( "pshield_icon" );

	m_hSprite1 = gHUD.GetSprite(HUD_Pshield);
	m_prc1 = &gHUD.GetSpriteRect(HUD_Pshield);

	m_iWidth = m_prc1->right - m_prc1->left;
return 1;
};

int CHudPshield::MsgFunc_Pshield(const char *pszName,  int iSize, void *pbuf )
{
	Pshield = READ_BYTE();

	if (Pshield)
		m_iFlags |= HUD_ACTIVE;
	else
		m_iFlags &= ~HUD_ACTIVE;
return 1;
}

int CHudPshield::Draw(float flTime)
{
	if ( gHUD.m_iHideHUDDisplay & ( HIDEHUD_ALL ) )
	return 1;

      int r,g,b,x,y;
      r = 0;
      g = 0;
      b = 255;

	if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT)) ))
		return 1;
      
	y = (m_prc1->bottom - m_prc1->top)+155;
	x = ScreenWidth - m_iWidth - m_iWidth/2;

	// Draw the icon
	SPR_Set(m_hSprite1, r, g, b );
	SPR_DrawAdditive( 0,  x, y, m_prc1);

      return 1;
}