#include "stdafx.h"
#include "RenderSystem.h"
#include "cstm2dlg.h"
#include "RenderSystemaw.h"

#ifdef _PSEUDO_DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CCustom2Dlg, CAppWizStepDlg)
	//{{AFX_MSG_MAP(CCustom2Dlg)
	ON_BN_CLICKED(IDC_BUTTON1, OnButtonAdd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CCustom2Dlg::CCustom2Dlg() : CAppWizStepDlg(CCustom2Dlg::IDD)
{
	//{{AFX_DATA_INIT(CCustom2Dlg)
	m_strArgName = _T("");
	m_strArgType = _T("");
	//}}AFX_DATA_INIT
/*	m_ArgTypeList.ResetContent();
	m_ArgTypeList.AddString("byte");
	m_ArgTypeList.AddString("int");
	m_ArgTypeList.AddString("float");
	m_ArgTypeList.AddString("vec3_t");
	m_ArgList.ResetContent();
	m_ArgList.SetColumnWidth(32);*/
}

void CCustom2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CAppWizStepDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCustom2Dlg)
	DDX_Control(pDX, IDC_COMBO1, m_ArgTypeList);
	DDX_Control(pDX, IDC_LIST1, m_ArgList);
	DDX_Text(pDX, IDC_EDIT2, m_strArgName);
	DDX_CBString(pDX, IDC_COMBO1, m_strArgType);
	//}}AFX_DATA_MAP
}

BOOL CCustom2Dlg::OnDismiss()
{
	if (!UpdateData(TRUE))
		return FALSE;

//	for(int i=0; i<m_ArgList.GetCount(); i++)
//	RenderSystemaw.m_Dictionary.SetAt(arg, val);

	return TRUE;// return FALSE if the dialog shouldn't be dismissed
}

void CCustom2Dlg::OnButtonAdd() 
{
	if (!m_strArgName.IsEmpty())
	{
		m_ArgList.AddString(m_strArgName);
	}
}
