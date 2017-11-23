#include "nwoa.h"

#include "nwol_sleep.h"
#include "nwol_runtime_impl.h"
#include "nwol_ascii_display.h"

DEFINE_RUNTIME_INTERFACE_FUNCTIONS(::SApplication, "No Workflow Overhead Application", 0, 1);

::nwol::error_t												cleanup									(::SApplication& instanceApp)							{ 
	gbit_clear(instanceApp.NetworkClient.State, ::gddm::NETWORK_STATE_ENABLED);
	uint32_t														waitLimit								= 500;
	uint32_t														waitTime								= 10;
	while(gbit_true(instanceApp.NetworkClient.State, ::gddm::NETWORK_STATE_RUNNING) && (waitLimit -= waitTime) > 0)
		::nwol::sleep(waitTime);

	instanceApp.NetworkClient.Connection.Endpoints				= {};
	::nwol::sleep(100);
	nwol_necall(::networkDisable(instanceApp), "Error when disabling network.");
	::nwol::sleep(100);

	::nwol::asciiDisplayDestroy();
	::nwol::asciiTargetDestroy(instanceApp.ASCIITarget);
	return 0; 
}

template<typename _tBase>
void														printTypeDebug							()														{
	static const _tBase::TRegistry									memberRegistry							= _tBase::get_member_registry();
	for(uint32_t iMember=0; iMember < memberRegistry.get_member_count(); ++iMember) {
		const ::nwol::STypeIdentifier									& identifier							= memberRegistry.get_types()[iMember];
		always_printf
			(	"0x%X: %s::%s %s (%s): %s"
			,	memberRegistry.get_data_type_ids	()[iMember]
			,	identifier.NameSpace	.c_str()
			,	identifier.Name			.c_str()
			,	memberRegistry.get_names			()[iMember].c_str()
			,	memberRegistry.get_display_names	()[iMember].c_str()
			,	memberRegistry.get_descriptions		()[iMember].c_str()
			);
	}
}

::nwol::error_t												setupGUI								(::SApplication& instanceApp);
::nwol::error_t												setupConsole							(::SApplication& instanceApp);
::nwol::error_t												setupInputDevices						(::SApplication& instanceApp);

::nwol::error_t												setup									(::SApplication& instanceApp)							{ 
	nwol_necall(::setupConsole		(instanceApp), "Failed to setup application console."	);
	nwol_necall(::setupInputDevices	(instanceApp), "Failed to setup application input."		);
	nwol_necall(::setupGUI			(instanceApp), "Failed to setup application GUI."		);

	::printTypeDebug<::SApplication>();
	::printTypeDebug<::gddm::SFrameworkNetworkClient>();

	nwol_necall(::networkEnable	(instanceApp), "Failed to enable network."	);
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
	return 0; 
}

::nwol::error_t												render									(::SApplication& instanceApp)							{ 
	{
		::nwol::CMutexGuard											lockConsole								(instanceApp.ConsoleMutex);
		::nwol::SASCIITarget										& target								= instanceApp.ASCIITarget;
		::nwol::asciiTargetClear(target);
		error_if(errored(::nwol::renderGUIASCII(target, instanceApp.GUI)), "%s", "Failed to render ASCII GUI!")
	}
	return 0; 
}
