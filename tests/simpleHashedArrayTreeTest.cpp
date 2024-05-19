#include <cassert>
#include <iostream>
#include <limits>

#include "simpleContainers/simpleHashedArrayTree.hpp"

#include "someTestClass.hpp"
#include "someTemplateTestClass.hpp"
#include "someAllocatorClass.hpp"

void test_internal_helpers();
void test_hashed_array_tree_construction();

int main() {
    test_internal_helpers();
    test_hashed_array_tree_construction();
    return 0;
}

void test_internal_helpers() {
    // assert(0 == simpleContainers::internal::next_power_of_2<size_t>(-1));    // implicit conversion
    // assert(_ == simpleContainers::internal::next_power_of_2<int>(-1));       // will fail on assert as expected
    assert(1 == simpleContainers::internal::next_power_of_2<size_t>(0));
    assert(1 == simpleContainers::internal::next_power_of_2<size_t>(1));
    assert(2 == simpleContainers::internal::next_power_of_2<size_t>(2));
    assert(4 == simpleContainers::internal::next_power_of_2<size_t>(3));
    assert(8 == simpleContainers::internal::next_power_of_2<size_t>(6));
    assert(65536 == simpleContainers::internal::next_power_of_2<size_t>(41523));
    
    // due to how bit shifting is used for this function
    assert(0 == simpleContainers::internal::next_power_of_2<size_t>(std::numeric_limits<size_t>::max()));
    assert(1 == simpleContainers::internal::next_power_of_2<size_t>(std::numeric_limits<size_t>::min()));
}

void test_hashed_array_tree_construction() {
    simpleContainers::HashedArrayTree<int> hat1;
    assert(hat1.capacity() == 0);
    assert(hat1.size() == 0);

    hat1.debugPrint();

    hat1.reserve(1);
    hat1.debugPrint();
    hat1.reserve(2);
    hat1.debugPrint();
    hat1.reserve(3);
    hat1.debugPrint();
    hat1.reserve(4);
    hat1.debugPrint();
    hat1.reserve(5);
    hat1.debugPrint();
    hat1.reserve(65);
    hat1.debugPrint();

    hat1.clear();
    assert(hat1.size() == 0);
}
