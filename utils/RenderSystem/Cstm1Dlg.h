#if !defined(AFX_CSTM1DLG_H__DB86D14B_4BE6_45E2_A79A_B384BD1D561E__INCLUDED_)
#define AFX_CSTM1DLG_H__DB86D14B_4BE6_45E2_A79A_B384BD1D561E__INCLUDED_

class CCustom1Dlg : public CAppWizStepDlg
{
public:
	CCustom1Dlg();
	virtual BOOL OnDismiss();

	//{{AFX_DATA(CCustom1Dlg)
	enum { IDD = IDD_CUSTOM1 };
	CButton	m_FnInitializeParticle;
	BOOL	m_flFnRender;
	BOOL	m_flFnInitializeParticle;
	BOOL	m_flFnUpdate;
	BOOL	m_flArgOrigin;
	BOOL	m_flArgAngles;
	BOOL	m_flArgSprite;
	BOOL	m_flArgRenderMode;
	BOOL	m_flArgLife;
	BOOL	m_flArgBrightness;
	BOOL	m_flArgScale;
	BOOL	m_flArgRenderColor;
	//}}AFX_DATA

	BOOL m_flPartSystem;
	BOOL m_flRotatingSystem;

// Overrides
	//{{AFX_VIRTUAL(CCustom1Dlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCustom1Dlg)
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnRadio3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_CSTM1DLG_H__DB86D14B_4BE6_45E2_A79A_B384BD1D561E__INCLUDED_)
