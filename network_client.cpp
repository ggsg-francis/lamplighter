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


	//network id
	btui8 nid = 0ui8;

	//-------------------------------- ENCODING AND DECODING

	//networking stuff
	SOCKET connHandle;
	//message from server
	char servMessage[PACKET_SIZE] = "NO CONNECTION";

	//________________________________________________________________________________________________________________________________
	//-------------------------------- UTILITY

	// net_wins.c line 73
	bool GetMyAddr()
	{
		// Get this PC's name on the network
		char hostname[MAXHOSTNAMELEN];
		if (gethostname(hostname, MAXHOSTNAMELEN) == SOCKET_ERROR) {
			std::cerr << "Error " << WSAGetLastError() <<
				" when getting local host name." << std::endl;
			return false;
		}
		hostname[MAXHOSTNAMELEN - 1] = 0; // Force null terminate. Quake does this, so I will
		std::cout << "Host name is " << hostname << "." << std::endl;
		// Get Address from Name
		hostent* host_details = gethostbyname(hostname);
		if (host_details == NULL) {
			std::cerr << "Error: Bad host lookup." << std::endl;
			return false;
		}
		// Copy first address
		memcpy(&myAddr, host_details->h_addr_list[0], sizeof(in_addr));
		std::cout << "Address " << 0 << ": " << inet_ntoa(myAddr) << std::endl;
		/*for (int i = 0; host_details->h_addr_list[i] != 0; ++i) {
		in_addr addr;
		memcpy(&addr, host_details->h_addr_list[i], sizeof(in_addr));
		std::cout << "Address " << i << ": " << inet_ntoa(addr) << std::endl;
		}*/
		return true;
	}

	// net_wins.c line 369
	static int MakeSocketBroadcastCapable(SOCKET socketid)
	{
		int	i = 1;
		// Make this socket broadcast capable
		if (setsockopt(socketid, SOL_SOCKET, SO_BROADCAST, (char*)&i, sizeof(i)) == SOCKET_ERROR)
		{
			//int err = SOCKETERRNO;
			//Con_SafePrintf("UDP, setsockopt: %s\n", socketerror(err));
			return -1;
		}
		net_broadcastsocket = socketid;
		return 0;
	}

	//________________________________________________________________________________________________________________________________
	//-------------------------------- WRITE

	int Write(SOCKET socketid, byte* buf, int len, sockaddr* addr)
	{
		int	ret;
		ret = sendto(socketid, (char*)buf, len, 0, (sockaddr*)addr,
			sizeof(sockaddr));
		if (ret == SOCKET_ERROR)
			int bp = 0;
		return ret;
	}

	int Broadcast(SOCKET socketid, byte *buf, int len)
	{
		int	ret;
		if (socketid != net_broadcastsocket)
		{
			if (net_broadcastsocket != 0) {
				//Sys_Error("Attempted to use multiple broadcasts sockets");
			} //GetMyAddr();
			ret = MakeSocketBroadcastCapable(socketid);
			if (ret == -1)
			{
				//Con_Printf("Unable to make socket broadcast capable\n");
				return ret;
			}
		}
		return Write(socketid, buf, len, (sockaddr*)&broadcastaddr);
	}

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
		// Get this PC's network address
		GetMyAddr();
	}

	void Connect()
	{
		unsigned short port = (short)cfg::iIPPORT;

		std::uint32_t ip_address_connect = (cfg::iIPA << 24) | (cfg::iIPB << 16) | (cfg::iIPC << 8) | cfg::iIPD;




		SOCKADDR_IN addr;
		int addrlength = sizeof(addr);
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(ip_address_connect);
		//addr.sin_addr.s_addr = INADDR_BROADCAST;
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
#endif // DEF_NMP
