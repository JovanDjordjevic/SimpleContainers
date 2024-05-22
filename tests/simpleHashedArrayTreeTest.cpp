#include <cassert>
#include <iostream>
#include <limits>

#include "simpleContainers/simpleHashedArrayTree.hpp"

#include "someTestClass.hpp"
#include "someTemplateTestClass.hpp"
#include "someAllocatorClass.hpp"

void test_internal_helpers();
void test_hashed_array_tree_construction();
void test_hashed_array_tree_member_functions();
void test_hashed_array_tree_insertion();

int main() {
    test_internal_helpers();
    test_hashed_array_tree_construction();
    test_hashed_array_tree_member_functions();
    test_hashed_array_tree_insertion();
    return 0;
}

void test_internal_helpers() {
    std::cout << "================= TESTING INTERNAL HELPERS =================" << std::endl;

    // assert(0 == simpleContainers::hat_internal::next_power_of_2<size_t>(-1));    // implicit conversion
    // assert(_ == simpleContainers::hat_internal::next_power_of_2<int>(-1));       // will fail on assert as expected
    assert(1 == simpleContainers::hat_internal::next_power_of_2<size_t>(0));
    assert(1 == simpleContainers::hat_internal::next_power_of_2<size_t>(1));
    assert(2 == simpleContainers::hat_internal::next_power_of_2<size_t>(2));
    assert(4 == simpleContainers::hat_internal::next_power_of_2<size_t>(3));
    assert(8 == simpleContainers::hat_internal::next_power_of_2<size_t>(6));
    assert(65536 == simpleContainers::hat_internal::next_power_of_2<size_t>(41523));
    
    // due to how bit shifting is used for this function
    assert(0 == simpleContainers::hat_internal::next_power_of_2<size_t>(std::numeric_limits<size_t>::max()));
    assert(1 == simpleContainers::hat_internal::next_power_of_2<size_t>(std::numeric_limits<size_t>::min()));

    assert(0 == simpleContainers::hat_internal::what_power_of_2<size_t>(1));
    assert(1 == simpleContainers::hat_internal::what_power_of_2<size_t>(2));
    assert(1 == simpleContainers::hat_internal::what_power_of_2<size_t>(3));
    assert(2 == simpleContainers::hat_internal::what_power_of_2<size_t>(4));
    assert(11 == simpleContainers::hat_internal::what_power_of_2<size_t>(2048));
}

void test_hashed_array_tree_construction() {
    std::cout << "================= TESTING HASHED ARRAY TREE CONSTRUCTION =================" << std::endl;

    simpleContainers::HashedArrayTree<int> hat1;
    assert(hat1.capacity() == 0);
    assert(hat1.size() == 0);

    simpleContainers::HashedArrayTree<SomeClass> hat2;
    assert(hat2.capacity() == 0);
    assert(hat2.size() == 0);

    SomeAllocatorClass<SomeClass> customAlloc;
    simpleContainers::HashedArrayTree<SomeClass, SomeAllocatorClass<SomeClass>> hat3(customAlloc); // default ctor with custom allocator

    simpleContainers::HashedArrayTree<SomeClass> hat4 = hat2;  // copy ctor
    assert(hat4.capacity() == hat2.capacity());
    assert(hat4.size() == hat2.size());

    simpleContainers::HashedArrayTree<SomeClass> hat5{hat2};  // copy ctor
    assert(hat5.capacity() == hat2.capacity());
    assert(hat5.size() == hat2.size());

    hat5 = hat4; // copy assignment
    assert(hat5.capacity() == hat4.capacity());
    assert(hat5.size() == hat4.size());

    simpleContainers::HashedArrayTree<SomeClass> tmpHat1{};
    auto tmpHat1Capacity = tmpHat1.capacity();
    auto tmpHat1Size = tmpHat1.size();

    simpleContainers::HashedArrayTree<SomeClass> hat6 = std::move(tmpHat1); // move ctor
    assert(hat6.capacity() == tmpHat1Capacity);
    assert(hat6.size() == tmpHat1Size);

    simpleContainers::HashedArrayTree<SomeClass> hat7 = simpleContainers::HashedArrayTree<SomeClass>{}; // default ctor for temporary, move ctor for hat7, dtor for temporary
    assert(hat7.capacity() == 0);
    assert(hat7.size() == 0);

    hat7 = std::move(hat6); // move assignment
    assert(hat7.capacity() == tmpHat1Capacity);
    assert(hat7.size() == tmpHat1Size);
}

void test_hashed_array_tree_member_functions() {
    std::cout << "================= TESTING HASHED ARRAY TREE MEMBER FUNCTIONS =================" << std::endl;

    simpleContainers::HashedArrayTree<int> hat1;
    std::vector<std::vector<int>> tmpVectorOfVectors;

    assert(hat1.max_size() == tmpVectorOfVectors.max_size());
    assert(hat1.capacity() == 0);
    assert(hat1.max_capacity() == 0);
    assert(hat1.size() == 0);
    assert(hat1.empty());
    assert(!hat1.full());

    for (int i = 0; i < 4; ++i) { hat1.push_back(i); }

    assert(hat1.capacity() == 4);
    assert(hat1.max_capacity() == 4);
    assert(hat1.size() == 4);
    assert(!hat1.empty());
    assert(hat1.full());

    hat1.reserve(5);
    assert(hat1.capacity() == 8);
    assert(hat1.max_capacity() == 16);
    assert(hat1.size() == 4);
    assert(!hat1.empty());
    assert(!hat1.full());

    hat1.reserve(3);        // capacity should remain unchanged
    assert(hat1.capacity() == 8);
    assert(hat1.max_capacity() == 16);
    assert(hat1.size() == 4);
    assert(!hat1.empty());
    assert(!hat1.full());

    hat1.reserve(9);        // capacity should change but not max_capacity
    assert(hat1.capacity() == 12);
    assert(hat1.max_capacity() == 16);
    assert(hat1.size() == 4);
    assert(!hat1.empty());
    assert(!hat1.full());

    hat1.clear();       // clear should only affect size, not capacity
    assert(hat1.capacity() == 12);
    assert(hat1.max_capacity() == 16);
    assert(hat1.size() == 0);
    assert(hat1.empty());
    assert(!hat1.full());

    hat1.reserve(7 * 16 - 8);
    assert(hat1.capacity() == 112); 
    assert(hat1.max_capacity() == 256); 
}

void test_hashed_array_tree_insertion() {
    std::cout << "================= TESTING HASHED ARRAY TREE INSERTION =================" << std::endl;

    simpleContainers::HashedArrayTree<unsigned int> hat1;
    assert(hat1.capacity() == 0);
    assert(hat1.size() == 0);
    
    for (unsigned int i = 0; i < 65; ++i) {
        hat1.push_back(i);
        hat1.debugPrint();

        assert(hat1.size() == i + 1);
        assert(hat1[i] == i);

        if (i == 0) {
            assert(hat1.capacity() == 1); 
            assert(hat1.max_capacity() == 1); 
        }
        if (i == 1) {
            assert(hat1.capacity() == 2); 
            assert(hat1.max_capacity() == 4); 
        }
        else if (i == 2) {
            assert(hat1.capacity() == 4);
            assert(hat1.max_capacity() == 4); 
        }
        else if (i == 4) {
            assert(hat1.capacity() == 8);
            assert(hat1.max_capacity() == 16); 
        }
        else if (i == 16) {
            assert(hat1.capacity() == 24); 
            assert(hat1.max_capacity() == 64); 
        }
        else if (i == 48) {
            assert(hat1.capacity() == 56); 
            assert(hat1.max_capacity() == 64); 
        }
        else if (i == 64) {
            assert(hat1.capacity() == 80); 
            assert(hat1.max_capacity() == 256); 
        }
    }
}
