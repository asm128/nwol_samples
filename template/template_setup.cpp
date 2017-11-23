#include "template.h"
#include "nwol_ascii_display.h"
#include "nwol_sleep.h"

#include <chrono>
#include <thread>

::nwol::error_t												setupInputDevices						(::SApplication& instanceApp)							{ 
	//gbit_set(instanceApp.MainScreenInput.PlatformDetail.DeviceFlagsKeyboard	, ::nwol::WINDOWS_INPUT_STATE_FLAG_Exclusive);
	//gbit_set(instanceApp.MainScreenInput.PlatformDetail.DeviceFlagsMouse	, ::nwol::WINDOWS_INPUT_STATE_FLAG_Exclusive);
	::nwol::setCooperativeLevels(instanceApp.RuntimeValues->Screen.PlatformDetail, instanceApp.MainScreenInput);	// This tells the input system that it has to bind to the main window.
	warn_if(errored(::nwol::acquireInput(instanceApp.MainScreenInput)), "Failed to acquire device. This is normal if the window is not yet focused.");
	return 0; 
}

::nwol::error_t												setupConsole							(::SApplication& instanceApp)							{ 
	::nwol::SCoord2<uint32_t>										& sizeTargetASCII						= instanceApp.GUI.TargetSizeASCII;
	::nwol::asciiDisplayCreate(sizeTargetASCII.x, sizeTargetASCII.y);
	::nwol::asciiTargetCreate(instanceApp.ASCIITarget, sizeTargetASCII);
	char												windowTitle[512]							= {};
	char												moduleTitle[240]							= {};
	uint8_t												moduleTitleLen								= (uint8_t)::nwol::size(moduleTitle);
	uint32_t											moduleVersion								= 0;
	windowTitle[239]								= 0;
	nwol_necall(::nwol_moduleVersion(&moduleVersion), "??");
	nwol_necall(::nwol_moduleTitle(moduleTitle, &moduleTitleLen), "If this fails then something weird is going on.");
	::sprintf_s(windowTitle, "%s v%u.%u", moduleTitle, (moduleVersion & 0xFF00U) >> 8, moduleVersion & 0xFFU);
	::nwol::asciiDisplayTitleSet(windowTitle);
	return 0; 
}

::nwol::error_t												setupGUI								(::SApplication& instanceApp)							{ 
	::nwol::SGUI													& guiSystem								= instanceApp.GUI;
	static const ::nwol::glabel										newControlLabel							= "Exit";
	::nwol::SGUIControl												newControl;

	newControl.AreaASCII										= {1, 1, (int32_t)newControlLabel.size(), 1}	;
	newControl.Text												= newControlLabel								;
	nwol_necall(::nwol::createControl(guiSystem, newControl), "Failed to create control!");

	newControl.AlignArea										= ::nwol::SCREEN_CENTER_TOP;
	newControl.TextColorsASCII.Color.Background					= ::nwol::ASCII_COLOR_INDEX_GREEN;
	newControl.TextColorsASCII.Color.Foreground					= ::nwol::ASCII_COLOR_INDEX_RED;
	newControl.TextColorsASCII.ColorPressed.Background			= ::nwol::ASCII_COLOR_INDEX_BLACK;
	newControl.TextColorsASCII.ColorPressed.Foreground			= ::nwol::ASCII_COLOR_INDEX_WHITE;
	newControl.Text												= "Fuck"					;
	nwol_necall(::nwol::createControl(guiSystem, newControl), "Failed to create control!");
	return 0;
}
