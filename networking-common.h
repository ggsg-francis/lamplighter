#pragma once

#include "global.h"

#define PACKET_SIZE 256

typedef unsigned int NW_ID;

enum paktype
{
	MSG_CLIENT_CONNECT, //Sent by client on connection
	SERVER_CONNECT_CLIENT, //when a client connects, send it a message telling it its ID
	SERVER_DISCONNECT_CLIENT, //if there's some incompatibility, send this

	SPAWN_PROJECTILE, //spawn projectile

	SET_UNIT_POSE, //Set player position and rotation
	MSG_SET_UNIT_STATS, //player set properties like health

	//unused packets

	//MSG_CLIENT_DISCONNECT,
	//PAK_TYPE_SETPOS,
	//PAK_TYPE_ACTIVATE,
	//PAK_TYPE_SPAWN,
	//PAK_TYPE_CHATENTRY,
	//PAK_TYPE_CONNECT,
	//PAK_TYPE_DISCONNECT
};

struct packet_wrap
{
	paktype type;
	char msg[PACKET_SIZE - sizeof(char)] = { 0 };
};

namespace msg
{
	//....................................... CONNECTION

	struct client_connect
	{
		unsigned int version_major;
		unsigned int version_minor;
	};

	//sttuct the server sends back to tell you your client info
	struct server_connect_client
	{
		btui8 id;
	};

	struct server_disconnect_client
	{
		unsigned int reason;
	};

	//....................................... PROJECTILE

	//spawn projectile
	struct spawn_projectile
	{
		//id number of this projectile
		NW_ID id;
		//Position vector
		float px, py, pz;
		//Direction vector
		float dx, dy, dz;
	};

	//....................................... PLAYER

	//set stats of player
	struct set_unit_pose
	{
		btui8 id;
		//player position
		float px, py, yaw;
		//player horizontal view
		float vh;
		//player vertical view
		float vv;
	};

	//PLAYER STATS
	struct set_unit_stats
	{
		char id;
		float hp;
	};
}

//....................................... TEST ZONE

//struct will be used to set a character to move gradually instead of snapping to positions
struct msg_setmovement
{
	//temp
};

//perform action on object
struct msg_activate
{
	char id;
	int action;
};

struct msg_spawn
{
	char id;
	float x, y, z;
};