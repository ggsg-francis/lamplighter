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

	template <typename Type> class Buffer64 {
	private:
		lui32 index_end = 0u;
		lui64 used = 0u;
		Type buffer[64u];
		inline void DecrementEnd() {
			// Go back one step
			--index_end;
		}
	public:
		lui32 Add(Type element) {
			for (lui32 i = 0; i < 64u; i++) { // For every space in the buffer
				// If this space is free, copy what we created into it
				if (!bvget(used, (lui64)1u << (lui64)i)) {
					bvset(used, (lui64)1u << (lui64)i);
					buffer[i] = element;
					if (i > index_end) index_end = i; // If we hit new ground, expand the end index
					return i; // End the loop
				}
			}
			return 0u;
		}
		void Remove(lui32 index) {
			// If within range (attempt to fix buffer overrun)
			if (index <= index_end) {
				bvunset(used, (lui64)1u << (lui64)index);
				if (index == index_end && index > 0u) {
					// Decrement index last (no point checking if it's not used, we already know)
					DecrementEnd();
					// Continue decrementing until we reach the next last full space
					while (!Used(index_end) && index_end > 0u) DecrementEnd();
				}
			}
		}
		bool Used(lui32 index) {
			return bvget(used, (lui64)1u << (lui64)index);
		}
		lui32 Size() { return index_end + 1u; }
		Type& operator[](lui32 index) { return buffer[index]; }
	};

	template <typename Type> class Buffer32
	{
	private:
		lui32 index_end = 0u;
		lui32 used = 0u;
		Type buffer[32u];
		inline void DecrementEnd() {
			// Go back one step
			--index_end;
		}
	public:
		lui32 Add(Type element) {
			for (lui32 i = 0; i < 32u; i++) { // For every space in the buffer
				// If this space is free, copy what we created into it
				if (!bvget(used, (lui32)1u << (lui32)i)) {
					bvset(used, (lui32)1u << (lui32)i);
					buffer[i] = element;
					if (i > index_end) index_end = i; // If we hit new ground, expand the end index
					return i; // End the loop
				}
			}
			return 0u;
		}
		void Remove(lui32 index) {
			// If within range (attempt to fix buffer overrun)
			if (index <= index_end) {
				bvunset(used, (lui32)1u << (lui32)index);
				if (index == index_end && index > 0u) {
					// Decrement index last (no point checking if it's not used, we already know)
					DecrementEnd();
					// Continue decrementing until we reach the next last full space
					while (!Used(index_end) && index_end > 0u) DecrementEnd();
				}
			}
		}
		bool Used(lui32 index) {
			return bvget(used, (lui32)1u << (lui32)index);
		}
		lui32 Size() {
			// TODO: returns 1 on an empty array, needs to be fixed
			return index_end + 1u;
		}
		Type& operator[](lui32 index) { return buffer[index]; }
	};

	typedef struct LtrID {
	private:
		lui64 guid;
	public:
		LtrID(lui32 _index, lui32 _instance) :
			guid{ (lui64)_index | ((lui64)_instance << (lui64)32u) } {};
		lui32 Index() {
			return (lui32)(guid & (lui64)0b0000000000000000000000000000000011111111111111111111111111111111u);
		};
		lui32 Instance() {
			return (lui32)((guid & (lui64)0b1111111111111111111111111111111100000000000000000000000000000000u) >> 32u);
		};
		lui64 GUID() {
			return guid;
		};
	} LtrID;

	// Fixed size object ID buffer (except this one holds its own data)
	// TODO: fast iteration
	template <typename DataType, typename Type_Signifier, Type_Signifier type_null, lui32 SIZE> struct ObjBuf
	{
	private:
		DataType data[SIZE];
		Type_Signifier type[SIZE];
		//lui32 instance[SIZE];
		lui32 size = 0u;
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
		// Assign new object this space on the buffer
		lid Add(Type_Signifier _type) {
			for (lui32 i = 0; i < SIZE; i++) { // For every space in the buffer
				if (Used(i)) continue;
				type[i] = _type;
				if (i >= size) size = i + 1u; // If we hit new ground expand the end index
				return i; // End the loop
			}
			return BUF_NULL;
		}
		void AddForceID(Type_Signifier _type, lui32 index) {
			if (Used(index)) {
				printf("Tried to overwrite buffer object! Bastard!\n");
				return;
			}
			type[index] = _type;
			if (index >= size) size = index + 1u;
		}
		// Clear this space on the buffer
		void Remove(lui32 index) {
			type[index] = type_null;
			if (index == size - 1u) {
				--size; // Go back one step
				while (size > 0u && !Used(size - 1u)) --size; // Continue rolling back until we reach the next last full space
			}
		}
		void Clear() {
			for (lui32 i = 0; i < SIZE; i++) { // For every space in the buffer
				type[i] = type_null;
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
		lui32 Size() {
			return size;
		}
		void SetSize(lui32 sz) {
			size = sz;
		}
	};

	// Location and size of entity
	struct LumpPtr {
		lui32 pos;
		lui32 size;
	};
	// Object storage containing anything
	template <typename Type_Signifier, Type_Signifier type_null, lui32 array_size, lui32 memory_size> struct Lump {
	private:
		mem::ObjBuf<LumpPtr, Type_Signifier, type_null, array_size> eptrs;
		lui8 buf[memory_size];
		inline lui32 AllocateSpace(lui32 size) {
			// Search for empty space between ents (or at the end, it just works)
			// For all assigned spaces
			for (int i = 0; i < eptrs.Size(); ++i) {
				// Skip self or unused
				if (!eptrs.Used(i)) continue;
				lui32 this_end = eptrs.Data(i).pos + eptrs.Data(i).size;
				lui32 nearest_start = memory_size - 1;
				// For all other spaces
				for (int j = 0; j <= eptrs.Size(); ++j) {
					// Skip self or comparing same or unused
					if (j == i || !eptrs.Used(j)) continue;
					lui32 next_start = eptrs.Data(j).pos;
					if (next_start < nearest_start && next_start >= this_end)
						nearest_start = next_start;
				}
				if (nearest_start - this_end >= size) return this_end;
			}
			// else return the first element
			if (size >= memory_size) {
				printf("Tried to add an object bigger than the lump itself, what??\n");
				return ID_NULL; // not enough room
			}
			return 0;
		}
	public:
		Lump() {
			memset(&buf[0], 0, memory_size);
		}
		lui32 AddEntForceID(lui32 size, Type_Signifier type, lui32 id) {
			lui32 pos = AllocateSpace(size);
			eptrs.AddForceID(type, id);
			eptrs.Data(id).pos = pos;
			eptrs.Data(id).size = size;
			return id;
		}
		lui32 AddEnt(lui32 size, Type_Signifier type) {
			lui32 pos = AllocateSpace(size);
			lui32 index = eptrs.Add(type);
			eptrs.Data(index).pos = pos;
			eptrs.Data(index).size = size;
			return index;
		}
		void RmvEnt(lui32 index) {
			memset(&buf[eptrs.Data(index).pos], 0, eptrs.Data(index).size);
			eptrs.Remove(index);
		}
		void Clear() {
			for (int i = 0; i < array_size; ++i) {
				eptrs.Data(i).pos = 0;
				eptrs.Data(i).size = 0;
			}
			eptrs.Clear();
			memset(&buf[0], 0, memory_size);
		}
		void* GetEnt(lui32 index) {
			return &buf[eptrs.Data(index).pos];
		}
		Type_Signifier GetType(lui32 index) {
			return eptrs.Type(index);
		}
		bool EntExists(lui32 index) {
			return eptrs.Used(index);
		}
		lui32 GetSize() {
			return eptrs.Size();
		}
	};
	
	// Fixed size object ID buffer, optimized for objects with short life cycles
	struct objbuf_caterpillar
	{
		bool used[BUF_SIZE]{ false };
		lid index_first = 0u;
		lid index_last = 0u;
		lid add(); // Assign new object this space on the buffer
		void remove(lid ID); // Clear this space on the buffer
	};

	#define IDBUF_SIZE 16u
	// Automatic unfixed size object ID buffer
	struct idbuf
	{
	private:
		lid ptr_id[IDBUF_SIZE]{
			ID_NULL, ID_NULL, ID_NULL, ID_NULL,
			ID_NULL, ID_NULL, ID_NULL, ID_NULL,
			ID_NULL, ID_NULL, ID_NULL, ID_NULL,
			ID_NULL, ID_NULL, ID_NULL, ID_NULL };
			//ID_NULL, ID_NULL, ID_NULL, ID_NULL,
			//ID_NULL, ID_NULL, ID_NULL, ID_NULL,
			//ID_NULL, ID_NULL, ID_NULL, ID_NULL,
			//ID_NULL, ID_NULL, ID_NULL, ID_NULL };
		bool ptr_used[IDBUF_SIZE]{ false };
		lui32 id_end = 0u;
		lui32 size = 0u;
	public:
		idbuf(); // Constructor
		~idbuf(); // Destructor
		void Add(lid ID);
		void Remove(lid ID);
		void Clear();
		lui32 Size();
		lid operator[] (lui32 x);
	};
}

#endif
