#include "nwoa.h"

#include "nwol_ascii_display.h"
#include "gui.h"

#include "nwol_runtime_impl.h"
#include "nwol_debug.h"

#include <thread>
#include <process.h>
#include <ctime>

#include <algorithm>

DEFINE_RUNTIME_INTERFACE_FUNCTIONS(SApplication, "ASCII Reference", 0, 1);

				int32_t						setupGUI							(::SApplication& instanceApp)																			{ 
    info_printf("%s", "Initializing Graphical User Interface.");
	::nwol::SGUI									& guiSystem							= instanceApp.GUI;

	// Create exit button
	static const ::nwol::glabel						labelExit							= "Exit";
	::nwol::SGUIControl								newControl;
	newControl.AreaASCII						= {{1, 1}, {(int32_t)labelExit.size(), 1}}	;
	newControl.Text								= labelExit									;
	nwol_necall(::nwol::createControl(guiSystem, newControl), "%s: \"%s\".", "Failed to create control", labelExit.begin());

	// Create game board buttons: each board cell is a button.
	newControl.AlignArea						= (::nwol::ALIGN_SCREEN)(::nwol::SCREEN_CENTER | ::nwol::SCREEN_VCENTER)		;

	char blech[]	= "   ";
	char blechHex[] = " 0x  ";
	
	static constexpr const char						GRID_WIDTH							= 32;
	static constexpr const char						GRID_HEIGHT							= 8;
	
	static constexpr const char						cell_width_symbols					= 3;
	newControl.AreaASCII.Size					= {cell_width_symbols, 1}																	;
	::nwol::SControlTextColorASCII					& colorsConsole						= newControl.TextColorsASCII						;
	colorsConsole.Color.Foreground				= COLOR_BLACK																				;
	colorsConsole.ColorPressed					= {COLOR_DARKGREY, COLOR_YELLOW}															;
	for(int32_t y = 0; y < GRID_HEIGHT; ++y) 
		for(int32_t x = 0; x < GRID_WIDTH; ++x) {
			const int32_t									linearIndex							= y * GRID_WIDTH + x;
			newControl.AreaASCII.Offset					= {-((GRID_WIDTH / 2) * cell_width_symbols) + x * cell_width_symbols, -16 + y}				;
			blech[1]									= char(linearIndex)																			;
			newControl.Text								= blech																						;		
			colorsConsole.Color.Background				= ((y * cell_width_symbols + x) % 2) ? COLOR_DARKGREY : COLOR_WHITE							;
			//newControl.AlignText						= ::nwol::SCREEN_CENTER												this is not working yet
			error_if(errored(::nwol::createControl(guiSystem, newControl)), "%s", "Failed to create control.");
		}

	static constexpr const char						cell_width_codes					= 5;
	newControl.AreaASCII.Size					= {cell_width_codes, 1};
	for(int32_t y = 0; y < GRID_HEIGHT; ++y) 
		for(int32_t x = 0; x < GRID_WIDTH; ++x) {
			const int32_t									linearIndex							= y * GRID_WIDTH + x;
			newControl.AreaASCII.Offset					= {-((GRID_WIDTH / 2) * cell_width_codes) + x * cell_width_codes, 0 + y};
			newControl.Text								= " " + ::std::to_string(linearIndex)													;
			colorsConsole.Color.Background				= ((y * cell_width_codes + x) % 2) ? COLOR_DARKGREY : COLOR_WHITE						;
			//newControl.AlignText						= ::nwol::SCREEN_CENTER												this is not working yet
			error_if(errored(::nwol::createControl(guiSystem, newControl)), "%s", "Failed to create control.");
		}

	static constexpr const char						cell_width_hex					= 6;
	newControl.AreaASCII.Size					= {cell_width_hex, 1};
	for(int32_t y = 0; y < GRID_HEIGHT; ++y) 
		for(int32_t x = 0; x < GRID_WIDTH; ++x) {
			const int32_t									linearIndex							= y * GRID_WIDTH + x;
			newControl.AreaASCII.Offset					= {-((GRID_WIDTH / 2) * cell_width_hex) + x * cell_width_hex, 16 + y};
			sprintf_s(&blechHex[3], 3, "%x", linearIndex);
			newControl.Text								= blechHex;
			colorsConsole.Color.Background				= ((y * 5 + x) % 2) ? COLOR_DARKGREY : COLOR_WHITE						;
			//newControl.AlignText						= ::nwol::SCREEN_CENTER												this is not working yet
			error_if(errored(::nwol::createControl(guiSystem, newControl)), "%s", "Failed to create control.");
		}

	// Create restart button
	static const ::nwol::glabel						labelRestart						= "Restart";
	newControl									= {}																	;
	newControl.AlignArea						= (::nwol::ALIGN_SCREEN)(::nwol::SCREEN_CENTER | ::nwol::SCREEN_BOTTOM)	;
	newControl.AreaASCII						= {{0, 1}, {(int32_t)labelRestart.size(), 1}}							;
	newControl.Text								= labelRestart															;

	nwol_necall(::nwol::createControl(guiSystem, newControl), "%s: \"%s\".", "Failed to create control", labelRestart.begin());
    info_printf("%s", "Done initializing GUI.");
	return 0;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
				int32_t						cleanup								(::SApplication& instanceApp)																		{
    info_printf("%s", "Cleaning up resources...");
	::nwol::asciiDisplayDestroy();
	::nwol::asciiTargetDestroy(instanceApp.ASCIITarget);
	info_printf("%s", "Done cleaning up resources.");
	return 0; 
}

				int32_t						setup								(::SApplication& instanceApp)																			{ 
    info_printf("%s", "Initializing application...");
	srand((unsigned int)time(0));

	::nwol::SGUI									& guiSystem							= instanceApp.GUI;
	nwol_necall(::nwol::asciiDisplayCreate(guiSystem.TargetSizeASCII), "Failed to crete our humble ASCII display.");
	nwol_necall(::nwol::asciiTargetCreate(instanceApp.ASCIITarget, guiSystem.TargetSizeASCII), "Failed to crete our humble ASCII display.");
	char												windowTitle[512]							= {};
	char												moduleTitle[240]							= {};
	uint8_t												moduleTitleLen								= (uint8_t)::nwol::size(moduleTitle);
	uint32_t											moduleVersion								= 0;
	windowTitle[239]								= 0;
	nwol_necall(::nwol_moduleVersion(&moduleVersion), "??");
	nwol_necall(::nwol_moduleTitle(moduleTitle, &moduleTitleLen), "If this fails then something weird is going on.");
	sprintf_s(windowTitle, "%s v%u.%u", moduleTitle, (moduleVersion & 0xFF00U) >> 8, moduleVersion & 0xFFU);
	::nwol::asciiDisplayTitleSet(windowTitle);

	nwol_necall(::setupGUI(instanceApp), "%s", "Failed to set up GUI."			);

    info_printf("%s", "Done initializing application.");
	return 0; 
}

#if defined(__WINDOWS__)
#	ifndef VK_ESCAPE
#		define VK_ESCAPE 0x1B
#	endif
#else 
#	error "Unsupported platform. Define key code for this platform in order to continue."
#endif

				int32_t						update								(::SApplication& instanceApp, bool exitRequested)														{
	verbose_printf("%s", "Enter.");
	if(exitRequested)
		return ::nwol::APPLICATION_STATE_EXIT;

	// Update subsystems
	::nwol::SInput									& inputSystem						= instanceApp.Input;	::nwol::pollInput(inputSystem);
	::nwol::SGUI									& guiSystem							= instanceApp.GUI;		::nwol::updateGUI(guiSystem, inputSystem);
	::nwol::SGUIControlTable						& guiControls						= guiSystem.Controls;	

	::nwol::array_pod<::nwol::CONTROL_STATE>		& controlFlags						= guiControls.ControlFlags;	// 

	bool											handledControlEvent					= false;
	int32_t											otherControlIndex, otherControlIndexHex;
	for(uint32_t iControl = 0, controlCount = controlFlags.size(); iControl < controlCount; ++iControl) {
		if(::nwol::bit_true(controlFlags[iControl], ::nwol::CONTROL_STATE_EXECUTE)) {
			info_printf("Execute %u.", iControl);
			switch(iControl) {
			case 0:		return ::nwol::APPLICATION_STATE_EXIT; // Exit button clicked.
			default:	// Board cell clicked 
				for(int32_t y = 0; y < 8; ++y) 
					for(int32_t x = 0; x < 32; ++x) {
						int32_t											iOtherControl						= y * 32 + x + 1;
						guiControls.TextColorsASCII[iOtherControl+256].Color.Foreground	= guiControls.TextColorsASCII[iOtherControl].Color.Foreground	= COLOR_BLACK;
						guiControls.TextColorsASCII[iOtherControl+256].Color.Background	= guiControls.TextColorsASCII[iOtherControl].Color.Background	= ((y * 5 + x) % 2) ? COLOR_DARKGREY : COLOR_WHITE;
						guiControls.TextColorsASCII[iOtherControl+512].Color.Foreground	= guiControls.TextColorsASCII[iOtherControl].Color.Foreground	= COLOR_BLACK;
						guiControls.TextColorsASCII[iOtherControl+512].Color.Background	= guiControls.TextColorsASCII[iOtherControl].Color.Background	= ((y * 5 + x) % 2) ? COLOR_DARKGREY : COLOR_WHITE;
					}

				 if(iControl > 512) {
					otherControlIndex																	= iControl - 256;
					otherControlIndexHex																= iControl - 512;
				}
				else if(iControl > 256) {
					otherControlIndex																	= iControl + 256;
					otherControlIndexHex																= iControl - 256;
				}
				else {
					otherControlIndex																	= iControl + 256;
					otherControlIndexHex																= iControl + 512;
				}
				guiControls.TextColorsASCII[otherControlIndex]		.Color	.Foreground					= COLOR_DARKCYAN;
				guiControls.TextColorsASCII[otherControlIndexHex]	.Color	.Foreground					= COLOR_DARKCYAN;
				guiControls.TextColorsASCII[iControl]				.Color	.Foreground					= COLOR_DARKCYAN;
				guiControls.TextColorsASCII[otherControlIndex]		.Color	.Background					= COLOR_YELLOW;
				guiControls.TextColorsASCII[otherControlIndexHex]	.Color	.Background					= COLOR_YELLOW;
				guiControls.TextColorsASCII[iControl]				.Color	.Background					= COLOR_YELLOW;
				handledControlEvent							= true;
				break;
			}
		}
		if(handledControlEvent)
			break;
	}

	return 0; 
}

				int32_t						render							(::SApplication& instanceApp)																				{
    verbose_printf("%s", "Enter.");
	::nwol::SASCIITarget							& actualScreen					= instanceApp.ASCIITarget;
	::nwol::asciiTargetClear(actualScreen);
	::nwol::renderGUIASCII(actualScreen, instanceApp.GUI);
	::nwol::asciiDisplayPresent(actualScreen);
	verbose_printf("%s", "Exit.");
	return 0; 
}
