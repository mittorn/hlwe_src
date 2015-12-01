#if !defined(AFX_CSTM2DLG_H__5AF6AB07_E82F_4E4A_8D55_1B52C9AD8579__INCLUDED_)
#define AFX_CSTM2DLG_H__5AF6AB07_E82F_4E4A_8D55_1B52C9AD8579__INCLUDED_

class CCustom2Dlg : public CAppWizStepDlg
{
public:
	CCustom2Dlg();
	virtual BOOL OnDismiss();

	//{{AFX_DATA(CCustom2Dlg)
	enum { IDD = IDD_CUSTOM2 };
	CComboBox	m_ArgTypeList;
	CListBox	m_ArgList;
	CString	m_strArgName;
	CString	m_strArgType;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CCustom2Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CCustom2Dlg)
	afx_msg void OnButtonAdd();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_CSTM2DLG_H__5AF6AB07_E82F_4E4A_8D55_1B52C9AD8579__INCLUDED_)
