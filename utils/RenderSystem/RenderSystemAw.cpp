#include "stdafx.h"
#include "RenderSystem.h"
#include "RenderSystemaw.h"
#include "chooser.h"

#ifdef _PSEUDO_DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CRenderSystemAppWiz::InitCustomAppWiz()
{
	m_pChooser = new CDialogChooser;
	SetNumberOfSteps(LAST_DLG);
}

void CRenderSystemAppWiz::ExitCustomAppWiz()
{
	ASSERT(m_pChooser != NULL);
	delete m_pChooser;
	m_pChooser = NULL;
}

CAppWizStepDlg* CRenderSystemAppWiz::Next(CAppWizStepDlg* pDlg)
{
	return m_pChooser->Next(pDlg);
}

CAppWizStepDlg* CRenderSystemAppWiz::Back(CAppWizStepDlg* pDlg)
{
	return m_pChooser->Back(pDlg);
}

void CRenderSystemAppWiz::CustomizeProject(IBuildProject* pProject)
{
	// TODO: Add code here to customize the project.  If you don't wish
	//  to customize project, you may remove this virtual override.

	// This is called immediately after the default Debug and Release
	//  configurations have been created for each platform.  You may customize
	//  existing configurations on this project by using the methods
	//  of IBuildProject and IConfiguration such as AddToolSettings,
	//  RemoveToolSettings, and AddCustomBuildStep. These are documented in
	//  the Developer Studio object model documentation.

	// WARNING!!  IBuildProject and all interfaces you can get from it are OLE
	//  COM interfaces.  You must be careful to release all new interfaces
	//  you acquire.  In accordance with the standard rules of COM, you must
	//  NOT release pProject, unless you explicitly AddRef it, since pProject
	//  is passed as an "in" parameter to this function.  See the documentation
	//  on CCustomAppWiz::CustomizeProject for more information.
}

CRenderSystemAppWiz RenderSystemaw;

