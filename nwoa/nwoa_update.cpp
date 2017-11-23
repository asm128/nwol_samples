#include "nwoa.h"

::nwol::error_t												updateDevices							(::SApplication& instanceApp)									{
	::nwol::SInput													& consoleInputSystem					= instanceApp.Input;
	::nwol::SDisplayInput											& mainScreeninputSystem					= instanceApp.MainScreenInput;
	error_if(errored(::nwol::pollInput(consoleInputSystem)														), "The application failed to poll user input devices.");
	error_if(errored(::nwol::pollInput(mainScreeninputSystem, instanceApp.RuntimeValues->Screen.PlatformDetail)	), "The application failed to poll user input devices.");
	return 0;
}

::nwol::error_t												updateEvents							(::SApplication& instanceApp)									{
	::nwol::array_pod<::nwol::CONTROL_STATE>							& controlFlags							= instanceApp.GUI.Controls.ControlFlags;
	for(uint32_t iControl = 0, controlCount = controlFlags.size(); iControl < controlCount; ++iControl)
		if gbit_true(controlFlags[iControl], ::nwol::CONTROL_STATE_EXECUTE) {
			info_printf("Execute control %u.", iControl);
			switch(iControl) {
			case 0	:	return ::nwol::APPLICATION_STATE_EXIT;	
			default	:	warning_printf("Unhandled control id: %u.", iControl);	break;
			}
		}
	return 0;
}
