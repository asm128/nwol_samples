#include "nwoa.h"

#include "text.h"
#include "ascii_screen.h"
#include "gui.h"

#include "runtime_impl.h"

#include <thread>
#include <process.h>

DEFINE_RUNTIME_INTERFACE_FUNCTIONS(SApplication, "Quatris", 0, 1);

#define errmsg(functionCall, format, ...)																	\
	{																										\
		::nwol::error_t							_err_errMy		= 0;										\
		error_if(errored(_err_errMy = functionCall), "Error code: 0x%x. " format, _err_errMy, __VA_ARGS__);	\
	}

int32_t										cleanup							(::SApplication& instanceApp)							{ 
	::nwol::shutdownASCIIScreen();

	::networkDisable(instanceApp);

	return 0; 
}

::nwol::error_t								setup							(::SApplication& instanceApp)							{ 
	::nwol::SGUI									& guiSystem						= instanceApp.GUI;

	::nwol::initASCIIScreen(guiSystem.TargetSizeASCII.x, guiSystem.TargetSizeASCII.y);
	::nwol::setASCIIScreenTitle(appTitle());

	errmsg(::networkEnable(instanceApp), "Failed to enable network.");

	guiSystem.TargetSizeASCII.x						= nwol::getASCIIBackBufferWidth		();
	guiSystem.TargetSizeASCII.y						= nwol::getASCIIBackBufferHeight	();

	static const ::nwol::glabel						newControlLabel					= "Exit";
	::nwol::SGUIControl								newControl;

	newControl.AreaASCII						= {1, 1, (int32_t)newControlLabel.size(), 1}	;
	newControl.Text								= newControlLabel								;

	::nwol::createControl(guiSystem, newControl);

	instanceApp.Game.Board.Cells	[0]			= 10;

	return 0; 
}

#ifndef VK_ESCAPE
#define VK_ESCAPE 0x1B
#endif


::nwol::error_t								update							(::SApplication& instanceApp, bool exitRequested)		{
	if(exitRequested)
		return ::nwol::APPLICATION_STATE_EXIT;

	::nwol::SInput									& inputSystem					= instanceApp.Input;
	::nwol::pollInput(inputSystem);
	//if(inputSystem.Keys[VK_ESCAPE])
	//	return ::nwol::APPLICATION_STATE_EXIT;

	::nwol::SGUI									& guiSystem						= instanceApp.GUI;
	::nwol::updateGUI(guiSystem, inputSystem);

	::nwol::array_pod<::nwol::CONTROL_FLAG>			& controlFlags					= guiSystem.Controls.ControlFlags;
	for(uint32_t iControl = 0, controlCount = controlFlags.size(); iControl < controlCount; ++iControl)
		if(::nwol::bit_true(controlFlags[iControl], ::nwol::CONTROL_FLAG_EXECUTE)) {
			debug_printf("Execute control %u.", iControl);
			switch(iControl) {
			case 0:		
				return ::nwol::APPLICATION_STATE_EXIT; 
				break;
			default:
				break;
			}
		}
	return 0; 
}

::nwol::error_t								render							(::SApplication& instanceApp)							{ 
	::nwol::clearASCIIBackBuffer(' ', COLOR_WHITE);

	::nwol::SASCIITarget							target;
	::nwol::error_t									errMy							= ::nwol::getASCIIBackBuffer(target);
	reterr_error_if_errored(errMy, "%s", "Failed to get ASCII target!");
	
	::nwol::renderGUIASCII(target, instanceApp.GUI);

	::nwol::presentASCIIBackBuffer();
	
	return 0; 
}
