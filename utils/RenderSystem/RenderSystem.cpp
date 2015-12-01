#include "stdafx.h"
#include <afxdllx.h>
#include "RenderSystem.h"
#include "RenderSystemaw.h"

#ifdef _PSEUDO_DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static AFX_EXTENSION_MODULE RenderSystemDLL = { NULL, NULL };

extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("RENDERSYSTEM.AWX Initializing!\n");

		// Extension DLL one-time initialization
		AfxInitExtensionModule(RenderSystemDLL, hInstance);

		// Insert this DLL into the resource chain
		new CDynLinkLibrary(RenderSystemDLL);

		// Register this custom AppWizard with MFCAPWZ.DLL
		SetCustomAppWizClass(&RenderSystemaw);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("RENDERSYSTEM.AWX Terminating!\n");

		// Terminate the library before destructors are called
		AfxTermExtensionModule(RenderSystemDLL);
	}
	return 1;// ok
}
