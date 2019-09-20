#pragma once

// This include provides the malloc function
#include <iostream>

typedef unsigned int uint;

namespace mem
{
	//-------------------------------- BUFFER TYPES

	template <class type> struct Buffer
	{
	private:
		type* b = nullptr;
		btui32 s = 0u;
		Buffer() { b = new type; s = 1u; };
		~Buffer() { delete b; };
	public:
		// Initialize at size
		Buffer(btui32 size) { b = new type[size]; s = size; };
		btui32 Size()
		{
			return s;
		}
		// Set new size
		void SetSize(btui32 size)
		{
			type* b2 = new type[size];
			for (btui32 i = 0; i < s; ++i)
			{
				b2[i] = b[i];
			}
			delete b;
			b = b2;
			s = size;
		}
		// Set new size
		void IncrSize()
		{
			type* b2 = new type[s + 1];
			for (btui32 i = 0; i < s; ++i)
			{
				b2[i] = b[i];
			}
			delete b;
			b = b2;
			s = size;
		}
		type& Last()
		{
			return b[s - 1u];
		}
		type& operator[](btui32 index)
		{
			//index < s ? return b[index] : return nullptr;
			return b[index];
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

	// Automatic unfixed size object ID buffer 
	struct idbuf
	{
	private:
		btID* ptr_id; btui32 i_allocated; btui32 i_used;
		void del(btID index);
		void resize(size_t size);
	public:
		idbuf(); // Constructor
		idbuf(idbuf& COPY); // Copy constructor (only maybe)
		~idbuf(); // Destructor
		void add(btID ID);
		void remove(btID ID);
		int size();
		btID operator[] (btui32 x);
	};

	//-------------------------------- BIT-VECTOR OPERATIONS

	// Bitvector (the safest iteration yet)
	template <class tcast, class trecv> class bv
	{
	private:
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
}