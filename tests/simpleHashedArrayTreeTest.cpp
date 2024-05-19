#include <cassert>
#include <iostream>

#include "simpleContainers/simpleHashedArrayTree.hpp"

#include "someTestClass.hpp"
#include "someTemplateTestClass.hpp"
#include "someAllocatorClass.hpp"

void test_hashed_array_tree_construction();

int main() {
    test_hashed_array_tree_construction();
    return 0;
}

void test_hashed_array_tree_construction() {
    simpleContainers::HashedArrayTree<int> hat1;
    assert(hat1.capacity() == 0);
}
