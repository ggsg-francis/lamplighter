#ifndef MEMORY_H
#define MEMORY_H

#include "global.h"

// This include provides the malloc function
#include <iostream>
//#include "C:\MinGW\lib\gcc\mingw32\9.2.0\include\c++\iostream"

namespace mem
{
	//-------------------------------- BIT-VECTOR OPERATIONS

	// Bitvector (the safest iteration yet)
	template <class tcast, class trecv> class bv
	{
	public:
		tcast bits = (tcast)0u; // Always start at zeroed
	public:
		inline bool get(trecv mask) {
			return (bits & (tcast)mask) != (tcast)0u;
		}
		inline void setto(trecv mask, bool to) {
			to ? bits |= (tcast)mask : bits &= ~(tcast)mask;
		}
		inline void toggle(trecv mask) {
			bits & (tcast)mask ? bits &= ~(tcast)mask : bits |= (tcast)mask;
		}
		inline void set(trecv mask) {
			bits |= (tcast)mask;
		}
		inline void unset(trecv mask) {
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

	template <typename Type> class Buffer64
	{
	private:
		btui32 index_end = 0u;
		btui64 used = 0u;
		Type buffer[64u];
		inline void DecrementEnd()
		{
			// Go back one step
			--index_end;
		}
	public:
		btui32 Add(Type element)
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
		Type& operator[](btui32 index) { return buffer[index]; }
	};

	template <typename Type> class Buffer32
	{
	private:
		btui32 index_end = 0u;
		btui32 used = 0u;
		Type buffer[32u];
		inline void DecrementEnd()
		{
			// Go back one step
			--index_end;
		}
	public:
		btui32 Add(Type element)
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
		Type& operator[](btui32 index) { return buffer[index]; }
	};

	// Fixed size object ID buffer (except this one holds its own data)
	// TODO: store actual data in a separate 'compressed' private buffer
	template <typename DataType, typename Type_Signifier, Type_Signifier type_null, int SIZE> struct ObjBuf
	{
	private:
		DataType data[SIZE];
		Type_Signifier type[SIZE];
	public:
		ObjBuf() {
			for (int i = 0; i < SIZE; ++i) {
				type[i] = type_null;
			}
		}
	public:
		bool Used(int index) {
			return type[index] != type_null;
		}
		btID index_end = 0;
		// Assign new object this space on the buffer
		btID Add(Type_Signifier _type) {
			for (btID i = 0; i < SIZE; i++) { // For every space in the buffer
				if (!Used(i)) { // If this space is free
					type[i] = _type;
					if (i > index_end) index_end = i; // If we hit new ground expand the end index
					return i; // End the loop
				}
			}
			return BUF_NULL;
		}
		// Clear this space on the buffer
		void Remove(btID index) {
			type[index] = type_null;
			if (index == index_end) {
				--index_end; // Go back one step
				while (!Used(index_end)) --index_end; // Continue rolling back until we reach the next last full space
			}
		}
		DataType& Data(int index) {
			return data[index];
		}
		Type_Signifier Type(int index) {
			return type[index];
		}
		Type_Signifier* TypeRW() {
			return (Type_Signifier*)&type;
		}
	};

	// Fixed size object ID buffer (except this one holds its own data)
	// TODO: store actual data in a separate 'compressed' private buffer
	template <typename DataType, typename Type_Signifier, Type_Signifier type_null, int SIZE> struct ObjBuf2
	{
	private:
		DataType data[SIZE];
	public:
		btui32 useBV[SIZE / 32];
	public:
		bool Used(btui32 index) {
			btui32 bvIndex = index / 32u;
			return bvget<btui32>(useBV[bvIndex], (btui32)1u << (btui32)index - (bvIndex * 32u));
		}
		btID index_end = 0;
		// Assign new object this space on the buffer
		btID Add(Type_Signifier _type) {
			for (btID i = 0; i < SIZE; i++) { // For every space in the buffer
				if (!Used(i)) { // If this space is free
					btui32 bvIndex = i / 32u;
					mem::bvset<btui32>(useBV[bvIndex], (btui32)1u << (btui32)i - (bvIndex * 32u));
					if (i > index_end) index_end = i; // If we hit new ground expand the end index
					return i; // End the loop
				}
			}
			return BUF_NULL;
		}
		// Clear this space on the buffer
		void Remove(btID index) {
			btui32 bvIndex = index / 32u;
			mem::bvunset<btui32>(useBV[bvIndex], (btui32)1u << (btui32)index - (bvIndex * 32u));
			if (index == index_end) {
				--index_end; // Go back one step
				while (!Used(index_end)) --index_end; // Continue rolling back until we reach the next last full space
			}
		}
		DataType& Data(int index) {
			return data[index];
		}
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

#endif
