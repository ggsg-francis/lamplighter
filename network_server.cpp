#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <iostream>
#include <string>
#include <thread>
#include <fstream>

#include "network_common.h"

#define NUM_CONNECTIONS 2
//default local: 127.0.0.1
#define IP_ADDR "127.0.0.1"

//socket to use to listen for connections
SOCKET socket_connection_listen;

//SOCKET newConnection; //socket to hold the client's connection
SOCKET sockets[NUM_CONNECTIONS];
bool socketsUsed[NUM_CONNECTIONS];

//TO DO: DUPLICATE OF SEND IN NETWORK.H ON CLIENT CODE
// Send a struct formatted message across the network
void SendMsgServer(unsigned int socknum, paktype type, void* msg)
{
	void* buffer = malloc(PACKET_SIZE); //allocate memory to write our packet buffer into
	packet_wrap pw; //create wrapper class to contain the message
	pw.type = type; //set wrapper message type to correct type
	memcpy(pw.msg, msg, PACKET_SIZE - sizeof(char)); //copy the message struct into this wrapper class
	memcpy(buffer, &pw, PACKET_SIZE); //copy the wrapper class into the buffer for sending
	send(sockets[socknum], (char*)buffer, PACKET_SIZE, NULL); //send the buffer
	free(buffer); //clear the buffer from memory
}

bool listenConnections(void)
{
	SOCKADDR_IN addr; //address that we will bind out listening socket to

	int addrlength = sizeof(addr); //length of the address (required for accept call)
	//addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //broadcast locally
	//new non-deprecated address set
	std::uint32_t ip_address;
	inet_pton(AF_INET, IP_ADDR, &ip_address);
	addr.sin_addr.s_addr = ip_address;

	addr.sin_port = htons(1111); //port
	addr.sin_family = AF_INET; //ipv4 socket, use AF_INET6 for ipv6, if needed

	// Create socket
	socket_connection_listen = socket(AF_INET, SOCK_STREAM, NULL); //create socket to listen for new connections
	bind(socket_connection_listen, (SOCKADDR*)&addr, sizeof(addr)); //bind the new address to the socket
	//this is necessary for people to connect to this server
	listen(socket_connection_listen, SOMAXCONN); //places slisten socket in a state in which it is listening for an incoming connection

	for (int i = 0; i < NUM_CONNECTIONS; i++)
	{
		sockets[i] = accept(socket_connection_listen, (SOCKADDR*)&addr, &addrlength); //accept a new connection
		if (sockets[i] == 0) //if connection failed
		{
			std::cout << "Failed to accept the client's connection." << std::endl;
		}
		else //if client connection is accepted
		{
			std::cout << "Client " << i << " connected!" << std::endl;
			char servMessage[256] = "Hey Client!"; //message from the server buffer
			send(sockets[i], servMessage, sizeof(servMessage), NULL); //sent motd buffer
			socketsUsed[i] = true;
			// set socket to non-blocking
			DWORD nonBlocking = 1;
			if (ioctlsocket(sockets[i], FIONBIO, &nonBlocking) != 0)
			{
				printf("failed to set non-blocking\n");
			}
		}
	}
	return TRUE;
}

void ReceiveSocket(int socknum)
{
	char msg[PACKET_SIZE];
	int recvd = recv(sockets[socknum], msg, sizeof(msg), NULL); //receive message from server
	if (recvd > 0)
	{
		//char c;
		packet_wrap pw;
		memcpy(&pw, msg, PACKET_SIZE);
		switch (pw.type)
		{
		case eCLIENT_CONNECT:
			msg::ClientConnect mcc;
			memcpy(&mcc, pw.msg, sizeof(msg::ClientConnect));
			if (mcc.version_major == VERSION_MAJOR && mcc.version_minor == VERSION_BUILD)
			{
				std::cout << "VERSION NUMBER ACCEPTED" << std::endl;
				msg::ServerConnectClient m;
				m.id = (btui8)socknum;
				SendMsgServer(socknum, eSERVER_CONNECT_CLIENT, &m);
			}
			else
			{
				std::cout << "BAD VERSION NUMBER" << std::endl;
				msg::ServerDisconnectClient m;
				m.reason = VERSION_MAJOR;
				SendMsgServer(socknum, eSERVER_DISCONNECT_CLIENT, &m);
			}
			break;
		case eSPAWN_PROJECTILE:
			//msg::SpawnProj msp;
			//memcpy(&msp, pw.msg, sizeof(msg::SpawnProj));
			//std::cout << "MSG from " << socknum << " SPAWNPROJ: " << msp.id << std::endl;
			//index::CreateProjectile2(msp.id, 0, fw::Vector3(msp.px, msp.py, msp.pz), fw::Vector3(msp.dx, msp.dy, msp.dz));
			break;
		default:
			break;
		}
		//relay the message...
		for (int i = 0; i < NUM_CONNECTIONS; i++)
		{
			if (i != socknum && socketsUsed[i] == true)
				send(sockets[i], msg, sizeof(msg), NULL); //sent motd buffer
		}
	}
}

int main()
{
	//initialize winsock
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0)
	{
		MessageBoxA(NULL, "Winsock startup failed", "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}

	std::thread connections(listenConnections);

	while (true)
	{
		for (int i = 0; i < NUM_CONNECTIONS; i++)
		{
			//incoming[i] = std::thread(listenSocket, i);
			ReceiveSocket(i);
		}
	}

	connections.detach();
	return 0;
}