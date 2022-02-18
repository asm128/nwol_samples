#include "nwoa.h"
#include "netlib_command.h"
#include "nwol_sleep.h"

#include <thread>

#if defined (__WINDOWS__)
#include <process.h>	// This is a Windows SDK header used for _beginthread().
#endif

int32_t										nodeHandleUserData						(::gdnet::SNetworkNode* /*client*/, const char* /*buffer*/, uint32_t /*bufferLen*/)	{ return 0; }
int											runCommunications						(::gddm::SFrameworkNetworkClient& appNetwork)										{
	::gdnet::SNetworkNode							& instanceClient						= appNetwork.Connection;

	nwol_necall(::gdnet::nodeInit	(instanceClient, instanceClient.Address, ::gdnet::TRANSPORT_PROTOCOL_UDP), "%s", "Failed to initialize client connection.");
	nwol_necall(::gdnet::nodeConnect	(instanceClient), "%s", "Failed to connect.");

	::nwol::error_t									result									= 0;
	while gbit_true(appNetwork.State, ::gddm::NETWORK_STATE_ENABLED) {
		break_error_if(errored(result = ::gdnet::ping(instanceClient.Endpoints) ? 0 : -1), "%s", "Ping timeout.");	// Ping before anything else to make sure everything is more or less in order.
		uint64_t										current_time;
		break_error_if(errored(result = ::gdnet::time(instanceClient.Endpoints, current_time)), "Failed to get server time.");	// get server time
		{	// here we update the game instance with the data received from the server.
			::nwol::CMutexGuard								thelock									(appNetwork.ServerTimeMutex);
			appNetwork.ServerTime						= current_time;
		}
		info_printf("%s", "Client instance updated successfully.");
		bi_if(gbit_false(appNetwork.State, ::gddm::NETWORK_STATE_ENABLED), "Disconnect as the network was disabled.");
		::nwol::sleep(1000);
	}

	error_if(::gdnet::nodeRequestDisconnect(instanceClient), "Why would this happen?");
	gbit_clear(appNetwork.State, ::gddm::NETWORK_STATE_RUNNING);
	instanceClient.Endpoints					= {};
	return result;
}

void										runCommunications						(void* pInstanceApp)						{
	info_printf("Communications loop initializing.");
	if(0 == pInstanceApp)
		return;

	::SApplication									& instanceApp							= *(::SApplication*)pInstanceApp;
	::gddm::SFrameworkNetworkClient					& instanceAppNetwork					= instanceApp.NetworkClient;

	[[maybe_unused]] const ::nwol::error_t							errMyComm								= ::runCommunications(instanceAppNetwork); 
	(void)errMyComm; // obviously [[maybe_unused]] is worse than this.

	gbit_clear(instanceAppNetwork.State, ::gddm::NETWORK_STATE_RUNNING);			
	info_printf("Communications loop exited with code 0x%X.", errMyComm);
}

::nwol::error_t								networkEnable							(::SApplication& instanceApp)				{
	nwol_necall(::gdnet::endpointSystemInitialize(), "Failed to initialize network.");
	info_printf("%s", "Network successfully initialized.");

	::gddm::SFrameworkNetworkClient					& instanceAppNetwork					= instanceApp.NetworkClient;
	gbit_set(instanceAppNetwork.State, ::gddm::NETWORK_STATE_ENABLED);
	gbit_set(instanceAppNetwork.State, ::gddm::NETWORK_STATE_RUNNING);
	_beginthread(::runCommunications, 0, &instanceApp);
	::nwol::sleep(1000);

	info_printf("%s", "Communications thread started.");
	return 0;
}

::nwol::error_t								networkDisable							(::SApplication& instanceApp)				{
	::gddm::SFrameworkNetworkClient					& instanceAppNetwork					= instanceApp.NetworkClient;
	gbit_clear(instanceAppNetwork.State, ::gddm::NETWORK_STATE_ENABLED);
	if gbit_true(instanceAppNetwork.State, ::gddm::NETWORK_STATE_RUNNING)
		instanceAppNetwork.Connection.Endpoints		= {};
	
	while gbit_true(instanceAppNetwork.State, ::gddm::NETWORK_STATE_RUNNING)
		::nwol::sleep(1000);

	::nwol::sleep(1000);
	::gdnet::endpointSystemShutdown();

	return 0;
}
