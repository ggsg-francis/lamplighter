#include "test_zone.h"

#include "memory.hpp"
//#include "maths.hpp"

void InitTest()
{
	// Test things here
}

struct p_settarget
{
	btui8 id;
	btui16 px;
	btui16 py;
};

// Maximum IPv4 size: 576 bytes

struct packet // can set pose of so many units in one packet
{
	btui8 msgtype;
	btui8 spcount;
	p_settarget sp[95];
};

size_t sz = sizeof(p_settarget);
size_t sz2 = sizeof(packet);