#include "hud.h"
#include "cl_util.h"
#include "triangleapi.h"
#include "parsemsg.h"
#include "com_model.h"
#include <string.h>
#include <stdio.h>

void DrawQuad(float xmin, float ymin, float xmax, float ymax)
{
	//top left
	gEngfuncs.pTriAPI->TexCoord2f(0,0);
	gEngfuncs.pTriAPI->Vertex3f(xmin, ymin, 0); 
	//bottom left
	gEngfuncs.pTriAPI->TexCoord2f(0,1);
	gEngfuncs.pTriAPI->Vertex3f(xmin, ymax, 0);
	//bottom right
	gEngfuncs.pTriAPI->TexCoord2f(1,1);
	gEngfuncs.pTriAPI->Vertex3f(xmax, ymax, 0);
	//top right
	gEngfuncs.pTriAPI->TexCoord2f(1,0);
	gEngfuncs.pTriAPI->Vertex3f(xmax, ymin, 0);
}

int CHudScope::Init()
{
	m_iHudMode = 0;
	m_iFlags |= HUD_ACTIVE;

	gHUD.AddHudElem(this);
	return 1;
}

int CHudScope::VidInit()
{
	m_hCrosshair = SPR_Load("sprites/hud_scope.spr");
	m_iHudMode = 0;
	return 1;
}

int CHudScope::MsgFunc_Scope(const char *pszName,  int iSize, void *pbuf )
{
	m_iHudMode = READ_BYTE();
	return 1;
}

int CHudScope::Draw(float flTime)
{
	if(!m_hCrosshair)
	return 0;

	if(!m_iHudMode)
	return 0;//draw scope

	gEngfuncs.pTriAPI->RenderMode(kRenderTransColor);
	gEngfuncs.pTriAPI->Brightness(1.0);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
	gEngfuncs.pTriAPI->CullFace(TRI_NONE);
	float left = (ScreenWidth - ScreenHeight)/2;
	float right = left + ScreenHeight;
	float centerx = ScreenWidth/2;
	float centery = ScreenHeight/2;

	if(m_iHudMode == 3)//draw alternative scope
	{
		gEngfuncs.pTriAPI->SpriteTexture((struct model_s *)gEngfuncs.GetSpritePointer( m_hCrosshair ), 5);
		gEngfuncs.pTriAPI->Begin(TRI_QUADS);
		DrawQuad(left, 0, centerx, centery);
		gEngfuncs.pTriAPI->End();

		gEngfuncs.pTriAPI->SpriteTexture((struct model_s *)gEngfuncs.GetSpritePointer( m_hCrosshair ), 6);
		gEngfuncs.pTriAPI->Begin(TRI_QUADS);
		DrawQuad(centerx, 0, right, centery);
		gEngfuncs.pTriAPI->End();

		gEngfuncs.pTriAPI->SpriteTexture((struct model_s *)gEngfuncs.GetSpritePointer( m_hCrosshair ), 7);
		gEngfuncs.pTriAPI->Begin(TRI_QUADS);
		DrawQuad(centerx, centery, right, ScreenHeight);
		gEngfuncs.pTriAPI->End();

		gEngfuncs.pTriAPI->SpriteTexture((struct model_s *)gEngfuncs.GetSpritePointer( m_hCrosshair ), 8);
		gEngfuncs.pTriAPI->Begin(TRI_QUADS);
		DrawQuad(left, centery, centerx, ScreenHeight);
		gEngfuncs.pTriAPI->End();
	}
	else if(m_iHudMode == 4)//draw alternative scope 2
	{
		gEngfuncs.pTriAPI->SpriteTexture((struct model_s *)gEngfuncs.GetSpritePointer( m_hCrosshair ), 9);
		gEngfuncs.pTriAPI->Begin(TRI_QUADS);
		DrawQuad(left, 0, centerx, centery);
		gEngfuncs.pTriAPI->End();

		gEngfuncs.pTriAPI->SpriteTexture((struct model_s *)gEngfuncs.GetSpritePointer( m_hCrosshair ), 10);
		gEngfuncs.pTriAPI->Begin(TRI_QUADS);
		DrawQuad(centerx, 0, right, centery);
		gEngfuncs.pTriAPI->End();

		gEngfuncs.pTriAPI->SpriteTexture((struct model_s *)gEngfuncs.GetSpritePointer( m_hCrosshair ), 11);
		gEngfuncs.pTriAPI->Begin(TRI_QUADS);
		DrawQuad(centerx, centery, right, ScreenHeight);
		gEngfuncs.pTriAPI->End();

		gEngfuncs.pTriAPI->SpriteTexture((struct model_s *)gEngfuncs.GetSpritePointer( m_hCrosshair ), 12);
		gEngfuncs.pTriAPI->Begin(TRI_QUADS);
		DrawQuad(left, centery, centerx, ScreenHeight);
		gEngfuncs.pTriAPI->End();
	}
	else//draw default scope
	{
		gEngfuncs.pTriAPI->SpriteTexture((struct model_s *)gEngfuncs.GetSpritePointer( m_hCrosshair ), 1);
		gEngfuncs.pTriAPI->Begin(TRI_QUADS);
		DrawQuad(left, 0, centerx, centery);
		gEngfuncs.pTriAPI->End();

		gEngfuncs.pTriAPI->SpriteTexture((struct model_s *)gEngfuncs.GetSpritePointer( m_hCrosshair ), 2);
		gEngfuncs.pTriAPI->Begin(TRI_QUADS);
		DrawQuad(centerx, 0, right, centery);
		gEngfuncs.pTriAPI->End();

		gEngfuncs.pTriAPI->SpriteTexture((struct model_s *)gEngfuncs.GetSpritePointer( m_hCrosshair ), 3);
		gEngfuncs.pTriAPI->Begin(TRI_QUADS);
		DrawQuad(centerx, centery, right, ScreenHeight);
		gEngfuncs.pTriAPI->End();

		gEngfuncs.pTriAPI->SpriteTexture((struct model_s *)gEngfuncs.GetSpritePointer( m_hCrosshair ), 4);
		gEngfuncs.pTriAPI->Begin(TRI_QUADS);
		DrawQuad(left, centery, centerx, ScreenHeight);
		gEngfuncs.pTriAPI->End();
	}

	gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
	gEngfuncs.pTriAPI->SpriteTexture((struct model_s *)gEngfuncs.GetSpritePointer(m_hCrosshair), 0);
	gEngfuncs.pTriAPI->Begin(TRI_QUADS);
	DrawQuad(0, 0, left+1, ScreenHeight);
	DrawQuad(right-1, 0, ScreenWidth, ScreenHeight);
	gEngfuncs.pTriAPI->End();
	return 1;
}