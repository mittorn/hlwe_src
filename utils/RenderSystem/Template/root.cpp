#include "hud.h"
#include "cl_util.h"
$$IF(PARTSYSTEM)
#include "Particle.h"
$$ENDIF
#include "RenderManager.h"
#include "RenderSystem.h"
$$IF(PARTSYSTEM)
#include "PartSystem.h"
$$ENDIF
#include "$$PREFIX$$$$Root$$.h"
$$IF(PARTSYSTEM)
#include "pm_defs.h"
#include "event_api.h"
$$ENDIF
$$IF(FN_RENDER)
#include "triangleapi.h"
#include "studio_util.h"
$$ENDIF

C$$PREFIX$$$$Safe_root$$::C$$PREFIX$$$$Safe_root$$(void)
{
$$IF(PARTSYSTEM)
	CPartSystem();
$$ELSE
$$IF(ROTSYSTEM)
	CRotatingSystem();
$$ELSE
	CRenderSystem();
$$ENDIF
$$ENDIF
}

C$$PREFIX$$$$Safe_root$$::~C$$PREFIX$$$$Safe_root$$(void)
{
	// TODO: clear private data here
	KillSystem();
}

C$$PREFIX$$$$Safe_root$$::C$$PREFIX$$$$Safe_root$$(
$$IF(PARTSYSTEM)
int maxParticles,
$$ENDIF
$$IF(ARG_ORIGIN)
vec3_t origin,
$$ENDIF
$$IF(ARG_ANGLES)
vec3_t angles,
$$ENDIF
$$IF(ROTSYSTEM)
vec3_t anglesdelta,
$$ENDIF
$$IF(ARG_SPRITE)
int sprindex,
$$ENDIF
$$IF(ARG_RENDERMODE)
int r_mode,
$$ENDIF
$$IF(ARG_RENDERCOLOR)
byte r, byte g, byte b,
$$ENDIF
$$IF(ARG_BRIGHTNESS)
float a, float adelta,
$$ENDIF
$$IF(ARG_SCALE)
float scale, float scaledelta,
$$ENDIF
$$IF(ARG_LIFE)
float timetolive
$$ENDIF
)
{
	m_pTexture = NULL;// MUST be before InitTexture()
$$IF(ARG_SPRITE)
	if (!InitTexture(sprindex))
		return;

$$ENDIF
$$IF(ARG_ORIGIN)
	VectorCopy(origin, m_origin);
$$ENDIF
$$IF(ARG_ANGLES)
	VectorCopy(angles, m_angles);
$$ENDIF
$$IF(ROTSYSTEM)
	m_vecAnglesDelta = anglesdelta;
$$ENDIF
$$IF(ARG_COLOR)
	m_color.r = r;
	m_color.g = g;
	m_color.b = b;
$$ENDIF
$$IF(ARG_BRIGHTNESS)
	m_fBrightness = a;
	m_fBrightnessDelta = adelta;
$$ENDIF
$$IF(ARG_SCALE)
	m_fScale = scale;
	m_fScaleDelta = scaledelta;
$$ENDIF

$$IF(PARTSYSTEM)
	m_iMaxParticles = maxParticles;
$$ENDIF
	m_iFollowEntity = -1;
$$IF(ARG_RENDERMODE)
	rendermode = r_mode;
$$ENDIF

$$IF(ARG_LIFE)
	if (timetolive <= 0)// if 0, just display all frames
		m_fDieTime = timetolive;
	else
		m_fDieTime = gEngfuncs.GetClientTime() + timetolive;

$$ENDIF
	InitializeSystem();
}

$$IF(PARTSYSTEM)
$$IF(FN_INITIALIZEPARTICLE)
void C$$PREFIX$$$$Safe_root$$::InitializeParticle(int index)
{
	m_pParticleList[index].m_pos = m_origin;
	m_pParticleList[index].m_prevPos = m_origin;
	m_pParticleList[index].m_accel = vec3_origin;
	m_pParticleList[index].m_velocity = vec3_origin;
	m_pParticleList[index].m_energy = 1.0;
	m_pParticleList[index].m_sizeX = 8.0;
	m_pParticleList[index].m_sizeY = 8.0;

	if (flags & PARTSYSTEM_FLAG_SCALE)
		m_pParticleList[index].m_sizeDelta = 50.0;
	else
		m_pParticleList[index].m_sizeDelta = 0.0;

	m_pParticleList[index].m_texture = m_pTexture;
	m_pParticleList[index].m_frame = 0;
	m_pParticleList[index].m_color[0] = 1.0;
	m_pParticleList[index].m_color[1] = 1.0;
	m_pParticleList[index].m_color[2] = 1.0;
	m_pParticleList[index].m_color[3] = 1.0;
	m_pParticleList[index].m_colorDelta[0] = 0.0;
	m_pParticleList[index].m_colorDelta[1] = 0.0;
	m_pParticleList[index].m_colorDelta[2] = 0.0;
	m_pParticleList[index].m_colorDelta[3] = 0.0;
//	m_pParticleList[index].m_weight = 0;
//	m_pParticleList[index].m_weightDelta = 0;
}
$$ENDIF
$$ENDIF

$$IF(FN_UPDATE)
bool C$$PREFIX$$$$Safe_root$$::Update(float time, double elapsedTime)
{
$$IF(PARTSYSTEM)
	if (m_fDieTime > 0 && m_fDieTime <= time)
		dying = true;

	if (dying)
		return 1;// remove

//	FollowEntity();// update origin according to follow entity
	Emit(1);
	CParticle *curPart = NULL;
	gEngfuncs.pEventAPI->EV_SetTraceHull(2);
	for (int i = 0; i < m_iNumParticles; i++)
	{
		curPart = &m_pParticleList[i];

		if (curPart->m_energy < 0.0)
			m_pParticleList[i] = m_pParticleList[--m_iNumParticles];

		curPart->m_prevPos = curPart->m_pos;
		VectorMA(curPart->m_velocity, elapsedTime, curPart->m_accel, curPart->m_velocity);
		VectorMA(curPart->m_pos, elapsedTime, curPart->m_velocity, curPart->m_pos);

		if (flags & PARTSYSTEM_FLAG_RANDOMFRAME)
			curPart->FrameRandomize();
		else
			curPart->FrameIncrease();

		curPart->m_energy -= 1.5 * elapsedTime;
		curPart->UpdateColor(elapsedTime);
		curPart->UpdateSize(elapsedTime);
//		if (curPart->m_size > 1.0) curPart->m_size = 0.01;
	}
	return 0;
$$ELSE
	bool ret = CRenderSystem::Update(time, elapsedTime);
$$IF(ROTSYSTEM)
	if (ret == 0)
		UpdateAngles(elapsedTime, true);
$$ENDIF
// TODO: update your custom parameters here
	return ret;
$$ENDIF
}
$$ENDIF

$$IF(FN_RENDER)
void C$$PREFIX$$$$Safe_root$$::Render(void)
{
$$IF(PARTSYSTEM)
	if (gHUD.m_iPaused <= 0)
	{
		gEngfuncs.GetViewAngles(m_angles);
//		m_angles = m_angles + ev_punchangle;
	}

	if (flags & PARTSYSTEM_FLAG_ZROTATION)
	{
		m_angles[0] = 0.0;
		m_angles[2] = 0.0;
	}

	vec3_t v_up, v_right;
	AngleVectors(m_angles + ev_punchangle, NULL, v_right, v_up);

	for (int i = 0; i < m_iNumParticles; i++)
	{
		if (m_pParticleList[i].m_energy < 0.0)
			continue;

		if (!PointIsVisible(m_pParticleList[i].m_pos))// faster?
			continue;

		m_pParticleList[i].Render(v_right, v_up, rendermode);
	}
	gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
$$ELSE
	if (!InitTexture(texindex))
		return;

	if (!gEngfuncs.pTriAPI->SpriteTexture(m_pTexture, frame))
		return;

	vec3_t right, up;
	AngleVectors(m_angles, NULL, right, up);

	gEngfuncs.pTriAPI->RenderMode(rendermode);
	gEngfuncs.pTriAPI->Color4ub(m_color.r, m_color.g, m_color.b, 255);
	gEngfuncs.pTriAPI->Brightness(m_fBrightness);
	gEngfuncs.pTriAPI->CullFace(TRI_NONE);
	gEngfuncs.pTriAPI->Begin(TRI_QUADS);

$$IF(ROTSYSTEM)
	gEngfuncs.pTriAPI->TexCoord2f(0,0);
	gEngfuncs.pTriAPI->Vertex3fv(LocalToWorld(m_fScale*m_pTexture->mins[1], m_fScale*m_pTexture->mins[2], 0));// - -
	gEngfuncs.pTriAPI->TexCoord2f(0,1);
	gEngfuncs.pTriAPI->Vertex3fv(LocalToWorld(m_fScale*m_pTexture->mins[1], m_fScale*m_pTexture->maxs[2], 0));// - +
	gEngfuncs.pTriAPI->TexCoord2f(1,1);
	gEngfuncs.pTriAPI->Vertex3fv(LocalToWorld(m_fScale*m_pTexture->maxs[1], m_fScale*m_pTexture->maxs[2], 0));// + +
	gEngfuncs.pTriAPI->TexCoord2f(1,0);
	gEngfuncs.pTriAPI->Vertex3fv(LocalToWorld(m_fScale*m_pTexture->maxs[1], m_fScale*m_pTexture->mins[2], 0));// + -
$$ELSE
	gEngfuncs.pTriAPI->TexCoord2f(0,0);
	gEngfuncs.pTriAPI->Vertex3fv(m_origin + right*m_fScale*m_pTexture->mins[1] + up*m_fScale*m_pTexture->mins[2]);// - -
	gEngfuncs.pTriAPI->TexCoord2f(0,1);
	gEngfuncs.pTriAPI->Vertex3fv(m_origin + right*m_fScale*m_pTexture->mins[1] + up*m_fScale*m_pTexture->maxs[2]);// - +
	gEngfuncs.pTriAPI->TexCoord2f(1,1);
	gEngfuncs.pTriAPI->Vertex3fv(m_origin + right*m_fScale*m_pTexture->maxs[1] + up*m_fScale*m_pTexture->maxs[2]);// + +
	gEngfuncs.pTriAPI->TexCoord2f(1,0);
	gEngfuncs.pTriAPI->Vertex3fv(m_origin + right*m_fScale*m_pTexture->maxs[1] + up*m_fScale*m_pTexture->mins[2]);// + -
$$ENDIF

	gEngfuncs.pTriAPI->End();
	gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
$$ENDIF
}
$$ENDIF
