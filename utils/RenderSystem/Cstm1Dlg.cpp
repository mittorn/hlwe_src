#include "stdafx.h"
#include "RenderSystem.h"
#include "cstm1dlg.h"
#include "RenderSystemaw.h"

#ifdef _PSEUDO_DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CCustom1Dlg, CAppWizStepDlg)
	//{{AFX_MSG_MAP(CCustom1Dlg)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CCustom1Dlg::CCustom1Dlg():CAppWizStepDlg(CCustom1Dlg::IDD)
{
	//{{AFX_DATA_INIT(CCustom1Dlg)
	m_flFnRender = FALSE;
	m_flFnInitializeParticle = FALSE;
	m_flFnUpdate = FALSE;
	m_flArgOrigin = FALSE;
	m_flArgAngles = FALSE;
	m_flArgSprite = FALSE;
	m_flArgRenderMode = FALSE;
	m_flArgLife = FALSE;
	m_flArgBrightness = FALSE;
	m_flArgScale = FALSE;
	m_flArgRenderColor = FALSE;
	//}}AFX_DATA_INIT
}

void CCustom1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CAppWizStepDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCustom1Dlg)
	DDX_Control(pDX, IDC_CHECK11, m_FnInitializeParticle);
	DDX_Check(pDX, IDC_CHECK10, m_flFnRender);
	DDX_Check(pDX, IDC_CHECK11, m_flFnInitializeParticle);
	DDX_Check(pDX, IDC_CHECK9, m_flFnUpdate);
	DDX_Check(pDX, IDC_CHECK3, m_flArgOrigin);
	DDX_Check(pDX, IDC_CHECK8, m_flArgAngles);
	DDX_Check(pDX, IDC_CHECK4, m_flArgSprite);
	DDX_Check(pDX, IDC_CHECK5, m_flArgRenderMode);
	DDX_Check(pDX, IDC_CHECK6, m_flArgLife);
	DDX_Check(pDX, IDC_CHECK2, m_flArgBrightness);
	DDX_Check(pDX, IDC_CHECK7, m_flArgScale);
	DDX_Check(pDX, IDC_CHECK1, m_flArgRenderColor);
	//}}AFX_DATA_MAP
}

// This is called whenever the user presses Next, Back, or Finish with this step
//  present.  Do all validation & data exchange from the dialog in this function.
BOOL CCustom1Dlg::OnDismiss()
{
	if (!UpdateData(TRUE))
		return FALSE;

	if (m_flPartSystem)
	{
		RenderSystemaw.m_Dictionary.SetAt("PARTSYSTEM", "Yes");
		RenderSystemaw.m_Dictionary.SetAt("PREFIX", "PS");
	}
	else
	{
		if (m_flRotatingSystem)
			RenderSystemaw.m_Dictionary.SetAt("ROTSYSTEM", "Yes");

		RenderSystemaw.m_Dictionary.SetAt("PREFIX", "RS");
	}

	if (m_flFnUpdate)
		RenderSystemaw.m_Dictionary.SetAt("FN_UPDATE", "Yes");
	if (m_flFnRender)
		RenderSystemaw.m_Dictionary.SetAt("FN_RENDER", "Yes");
	if (m_flFnInitializeParticle)
		RenderSystemaw.m_Dictionary.SetAt("FN_INITIALIZEPARTICLE", "Yes");

	if (m_flArgOrigin)
		RenderSystemaw.m_Dictionary.SetAt("ARG_ORIGIN", "Yes");
	if (m_flArgAngles)
		RenderSystemaw.m_Dictionary.SetAt("ARG_ANGLES", "Yes");
	if (m_flArgSprite)
		RenderSystemaw.m_Dictionary.SetAt("ARG_SPRITE", "Yes");
	if (m_flArgRenderMode)
		RenderSystemaw.m_Dictionary.SetAt("ARG_RENDERMODE", "Yes");
	if (m_flArgRenderColor)
		RenderSystemaw.m_Dictionary.SetAt("ARG_RENDERCOLOR", "Yes");
	if (m_flArgBrightness)
		RenderSystemaw.m_Dictionary.SetAt("ARG_BRIGHTNESS", "Yes");
	if (m_flArgScale)
		RenderSystemaw.m_Dictionary.SetAt("ARG_SCALE", "Yes");
	if (m_flArgLife)
		RenderSystemaw.m_Dictionary.SetAt("ARG_LIFE", "Yes");

	return TRUE;// return FALSE if the dialog shouldn't be dismissed
}

void CCustom1Dlg::OnRadio1() 
{
	m_flRotatingSystem = FALSE;
	m_flPartSystem = FALSE;
	m_FnInitializeParticle.EnableWindow(FALSE);
}

void CCustom1Dlg::OnRadio2() 
{
	m_flRotatingSystem = TRUE;
	m_flPartSystem = FALSE;
	m_FnInitializeParticle.EnableWindow(FALSE);
}

void CCustom1Dlg::OnRadio3() 
{
	m_flRotatingSystem = FALSE;
	m_flPartSystem = TRUE;
	m_FnInitializeParticle.EnableWindow(TRUE);
}
