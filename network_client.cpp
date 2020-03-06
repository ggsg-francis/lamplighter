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

// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")
// link with so so...
//#pragma comment(lib, "IPHLPAPI.lib")

#include "network_common.h"

#include "network_client.h"

//try cut down on some of these
#include "cfg.h"
#include "core.h"
#include "maths.hpp"

//void SetTitle(btui8 id);

namespace network
{
	//quake
	static SOCKET net_acceptsocket = INVALID_SOCKET;	// socket for fielding new connections
	static SOCKET net_controlsocket;
	static SOCKET net_broadcastsocket = 0;
	static sockaddr_in broadcastaddr;
	static in_addr myAddr;

	UDPsocket socket_connection;
	UDPsocket socket_;

	//network id
	btui8 nid = 0ui8;

	//-------------------------------- ENCODING AND DECODING

	//networking stuff
	SOCKET connHandle;
	//message from server
	char servMessage[PACKET_SIZE] = "NO CONNECTION";

	//________________________________________________________________________________________________________________________________
	//-------------------------------- MY FUNCTIONS

	void Recv2(int type)
	{
		void* buffer = malloc(PACKET_SIZE);
		int recvd = recv(connHandle, (char*)buffer, PACKET_SIZE, NULL); //receive message from server
		if (recvd > 0)
		{
			packet_wrap pw;
			memcpy(&pw, buffer, PACKET_SIZE);
			switch (pw.type)
			{
			case eSET_ENT_POSE:
				msg::SetEntPose msup;
				memcpy(&msup, pw.msg, sizeof(msg::SetEntPose)); //std::cout << "TYPE: " << (int)pw.type << " INPUT: " << msup.px << ", " << msup.py << ", " << msup.yaw << std::endl;
				//index::NetSetPose(&msup);
				break;
			case eSET_CHARA_ANIM:
				msg::SetCharaAnim msa;
				memcpy(&msa, pw.msg, sizeof(msg::SetCharaAnim));
				//index::NetSetAnim(&msa);
				break;
			case eSPAWN_PROJECTILE:
				msg::SpawnProj msp;
				memcpy(&msp, pw.msg, sizeof(msg::SpawnProj));
				//std::cout << "Received spawn proj ID " << msp.id << std::endl;
				//index::BufferAddProjectile(msp.id, 0, fw::Vector3(msp.px, msp.py, msp.pz), fw::Vector3(msp.dx, msp.dy, msp.dz));
				//index::SpawnProjectile();
				break;
			case eSERVER_CONNECT_CLIENT:
				msg::ServerConnectClient mscc;
				memcpy(&mscc, pw.msg, sizeof(msg::ServerConnectClient));
				nid = mscc.id;
				//SetTitle(mscc.id);
				std::cout << "Server set our ID to " << (unsigned int)mscc.id << std::endl;
				break;
			case eSERVER_DISCONNECT_CLIENT:
				std::cout << "Incompatible version number with server." << std::endl;
				break;
			default:
				std::cout << "ERROR network::Recv2 : packet type not handled!" << std::endl;
				break;
			}
		}
		free(buffer);
	}

	// Send a struct formatted message across the network
	void SendMsg(paktype type, void* msg)
	{
		void* buffer = malloc(PACKET_SIZE); //allocate memory to write our packet buffer into
		packet_wrap pw; //create wrapper class to contain the message
		pw.type = type; //set wrapper message type to correct type
		memcpy(pw.msg, msg, PACKET_SIZE - sizeof(char)); //copy the message struct into this wrapper class
		memcpy(buffer, &pw, PACKET_SIZE); //copy the wrapper class into the buffer for sending
		send(connHandle, (char*)buffer, PACKET_SIZE, NULL); //send the buffer
		
		UDPpacket packet;
		
		SDLNet_UDP_Send(socket_, 0, &packet);
		free(buffer); //clear the buffer from memory
	}

	void Init()
	{
		// Initialize network library
		int err = SDLNet_Init();
		if (err == -1) goto exiterr;
		// Open UDP socket
		socket_connection = SDLNet_UDP_Open(cfg::sIPPORT); // pass 0 to open on any available port
		if (!socket_connection) goto exiterr;

		if (cfg::bHost) // if acting as server
		{
			IPaddress addr; // broadcast
			addr.host = INADDR_ANY;
			addr.port = cfg::sIPPORT;

			int found_other = SDLNet_ResolveHost(&addr, NULL, cfg::sIPPORT);
			if (found_other == -1) goto exiterr;
			int channel = SDLNet_UDP_Bind(socket_connection, -1, &addr);

			// try to receive a waiting udp packet
			UDPsocket udpsock = NULL;
			UDPpacket packet;
			int numrecv;
			printf("waiting for packet........ \n");
			while (true)
			{
				numrecv = SDLNet_UDP_Recv(udpsock, &packet);
				if (numrecv) {
					// do something with packet
					printf("Received packet!\n");
					goto exit; //bail for now
				}
				if (numrecv == -1)
				{
					printf("Fuckt\n");
				}
			}
		}
		else // if acting as client
		{
			IPaddress addr; // listen
			addr.host = INADDR_BROADCAST;
			addr.port = cfg::sIPPORT;

			int found_other = SDLNet_ResolveHost(&addr, "Sirennus0000", cfg::sIPPORT);
			if (found_other == -1) goto exiterr;
			//int found_other = SDLNet_ResolveHost(&addr, "255.255.255.255", cfg::sIPPORT);
			//int channel = 0;
			int channel = SDLNet_UDP_Bind(socket_connection, -1, &addr);

			// send a packet using a UDPsocket, using the packet's channel as the channel
			UDPpacket* packet;

			packet = SDLNet_AllocPacket(256);

			packet->address = addr;
			packet->channel = channel;
			char* c = "hewwo..?";
			memcpy(packet->data, c, strlen(c));
			packet->len = strlen(c);
			packet->maxlen = 256;

			int numsent;
			numsent = SDLNet_UDP_Send(socket_connection, channel, packet);
			if (!numsent) {
				printf("SDLNet_UDP_Send: %s\n", SDLNet_GetError());
				// do something because we failed to send
				// this may just be because no addresses are bound to the channel...
			}
			else 
				printf("Sent packet OK\n");

			SDLNet_FreePacket(packet);

			while (true)
			{
				//do nothing;
			}
		}

	exit:
		printf("Initialized Net\n");
		return;
	exiterr:
		printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
	}
	void End()
	{
		SDLNet_UDP_Close(socket_connection);
		SDLNet_Quit();
	}

	void Connect()
	{
		
	}
}
#endif // DEF_NMP
