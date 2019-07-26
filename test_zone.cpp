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