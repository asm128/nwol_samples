#include "template.h"

#include "nwol_sleep.h"
#include "nwol_runtime_impl.h"
#include "nwol_ascii_display.h"

#if defined(__WINDOWS__)
using ::nwol::min;
using ::nwol::max;
#pragma warning(disable:4458)
#include <objidl.h>
#include <gdiplus.h>

#pragma comment (lib,"Gdiplus.lib")
#pragma warning(default:4458)
#endif // __WINDOWS__

DEFINE_RUNTIME_INTERFACE_FUNCTIONS(::SApplication, "Template Application", 0, 1);

void														repaint									(::HDC hdc, const ::nwol::SCoord2<int16_t>& size)		{
	::Gdiplus::Graphics												graphics								= {hdc};
	::Gdiplus::Pen													pen										= {::Gdiplus::Color(255, 0, 0, 255)};
	graphics.DrawLine(&pen, 0, 0, size.x, size.y);
	::Gdiplus::SolidBrush											blackBrush								= {::Gdiplus::Color(255, 0, 0, 0)};
	::Gdiplus::SolidBrush											gBrush									= {::Gdiplus::Color(255, 0, 255, 0)};
	::Gdiplus::Font													myFont									= {L"Comic Sans MS", 16};
	graphics.DrawString(L"Fuck again", -1, &myFont, {10, 20}, &gBrush);
}

int32_t														gameUpdate								(::SGame game)											{ game; return 0; }
int32_t														gameRender								(::SGame game)											{ game; return 0; }
int32_t														gameSetup								(::SGame game)											{ game; return 0; }
int32_t														gameCleanup								(::SGame game)											{ game; return 0; }

::nwol::error_t												cleanup									(::SApplication& instanceApp)							{ 
	::nwol::asciiDisplayDestroy();
	::nwol::asciiTargetDestroy(instanceApp.ASCIITarget);
	::gameCleanup(instanceApp.Game);
	::Gdiplus::GdiplusShutdown(instanceApp.GDIToken);
	return 0; 
}

::nwol::error_t												setupGUI								(::SApplication& instanceApp);
::nwol::error_t												setupConsole							(::SApplication& instanceApp);
::nwol::error_t												setupInputDevices						(::SApplication& instanceApp);
::nwol::error_t												setup									(::SApplication& instanceApp)							{ 
	nwol_necall(::setupConsole		(instanceApp), "Failed to setup application console."	);
	nwol_necall(::setupInputDevices	(instanceApp), "Failed to setup application input."		);
	nwol_necall(::setupGUI			(instanceApp), "Failed to setup application GUI."		);
	::Gdiplus::GdiplusStartupInput										gdiplusStartupInput;
	// Initialize GDI+.
	::Gdiplus::GdiplusStartup(&instanceApp.GDIToken, &gdiplusStartupInput, 0);
	//::nwol::graphicsInitialize(instanceApp.Graphics);
	//::nwol::graphicsAttach(instanceApp.Graphics, &instanceApp.RuntimeValues->Screen);
	::gameSetup(instanceApp.Game);
	return 0; 
}

::nwol::error_t												updateDevices							(::SApplication& instanceApp);
::nwol::error_t												updateEvents							(::SApplication& instanceApp);
::nwol::error_t												update									(::SApplication& instanceApp, bool exitRequested)		{
	if(exitRequested)
		return ::nwol::APPLICATION_STATE_EXIT;

	nwol_necall(::updateDevices(instanceApp), "Not sure why this would fail.");
	error_if(errored(::nwol::updateGUI(instanceApp.GUI, instanceApp.Input)), "The application failed to update the Graphical User Interface.");
	::nwol::error_t													updateResult;
	nwol_necall(updateResult = ::updateEvents(instanceApp), "Now idea why this would fail. Our code doesn't fail.");
	if(updateResult == ::nwol::APPLICATION_STATE_EXIT)
		return ::nwol::APPLICATION_STATE_EXIT;
	{
		::nwol::CMutexGuard												lockConsole								(instanceApp.ConsoleMutex);
		::nwol::asciiDisplayPresent(instanceApp.ASCIITarget);
	}
	::gameUpdate(instanceApp.Game);

	if(instanceApp.RuntimeValues->Screen.State.Repaint) {
		::HDC															dc										= ::GetDC(instanceApp.RuntimeValues->Screen.PlatformDetail.hWnd); // esto es de windows
		::repaint(dc, instanceApp.RuntimeValues->Screen.Metrics.Size); // esto no
		instanceApp.RuntimeValues->Screen.State.Repaint = false;
	}
	return 0; 
}

::nwol::error_t												render									(::SApplication& instanceApp)							{ 
	{
		::nwol::CMutexGuard												lockConsole								(instanceApp.ConsoleMutex);
		::nwol::SASCIITarget											& target								= instanceApp.ASCIITarget;
		::nwol::asciiTargetClear(target);
		error_if(errored(::nwol::renderGUIASCII(target, instanceApp.GUI)), "%s", "Failed to render ASCII GUI!")
	}
	::gameRender(instanceApp.Game);
	return 0; 
}
