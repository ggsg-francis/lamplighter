#ifdef DEF_NMP
//c++ stuff
#include <iostream>
#include <array>
#include <string>

#include "SDL2\SDL_net.h"

#include <stdio.h>
#include <time.h>

#include "network.h"

// try cut down on some of these
#include "cfg.h"
#include "core.h"
#include "maths.hpp"

#include "input.h"

// Enough c++ nerds said never to use macros, so here are some macros
#define PACKET_WRITE(pak, ind, val, type) (*((type*)&pak[ind])=val)
#define PACKET_READ(pak, ind, type) (*((type*)&pak[ind]))

#define NUM_SET_SOCKETS 1

#define DEF_UDP 0

namespace network
{
	TCPsocket socket_reliable_sv_incoming;
	TCPsocket socket_reliable[NUM_PLAYERS - 1u];
	btui32 player_count = 2u;
	btui32 socket_count = 1u;
	UDPsocket socket_connection;

	#if DEF_UDP
	UDPsocket socket_udp;
	#endif

	// Create a socket set to handle up to 16 sockets
	SDLNet_SocketSet socketset;
	
	// Network ID
	btui8 nid = 0u;

	btf32 FloatTest() {
		bti32 xm = 0x3f18492a;
		btf32 x = *(btf32*)&xm;
		x = (sqrt(x) + 1) / 2.0f;
		return x;
	}

	//-------------------------------- TCP

	void RecvTCPClient() {
		btPacket pak;
		bool read = false;
		// Loop until we have a packet
	checksock:
		// If no packet is available
		int numready = SDLNet_CheckSockets(socketset, 0u);
		if (numready == -1) {
			std::cout << "ERROR RecvTCP : some shit!" << std::endl;
			return;
		}
		if (numready == 0) {
			// Have we read from all of the packets yet?
			if (read) return;
			else goto checksock;
		}
		// If we get this far, that means at least one of the sockets has information
		if (SDLNet_TCP_Recv(socket_reliable[0], pak, PACKET_SIZE) == PACKET_SIZE) {
			paktype ptype = PACKET_READ(pak, 0, paktype);
			switch (ptype) {
			case eINPUT_BUFFER:
				// For every player (excluding us), read input
				for (int i = 0; i < player_count; ++i) {
					if (i == nid) continue; // Don't set our own input
					// Read input buffer into local input buffer for this player
					input::input_buffer[i]
						= PACKET_READ(pak, i * sizeof(InputBuffer) + 4u, InputBuffer);
				}
				break;
			default:
				std::cout << "ERROR RecvTCP : packet type not handled!" << std::endl;
				break;
			}
			read = true;
		}
		goto checksock;
	}
	void RecvTCPHost() {
		btPacket pak;
		//btui32 readCount = 0u;
		bool* read = new bool[socket_count];
		for (int i = 0; i < socket_count; ++i) {
			read[i] = false; // Set all to false
		}
		// Loop until we have a packet
	checksock:
		// If no packet is available
		int numready = SDLNet_CheckSockets(socketset, 0u);
		if (numready == -1) {
			std::cout << "ERROR RecvTCP : some shit!" << std::endl;
			return;
		}
		if (numready == 0) {
			// Have we read from all of the packets yet?
			// Also, we're assuming a single socket won't send us two updates at once, welp
			//if (readCount == socket_count) return;
			bool all_read = true;
			for (int i = 0; i < socket_count; ++i)
				if (!read[i]) all_read = false; // if any not read
			if (all_read) return;
			else goto checksock;
		}
		// If we get this far, that means at least one of the sockets has information
		for (int i = 0; i < socket_count; ++i) {
			if (SDLNet_TCP_Recv(socket_reliable[i], pak, PACKET_SIZE) != PACKET_SIZE) continue;
			paktype ptype = PACKET_READ(pak, 0, paktype);
			switch (ptype) {
			case eINPUT_BUFFER:
				// Read input buffer into local input buffer for this player
				input::input_buffer[PACKET_READ(pak, 3, btui8)]
					= PACKET_READ(pak, 4, InputBuffer);
				break;
			default:
				std::cout << "ERROR RecvTCP : packet type not handled!" << std::endl;
				break;
			}
			//++readCount;
			read[i] = true;
		}
		goto checksock;
		delete[] read;
	}
	bool SendTCPClient(btPacket* pak) {
		// Send to host
		if (SDLNet_TCP_Send(socket_reliable[0], pak, PACKET_SIZE) < PACKET_SIZE) {
			printf("Could not send TCP packet ;~;\n");
			return false;
		}
		return true;
	}
	bool SendTCPHost(btPacket* pak) {
		// Send to all clients
		for (int i = 0; i < socket_count; ++i) {
			if (SDLNet_TCP_Send(socket_reliable[i], pak, PACKET_SIZE) < PACKET_SIZE) {
				printf("Could not send TCP packet ;~;\n");
				return false;
			}
		}
		return true;
	}

	//-------------------------------- UDP

	#if DEF_UDP
	void RecvUDP()
	{
		UDPpacket* pak = NULL;
		// only works on client atm
		while (SDLNet_UDP_Recv(socket_udp, pak) == PACKET_SIZE)
		{
			paktype type = PACKET_READ(pak, 0, paktype);
			switch (type)
			{
			default:
				std::cout << "ERROR RecvTCP : packet type not handled!" << std::endl;
				break;
			}
		}
	}
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
	#endif

	//-------------------------------- INIT AND END

	bool Init()
	{
		// Set destination player count
		player_count = config.iNumNWPlayers;
		if (config.bHost)
			socket_count = config.iNumNWPlayers - 1u;
		else
			socket_count = 1u;

		// Initialize network library
		int err = SDLNet_Init();
		if (err == -1) goto exiterr;
		// Open UDP socket
		socket_connection = SDLNet_UDP_Open((Uint16)config.iPort); // pass 0 to open on any available port
		if (!socket_connection) goto exiterr;

		if (config.bHost) // if acting as server
		{
			IPaddress addr; // broadcast

			btui32 seed = (btui32)time(NULL); // Server decides the seed
			srand(seed); //initialize the random seed

			int found_other = SDLNet_ResolveHost(&addr, NULL, (Uint16)config.iPort);
			if (found_other == -1) goto exiterr;
			int channel = SDLNet_UDP_Bind(socket_connection, -1, &addr);

			//open socket
			socket_reliable_sv_incoming = SDLNet_TCP_Open(&addr);
			if (!socket_reliable_sv_incoming) goto exiterr;
			printf("Socket opened OK\n");

			btui32 taken_connections = 0u;
			printf("waiting for connection... \n");
			while (true) {
			wait:
				// accept a connection coming in on server_tcpsock
				socket_reliable[taken_connections] = SDLNet_TCP_Accept(socket_reliable_sv_incoming);
				if (socket_reliable[taken_connections]) {
					// communicate over new_tcpsock
					printf("Got connection! \n");
					// now wait for packet
					printf("waiting for confirmation... \n");
					btPacket pak;
					while (true) {
						if (SDLNet_TCP_Recv(socket_reliable[taken_connections], pak, PACKET_SIZE) == PACKET_SIZE) {
							printf("Got packet! \n");
							paktype type = PACKET_READ(pak, 0, paktype);
							if (type == eCLIENT_CONNECT_REQUEST) {
								if (_MSC_VER != PACKET_READ(pak, 12, btui32)) { // check compiler version number
									printf("WARNING -- This client was built with a different compiler, desynchronization is likely.\n");
								}
								if (PACKET_READ(pak, 4, btui32) == VERSION_MAJOR // if the version number matches
									&& PACKET_READ(pak, 8, btui32) == VERSION_MINOR // if the version number matches
									&& FloatTest() == PACKET_READ(pak, 16, btf32)) { // and the floating point calculation matches
									printf("Version accepted!\n");
									// send connection packet
									PACKET_WRITE(pak, 0, eSERVER_CONNECT_CONFIRM, paktype);
									PACKET_WRITE(pak, 1, taken_connections + 1u, btui8); // send client NID (zero is always host)
									PACKET_WRITE(pak, 4, seed, btui32); // send RNG seed
									PACKET_WRITE(pak, 8, config.iNumNWPlayers, btui32); // send Player count
									SDLNet_TCP_Send(socket_reliable[taken_connections], pak, PACKET_SIZE);
									// One more connection
									++taken_connections;
									printf("Connection %i of %i ready!\n", taken_connections, socket_count);
									// Have we taken in enough connections to continue?
									if (taken_connections == socket_count) goto exit;
									printf("Waiting for another connection...\n");
									goto wait;
								}
								else {
									printf("Version number incompatible! Closing connection.\n");
									// send disconnection packet
									PACKET_WRITE(pak, 0, eSERVER_DISCONNECT_CLIENT, paktype);
									SDLNet_TCP_Send(socket_reliable[taken_connections], pak, PACKET_SIZE);
									SDLNet_TCP_Close(socket_reliable[taken_connections]);
								}
							}
							else printf("Wrong packet type..?\n");
						}
					}
				}
			}
		}
		else // if acting as client
		{
			IPaddress addr; // listen
			addr.host = INADDR_BROADCAST;
			addr.port = (Uint16)config.iPort;

			int found_other = SDLNet_ResolveHost(&addr, config.sConnAddr, (Uint16)config.iPort);
			if (found_other == -1) {
				printf("Could not find the host!\n");
				goto exiterr;
			}
			int channel = SDLNet_UDP_Bind(socket_connection, -1, &addr);

			//open socket
			socket_reliable[0] = SDLNet_TCP_Open(&addr);
			if (!socket_reliable[0]) {
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
			PACKET_WRITE(pak, 16, FloatTest(), btf32); // send float test
			printf("Sending\n");
			if (SDLNet_TCP_Send(socket_reliable[0], pak, PACKET_SIZE) < PACKET_SIZE)
				printf("Could not send ;~;\n");

			while (true) {
				if (SDLNet_TCP_Recv(socket_reliable[0], pak, PACKET_SIZE) == PACKET_SIZE) {
					switch (pak[0]) {
					case eSERVER_CONNECT_CONFIRM:
						printf("Connection accepted! NID is %u\n", pak[1]);
						nid = PACKET_READ(pak, 1, btui8);
						srand(PACKET_READ(pak, 4, btui32)); //initialize the random seed
						config.iNumNWPlayers = PACKET_READ(pak, 8, btui32); // read Player count
						player_count = config.iNumNWPlayers;
						goto exit;
						break;
					case eSERVER_DISCONNECT_CLIENT:
						printf("Connection not accepted - Version number incompatible.\n");
						goto exiterr;
						break;
					default:
						printf("Couldn't understand the packet from the host.\n");
						break;
					}
				}
			}
		}

	exit:
	//socketset = SDLNet_AllocSocketSet(NUM_SET_SOCKETS);
		socketset = SDLNet_AllocSocketSet(socket_count);
		if (!socketset) goto exiterr;
		for (int i = 0; i < socket_count; ++i)
			if (SDLNet_TCP_AddSocket(socketset, socket_reliable[i]) == -1) goto exiterr;
		printf("Initialized Net\n");
		return true;
	exiterr:
		printf("Couldn't initialize/connect network! | %s\n", SDLNet_GetError());
		//Sleep(1000);
		return false;
	}

	void End()
	{
		SDLNet_UDP_Close(socket_connection);
		for (int i = 0; i < socket_count; ++i)
			SDLNet_TCP_Close(socket_reliable[i]);
		SDLNet_TCP_Close(socket_reliable_sv_incoming);
		SDLNet_FreeSocketSet(socketset);
		SDLNet_Quit();
	}

	//-------------------------------- PACKET TEMPLATES

	bool SendInputClient() {
		btPacket pak;
		// Write packet type
		PACKET_WRITE(pak, 0, eINPUT_BUFFER, btui8);
		// Write own input (and nw address)
		PACKET_WRITE(pak, 3, nid, btui8);
		PACKET_WRITE(pak, 4, input::input_buffer[nid], InputBuffer);
		// Send to server
		return SendTCPClient(&pak);
	}
	bool SendInputHost() {
		btPacket pak;
		// Write packet type
		PACKET_WRITE(pak, 0, eINPUT_BUFFER, btui8);
		// For every player (including us), write input
		for (int i = 0; i < player_count; ++i) {
			PACKET_WRITE(pak, i * sizeof(InputBuffer) + 4u,
				input::input_buffer[i], InputBuffer);
		}
		// Send to all clients
		return SendTCPHost(&pak);
	}
}
#endif // DEF_NMP
