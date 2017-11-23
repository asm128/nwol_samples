// These sources are best viewed in Visual Studio 2017 with a screen of at least 1920x1080 screen and the zoom set to 81 %.
// Pablo Ariel Zorrilla Cepeda (asm128) Copyright (c) 2010-2017 - Distributed under the MIT License.
#include "application.h"
#include "nwol_display_graphics.h"

#ifndef NWOA_H__98326490226436542
#define NWOA_H__98326490226436542

struct SGame {
	int a;
};

struct SApplication : public ::gddm::SFramework {
	SGame												Game							= {};
	ULONG_PTR											GDIToken						= 0;
	::nwol::SGraphics									Graphics						= {};

	inline 												SApplication					(::nwol::SRuntimeValues	* runtimeValues)					: SFramework(runtimeValues)	{
		GUI													= {runtimeValues ? runtimeValues->Screen.Metrics.Size.Cast<uint32_t>() : ::nwol::SCoord2<uint32_t>{320, 240}, ::nwol::SCoord2<uint32_t>{132, 60}};
	}
};

int32_t												setup			(::SApplication	& instanceApp);
int32_t												update			(::SApplication	& instanceApp, bool exitRequested);
int32_t												render			(::SApplication	& instanceApp);
int32_t												cleanup			(::SApplication	& instanceApp);

#endif // NWOA_H__98326490226436542
