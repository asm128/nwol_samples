#include "application.h"
#include "nwol_ascii_display.h"

#include "TicTacToe.h"

#ifndef __NWOA_H__983264902__
#define __NWOA_H__983264902__

struct SApplication	: public ::gddm::SFramework {
	::ttt::TicTacToe		Game					= {};

							SApplication			(::nwol::SRuntimeValues	* runtimeValues)	: SFramework(runtimeValues)	{
		GUI						= {{320, 200}, {::ttt::TicTacToe::SCREEN_WIDTH, ::ttt::TicTacToe::SCREEN_HEIGHT}};
	}
};

int32_t					setup					(::SApplication	& instanceApp);
int32_t					update					(::SApplication	& instanceApp, bool exitRequested);
int32_t					render					(::SApplication	& instanceApp);
int32_t					cleanup					(::SApplication	& instanceApp);
//												 
::nwol::error_t			networkEnable			(::SApplication	& instanceApp);
::nwol::error_t			networkDisable			(::SApplication	& instanceApp);

#endif // __NWOA_H__983264902__
