#pragma once

#include "global.h"

// Maximum IPv4 size: 576 bytes
#define PACKET_SIZE 256

enum paktype : btui8
{
	ePING, // Does nothing

	eCLIENT_CONNECT, // Sent by client on connection
	eSERVER_CONNECT_CLIENT, // When a client connects, send it a message telling it its ID
	eSERVER_DISCONNECT_CLIENT, // If there's a version incompatibility, send this

	eSPAWN_PROJECTILE, // Spawn projectile

	eSET_ENT_POSE, // Set entity position and rotation
	eSET_CHARA_ANIM, // Set character animation
	eSET_CHARA_STATS, // Player set properties like health
};

struct packet_wrap
{
	paktype type;
	char msg[PACKET_SIZE - sizeof(char)] = { 0 };
};

namespace msg
{
	//....................................... CONNECTION

	struct ClientConnect
	{
		btui32 version_major;
		btui32 version_minor;
	};
	struct ServerConnectClient // Struct the server sends back to tell you your client info
	{
		btui8 id;
	};
	struct ServerDisconnectClient // Struct the server sends back to tell you 'pis off idiot wrong version number'
	{
		unsigned int reason;
	};

	//....................................... PROJECTILE

	struct SpawnProj // Spawn projectile
	{
		btf32 px, py, ph; // Position vector
		btf32 dx, dy; // Direction vector
	};

	//....................................... PLAYER

	struct SetEntPose // Set pose of entity
	{
		btui8 id;
		btf32 px, py, yaw; // Player position
		btf32 vyaw, vpit; // Player horizontal & vertical view
		//SetEntPose(btui8 _id, btf32  _px, btf32  _py, btf32 _yaw, btf32 _vyaw, btf32 _vpit)
		//	: id{ _id }, px{ _px }, py{ _py }, yaw{ _yaw }, vyaw{ _vyaw }, vpit{ _vpit } {}
	};
	/*void ConstructSetEntPose(packet_wrap& packet, SetEntPose message)
	{
	packet.type = eSET_ENT_POSE;
	memcpy(packet.msg, &message, sizeof(SetEntPose));
	}*/
	struct SetCharaAnim // Set animation of player
	{
		btui8 id;
		btf32 px, py, yaw; // Player position
		btf32 vyaw, vpch; // Player view yaw & pitch
		btui8 ani_id_lower; // Animation ID
	};
	struct set_unit_stats // Player stats
	{
		btui8 id;
		btf32 hp;
	};
}