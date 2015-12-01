#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include <string.h>
#include <stdio.h>

// HUD-firemode icons
#define FIREMODE_SCOPE		1
#define FIREMODE_CHARGE		2
#define FIREMODE_PULSE		3
#define FIREMODE_BEAM		4
#define FIREMODE_NARROW		5
#define FIREMODE_WIDE		6
#define FIREMODE_BOLT		7
#define FIREMODE_HALF		8
#define FIREMODE_FULL		9
#define FIREMODE_SHOOT		10
#define FIREMODE_KNIFE		11
#define FIREMODE_ACCELBOLT	12
#define FIREMODE_EXPBOLT	13
#define FIREMODE_SENTRYTURRET	14
#define FIREMODE_MISSILETURRET	15
#define FIREMODE_IONTURRET	16
#define FIREMODE_QUAD		17
#define FIREMODE_GLAUNCHER	18

int CHudFiremode::Init(void)
{
	gHUD.AddHudElem(this);
	m_iType = 0;
	return 1;
};

int CHudFiremode::VidInit(void)
{
	m_FIREMODE = gHUD.GetSpriteIndex( "firemode" );
	m_FIREMODE_GLAUNCHER = gHUD.GetSpriteIndex( "mode_glauncher" );
	m_FIREMODE_QUAD = gHUD.GetSpriteIndex( "mode_quad" );
	m_FIREMODE_SCOPE = gHUD.GetSpriteIndex( "mode_scope" );
	m_FIREMODE_CHARGE = gHUD.GetSpriteIndex( "mode_charge" );
	m_FIREMODE_PULSE = gHUD.GetSpriteIndex( "mode_pulse" );
	m_FIREMODE_BEAM = gHUD.GetSpriteIndex( "mode_beam" );
	m_FIREMODE_NARROW = gHUD.GetSpriteIndex( "mode_narrow" );
	m_FIREMODE_WIDE = gHUD.GetSpriteIndex( "mode_wide" );
	m_FIREMODE_BOLT = gHUD.GetSpriteIndex( "mode_bolt" );
	m_FIREMODE_HALF = gHUD.GetSpriteIndex( "mode_half" );
	m_FIREMODE_FULL = gHUD.GetSpriteIndex( "mode_full" );
	m_FIREMODE_SHOOT = gHUD.GetSpriteIndex( "mode_shoot" );
	m_FIREMODE_KNIFE = gHUD.GetSpriteIndex( "mode_knife" );
	m_FIREMODE_ACCELBOLT = gHUD.GetSpriteIndex( "mode_accelbolt" );
	m_FIREMODE_EXPBOLT = gHUD.GetSpriteIndex( "mode_expbolt" );
	m_FIREMODE_SENTRYTURRET = gHUD.GetSpriteIndex( "mode_turret_sentry" );
	m_FIREMODE_MISSILETURRET = gHUD.GetSpriteIndex( "mode_turret_missile" );
	m_FIREMODE_IONTURRET = gHUD.GetSpriteIndex( "mode_turret_ion" );
	m_iType = 0;
	return 1;
};

int CHudFiremode:: MsgFunc_Firemode(const char *pszName,  int iSize, void *pbuf )
{
	m_iType = READ_BYTE();

	if (m_iType)
	m_iFlags |= HUD_ACTIVE;
	else
	m_iFlags &= ~HUD_ACTIVE;
	return 1;
}

int CHudFiremode::Draw(float flTime)
{
	if ( gHUD.m_iHideHUDDisplay & ( HIDEHUD_ALL ) )
	return 1;

	if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT)) ))
		return 1;

	int r,g,b,x,y,x2;
	r = 200;
	g = 140;
	b = 0;

	y = ScreenHeight - gHUD.m_iFontHeight - gHUD.m_iFontHeight / 2;
	x = ScreenWidth/2;
	x2 = ScreenWidth/1.6;

	if (m_iType!=0)
	{
		SPR_Set(gHUD.GetSprite(m_FIREMODE), r, g, b);
		SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(m_FIREMODE));
	}

		if (m_iType==FIREMODE_GLAUNCHER)
	{
		SPR_Set(gHUD.GetSprite(m_FIREMODE_GLAUNCHER), r, g, b);
		SPR_DrawAdditive(0, x2, y, &gHUD.GetSpriteRect(m_FIREMODE_GLAUNCHER));
	}
		else if (m_iType==FIREMODE_QUAD)
	{
		SPR_Set(gHUD.GetSprite(m_FIREMODE_QUAD), r, g, b);
		SPR_DrawAdditive(0, x2, y, &gHUD.GetSpriteRect(m_FIREMODE_QUAD));
	}
		else if (m_iType==FIREMODE_SCOPE)
	{
		SPR_Set(gHUD.GetSprite(m_FIREMODE_SCOPE), r, g, b);
		SPR_DrawAdditive(0, x2, y, &gHUD.GetSpriteRect(m_FIREMODE_SCOPE));
	}
		else if (m_iType==FIREMODE_CHARGE)
	{
		SPR_Set(gHUD.GetSprite(m_FIREMODE_CHARGE), r, g, b);
		SPR_DrawAdditive(0, x2, y, &gHUD.GetSpriteRect(m_FIREMODE_CHARGE));
	}
		else if (m_iType==FIREMODE_PULSE)
	{
		SPR_Set(gHUD.GetSprite(m_FIREMODE_PULSE), r, g, b);
		SPR_DrawAdditive(0, x2, y, &gHUD.GetSpriteRect(m_FIREMODE_PULSE));
	}
		else if (m_iType==FIREMODE_BEAM)
	{
		SPR_Set(gHUD.GetSprite(m_FIREMODE_BEAM), r, g, b);
		SPR_DrawAdditive(0, x2, y, &gHUD.GetSpriteRect(m_FIREMODE_BEAM));
	}
		else if (m_iType==FIREMODE_NARROW)
	{
		SPR_Set(gHUD.GetSprite(m_FIREMODE_NARROW), r, g, b);
		SPR_DrawAdditive(0, x2, y, &gHUD.GetSpriteRect(m_FIREMODE_NARROW));
	}
		else if (m_iType==FIREMODE_WIDE)
	{
		SPR_Set(gHUD.GetSprite(m_FIREMODE_WIDE), r, g, b);
		SPR_DrawAdditive(0, x2, y, &gHUD.GetSpriteRect(m_FIREMODE_WIDE));
	}
		else if (m_iType==FIREMODE_BOLT)
	{
		SPR_Set(gHUD.GetSprite(m_FIREMODE_BOLT), r, g, b);
		SPR_DrawAdditive(0, x2, y, &gHUD.GetSpriteRect(m_FIREMODE_BOLT));
	}
		else if (m_iType==FIREMODE_ACCELBOLT)
	{
		SPR_Set(gHUD.GetSprite(m_FIREMODE_ACCELBOLT), r, g, b);
		SPR_DrawAdditive(0, x2, y, &gHUD.GetSpriteRect(m_FIREMODE_ACCELBOLT));
	}
		else if (m_iType==FIREMODE_EXPBOLT)
	{
		SPR_Set(gHUD.GetSprite(m_FIREMODE_EXPBOLT), r, g, b);
		SPR_DrawAdditive(0, x2, y, &gHUD.GetSpriteRect(m_FIREMODE_EXPBOLT));
	}
		else if (m_iType==FIREMODE_HALF)
	{
		SPR_Set(gHUD.GetSprite(m_FIREMODE_HALF), r, g, b);
		SPR_DrawAdditive(0, x2, y, &gHUD.GetSpriteRect(m_FIREMODE_HALF));
	}
		else if (m_iType==FIREMODE_FULL)
	{
		SPR_Set(gHUD.GetSprite(m_FIREMODE_FULL), r, g, b);
		SPR_DrawAdditive(0, x2, y, &gHUD.GetSpriteRect(m_FIREMODE_FULL));
	}
		else if (m_iType==FIREMODE_SHOOT)
	{
		SPR_Set(gHUD.GetSprite(m_FIREMODE_SHOOT), r, g, b);
		SPR_DrawAdditive(0, x2, y, &gHUD.GetSpriteRect(m_FIREMODE_SHOOT));
	}
		else if (m_iType==FIREMODE_KNIFE)
	{
		SPR_Set(gHUD.GetSprite(m_FIREMODE_KNIFE), r, g, b);
		SPR_DrawAdditive(0, x2, y, &gHUD.GetSpriteRect(m_FIREMODE_KNIFE));
	}
		else if (m_iType==FIREMODE_SENTRYTURRET)
	{
		SPR_Set(gHUD.GetSprite(m_FIREMODE_SENTRYTURRET), r, g, b);
		SPR_DrawAdditive(0, x2, y, &gHUD.GetSpriteRect(m_FIREMODE_SENTRYTURRET));
	}
		else if (m_iType==FIREMODE_MISSILETURRET)
	{
		SPR_Set(gHUD.GetSprite(m_FIREMODE_MISSILETURRET), r, g, b);
		SPR_DrawAdditive(0, x2, y, &gHUD.GetSpriteRect(m_FIREMODE_MISSILETURRET));
	}
		else if (m_iType==FIREMODE_IONTURRET)
	{
		SPR_Set(gHUD.GetSprite(m_FIREMODE_IONTURRET), r, g, b);
		SPR_DrawAdditive(0, x2, y, &gHUD.GetSpriteRect(m_FIREMODE_IONTURRET));
	}
return 1;
}