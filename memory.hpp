#pragma once

#include "global.h"

// This include provides the malloc function
#include <iostream>

#define CHUNK_SIZE 32u
typedef btui32 CHUNK_BITVEC;
#define CHUNK_BUFFER_SIZE 32u
#define CHUNK_BUFFER_MAX_INDEX (CHUNK_SIZE * CHUNK_BUFFER_SIZE)

namespace mem
{
	//-------------------------------- BIT-VECTOR OPERATIONS

	// Bitvector (the safest iteration yet)
	template <class tcast, class trecv> class bv
	{
	//private:
	public:
		tcast bits = (tcast)0u; // Always start at zeroed
	public:

		inline bool get(trecv mask)
		{
			return (bits & (tcast)mask) != (tcast)0u;
		}
		inline void setto(trecv mask, bool to)
		{
			to ? bits |= (tcast)mask : bits &= ~(tcast)mask;
		}
		inline void toggle(trecv mask)
		{
			bits & (tcast)mask ? bits &= ~(tcast)mask : bits |= (tcast)mask;
		}
		inline void set(trecv mask)
		{
			bits |= (tcast)mask;
		}
		inline void unset(trecv mask)
		{
			bits &= ~(tcast)mask;
		}
	};

	// Get bit from any-size type
	template <class T> inline bool bvget(T flags, T modflag)
	{
		return (flags & modflag) != (T)0u;
	}
	// Set bit of any-size type to value
	template <class T> inline void bvsetto(T& flags, T modflag, bool to)
	{
		to ? flags |= modflag : flags &= ~modflag;
	}
	// Set bit of any-size type to value
	template <class typea, class typeb> inline void bvsetto2(typea& flags, typeb modflag, bool to)
	{
		to ? flags |= modflag : flags &= ~modflag;
	}
	// Set bit of any-size type
	template <class T> inline void bvset(T& flags, T modflag)
	{
		flags |= modflag;
	}
	// Unset bit of any-size type
	template <class T> inline void bvunset(T& flags, T modflag)
	{
		flags &= ~modflag;
	}

	//-------------------------------- BUFFER TYPES

	template <class type> class Chunk
	{
	public:
		type buffer[CHUNK_SIZE];
		CHUNK_BITVEC buffer_used = 0u;
	};
	template <class type> class CkBuffer
	{
	private:
		btui32 index_end = 0u;
		Chunk<type>* chunks[CHUNK_BUFFER_SIZE]{ nullptr };
		inline void DecrementEnd()
		{
			// test (is this the first element of this buffer)
			btui32 iChunk = index_end / CHUNK_SIZE;
			btui32 iElement = index_end - iChunk * CHUNK_SIZE;
			if (iElement == 0u) {
				delete chunks[iChunk];
				chunks[iChunk] = nullptr;
			}
			// Go back one step
			--index_end;
		}
	public:
		CkBuffer() {}
		~CkBuffer()
		{
			for (btui32 iChunk = 0u; iChunk < CHUNK_BUFFER_SIZE; ++iChunk)
				if (chunks[iChunk] != nullptr)
				{
					for (btui32 iElement = 0u; iElement < CHUNK_SIZE; ++iChunk)
						// If this space is used, call it's destructor
						if (bvget<CHUNK_BITVEC>(chunks[iChunk]->buffer_used, (CHUNK_BITVEC)(1u << iElement)))
							chunks[iChunk]->buffer[iElement].~type();
					// Delete the chunk
					delete (void*)chunks[iChunk];
				}
		}
		btui32 Add(type* element)
		{
			for (btui32 i = 0; i < CHUNK_BUFFER_MAX_INDEX; i++) // For every space in the buffer
			{
				btui32 iChunk = i / CHUNK_SIZE;
				btui32 iElement = i - iChunk * CHUNK_SIZE;
				// If this chunk is not created, create it
				if (chunks[iChunk] == nullptr) chunks[iChunk] = new Chunk<type>;
				// If this space is free, copy what we created into it
				if (!bvget(chunks[iChunk]->buffer_used, 1u << iElement))
				{
					bvset(chunks[iChunk]->buffer_used, 1u << iElement);
					chunks[iChunk]->buffer[iElement] = *element;
					delete (void*)element;
					if (i > index_end) index_end = i; // If we hit new ground, expand the end index
					return i; // End the loop
				}
			}
			return 0u;
		}
		void Remove(btui32 index)
		{
			// If within range (attempt to fix buffer overrun)
			if (index <= index_end)
			{
				btui32 iChunk = index / CHUNK_SIZE;
				btui32 iElement = index - iChunk * CHUNK_SIZE;
				bvunset(chunks[iChunk]->buffer_used, 1u << iElement);
				chunks[iChunk]->buffer[iElement].~type();
				if (index == index_end && index > 0u)
				{
					// Decrement index last (no point checking if it's not used, we already know)
					DecrementEnd();
					// Continue decrementing until we reach the next last full space
					while (!Used(index_end) && index_end > 0u) DecrementEnd();
				}
			}
		}
		bool Used(btui32 index)
		{
			btui32 iChunk = index / CHUNK_SIZE;
			btui32 iElement = index - iChunk * CHUNK_SIZE;
			if (chunks[iChunk] != nullptr)
				return bvget(chunks[iChunk]->buffer_used, 1u << iElement);
			else return false;
		}
		btui32 Size() { return index_end + 1u; }
		type& operator[](btui32 index)
		{
			btui32 iChunk = index / CHUNK_SIZE;
			btui32 iElement = index - iChunk * CHUNK_SIZE;
			return chunks[iChunk]->buffer[iElement];
		}
	};
	template <class type> class CkBuffer2
	{
	private:
		btui32 index_end = 0u;
		Chunk<type>* chunks[CHUNK_BUFFER_SIZE]{ nullptr };
		inline void DecrementEnd()
		{
			// test (is this the first element of this buffer)
			btui32 iChunk = index_end / CHUNK_SIZE;
			btui32 iElement = index_end - iChunk * CHUNK_SIZE;
			if (iElement == 0u) {
				delete chunks[iChunk];
				chunks[iChunk] = nullptr;
			}
			// Go back one step
			--index_end;
		}
	public:
		CkBuffer2() {}
		~CkBuffer2()
		{
			for (btui32 iChunk = 0u; iChunk < CHUNK_BUFFER_SIZE; ++iChunk)
				if (chunks[iChunk] != nullptr)
				{
					for (btui32 iElement = 0u; iElement < CHUNK_SIZE; ++iChunk)
						// If this space is used, call it's destructor
						if (bvget<CHUNK_BITVEC>(chunks[iChunk]->buffer_used, (CHUNK_BITVEC)(1u << iElement)))
							chunks[iChunk]->buffer[iElement].~type();
					// Delete the chunk
					delete (void*)chunks[iChunk];
				}
		}
		btui32 Add(type* element)
		{
			for (btui32 i = 0; i < CHUNK_BUFFER_MAX_INDEX; i++) // For every space in the buffer
			{
				btui32 iChunk = i / CHUNK_SIZE;
				btui32 iElement = i - iChunk * CHUNK_SIZE;
				// If this chunk is not created, create it
				if (chunks[iChunk] == nullptr) chunks[iChunk] = new Chunk<type>;
				// If this space is free, copy what we created into it
				if (!bvget(chunks[iChunk]->buffer_used, 1u << iElement))
				{
					bvset(chunks[iChunk]->buffer_used, 1u << iElement);
					chunks[iChunk]->buffer[iElement] = *element;
					delete (void*)element;
					if (i > index_end) index_end = i; // If we hit new ground, expand the end index
					return i; // End the loop
				}
			}
			return 0u;
		}
		void Remove(btui32 index)
		{
			// If within range (attempt to fix buffer overrun)
			if (index <= index_end)
			{
				btui32 iChunk = index / CHUNK_SIZE;
				btui32 iElement = index - iChunk * CHUNK_SIZE;
				bvunset(chunks[iChunk]->buffer_used, 1u << iElement);
				chunks[iChunk]->buffer[iElement].~type();
				if (index == index_end && index > 0u)
				{
					// Decrement index last (no point checking if it's not used, we already know)
					DecrementEnd();
					// Continue decrementing until we reach the next last full space
					while (!Used(index_end) && index_end > 0u) DecrementEnd();
				}
			}
		}
		bool Used(btui32 index)
		{
			btui32 iChunk = index / CHUNK_SIZE;
			btui32 iElement = index - iChunk * CHUNK_SIZE;
			if (chunks[iChunk] != nullptr)
				return bvget(chunks[iChunk]->buffer_used, 1u << iElement);
			else return false;
		}
		btui32 Size() { return index_end + 1u; }
		type& operator[](btui32 index)
		{
			btui32 iChunk = index / CHUNK_SIZE;
			btui32 iElement = index - iChunk * CHUNK_SIZE;
			return chunks[iChunk]->buffer[iElement];
		}
	};

	#undef CHUNK_SIZE
	#undef CHUNK_BITVEC
	#undef CHUNK_BUFFER_SIZE
	#undef CHUNK_BUFFER_MAX_INDEX

	template <class type> class Buffer64
	{
	private:
		btui32 index_end = 0u;
		btui64 used = 0u;
		type buffer[64u];
		inline void DecrementEnd()
		{
			// Go back one step
			--index_end;
		}
	public:
		btui32 Add(type element)
		{
			for (btui32 i = 0; i < 64u; i++) // For every space in the buffer
			{
				// If this space is free, copy what we created into it
				if (!bvget(used, (btui64)1u << (btui64)i))
				{
					bvset(used, (btui64)1u << (btui64)i);
					buffer[i] = element;
					if (i > index_end) index_end = i; // If we hit new ground, expand the end index
					return i; // End the loop
				}
			}
			return 0u;
		}
		void Remove(btui32 index)
		{
			// If within range (attempt to fix buffer overrun)
			if (index <= index_end)
			{
				bvunset(used, (btui64)1u << (btui64)index);
				if (index == index_end && index > 0u)
				{
					// Decrement index last (no point checking if it's not used, we already know)
					DecrementEnd();
					// Continue decrementing until we reach the next last full space
					while (!Used(index_end) && index_end > 0u) DecrementEnd();
				}
			}
		}
		bool Used(btui32 index)
		{
			return bvget(used, (btui64)1u << (btui64)index);
		}
		btui32 Size() { return index_end + 1u; }
		type& operator[](btui32 index) { return buffer[index]; }
	};

	template <class type> class Buffer32
	{
	private:
		btui32 index_end = 0u;
		btui32 used = 0u;
		type buffer[32u];
		inline void DecrementEnd()
		{
			// Go back one step
			--index_end;
		}
	public:
		btui32 Add(type element)
		{
			for (btui32 i = 0; i < 32u; i++) // For every space in the buffer
			{
				// If this space is free, copy what we created into it
				if (!bvget(used, (btui32)1u << (btui32)i))
				{
					bvset(used, (btui32)1u << (btui32)i);
					buffer[i] = element;
					if (i > index_end) index_end = i; // If we hit new ground, expand the end index
					return i; // End the loop
				}
			}
			return 0u;
		}
		void Remove(btui32 index)
		{
			// If within range (attempt to fix buffer overrun)
			if (index <= index_end)
			{
				bvunset(used, (btui32)1u << (btui32)index);
				if (index == index_end && index > 0u)
				{
					// Decrement index last (no point checking if it's not used, we already know)
					DecrementEnd();
					// Continue decrementing until we reach the next last full space
					while (!Used(index_end) && index_end > 0u) DecrementEnd();
				}
			}
		}
		bool Used(btui32 index)
		{
			return bvget(used, (btui32)1u << (btui32)index);
		}
		btui32 Size() 
		{
			// TODO: returns 1 on an empty array, needs to be fixed
			return index_end + 1u;
		}
		type& operator[](btui32 index) { return buffer[index]; }
	};

	template <class type> class Buffer
	{
	private:
		btui32 size = 0u;
		bool* used = nullptr;
		type* buf = nullptr;
	public:
		Buffer(btui32 _size = 16u)
		{
			used = new bool[_size];
			buf = new type[_size];
			size = _size;
		}
		~Buffer()
		{
			delete[] used;
			delete[] buf;
		}
		btui32 Size() { return size; }
		type& operator[](btui32 index)
		{
			return buf[index];
		}
	};

	// Fixed size object ID buffer
	struct objbuf
	{
		bool used[BUF_SIZE]{ false };
		btID index_end = 0;
		btID add(); // Assign new object this space on the buffer
		void remove(btID ID); // Clear this space on the buffer
	};

	// Fixed size object ID buffer, optimized for objects with short life cycles
	struct objbuf_caterpillar
	{
		bool used[BUF_SIZE]{ false };
		btID index_first = 0;
		btID index_last = 0;
		btID add(); // Assign new object this space on the buffer
		void remove(btID ID); // Clear this space on the buffer
	};

	// Type buffer, used for a cell's buffer of all contained objects, for example.
	// Don't define templates in the cpp file, but put the implementation of the functions in the header file
	// and leave your main function as it is. Templates get inlined by default. Therefore they are not visible
	//to the linker. And the file that contains main() cannot see the definition of the templates. Hence the error.

	template <class T> struct varbuf {
	private:
		//T values[2];
		T* t_mem; btui32 size_alloc; btui32 size_used;
		void resize(size_t s)
		{
			// old way
			///*
			T* items2 = (T*)realloc(t_mem, sizeof(T) * s);
			if (items2) {
				t_mem = items2;
				size_alloc = (btui32)s;
				std::cout << "IDBUF resized to size " << size_alloc << std::endl; std::cout << "IDBUF current usage: " << size_used << std::endl;
			}//*/

			// 'new' way
			/*
			T* items2 = new T[s];
			if (items2) {
				for (int i = 0; i <= size_used; i++)
					items2[i] = t_mem[i]; // slowly copy buffer over (seriously how is memcpy done?)
				delete t_mem; // Free old buffer
				t_mem = items2;
				size_alloc = s;
				//std::cout << "IDBUF resized to size " << size_alloc << ", current usage: " << size_used << std::endl;
			}//*/
		}
	public:
		varbuf() // Constructor
		{
			size_alloc = 4; size_used = 0; // initial size is 4
			//t_mem = (T*)malloc(sizeof(T) * size_alloc);
			t_mem = new T[size_alloc];
		}
		~varbuf() // Destructor
		{
			free(t_mem);
			//if (size_alloc > 0)
			//delete t_mem;
		}
		void add(T tnew)
		{
			if (size_alloc == size_used) resize(size_alloc * 2); // If we've overstepped our size, upscale
			t_mem[size_used++] = tnew; // Set this index and increment i_used
			//std::cout << "IDBUF added index " << size_used << std::endl;
		}
		void register_id(btui32 id)
		{
			if (id > size_used) // if index is higher
			{
				size_used = id;
				if (size_alloc == size_used) resize(size_alloc * 2); // If we've overstepped our size, upscale
			}
			//t_mem[size_used++] = tnew; // Set this index and increment i_used
			//std::cout << "IDBUF added index " << size_used << std::endl;
		}
		#define ERR_OOR //std::cout << "ERROR! buffer out of range!" << std::endl
		void remove(btui32 index)
		{
			if (index >= size_used) { ERR_OOR; return; } // Error, index too large
			t_mem[index] = nullptr; std::cout << "IDBUF deleted index " << index << std::endl; // Set this index to null
			for (int i = index; i < size_used - 1; ++i) { // For every index after this
				t_mem[i] = t_mem[i + 1]; t_mem[i + 1] = nullptr;
			} // Shift the next index's data back by one space
			--size_used; // Decrement number of used spaces
			if (size_used > 0 && size_used == size_alloc / 4) // If size is low enough
				resize(size_alloc / 2); // Resize to half size
		}
		void remove_noshift(btui32 index)
		{
			if (index >= size_used) { ERR_OOR; return; } // Error, index too large
			t_mem[index] = nullptr; std::cout << "IDBUF deleted index " << index << std::endl; // Set this index to null
			if (index == size_used - 1) // If this is the last one
				--size_used; // Decrement number of used spaces
			if (size_used > 0 && size_used == size_alloc / 4) // If size is low enough
				resize(size_alloc / 2); // Resize to half size
		}
		#undef ERR_OOR

		T* get(btui32 index)
		{
			return &t_mem[index];
		}
		btui32 size()
		{
			return size_used;
		}

		T& operator[] (btui32 x)
		{
			if (x >= 0 && x < size_used)
				return t_mem[x];
			//std::cout << "ERROR! IDBUF 'operator[]' out of range!" << std::endl;
			return t_mem[0];
		}
	};

	#define IDBUF_SIZE 16u
	// Automatic unfixed size object ID buffer 
	struct idbuf
	{
	private:
		btID ptr_id[IDBUF_SIZE]{ ID_NULL,ID_NULL,ID_NULL,ID_NULL,ID_NULL,ID_NULL,ID_NULL,ID_NULL,ID_NULL,ID_NULL,ID_NULL,ID_NULL,ID_NULL,ID_NULL,ID_NULL,ID_NULL };
		bool ptr_used[IDBUF_SIZE]{ false };
		btui32 id_end = 0u;
	public:
		idbuf(); // Constructor
		~idbuf(); // Destructor
		void add(btID ID);
		void remove(btID ID);
		void clear();
		btui32 end();
		btID operator[] (btui32 x);
	};
}
