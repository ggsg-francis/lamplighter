#ifndef MEMORY_H
#define MEMORY_H

#ifdef __cplusplus
extern "C" {
	#endif

	#define BUF_IMPL_H \
	typedef struct BUF_NAME\
	{\
		btui8 used[BUF_NMBR];\
		BUF_TYPE buffer[BUF_NMBR];\
		btui32 index_end, size;\
	} BUF_NAME;\
	btID BUF_NAME_INI(BUF_NAME* const buf);\
	btID BUF_NAME_ADD(BUF_NAME* const buf, BUF_TYPE add);\
	void BUF_NAME_RMV(BUF_NAME* const buf, btID i);

	#define BUF_NAME Buf64ID
	#define BUF_NAME_INI Buf64_ini
	#define BUF_NAME_ADD Buf64_add
	#define BUF_NAME_RMV Buf64_rmv
	#define BUF_TYPE btID
	#define BUF_NMBR 64u

	BUF_IMPL_H

	#undef BUF_NAME
	#undef BUF_NAME_INI
	#undef BUF_NAME_ADD
	#undef BUF_NAME_RMV
	#undef BUF_TYPE
	#undef BUF_NMBR

	// Fixed size object ID buffer
	typedef struct ObjBuf
	{
		btui8 used[BUF_SIZE];
		btID index_end;
	} ObjBuf;

	btID ObjBuf_init(ObjBuf* const buf);
	btID ObjBuf_add(ObjBuf* const buf);
	void ObjBuf_remove(ObjBuf* const buf, btID i);

	// Fixed size object ID 'Caterpillar' buffer
	typedef struct ObjBufCP
	{
		btui8 used[BUF_SIZE];
		btID index_first;
		btID index_last;
	} ObjBufCP;

	btID ObjBufCP_init(ObjBufCP* const buf);
	btID ObjBufCP_add(ObjBufCP* const buf);
	void ObjBufCP_remove(ObjBufCP* const buf, btID i);

	#ifdef __cplusplus
}
#endif

#endif // END OF FILE
