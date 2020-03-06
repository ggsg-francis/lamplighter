#pragma once

#ifdef DEF_NMP

enum paktype : btui8;

namespace network
{
	extern btui8 nid;

	//-------------------------------- ENCODING AND DECODING

	////networking stuff
	//SOCKET connHandle;
	////message from server
	//char servMessage[PACKET_SIZE] = "NO CONNECTION";

	void Recv2(int type);

	// Send a struct formatted message across the network
	void SendMsg(paktype type, void* msg);

	void Init();
	void Connect();
}
#endif // DEF_NMP
