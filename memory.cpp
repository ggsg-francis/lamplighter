//#include "global.h"
#include "memory.hpp"
// This include provides the malloc function
//#include <iostream>

namespace mem
{
	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	//--------------------------- FIXED SIZE OBJECT ID BUFFER ------------------------------------------------------------------------
	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

	btID objbuf::add()
	{
		for (btID i = 0; i < BUF_SIZE; i++) // For every space in the buffer
		{
			if (!used[i]) // If this space is free
			{
				used[i] = true;
				if (i > index_end) index_end = i; // If we hit new ground expand the end index
				return i; // End the loop
			}
		}
		return BUF_NULL;
	}

	void objbuf::remove(btID i)
	{
		used[i] = false;
		if (i == index_end)
		{
			--index_end; // Go back one step
			while (!used[index_end]) --index_end; // Continue rolling back until we reach the next last full space
		}
	}

	btID objbuf_caterpillar::add()
	{
		used[index_last] = true; // Index is taken
		btID index = index_last; // Store index for return value
		++index_last; // Iterate
		if (index_last == BUF_SIZE) // Have we overrun?
			index_last = 0; // Wrap around
		return index; // Return added ID
	}

	// i was focused on when 0 was removed, when really i should have noticed how 511 was the last removed
	void objbuf_caterpillar::remove(btID i)
	{
		used[i] = false;
		if (i == index_first) // If this is the first element in the buffer
		{
			++index_first; // Iterate
			if (index_first == BUF_SIZE) // Have we overrun?
				index_first = 0; // Wrap around
			while (!used[index_first]) // iterate until we reach the new first
			{
				if (index_first == index_last) break; // If we hit index_last, that means the buffer is empty and nothing can be removed
				++index_first; // Iterate
				if (index_first == BUF_SIZE) // Have we overrun?
					index_first = 0; // Wrap around
			}
		}
	}

	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	//--------------------------- AUTOMATIC UNFIXED SIZE OBJECT ID BUFFER ------------------------------------------------------------
	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

	// Arbitrary initial size of a pointer buffer ( in sizeof(void*) )
	#define PTRBUF_INIT_SIZE 4

	idbuf::idbuf() // Constructor
	{
		i_allocated = PTRBUF_INIT_SIZE; i_used = 0;
		ptr_id = (btID*)malloc(sizeof(btID) * i_allocated);
	}

	idbuf::idbuf(idbuf & copy) // Copy constructor
	{
		// WIP
		//i_allocated = PTRBUF_INIT_SIZE; i_used = 0;
		//ptr_id = (id_t*)malloc(sizeof(id_t) * i_allocated);
		//memcpy(copy.ptr_id, ptr_id, PTRBUF_INIT_SIZE);
	}

	idbuf::~idbuf()
	{
		free(ptr_id);
	}

	void idbuf::resize(size_t s)
	{
		btID* items2 = (btID*)realloc(ptr_id, sizeof(btID) * s);
		if (items2) {
			ptr_id = items2;
			i_allocated = (btui32)s;
			std::cout << "IDBUF resized to size " << i_allocated << std::endl;
			std::cout << "IDBUF current usage: " << i_used << std::endl;
		}
	}

	void idbuf::add(btID id)
	{
		if (i_allocated == i_used)
			resize(i_allocated * 2);
		ptr_id[i_used++] = id;
		std::cout << "IDBUF added index " << i_used << std::endl;
	}

	// I realize that this is slow, but it will do for now
	//otherwise, store in every entity a 'cell ID' so it can remove itself from the cell faster
	void idbuf::remove(btID id)
	{
		//search this vector
		for (uint i = 0; i < i_allocated; i++)
			//if we found ptr in this vector
			if (ptr_id[i] == id)
			{
				del(i);
				return;
			}
		std::cout << "IDBUF attempted to remove pointer not in array" << std::endl;
	}

	int idbuf::size()
	{
		return i_used;
	}

	void idbuf::del(btID index)
	{
		if (index >= i_used)
		{
			std::cout << "ERROR! IDBUF 'remove' out of range!" << std::endl;
			return;
		}
		ptr_id[index] = ID_NULL;
		std::cout << "IDBUF deleted index " << index << std::endl;

		//shift every index after this back by one
		for (btui32 i = index; i < i_used - 1; i++) {
			ptr_id[i] = ptr_id[i + 1];
			ptr_id[i + 1] = ID_NULL;
		}

		i_used--;

		if (i_used > 0 && i_used == i_allocated / 4)
			resize(i_allocated / 2);
	}

	btID idbuf::operator[] (btui32 x)
	{
		if (x >= 0 && x < i_used)
			return ptr_id[x];
		std::cout << "ERROR! IDBUF 'operator[]' out of range!" << std::endl;
		return ID_NULL;
	}
}