#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "r_efx.h"
#include "rain.h"
#include "event_api.h"

#define MAX_CLIENTS 32

int CHud :: MsgFunc_ResetHUD(const char *pszName, int iSize, void *pbuf )
{
	ASSERT( iSize == 0 );

	// clear all hud data
	HUDLIST *pList = m_pHudList;

	while ( pList )
	{
		if ( pList->p )
			pList->p->Reset();
		pList = pList->pNext;
	}

	// reset sensitivity
	m_flMouseSensitivity = 0;

	// reset concussion effect
	m_iConcussionEffect = 0;

	return 1;
}

void CAM_ToFirstPerson(void);

void CHud :: MsgFunc_ViewMode( const char *pszName, int iSize, void *pbuf )
{
	CAM_ToFirstPerson();
}

void CHud :: MsgFunc_InitHUD( const char *pszName, int iSize, void *pbuf )
{
	// prepare all hud data
	HUDLIST *pList = m_pHudList;

	while (pList)
	{
		if ( pList->p )
			pList->p->InitHUDData();
		pList = pList->pNext;
	}
}

int CHud :: MsgFunc_GameMode(const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	m_Teamplay = READ_BYTE();

	return 1;
}

int CHud :: MsgFunc_Damage(const char *pszName, int iSize, void *pbuf )
{
	int		armor, blood;
	Vector	from;
	int		i;
	float	count;
	
	BEGIN_READ( pbuf, iSize );
	armor = READ_BYTE();
	blood = READ_BYTE();

	for (i=0 ; i<3 ; i++)
		from[i] = READ_COORD();

	count = (blood * 0.5) + (armor * 0.5);

	if (count < 10)
		count = 10;

	// TODO: kick viewangles,  show damage visually

	return 1;
}

// buz: special tank message
int CHud :: MsgFunc_SpecTank( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	
	m_SpecTank_on = READ_BYTE();
	if (m_SpecTank_on == 0) // turn off
		return 1;
	else if (m_SpecTank_on == 2) // only ammo update
	{
		m_SpecTank_Ammo = READ_LONG();
		m_SpecTank_Ammo2 = READ_LONG();
		m_SpecTank_CrossSize = READ_BYTE();
	}
	else // turn on
	{
		m_SpecTank_point.x = READ_COORD();
		m_SpecTank_point.y = READ_COORD();
		m_SpecTank_point.z = READ_COORD();
		m_SpecTank_defYaw = READ_COORD();
		m_SpecTank_coneHor = READ_COORD();
		m_SpecTank_coneVer = READ_COORD();
		m_SpecTank_distFwd = READ_COORD();
		m_SpecTank_distUp = READ_COORD();
		m_SpecTank_Ammo = READ_LONG();
		m_SpecTank_Ammo2 = READ_LONG();
		m_SpecTank_CrossSize = READ_BYTE();
	}
	return 1;
}
