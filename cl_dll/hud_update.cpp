#include <math.h>
#include "hud.h"
#include "cl_util.h"
#include <stdlib.h>
#include <memory.h>

int CL_ButtonBits( int );
void CL_ResetButtonBits( int bits );

extern float v_idlescale;
float in_fov;
extern void HUD_SetCmdBits( int bits );

int CHud::UpdateClientData(client_data_t *cdata, float time)
{
	memcpy(m_vecOrigin, cdata->origin, sizeof(vec3_t));
	memcpy(m_vecAngles, cdata->viewangles, sizeof(vec3_t));
	
	m_iKeyBits = CL_ButtonBits( 0 );
	m_iWeaponBits = cdata->iWeaponBits;

	in_fov = cdata->fov;

	Think();

	cdata->fov = m_iFOV;
	
	v_idlescale = m_iConcussionEffect;

	CL_ResetButtonBits( m_iKeyBits );

	// return 1 if in anything in the client_data struct has been changed, 0 otherwise
	return 1;
}


