#include "memoryC.h"

typedef btui8 bool;
#define false 0u
#define true 1u
typedef unsigned long long btsize;

// Get bit from any-size type
bool bvget(btui32 flags, btui32 modflag)
{
	return (flags & modflag) != 0u;
}
/*
// Set bit of any-size type to value
inline void bvsetto(btui32* flags, btui32 modflag, bool to)
{
	to ? *flags |= modflag : flags &= ~modflag;
}
// Set bit of any-size type to value
inline void bvsetto2(btui32& flags, btui32 modflag, bool to)
{
	to ? flags |= modflag : flags &= ~modflag;
}
// Set bit of any-size type
inline void bvset(btui32& flags, btui32 modflag)
{
	flags |= modflag;
}
// Unset bit of any-size type
inline void bvunset(btui32& flags, btui32 modflag)
{
	flags &= ~modflag;
}*/

#define BUF_IMPL_C \
btID BUF_NAME_INI(BUF_NAME* const buf) {\
 for (btui32 i = 0u; i < BUF_NMBR; ++i)\
  buf->used[i] = false;\
 buf->index_end = 0u; buf->size = buf->index_end + 1u; }\
btID BUF_NAME_ADD(BUF_NAME* const buf, BUF_TYPE add) {\
 for (btID i = 0; i < BUF_NMBR; i++) {\
  if (!buf->used[i]) {\
   buf->used[i] = true; buf->buffer[i] = add;\
   if (i > buf->index_end) buf->index_end = i;\
   return i;\
 }} return BUF_NULL; }\
void BUF_NAME_RMV(BUF_NAME* const buf, btID i) {\
 buf->used[i] = false;\
 if (i == buf->index_end) {\
  --buf->index_end;\
  while (!buf->used[buf->index_end]) --buf->index_end; }}

#define BUF_NAME Buf64ID
#define BUF_NAME_INI Buf64_ini
#define BUF_NAME_ADD Buf64_add
#define BUF_NAME_RMV Buf64_rmv
#define BUF_TYPE btID
#define BUF_NMBR 64u

BUF_IMPL_C

#undef BUF_NAME
#undef BUF_NAME_INI
#undef BUF_NAME_ADD
#undef BUF_NAME_RMV
#undef BUF_TYPE
#undef BUF_NMBR

btID ObjBuf_init(ObjBuf* const buf)
{
	for (btui32 i = 0u; i < BUF_SIZE; ++i)
		buf->used[i] = false;
	buf->index_end = 0u;
}
btID ObjBuf_add(ObjBuf* const buf)
{
	for (btID i = 0; i < BUF_SIZE; i++) // For every space in the buffer
	{
		if (!buf->used[i]) // If this space is free
		{
			buf->used[i] = true;
			if (i > buf->index_end) buf->index_end = i; // If we hit new ground expand the end index
			return i; // End the loop
		}
	}
	return BUF_NULL;
}
void ObjBuf_remove(ObjBuf* const buf, btID i)
{
	buf->used[i] = false;
	if (i == buf->index_end)
	{
		--buf->index_end; // Go back one step
		while (!buf->used[buf->index_end]) --buf->index_end; // Continue rolling back until we reach the next last full space
	}
}

btID ObjBufCP_init(ObjBufCP* const buf)
{
	for (btui32 i = 0u; i < BUF_SIZE; ++i)
		buf->used[i] = false;
	buf->index_first = 0u;
	buf->index_last = 0u;
}
btID ObjBufCP_add(ObjBufCP* const buf)
{
	buf->used[buf->index_last] = true; // Index is taken
	btID index = buf->index_last; // Store index for return value
	++buf->index_last; // Iterate
	if (buf->index_last == BUF_SIZE) // Have we overrun?
		buf->index_last = 0; // Wrap around
	return index; // Return added ID
}
void ObjBufCP_remove(ObjBufCP* const buf, btID i)
{
	buf->used[i] = false;
	if (i == buf->index_first) // If this is the first element in the buffer
	{
		++buf->index_first; // Iterate
		if (buf->index_first == BUF_SIZE) // Have we overrun?
			buf->index_first = 0; // Wrap around
		while (!buf->used[buf->index_first]) // iterate until we reach the new first
		{
			if (buf->index_first == buf->index_last) break; // If we hit index_last, that means the buffer is empty and nothing can be removed
			++buf->index_first; // Iterate
			if (buf->index_first == BUF_SIZE) // Have we overrun?
				buf->index_first = 0; // Wrap around
		}
	}
}

#define CHUNK_SIZE 32u
#define CHUNK_BITVEC btui32
#define CHUNK_BUFFER_SIZE 32u
#define CHUNK_BUFFER_MAX_INDEX (CHUNK_SIZE * CHUNK_BUFFER_SIZE)

#define DYN_ARRAY_INITIAL_SIZE 4

typedef struct
{
	btui32 size;
	btui32 capacity;
	btui8* buffer;
} DynArray;

void DynArrayInit(DynArray* da, int elem_size)
{
	da->size = 0u;
	da->capacity = DYN_ARRAY_INITIAL_SIZE;
	da->buffer = malloc(DYN_ARRAY_INITIAL_SIZE * elem_size);
}

void DynArraySet(DynArray *da, int index, void* value, int elem_size)
{
	// Check if a resize is necessary
	if (index >= da->capacity)
	{
		btui32 new_capacity = da->size * 2u;
		realloc(da->buffer, new_capacity * elem_size);
		da->capacity = new_capacity;
	}
	// Set the element at this index
	memcpy(da->buffer[index * elem_size], value, elem_size);
	if (index > da->size) da->size = index;
}

void DynArrayEnd(DynArray* da)
{
	free(da->buffer);
}

typedef struct
{
	btui32 index_end;
	btui8* buffer[CHUNK_BUFFER_SIZE];
	CHUNK_BITVEC buffer_used[CHUNK_BUFFER_SIZE];
} CkBuffer_c;

btui32 CkBufferAdd(CkBuffer_c* ckbuf, void* element)
{
	for (btui32 i = 0; i < CHUNK_BUFFER_MAX_INDEX; i++) // For every space in the buffer
	{
		btui32 iChunk = i / CHUNK_SIZE;
		btui32 iElement = i - iChunk * CHUNK_SIZE;
		// If this chunk is not created, create it
		if (!ckbuf->buffer[iChunk])
		{
			ckbuf->buffer[iChunk] = (btui8*)malloc(CHUNK_SIZE);
		}
		// If this space is free, copy what we created into it
		if (!bvget(ckbuf->buffer_used[iChunk], 1u << iElement))
		{
			//bvset(ckbuf->buffer_used[iChunk], 1u << iElement);
			ckbuf->buffer[iChunk][iElement] = (btui8*)element;
			free(element);
			if (i > ckbuf->index_end) ckbuf->index_end = i; // If we hit new ground, expand the end index
			return i; // End the loop
		}
	}
	return 0u;
}

#undef CHUNK_SIZE
#undef CHUNK_BITVEC
#undef CHUNK_BUFFER_SIZE
#undef CHUNK_BUFFER_MAX_INDEX
