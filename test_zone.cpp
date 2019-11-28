#include "test_zone.h"

#include "memory.hpp"
//#include "maths.hpp"

void InitTest()
{
	// Test things here
	btf32 f = INFINITY;
	size_t size1 = sizeof(short);
	size_t size2 = sizeof(short int);
	size_t size3 = sizeof(int);
	size_t size4 = sizeof(long);
	size_t size5 = sizeof(long long);
	size_t size6 = sizeof(long long);
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