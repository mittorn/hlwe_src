#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include <string.h>
#include <stdio.h>

#define GUIDE_S SPR_Width( m_hCrosshair, 0)//автоматический подсчет длины стороны спрайта прицела
#define READOUT_S 128

int CHudRedeemer::Init()
{
	m_iHudMode = 0;
	m_iFlags |= HUD_ACTIVE;
	gHUD.AddHudElem(this);
return 1;
}

int CHudRedeemer::VidInit()
{
	m_hSprite = SPR_Load("sprites/hud_wh_data.spr");
	m_hCrosshair = SPR_Load("sprites/hud_wh_crosshair.spr");
	m_hStatic = SPR_Load("sprites/hud_wh_flicker.spr");
	bSet = FALSE;
	m_iHudMode = 0;
	return 1;
}

int CHudRedeemer::MsgFunc_Warhead (const char *pszName,  int iSize, void *pbuf )
{
	m_iHudMode = READ_BYTE();
	return 1;
}

int CHudRedeemer::Draw(float flTime)
{
	int x, y, w, h;
	int frame;

	if(m_iHudMode == 1)
	{
		y = (ScreenWidth - GUIDE_S) / 2;
		x = (ScreenHeight - GUIDE_S) / 2;

		SPR_Set(m_hCrosshair, 256, 128, 128 );
		SPR_DrawAdditive( 0,  y, x, NULL);

		int yOffset = ((int)(flTime * 600) % READOUT_S) - READOUT_S;
		SPR_Set(m_hSprite, 256, 128, 128 );
		for (; yOffset < ScreenHeight; yOffset += READOUT_S)
		SPR_DrawAdditive( 0, 0, yOffset, NULL);

		SPR_Set(m_hStatic, 10, 100, 10 );

		// play at 15fps
		frame = (int)(flTime * 15) % SPR_Frames( m_hStatic );

		y = x = 0;
		w = SPR_Width( m_hStatic, 0);
		h = SPR_Height( m_hStatic, 0);

		for(y = -(rand() % h); y < ScreenHeight; y += h) 
		for(x = -(rand() % w); x < ScreenWidth; x += w) 
		SPR_DrawAdditive( frame, x, y, NULL );
	}
	return 1;
}