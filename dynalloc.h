#ifndef DYNALLOC_H
#define DYNALLOC_H

#include <cstddef>  	// For size_t
#include <cstdint>  	// For intptr_t
#include <iostream>
#include <assert.h>
#include <unistd.h> 	// for "sbrk" call

// Machine word size. Depending on the architecture, it can be 4 (int) or 8 (long) bytes
using word_t = intptr_t;

// Functions Declared
word_t *dynalloc(size_t size);
inline size_t align(size_t n);
inline size_t allocSize(size_t size);
void free(word_t *data);

struct Block {
	// 1) Object Header
	size_t size; 	// Block Size
  	bool used; 	// Whether the block is currently used or not
  	Block *next; 	// Next block in the list

	// 2) User data
  	word_t data[1];		// Payload pointer
};

// Functions Declared
Block *requestFromOS(size_t size);
Block *getHeader(word_t *data);
Block *findFreeBlock(size_t size);

// Memory Alignment
// Aligns the size by the machine word
inline size_t align(size_t n) {
  return (n + sizeof(word_t) - 1) & ~(sizeof(word_t) - 1);
}

// Memory Mapping
// Returns total allocation size (header + first data word): Since word_t data[1] allocates one word inside Block, we subtract it from size request
inline size_t allocSize(size_t size) {
    // Calculate total size including the header and data
    return size + sizeof(Block) - sizeof(Block::data);
}



// We’ll track the start and the end of the heap
static Block *heapStart = nullptr;			// Heap start. Initialized on first allocation
static auto top = heapStart;				// Current top. Updated on each allocation

/*
heapStart                         top
 |                                 |
 ---------------------------------------------------------------------
 |	|       |         HEAP     |					     |
 ---------------------------------------------------------------------
*/


// Dynamic Allocation
// Allocates block of memory of (atleast) 'size' bytes
word_t *dynalloc(size_t size) {
	size = align(size);

	// 1) Search for available free block
	if(auto block = findFreeBlock(size))
		return block -> data;

	// 2) Request memory from the OS, if no free block is found
	auto block = requestFromOS(size);
	block -> size = size;
	block -> used = true;

	// init heap
	if(heapStart == nullptr)
		heapStart = block;

	// Chain the Blocks
	if(top != nullptr)
		top -> next = block;

	top = block;

	// User Payload
	return block -> data;
}



// Mapping memory from the OS
Block *requestFromOS(size_t size) {
	// 1) Current Heap Break
	auto block = (Block *)sbrk(0);

	// 2) Out Of Memory
	if(sbrk(allocSize(size)) == (void *) - 1)
		return nullptr;

	return block;
}


// Helper Function to obtain Object Header from User Pointer
Block *getHeader(word_t *data) {
    return (Block *)((char *)data - offsetof(Block, data));
}


// Frees a previously allocated block
void free(word_t *data) {
  auto block = getHeader(data);
  block -> used = false;
}

// Search the linked list for available free block
Block *findFreeBlock(size_t size) {
	auto block = heapStart;
	while(block != nullptr) {
		if(block -> used || block -> size < size) {
			block = block -> next;
			continue;
		}
		// Block is Found
		return block;
	}
	return nullptr;
}










//////////////////////////////////////////////////////////////////////////
#endif // _H
