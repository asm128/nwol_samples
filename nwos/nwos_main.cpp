#include "nwos.h"

#include "nwol_ascii_display.h"

#include "nwol_runtime_impl.h"

DEFINE_RUNTIME_INTERFACE_FUNCTIONS(SApplication, "No Workflow Overhead Network Server", 0, 1);

::nwol::error_t												cleanup							(::SApplication& instanceApp)								{ 
	instanceApp.NetworkServer.ShutdownServer();
	::gdnet::endpointSystemShutdown();

	::nwol::asciiDisplayDestroy();
	error_if(errored(::nwol::asciiTargetDestroy(instanceApp.ASCIITarget)), "Failed to destroy render target. Not created in the first place?");
	return 0; 
}

::nwol::error_t												serverInit						(::SApplication& instanceApp);
::nwol::error_t												setup							(::SApplication& instanceApp)								{ 
	::nwol::SGUI													& guiSystem						= instanceApp.GUI;
	
	nwol_necall(::nwol::asciiDisplayCreate(guiSystem.TargetSizeASCII.x, guiSystem.TargetSizeASCII.y), "Cannot create our humble display.");
	char															moduleTitle[240]				= {};
	uint8_t															moduleTitleLen					= (uint8_t)::nwol::size(moduleTitle);
	nwol_necall(::nwol_moduleTitle(moduleTitle, &moduleTitleLen), "If this fails then something weird is going on.");
	nwol_necall(::nwol::asciiDisplayTitleSet({moduleTitle, moduleTitleLen}), "Cannot set console title. Unknown reason.");
	nwol_necall(::nwol::asciiTargetCreate(instanceApp.ASCIITarget, guiSystem.TargetSizeASCII), "Out of memory?");

	static const ::nwol::glabel										newControlLabel					= "Exit";
	::nwol::SGUIControl												newControl						= {};
	
	newControl.AreaASCII										= {1, 1, (int32_t)newControlLabel.size(), 1}	;
	newControl.Text												= newControlLabel								;

	nwol_necall(::nwol::createControl(instanceApp.GUI, newControl), "Failed to create GUI control %s.", newControl.Text.c_str());
	nwol_necall(::serverInit(instanceApp), "Failed to initialize server");
	return 0; 
}

#ifndef VK_ESCAPE
#define VK_ESCAPE 0x1B
#endif

::nwol::error_t												update									(::SApplication& instanceApp, bool exitRequested)											{
	if(exitRequested)
		return ::nwol::APPLICATION_STATE_EXIT;

	::nwol::SInput													& inputSystem							= instanceApp.Input;	nwol_necall(::nwol::pollInput(inputSystem), "Failed to poll input device.");
	::nwol::SGUI													& guiSystem								= instanceApp.GUI;		nwol_necall(::nwol::updateGUI(guiSystem, inputSystem), "Failed to update GUI states!");

	::nwol::array_pod<::nwol::CONTROL_STATE>							& controlFlags							= guiSystem.Controls.ControlFlags;
	for(uint32_t iControl = 0, controlCount = controlFlags.size(); iControl < controlCount; ++iControl)
		if(::nwol::bit_true(controlFlags[iControl], ::nwol::CONTROL_STATE_EXECUTE)) {
			info_printf("Execute %u.", iControl);
			switch(iControl) {
			case 0:		return ::nwol::APPLICATION_STATE_EXIT; 
			default:	break;
			}
		}
	return 0; 
}

int32_t														render									(::SApplication& instanceApp)																{ 
	::nwol::SASCIITarget											& target								= instanceApp.ASCIITarget;
	::nwol::asciiTargetClear(target);
	error_if(errored(::nwol::renderGUIASCII(target, instanceApp.GUI)), "%s", "Failed to render ASCII GUI!");
	error_if(errored(::nwol::asciiDisplayPresent(target)), "Failed to present backbuffer.");
	return 0; 
}
