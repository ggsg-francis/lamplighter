#ifdef DEF_NMP
//c++ stuff
#include <iostream>
#include <array>
#include <string>

#include "SDL2\SDL_net.h"

//NETCODE
#include <winsock2.h>
#include <Ws2tcpip.h>
//#include <iphlpapi.h>
#include <stdio.h>
#include <time.h>

// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")
// link with so so...
//#pragma comment(lib, "IPHLPAPI.lib")

#include "network_client.h"

//try cut down on some of these
#include "cfg.h"
#include "core.h"
#include "maths.hpp"

#include "objects_entities.h"

// Enough c++ nerds said never to use macros, so here are some macros
#define PACKET_WRITE(pak, ind, val, type) (*((type*)&pak[ind])=val)
#define PACKET_READ(pak, ind, type) (*((type*)&pak[ind]))

#define NUM_SET_SOCKETS 1

namespace network
{
	UDPsocket socket_connection;
	UDPsocket socket_;

	TCPsocket socket_reliable_sv_incoming;
	TCPsocket socket_reliable;
	UDPsocket socket_udp;

	// Create a socket set to handle up to 16 sockets
	SDLNet_SocketSet socketset;
	
	// Network ID
	btui8 nid = 0ui8;

	//-------------------------------- RECEIVE

	void RecvUDP()
	{
		UDPpacket* pak = NULL;
		// only works on client atm
		while (SDLNet_UDP_Recv(socket_udp, pak) == PACKET_SIZE)
		{
			Entity* ent;
			paktype type = PACKET_READ(pak, 0, paktype);
			switch (type)
			{
			default:
				std::cout << "ERROR RecvTCP : packet type not handled!" << std::endl;
				break;
			}
		}
	}

	void RecvTCP()
	{
		btPacket pak;
		bool any = false;
		// Loop until we have a packet
	checksock:
		// If no packet is available
		int numready = SDLNet_CheckSockets(socketset, 0u);
		if (numready == -1)
		{
			std::cout << "ERROR RecvTCP : some shit!" << std::endl;
			return;
		}
		if (numready == 0)
		{
			if (any) return;
			else goto checksock;
		}
		// do for() and CheckSock when we connect to more than one client
		// If there is a packet, read it
		if (SDLNet_TCP_Recv(socket_reliable, pak, PACKET_SIZE) == PACKET_SIZE)
		{
			Entity* ent;
			Chara* chr;
			paktype type = PACKET_READ(pak, 0, paktype);
			switch (type)
			{
			case eINPUT_BUFFER:
				// Read input buffer into local input buffer for this player
				input::buf[PACKET_READ(pak, 3, btui8)][INPUT_BUF_GET] = PACKET_READ(pak, 4, InputBuffer);
				break;
			default:
				std::cout << "ERROR RecvTCP : packet type not handled!" << std::endl;
				break;
			}
			any = true;
		}
		goto checksock;
	}

	//-------------------------------- PACKET TEMPLATES

	bool SendInputBuffer()
	{
		btPacket pak;
		PACKET_WRITE(pak, 0, eINPUT_BUFFER, btui8);
		// 1 is normally ID
		PACKET_WRITE(pak, 3, nid, btui8);
		PACKET_WRITE(pak, 4, input::buf[nid][INPUT_BUF_SET], InputBuffer);
		return SendTCP(&pak);
	}

	//-------------------------------- SEND

	bool SendUDP(btPacket* pak)
	{
		UDPpacket* udppak = SDLNet_AllocPacket(PACKET_SIZE);
		if (!udppak)
		{
			udppak->data = (Uint8*)pak;
			if (!SDLNet_UDP_Send(socket_udp, udppak->channel, udppak)) {
				printf("Could not send UDP packet ;~;\n");
				return false;
			}
			SDLNet_FreePacket(udppak);
			return true;
		}
		return false;
	}

	bool SendTCP(btPacket* pak)
	{
		if (SDLNet_TCP_Send(socket_reliable, pak, PACKET_SIZE) < PACKET_SIZE) {
			printf("Could not send TCP packet ;~;\n");
			return false;
		}
		return true;
	}

	bool Init()
	{
		// Initialize network library
		int err = SDLNet_Init();
		if (err == -1) goto exiterr;
		// Open UDP socket
		socket_connection = SDLNet_UDP_Open((Uint16)cfg::iPort); // pass 0 to open on any available port
		if (!socket_connection) goto exiterr;

		if (cfg::bHost) // if acting as server
		{
			IPaddress addr; // broadcast

			btui32 seed = (btui32)time(NULL); // Server decides the seed
			srand(seed); //initialize the random seed

			int found_other = SDLNet_ResolveHost(&addr, NULL, (Uint16)cfg::iPort);
			if (found_other == -1) goto exiterr;
			int channel = SDLNet_UDP_Bind(socket_connection, -1, &addr);

			//open socket
			socket_reliable_sv_incoming = SDLNet_TCP_Open(&addr);
			if (!socket_reliable_sv_incoming) goto exiterr;
			printf("Socket opened OK\n");

			printf("waiting for connection........ \n");
			while (true)
			{
				// accept a connection coming in on server_tcpsock
				socket_reliable = SDLNet_TCP_Accept(socket_reliable_sv_incoming);
				if (!socket_reliable) {
					//printf("SDLNet_TCP_Accept: %s\n", SDLNet_GetError());
				}
				else {
					// communicate over new_tcpsock
					printf("Got connection!!!!!! \n");
					// now wait for packet
					printf("waiting for packet........ \n");
					btPacket pak;
					while (true)
					{
						if (SDLNet_TCP_Recv(socket_reliable, pak, PACKET_SIZE) == PACKET_SIZE)
						{
							printf("Got packet! \n");
							paktype type = PACKET_READ(pak, 0, paktype);
							if (type == eCLIENT_CONNECT_REQUEST)
							{
								if (_MSC_VER != PACKET_READ(pak, 12, btui32)) { // check compiler version number
									printf("WARNING -- This client was built with a different compiler, desynchronization is likely.\n"); }
								bti32 xm = 0x3f18492a; btf32 x = *(btf32*)&xm; x = (sqrt(x) + 1) / 2.0f; // Floating point test
								if (PACKET_READ(pak, 4, btui32) == VERSION_MAJOR // if the version number matches
									&& PACKET_READ(pak, 8, btui32) == VERSION_MINOR // if the version number matches
									&& x == PACKET_READ(pak, 16, btf32)) // and the floating point calculation matches
								{
									printf("Version accepted! \n");
									// send connection packet
									PACKET_WRITE(pak, 0, eSERVER_CONNECT_CONFIRM, paktype);
									PACKET_WRITE(pak, 1, 1ui8, btui8); // send client NID (TODO:)
									PACKET_WRITE(pak, 4, seed, btui32); // send RNG seed
									SDLNet_TCP_Send(socket_reliable, pak, PACKET_SIZE);
									Sleep(1000);
									goto exit;
								}
								else
								{
									printf("Version number incompatible! Closing connection. \n");
									// send disconnection packet
									PACKET_WRITE(pak, 0, eSERVER_DISCONNECT_CLIENT, paktype);
									SDLNet_TCP_Send(socket_reliable, pak, PACKET_SIZE);
									SDLNet_TCP_Close(socket_reliable);
								}
							}
							else printf("Wrong packet type..? \n");
						}
					}
				}
			}
		}
		else // if acting as client
		{
			IPaddress addr; // listen
			addr.host = INADDR_BROADCAST;
			addr.port = (Uint16)cfg::iPort;

			#error If you're building the multiplayer version you'll want to change the hostname
			int found_other = SDLNet_ResolveHost(&addr, "Sirennus0000", (Uint16)cfg::iPort);
			//int found_other = SDLNet_ResolveHost(&addr, "10.1.1.144", cfg::sIPPORT);
			//int found_other = SDLNet_ResolveHost(&addr, "https://servertest.b-type.net", cfg::sIPPORT);
			//int found_other = SDLNet_ResolveHost(&addr, "F550C", cfg::sIPPORT);
			if (found_other == -1) goto exiterr;
			int channel = SDLNet_UDP_Bind(socket_connection, -1, &addr);

			//open socket
			socket_reliable = SDLNet_TCP_Open(&addr);
			if (!socket_reliable){
				printf("Could not open socket!\n");
				goto exiterr;
			}
			printf("Socket opened OK\n");

			// send connection packet
			btPacket pak;
			PACKET_WRITE(pak, 0, eCLIENT_CONNECT_REQUEST, paktype);
			PACKET_WRITE(pak, 4, VERSION_MAJOR, btui32);
			PACKET_WRITE(pak, 8, VERSION_MINOR, btui32);
			PACKET_WRITE(pak, 12, _MSC_VER, btui32); // send compiler version number
			bti32 xm = 0x3f18492a; btf32 x = *(btf32*)&xm; x = (sqrt(x) + 1) / 2.0f; // Floating point test
			PACKET_WRITE(pak, 16, x, btf32); // send float test
			printf("Sending\n");
			if (SDLNet_TCP_Send(socket_reliable, pak, PACKET_SIZE) < PACKET_SIZE)
				printf("Could not send ;~;\n");

			while (true)
			{
				if (SDLNet_TCP_Recv(socket_reliable, pak, PACKET_SIZE) == PACKET_SIZE)
				{
					switch (pak[0])
					{
					case eSERVER_CONNECT_CONFIRM:
						printf("Connection accepted! NID is %u\n", pak[1]);
						nid = PACKET_READ(pak, 1, btui8);
						srand(PACKET_READ(pak, 4, btui32)); //initialize the random seed
						Sleep(1000);
						goto exit;
						break;
					case eSERVER_DISCONNECT_CLIENT:
						printf("Connection not accepted - Version number incompatible.\n");
						goto exiterr;
						break;
					}
				}
			}
		}

	exit:
		socketset = SDLNet_AllocSocketSet(NUM_SET_SOCKETS);
		if (!socketset) goto exiterr;
		if (SDLNet_TCP_AddSocket(socketset, socket_reliable) == -1) goto exiterr;
		printf("Initialized Net\n");
		return true;
	exiterr:
		printf("Couldn't initialize/connect network! | %s\n", SDLNet_GetError());
		Sleep(1000);
		return false;
	}
	void End()
	{
		SDLNet_UDP_Close(socket_connection);
		SDLNet_TCP_Close(socket_reliable);
		SDLNet_TCP_Close(socket_reliable_sv_incoming);
		SDLNet_FreeSocketSet(socketset);
		SDLNet_Quit();
	}
}
#endif // DEF_NMP
