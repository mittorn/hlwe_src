#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include <string.h>
#include <stdio.h>
#include "game_fx.h"

#define MSGMGRMAXSIZE  5
void safeEndRead(int);

// HUD Icons Message Manager
#define MSG_BUYZONE		1
#define MSG_MONEY		2
#define MSG_CLOAK		3	
#define MSG_PHK			4	
#define MSG_PHEV		5	
#define MSG_ANTIGRAV		6	
#define MSG_LONGJUMP		7	
#define MSG_PSHIELD		8	
#define MSG_PT			9	
#define MSG_TURRETSENTRY	10	
#define MSG_TURRETMISSILE	11	
#define MSG_TURRETION		12	
#define MSG_KEVLAR		13	
#define MSG_HARMOR		14	
#define MSG_BATTERY		15	
#define MSG_TRAIN		16	
#define MSG_WARHEAD		17	
#define MSG_BODY		18	
#define MSG_SKIN		19	
#define MSG_SCOPE		20
#define MSG_FIREMODE		21
#define MSG_CONCUSS		22
#define MSG_FLASHLIGHT		23
#define MSG_FLASHBATTERY	24
#define MSG_HEALTH		25
#define MSG_TIMER		26
#define MSG_GAMETITLE		27
#define MSG_FOV			28

int __MsgFunc_MSGManager(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );
	int delta = 0;
	delta = (int)pbuf;
	int cmd = READ_BYTE();
	int res = 0;
	switch(cmd)
	{
		case MSG_BUYZONE:
				res = gHUD.m_BuyZone.MsgFunc_BuyZone(pszName,iSize,pbuf);
			break;
		case MSG_MONEY:
				res = gHUD.m_Money.MsgFunc_Money(pszName,iSize,pbuf);
			break;
		case MSG_CLOAK: 
				res = gHUD.m_Cloak.MsgFunc_Cloak(pszName,iSize,pbuf);
			break;
		case MSG_PHK: 
				res = gHUD.m_PHK.MsgFunc_PHK(pszName,iSize,pbuf);
			break;
		case MSG_PHEV: 
				res = gHUD.m_PHEV.MsgFunc_PHEV(pszName,iSize,pbuf);
			break;
		case MSG_ANTIGRAV:
				res = gHUD.m_Antigrav.MsgFunc_Antigrav(pszName,iSize,pbuf);
			break;
		case MSG_LONGJUMP:
				res = gHUD.m_Longjump.MsgFunc_Longjump(pszName,iSize,pbuf);
			break;
		case MSG_PSHIELD:
				res = gHUD.m_Pshield.MsgFunc_Pshield(pszName,iSize,pbuf);
			break;
		case MSG_PT:
				res = gHUD.m_Pt.MsgFunc_Pt(pszName,iSize,pbuf);
			break;
		case MSG_TURRETSENTRY:
				res = gHUD.m_TurretSentry.MsgFunc_TurretS(pszName,iSize,pbuf);
			break;
		case MSG_TURRETMISSILE:
				res = gHUD.m_TurretMissile.MsgFunc_TurretM(pszName,iSize,pbuf);
			break;
		case MSG_TURRETION:
				res = gHUD.m_TurretIon.MsgFunc_TurretI(pszName,iSize,pbuf);
			break;
		case MSG_HARMOR:
				res = gHUD.m_Harmor.MsgFunc_Harmor(pszName,iSize,pbuf);
			break;
		case MSG_KEVLAR:
				res = gHUD.m_Kevlar.MsgFunc_Kevlar(pszName,iSize,pbuf);
			break;
		case MSG_BATTERY:
				res = gHUD.m_Battery.MsgFunc_Battery(pszName,iSize,pbuf);
			break;
		case MSG_TRAIN:
				res = gHUD.m_Train.MsgFunc_Train(pszName,iSize,pbuf);
			break;
		case MSG_WARHEAD:
				res = gHUD.m_Warhead.MsgFunc_Warhead(pszName,iSize,pbuf);
			break;
		case MSG_SKIN:
				res = __MsgFunc_SetSkin(pszName,iSize,pbuf);
			break;
		case MSG_BODY:
				res = __MsgFunc_SetBody(pszName,iSize,pbuf);
			break;
		case MSG_SCOPE:
				res = gHUD.m_Scope.MsgFunc_Scope(pszName,iSize,pbuf);
			break;
		case MSG_FIREMODE:
				res = gHUD.m_Firemode.MsgFunc_Firemode(pszName,iSize,pbuf);
			break;
		case MSG_CONCUSS:
				res = __MsgFunc_Concuss(pszName,iSize,pbuf);
			break;
		case MSG_FLASHLIGHT:
				res = gHUD.m_Flash.MsgFunc_Flashlight(pszName,iSize,pbuf);
			break;
		case MSG_FLASHBATTERY:
				res = gHUD.m_Flash.MsgFunc_FlashBat(pszName,iSize,pbuf);
			break;
		case MSG_HEALTH:
				res = gHUD.m_Health.MsgFunc_Health(pszName,iSize,pbuf);
			break;
		case MSG_TIMER:
				res = gHUD.m_Timer.MsgFunc_Timer(pszName,iSize,pbuf);
			break;

		case MSG_GAMETITLE:
				res = gHUD.m_Message.MsgFunc_GameTitle(pszName,iSize,pbuf);
			break;

		case MSG_FOV:
				res = gHUD.MsgFunc_SetFOV(pszName,iSize,pbuf);
			break;

//	return gHUD.MsgFunc_SetFOV( pszName, iSize, pbuf );

		default:
			break;
	}
	safeEndRead(delta);
	return res;
}
void safeEndRead(int i)
{
	for( int j = i;j<MSGMGRMAXSIZE;j++ ) READ_BYTE();
	END_READ("safeEndRead");
}

void initMsgManager()
{
	HOOK_MESSAGE(MSGManager);
}