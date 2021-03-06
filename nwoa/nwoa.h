// These sources are best viewed in Visual Studio 2017 with a screen of at least 1920x1080 screen and the zoom set to 81 %.
// Pablo Ariel Zorrilla Cepeda (asm128) Copyright (c) 2010-2017 - Distributed under the MIT License.
#include "application.h"

#ifndef NWOA_H__98326490226436542
#define NWOA_H__98326490226436542

struct SApplicationDetail {	
#if defined(__WINDOWS__)
	::WNDCLASSEX										WindowClass						= {};
	::DWORD												WindowStyle						= WS_OVERLAPPED | WS_THICKFRAME | WS_BORDER | WS_MAXIMIZEBOX | WS_DLGFRAME | WS_SYSMENU | WS_MINIMIZEBOX;
#endif
};

struct SApplication : public ::gddm::SFramework {
	::SApplicationDetail								PlatformDetail					= {};

	inline 												SApplication					(::nwol::SRuntimeValues	* runtimeValues)					: SFramework(runtimeValues)	{
		GUI													= {runtimeValues ? runtimeValues->Screen.Metrics.Size.Cast<uint32_t>() : ::nwol::SCoord2<uint32_t>{320, 240}, ::nwol::SCoord2<uint32_t>{132, 60}};
	}
};

int32_t												setup			(::SApplication	& instanceApp);
int32_t												update			(::SApplication	& instanceApp, bool exitRequested);
int32_t												render			(::SApplication	& instanceApp);
int32_t												cleanup			(::SApplication	& instanceApp);
//																	 
::nwol::error_t										networkEnable	(::SApplication	& instanceApp);
::nwol::error_t										networkDisable	(::SApplication	& instanceApp);

#endif // NWOA_H__98326490226436542
