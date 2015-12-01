#if !defined(AFX_CHOOSER_H__780657A9_6546_4F57_8DD5_E34C981DC623__INCLUDED_)
#define AFX_CHOOSER_H__780657A9_6546_4F57_8DD5_E34C981DC623__INCLUDED_

#define LAST_DLG 1//!!!!!!! 2

class CDialogChooser
{
public:
	CDialogChooser();
	~CDialogChooser();

	// All calls by mfcapwz.dll to CRenderSystemAppWiz::Next
	//  & CRenderSystemAppWiz::Back are delegated to these member
	//  functions, which keep track of what dialog is up
	//  now, and what to pop up next.
	CAppWizStepDlg* Next(CAppWizStepDlg* pDlg);
	CAppWizStepDlg* Back(CAppWizStepDlg* pDlg);

protected:
	// Current step's index into the internal array m_pDlgs
	int m_nCurrDlg;

	// Internal array of pointers to the steps
	CAppWizStepDlg* m_pDlgs[LAST_DLG + 1];
};


//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_CHOOSER_H__780657A9_6546_4F57_8DD5_E34C981DC623__INCLUDED_)
