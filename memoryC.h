#ifndef MEMORY_H
#define MEMORY_H

#ifdef __cplusplus
extern "C" {
	#endif

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
