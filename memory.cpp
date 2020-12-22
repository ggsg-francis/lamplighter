//#include "global.h"
#include "memory.hpp"
// This include provides the malloc function
//#include <iostream>

namespace mem
{
	ID16 objbuf_caterpillar::add()
	{
		used[index_last] = true; // Index is taken
		ID16 index = index_last; // Store index for return value
		++index_last; // Iterate
		if (index_last == BUF_SIZE) // Have we overrun?
			index_last = 0; // Wrap around
		return index; // Return added ID
	}

	void objbuf_caterpillar::remove(ID16 i)
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

	void idbuf::Add(ID16 id) {
		// Search this vector ahead of time for a matching ID
		for (lui32 i = 0; i < IDBUF_SIZE; i++)
			if (ptr_id[i] == id) {
				std::cout << "IDBUF ID already in array!" << std::endl;
				return;
			}
		// If that passes, look for the first empty space (can optimize this)
		for (lui32 i = 0; i < IDBUF_SIZE; i++)
			// if no ptr in this vector
			if (ptr_used[i] == false) {
				// Add the ID
				++size;
				ptr_used[i] = true;
				ptr_id[i] = id;
				if (i > id_end) id_end = i; // If we hit new ground expand the end index
				//std::cout << "IDBUF added index " << i << std::endl;
				return;
			}
		std::cout << "IDBUF could not add ID, ran out of space" << std::endl;
	}
	// I realize that this is slow, but it will do for now
	// otherwise, store in every entity a 'cell ID' so it can remove itself from the cell faster
	void idbuf::Remove(ID16 id) {
		//search this vector
		for (lui32 i = 0; i < IDBUF_SIZE; i++)
			//if we found ptr in this vector
			if (ptr_id[i] == id && ptr_used[i] == true) {
				ptr_used[i] = false;
				ptr_id[i] = ID_NULL;
				if (i == id_end && id_end > 0u) {
					--size;
					--id_end; // Go back one step
				//	while (!ptr_used[id_end]) --id_end; // Continue rolling back until we reach the next last full space
				}
				return;
			}
		std::cout << "IDBUF attempted to remove pointer not in array" << std::endl;
	}
	void idbuf::Clear() {
		for (int i = 0; i < IDBUF_SIZE; ++i) {
			ptr_id[i] = ID_NULL;
			ptr_used[i] = false;
		}
		id_end = 0;
		size = 0;
	}
	lui32 idbuf::Size() {
		return size;
	}
	ID16 idbuf::operator[] (lui32 x) {
		if (x >= 0u && x < IDBUF_SIZE && ptr_used[x] == true)
			return ptr_id[x];
		//std::cout << "ERROR! IDBUF 'operator[]' out of range!" << std::endl;
		return ID_NULL;
	}

	void id2buf::Add(LtrID id) {
		// Search this vector ahead of time for a matching ID
		for (lui32 i = 0; i < IDBUF_SIZE; i++)
			if (ptr_id[i].GUID() == id.GUID()) {
				std::cout << "IDBUF ID already in array!" << std::endl;
				return;
			}
		// If that passes, look for the first empty space (can optimize this)
		for (lui32 i = 0; i < IDBUF_SIZE; i++)
			// if no ptr in this vector
			if (ptr_used[i] == false) {
				// Add the ID
				++size;
				ptr_used[i] = true;
				ptr_id[i] = id;
				if (i > id_end) id_end = i; // If we hit new ground expand the end index
				return;
			}
		std::cout << "IDBUF could not add ID, ran out of space" << std::endl;
	}
	// I realize that this is slow, but it will do for now
	// otherwise, store in every entity a 'cell ID' so it can remove itself from the cell faster
	void id2buf::Remove(LtrID id) {
		//search this vector
		for (lui32 i = 0; i < IDBUF_SIZE; i++)
			//if we found ptr in this vector
			if (ptr_id[i].GUID() == id.GUID() && ptr_used[i] == true) {
				ptr_used[i] = false;
				ptr_id[i] = ID2_NULL;
				if (i == id_end && id_end > 0u) {
					--size;
					--id_end; // Go back one step
					//	while (!ptr_used[id_end]) --id_end; // Continue rolling back until we reach the next last full space
				}
				//std::cout << "IDBUF removed index " << i << std::endl;
				return;
			}
		std::cout << "IDBUF attempted to remove pointer not in array" << std::endl;
	}
	void id2buf::Clear() {
		for (int i = 0; i < IDBUF_SIZE; ++i) {
			ptr_id[i] = ID2_NULL;
			ptr_used[i] = false;
		}
		id_end = 0;
		size = 0;
	}
	lui32 id2buf::Size() {
		return size;
	}
	LtrID id2buf::operator[] (lui32 x) {
		if (x >= 0u && x < IDBUF_SIZE && ptr_used[x] == true)
			return ptr_id[x];
		//std::cout << "ERROR! IDBUF 'operator[]' out of range!" << std::endl;
		return ID2_NULL;
	}
}