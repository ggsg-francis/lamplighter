//c++ stuff
#include <iostream>
#include <array>
#include <string>

//NETCODE
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>

// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#include "network_common.h"

#include "network_client.h"

//try cut down on some of these
#include "cfg.h"
#include "core.h"
#include "maths.hpp"

#include "version.h"

//void SetTitle(btui8 id);

namespace network
{
	//network id
	btui8 nid = 0ui8;

	//....................................... ENCODING AND DECODING

	//networking stuff
	SOCKET connHandle;
	//message from server
	char servMessage[PACKET_SIZE] = "NO CONNECTION";

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
		free(buffer); //clear the buffer from memory
	}

	void Init()
	{
		//initialize winsock
		WSAData wsaData;
		WORD DLLVersion = MAKEWORD(2, 1);
		if (WSAStartup(DLLVersion, &wsaData) != 0)
		{
			MessageBoxA(NULL, "Winsock startup failed", "Error", MB_OK | MB_ICONERROR);
			exit(1);
		}
	}

	void Connect()
	{
		unsigned short port = (short)cfg::iIPPORT;

		std::uint32_t ip_address_2 = (cfg::iIPA << 24) | (cfg::iIPB << 16) | (cfg::iIPC << 8) | cfg::iIPD;

		SOCKADDR_IN addr;
		int addrlength = sizeof(addr);
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(ip_address_2);
		addr.sin_port = htons(port);

		/*

		SOCKADDR_IN addr; //address that we will bind out listening socket to
		int addrlength = sizeof(addr); //length of the address (required for accept call)
		//addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //address = localhost
		//new non-deprecated address set
		std::uint32_t ip_address;
		inet_pton(AF_INET, "127.0.0.1", &ip_address);
		//const char* test = sIP.c_str();
		//inet_pton(AF_INET, sIP.c_str(), &ip_address);
		addr.sin_addr.s_addr = ip_address;

		addr.sin_port = htons(1111); //port
		addr.sin_family = AF_INET; //ipv4 socket, use AF_INET6 for ipv6, if needed

		*/

		connHandle = socket(AF_INET, SOCK_STREAM, NULL); //set connection socket

		if (connect(connHandle, (SOCKADDR*)&addr, addrlength) != 0) //if we are unable to connect
		{
			std::cout << "Could not connect!" << std::endl;
			//no need to exit the program if we can't connect
			//MessageBoxA(NULL,"Failed to Connect", "Error", MB_OK | MB_ICONERROR);
			//return 0;
		}
		//if we could connect to the server
		else
		{
			std::cout << "Connected!" << std::endl;

			recv(connHandle, servMessage, sizeof(servMessage), NULL); //receive message from server
			std::cout << "Message received: " << servMessage << std::endl;

			msg::ClientConnect msg;
			msg.version_major = VERSION_MAJOR;
			msg.version_minor = VERSION_BUILD;
			SendMsg(eCLIENT_CONNECT, &msg);
		}

		//set socket to non-blocking
		DWORD nonBlocking = 1;
		if (ioctlsocket(connHandle, FIONBIO, &nonBlocking) != 0)
		{
			printf("failed to set non-blocking\n");
			//return false;
		}
	}
}