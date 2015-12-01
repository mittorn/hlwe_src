#if !defined(AFX_RENDERSYSTEMAW_H__3A0142BE_B594_47E7_B4AA_1C1E44B0B8A7__INCLUDED_)
#define AFX_RENDERSYSTEMAW_H__3A0142BE_B594_47E7_B4AA_1C1E44B0B8A7__INCLUDED_

class CDialogChooser;

class CRenderSystemAppWiz : public CCustomAppWiz
{
public:
	virtual CAppWizStepDlg* Next(CAppWizStepDlg* pDlg);
	virtual CAppWizStepDlg* Back(CAppWizStepDlg* pDlg);
		
	virtual void InitCustomAppWiz();
	virtual void ExitCustomAppWiz();
	virtual void CustomizeProject(IBuildProject* pProject);

protected:
	CDialogChooser* m_pChooser;
};

extern CRenderSystemAppWiz RenderSystemaw;

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_RENDERSYSTEMAW_H__3A0142BE_B594_47E7_B4AA_1C1E44B0B8A7__INCLUDED_)
