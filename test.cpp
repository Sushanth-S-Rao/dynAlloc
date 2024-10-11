#include "dynalloc.h"
#include <cassert>
#include <iostream> 

int main(int argc, char const *argv[]) {
	  // Test case 1: Alignment
	  int s;
	  std::cout << "Enter bytes you want to be allocated: ";
	  std::cin >> s;
	  auto p1 = dynalloc(s);
	  auto p1b = getHeader(p1);
	  assert(p1b->size == ( ((s/8) + 1) * sizeof(word_t) ));
	  std::cout << "Alignment is Successful\n";

	  // Test case 2: Exact amount of aligned
	  auto p2 = dynalloc(8);
	  auto p2b = getHeader(p2);
	  assert(p2b->size == 8);
	  std::cout << "Alignment of word size is Successful\n";

	// Test case 3: Free the object
	free(p2);
	assert(p2b->used == false);
	std::cout << "Freeing the Memory Block Successful\n";

	// Test case 4: The block is reused
	auto p3 = dynalloc(8);
	auto p3b = getHeader(p3);
	assert(p3b->size == 8);
	assert(p3b == p2b);
	std::cout << "Reusing the Memory Block Successful\n";

  return 1;
}
