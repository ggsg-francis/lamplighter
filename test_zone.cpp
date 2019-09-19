#include "test_zone.h"

#include "memory.hpp"
//#include "maths.hpp"

void InitTest()
{
	struct str
	{
		btui16 i = 0ui16;
		btui16 var = 0ui16;
	};

	mem::varbuf<str> buf;
	buf.register_id(4);
	buf[0].i = 1ui16;
	buf[1].i = 5ui16;
	buf[2].i = 8ui16;
	buf[3].i = 17ui16;

	//buf.add(1);
	//buf.add(5);
	//buf.add(8);
	//buf.add(17);
	//buf.add(18);
	//buf.add(19);

	btui16 i1 = buf[0].i;
	btui16 i2 = buf[1].i;
	btui16 i3 = buf[2].i;
	btui16 i4 = buf[3].i;

	str i5 = buf[3];

	int ii = 0;
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