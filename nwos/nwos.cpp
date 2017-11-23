#include "nwos.h"

#include "netlib_server.h"
#include "netlib_command.h"
#include "nwol_sleep.h"

#include "nwol_runtime_impl.h"

#include <process.h>
#include <thread>

bool														bListenFailure					= false;

namespace nwol
{
	GDEFINE_ENUM_TYPE(SERVER_COMMAND, int16_t);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND, LOGIN				,	 0);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND, LOGIN_SUCCESS		,	 1);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND, LOGIN_FAIL			,	 2);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND, REGISTER				,	 3);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND, REGISTER_SUCCESS		,	 4);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND, REGISTER_FAIL		,	 5);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND, TAKEN_USERNAME		,	 6);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND, PLAYER_GET			,	 7);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND, PLAYER_SET			,	 8);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND, BUY					,	 9);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND, SELL					,	10);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND, RENAME				,	11);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND, RESERVED				,	12);
}

void														serverListen					(::gdnet::SServer* pServer)																							{
	while(pServer->bListening && !bListenFailure) {
		error_if(bListenFailure = errored(pServer->Listen()), "Failed to listen on server")
		else {
			if (!pServer->bListening)
				break;
			if errored(pServer->Accept()) {
				error_print("Failed to accept queued client or no client queued.");
				::nwol::sleep(1);
			}
		}
		::nwol::sleep(1);
	}
}

void														serverListen					( void* server )																									{ serverListen((::gdnet::SServer*)server);	}
::nwol::error_t												serverInit						(::SApplication& instanceApp)																						{
	uint32_t														port_number;				// Port number to use
	char															port_number_str	[]				= "45678";
	ree_if(sscanf_s(&port_number_str[0], "%u", &port_number) != 1, "%s.", "Invalid server port string.");	// Only run if port is provided

	nwol_necall(::gdnet::endpointSystemInitialize()										, "%s", "Failed to initialize network."				);	info_printf("%s initialized successfully.", "Network"			);
	nwol_necall(instanceApp.NetworkServer.InitServer((uint16_t)(port_number & 0xFFFFU))	, "%s", "Failed to initialize connection server."	);	info_printf("%s initialized successfully.", "Connection server"	);
	_beginthread( serverListen, 0, &instanceApp.NetworkServer );	
	return 0;
}

::nwol::error_t												processCommand8					(uint8_t )																											{ return 0; }
::nwol::error_t												processCommand16				(uint16_t)																											{ return 0; }
::nwol::error_t												processCommand32				(uint32_t)																											{ return 0; }
::nwol::error_t												processCommand64				(uint64_t)																											{ return 0; }

::nwol::error_t												nodeHandleUserData					(::gdnet::SNetworkNode* client, const char* payloadBuffer, uint32_t bufferLen)										{
	::gdnet::SIPv4													addressRemote					= {};
	::gdnet::endpointAddress(client->Endpoints.Remote, addressRemote);
	switch(bufferLen) {
	case 1:	{	uint8_t		receivedValue = (uint8_t)-1; memcpy(&receivedValue, payloadBuffer, bufferLen); return processCommand8	(receivedValue); break; }
	case 2:	{	uint16_t	receivedValue = (uint8_t)-1; memcpy(&receivedValue, payloadBuffer, bufferLen); return processCommand16	(receivedValue); break; }
	case 4:	{	uint32_t	receivedValue = (uint8_t)-1; memcpy(&receivedValue, payloadBuffer, bufferLen); return processCommand32	(receivedValue); break; }
	case 8:	{	uint64_t	receivedValue = (uint8_t)-1; memcpy(&receivedValue, payloadBuffer, bufferLen); return processCommand64	(receivedValue); break; }
	default:
		if (strcmp(payloadBuffer, "GET PLAYER\r\n") == 0) {
			::nwol::SERVER_COMMAND											response						= ::nwol::SERVER_COMMAND_INVALID;							
			static constexpr	const uint32_t								bytesToSend						= sizeof(response);
			error_if(errored(::gdnet::dataSend(client->Endpoints, ::gdnet::NWON_COMMAND_TYPE_RESPONSE, (const byte_t*)&response, bytesToSend)), "Failed to send player data to %u.%u.%u.%u:%u.", NWOL_EXPAND_IPV4_UINT32(addressRemote))
			else {
				//info_printf("Sent player data (%s) to %u.%u.%u.%u:%u.", player.Name.c_str()
				//	, (int)a1
				//	, (int)a2
				//	, (int)a3
				//	, (int)a4
				//	, (int)port_number
				//	);
			}
		}
		else {
			::nwol::SERVER_COMMAND											response						= ::nwol::SERVER_COMMAND_INVALID;							
			static constexpr	const uint32_t								bytesToSend						= sizeof(response);

			error_if(errored(::gdnet::dataSend(client->Endpoints, ::gdnet::NWON_COMMAND_TYPE_RESPONSE, (const byte_t*)&response, bytesToSend)), "Failed to send player data to %u.%u.%u.%u:%u.", NWOL_EXPAND_IPV4_UINT32(addressRemote))
			else {
				info_printf("Sent invalid message response to %u.%u.%u.%u:%u.", NWOL_EXPAND_IPV4_UINT32(addressRemote));
			}
		}
	}
	return 0;
}