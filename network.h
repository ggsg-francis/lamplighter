#ifndef NETWORK_H
#define NETWORK_H

#include "global.h"

#ifdef DEF_NMP

// Maximum IPv4 size: 576 bytes
#define PACKET_SIZE 256

// From Quake
#define	MAXHOSTNAMELEN 256

enum paktype : btui8
{
	ePING, // Does nothing

	eCLIENT_CONNECT_REQUEST, // Sent by client on connection
	eSERVER_CONNECT_CONFIRM, // When a client connects, send it a message telling it its ID
	eSERVER_DISCONNECT_CLIENT, // If there's a version incompatibility, send this

	eINPUT_BUFFER,
};

typedef btui8 btPacket[PACKET_SIZE];

namespace network
{
	// Network ID
	extern btui8 nid;

	// Receive and handle any available packets
	void RecvTCP();
	//
	bool SendInputBuffer();
	// Send a reliable message across the network
	bool SendTCP(btPacket* PACKET);
	// Initialize and connect. Returns true if succeeded
	bool Init();
	//
	void End();
}
#endif // DEF_NMP

#endif
