#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <limits>

#include "simpleContainers/simpleHashedArrayTree.hpp"

#include "someTestClass.hpp"
#include "someTemplateTestClass.hpp"
#include "someAllocatorClass.hpp"

void test_internal_helpers();
void test_hashed_array_tree_construction();
void test_hashed_array_tree_member_functions();
void test_hashed_array_tree_insertion();
void test_hashed_array_tree_comparison();
void test_hashed_array_tree_iterators();

int main() {
    test_internal_helpers();
    test_hashed_array_tree_construction();
    test_hashed_array_tree_member_functions();
    test_hashed_array_tree_insertion();
    test_hashed_array_tree_comparison();
    test_hashed_array_tree_iterators();
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

    static_assert(std::is_same<void, simpleContainers::hat_internal::void_t<>>::value, "void_t not working 1");
    static_assert(std::is_same<void, simpleContainers::hat_internal::void_t<void>>::value, "void_t not working 2");
    static_assert(std::is_same<void, simpleContainers::hat_internal::void_t<char>>::value, "void_t not working 3");
    static_assert(std::is_same<void, simpleContainers::hat_internal::void_t<
        int, double, void, SomeClass, SomeAllocatorClass<size_t>, 
        simpleContainers::HashedArrayTree<SomeTemplateClass<char>>>
    >::value, "void_t not working 4");

    static_assert(!simpleContainers::hat_internal::is_iterator<int>::value, "is_iterator not working 1");
    static_assert(!simpleContainers::hat_internal::is_iterator<std::vector<int>>::value, "is_iterator not working 2");
    static_assert(simpleContainers::hat_internal::is_iterator<int*>::value, "is_iterator not working 3");
    static_assert(simpleContainers::hat_internal::is_iterator<const int*>::value, "is_iterator not working 4");
    static_assert(!simpleContainers::hat_internal::is_iterator<const int* const>::value, "is_iterator not working 5");
    static_assert(simpleContainers::hat_internal::is_iterator<std::vector<int>::iterator>::value, "is_iterator not working 6");
    static_assert(simpleContainers::hat_internal::is_iterator<std::vector<int>::const_iterator>::value, "is_iterator not working 7");
    static_assert(simpleContainers::hat_internal::is_iterator<std::vector<int>::iterator>::value, "is_iterator not working 8");
    static_assert(simpleContainers::hat_internal::is_iterator<std::vector<int>::iterator>::value, "is_iterator not working 9");
    static_assert(simpleContainers::hat_internal::is_iterator<simpleContainers::HashedArrayTree<int>::iterator>::value, "is_iterator not working 10");
    static_assert(simpleContainers::hat_internal::is_iterator<simpleContainers::HashedArrayTree<int>::const_iterator>::value, "is_iterator not working 11");
    static_assert(simpleContainers::hat_internal::is_iterator<simpleContainers::HashedArrayTree<int>::pointer>::value, "is_iterator not working 12");
    static_assert(simpleContainers::hat_internal::is_iterator<simpleContainers::HashedArrayTree<int>::const_pointer>::value, "is_iterator not working 13");
    static_assert(simpleContainers::hat_internal::is_iterator<std::move_iterator<simpleContainers::HashedArrayTree<int>::iterator>>::value, "is_iterator not working 14");
    static_assert(simpleContainers::hat_internal::is_iterator<std::move_iterator<simpleContainers::HashedArrayTree<int>::const_iterator>>::value, "is_iterator not working 14");

    static_assert(std::is_same<
        bool,
        std::enable_if<
            simpleContainers::hat_internal::is_iterator<simpleContainers::HashedArrayTree<int>::iterator>::value, 
            bool
        >::type
    >::value, "is_iterator not working in enable_if 1");

    static_assert(std::is_same<
        bool,
        std::enable_if<
            simpleContainers::hat_internal::is_iterator<simpleContainers::HashedArrayTree<int>::const_iterator>::value, 
            bool
        >::type
    >::value, "is_iterator not working in enable_if 2");
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
    static_assert(std::is_same<decltype(hat3.get_allocator()), decltype(customAlloc)>::value, "Allocator types do not match");

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

    simpleContainers::HashedArrayTree<int> hat8(5); // capacity constructor
    assert(hat8.size() == 0);
    assert(hat8.capacity() == 8);

    simpleContainers::HashedArrayTree<int> hat9(5, 7); // fill constructor
    assert(hat9.size() == 5);
    assert(hat9.capacity() == 8);
    for (int i = 0; i < hat9.size(); ++i) {
        assert(hat9[i] == 7);
    }

    simpleContainers::HashedArrayTree<char> hat10(5, 'a'); // fill constructor different types
    assert(hat10.size() == 5);
    assert(hat10.capacity() == 8);
    for (int i = 0; i < hat10.size(); ++i) {
        assert(hat10[i] == 'a');
    }

    std::vector<SomeTemplateClass<int>> tmpVector1 = {SomeTemplateClass<int>{5}, SomeTemplateClass<int>{6}};
    simpleContainers::HashedArrayTree<SomeTemplateClass<int>> hat11(tmpVector1); // constructor from vector
    assert(hat11.size() == tmpVector1.size());
    assert(hat11[0] == tmpVector1[0]);
    assert(hat11[1] == tmpVector1[1]);

    simpleContainers::HashedArrayTree<int> hat12{0, 1, 2, 3, 4, 5, 6, 7}; // initializer list constructor
    assert(hat12.size() == 8);
    for (auto i = 0; i < hat12.size(); ++i) {
        assert(hat12[i] == i);
    }

    simpleContainers::HashedArrayTree<int> hat13(hat12.begin(), hat12.end()); // construct from iterators
    assert(hat13.size() == hat12.size());
    std::vector<int> hat13Expected = {0, 1, 2, 3, 4, 5, 6, 7};
    assert(hat13.get_as_vector() == hat13Expected);

    simpleContainers::HashedArrayTree<int> hat14(std::make_move_iterator(hat13.begin()), std::make_move_iterator(hat13.end())); // construct from move iterators
    assert(hat14.size() == hat13.size());
    std::vector<int> hat14Expected = {0, 1, 2, 3, 4, 5, 6, 7};
    assert(hat14.get_as_vector() == hat14Expected);

    // trigger a realloc before doing more move and copy ctor tests
    hat14.emplace_back(8);
    hat14.emplace_back(9);

    simpleContainers::HashedArrayTree<int> hat15{hat14}; // copy ctor with non-empty other
    assert(hat15.capacity() == hat14.capacity());
    assert(hat15.size() == hat14.size());

    hat15 = hat13; // copy assignment with non-empty other
    assert(hat15.capacity() == hat13.capacity());
    assert(hat15.size() == hat13.size());

    auto hat14Capacity = hat14.capacity();
    auto hat14Size = hat14.size();
    auto hat14Vec = hat14.get_as_vector();
    simpleContainers::HashedArrayTree<int> hat16 = std::move(hat14); // move ctor with non-empty other
    assert(hat16.capacity() == hat14Capacity);
    assert(hat16.size() == hat14Size);
    assert(hat16.get_as_vector() == hat14Vec);

    auto hat15Capacity = hat15.capacity();
    auto hat15Size = hat15.size();
    auto hat15Vec = hat15.get_as_vector();
    hat16 = std::move(hat15); // move assignment with non-empty other
    assert(hat16.capacity() == hat15Capacity);
    assert(hat16.size() == hat15Size);
    assert(hat16.get_as_vector() == hat15Vec);
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

    // inserting after clear
    for (int i = 0; i < 22; ++i) {
        hat1.push_back(i);
        hat1.debugPrint();
        assert(hat1.size() == i + 1);
        assert(hat1[i] == i);
        assert(hat1.at(i) == i);
    }   

    const auto& hat1AsVec = hat1.get_as_vector();
    assert(hat1AsVec.size() == hat1.size());
    assert(hat1AsVec.capacity() == hat1.size());
    for (auto i = 0; i < hat1AsVec.size(); ++i) {
        assert(hat1AsVec[i] == i);
    }

    simpleContainers::HashedArrayTree<int> hat2{1, 2, 3, 4};
    simpleContainers::HashedArrayTree<int> hat3{1, 2, 3, 4, 5, 6, 7, 8, 9};

    auto hat2OldSize = hat2.size();
    auto hat2OldCapacity = hat2.capacity();
    auto hat2OldAsVector = hat2.get_as_vector();
    auto hat3OldSize = hat3.size();
    auto hat3OldCapacity = hat3.capacity();
    auto hat3OldAsVector = hat3.get_as_vector();

    hat2.swap(hat3);

    assert(hat2.size() == hat3OldSize);
    assert(hat2.capacity() == hat3OldCapacity);
    assert(hat2.get_as_vector() == hat3OldAsVector);
    assert(hat3.size() == hat2OldSize);
    assert(hat3.capacity() == hat2OldCapacity);
    assert(hat3.get_as_vector() == hat2OldAsVector);

    simpleContainers::HashedArrayTree<int> hat4 = {1, 2, 3};
    assert(hat4.size() == 3);
    assert(hat4.capacity() == 4);
    hat4.reserve(128);
    assert(hat4.size() == 3);
    assert(hat4.capacity() == 128);
    hat4.shrink_to_fit();
    assert(hat4.size() == 3);
    assert(hat4.capacity() == 16);

    assert(hat4.front() == 1);
    assert(hat4.back() == 3);

    const simpleContainers::HashedArrayTree<int> hat5 = {2, 4, 6, 8, 10, 12, 14};
    assert(hat5.front() == 2);
    assert(hat5.back() == 14);
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
        assert(hat1.at(i) == i);

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

    hat1.pop_back();
    hat1.pop_back();
    hat1.pop_back();
    assert(hat1.back() == 61);
    assert(hat1.size() == 62);
    assert(hat1.capacity() == 80);

    hat1.emplace_back(62);
    hat1.emplace_back(63);
    hat1.emplace_back(64);
    assert(hat1.back() == 64);
    assert(hat1.size() == 65);
    assert(hat1.capacity() == 80);
}

void test_hashed_array_tree_comparison() {
    std::cout << "================= TESTING HASHED ARRAY TREE COMPARISON OPEARTIONS =================" << std::endl;

    simpleContainers::HashedArrayTree<int> hatCmp1{1, 2, 3};
    assert(hatCmp1 == hatCmp1);
    assert(hatCmp1 >= hatCmp1);
    assert(hatCmp1 <= hatCmp1);
    assert(!(hatCmp1 < hatCmp1));
    assert(!(hatCmp1 > hatCmp1));
    simpleContainers::HashedArrayTree<int> hatCmp2{1, 2, 3};
    assert(hatCmp1 == hatCmp2);
    assert(hatCmp1 <= hatCmp2);
    assert(hatCmp1 >= hatCmp2);
    assert(!(hatCmp1 < hatCmp2));
    assert(!(hatCmp1 > hatCmp2));

    hatCmp2 = {2, 3, 4, 5};
    assert(hatCmp1 != hatCmp2);
    assert(hatCmp1 < hatCmp2 && !(hatCmp2 < hatCmp1));
    assert(hatCmp1 <= hatCmp2);
    
    simpleContainers::HashedArrayTree<int> hatCmp3{5, 6, 7};
    assert(hatCmp1 < hatCmp2);
    assert(hatCmp2 < hatCmp3);
    assert(hatCmp1 < hatCmp3);

    hatCmp2 = {1, 2, 3};
    hatCmp3 = {1, 2, 3};
    assert(hatCmp1 == hatCmp2);
    assert(hatCmp2 == hatCmp3);
    assert(hatCmp1 == hatCmp3);
}

void test_hashed_array_tree_iterators() {
    std::cout << "================= TESTING HASHED ARRAY TREE ITERATORS =================" << std::endl;

    simpleContainers::HashedArrayTree<int> hat1(16);
    assert(hat1.empty());
    assert(hat1.begin() == hat1.end());

    size_t elemCnt = 0;
    std::for_each(hat1.begin(), hat1.end(), [&elemCnt](const int){ ++elemCnt; });
    assert(elemCnt == hat1.size());

    for (int i = 0; i < 5; ++i) { hat1.emplace_back(i); }

    elemCnt = 0;
    std::for_each(hat1.begin(), hat1.end(), [&elemCnt](const int){ ++elemCnt; });
    assert(elemCnt == 5 && hat1.size() == 5);
    assert(*(hat1.begin()) == 0);

    for (int i = 5; i < 16; ++i) {
        hat1.emplace_back(i); 
    }

    elemCnt = 0;
    std::for_each(hat1.begin(), hat1.end(), [&elemCnt](const int){ ++elemCnt; });

    assert(elemCnt == 16);
    assert(hat1.full());
    assert(*(hat1.begin()) == 0);
    std::vector<int> expectedResult = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    assert(hat1.get_as_vector() == expectedResult);

    auto itHat1Find1 = std::find(hat1.begin(), hat1.end(), 8);
    assert(itHat1Find1 != hat1.end() && *itHat1Find1 == 8);
    auto itHat1Find2 = std::find(hat1.begin(), hat1.end(), 12);
    assert(itHat1Find2 != hat1.end() && *itHat1Find2 == 12);

    itHat1Find1.swap(itHat1Find2);
    assert(*itHat1Find2 == 8 && *itHat1Find1 == 12);

    std::swap(itHat1Find1, itHat1Find2);
    assert(*itHat1Find1 == 8 && *itHat1Find2 == 12);

    std::iter_swap(itHat1Find1, itHat1Find2);
    assert(*itHat1Find1 == 12 && *itHat1Find2 == 8);
    expectedResult = {0, 1, 2, 3, 4, 5, 6, 7, 12, 9, 10, 11, 8, 13, 14, 15};
    assert(hat1.get_as_vector() == expectedResult);

    // test multipass
    simpleContainers::HashedArrayTree<int>::iterator itHat1Begin = hat1.begin();
    simpleContainers::HashedArrayTree<int>::iterator itHat1BeginCpy = itHat1Begin;
    auto hat1Item1 = *itHat1Begin;
    assert(hat1Item1 == 0);
    ++itHat1Begin;
    itHat1Begin++;
    hat1Item1 = *itHat1Begin;
    assert(hat1Item1 == 2);
    auto hat1Item2 = *itHat1BeginCpy;
    assert(hat1Item2 == 0);

    // test const iteration
    simpleContainers::HashedArrayTree<int> hatEmpty(10);
    assert(hatEmpty.empty() && hatEmpty.capacity() == 12);
    simpleContainers::HashedArrayTree<int>::const_iterator itHatEmpty = hatEmpty.begin();
    assert(itHatEmpty == hatEmpty.end());

    hatEmpty.emplace_back(1);
    itHatEmpty = hatEmpty.begin();
    assert(itHatEmpty != hatEmpty.end() && *itHatEmpty == 1);

    elemCnt = 0;
    for (auto i = hat1.cbegin(); i != hat1.cend(); ++i) {
        ++ elemCnt;
    }
    assert(elemCnt == 16);

    const simpleContainers::HashedArrayTree<int> hat1Const = hat1;
    elemCnt = 0;
    std::for_each(hat1.begin(), hat1.end(), [&elemCnt](const int){ ++elemCnt; });
    assert(elemCnt == 16);

    simpleContainers::HashedArrayTree<int>::const_iterator itHat1Cbegin = hat1.cbegin();
    simpleContainers::HashedArrayTree<int>::const_iterator itHat1CbeginCpy = itHat1Cbegin;
    auto hat1Citem1 = *itHat1Cbegin;
    assert(hat1Citem1 == 0);
    ++itHat1Cbegin;
    itHat1Cbegin++;
    hat1Citem1 = *itHat1Cbegin;
    assert(hat1Citem1 == 2);
    auto hat1Citem2 = *itHat1CbeginCpy;
    assert(hat1Citem2 == 0);

    // create const from non const iterator

    simpleContainers::HashedArrayTree<int>::const_iterator hat1ConstItFromNonConstIt = hat1.begin();
    assert(*hat1ConstItFromNonConstIt == *hat1.begin());

    auto tmpIt = hat1.cbegin();
    tmpIt = hat1.begin();

    simpleContainers::HashedArrayTree<int> hat2;
    for (int i = 0; i < 500; ++i) { hat2.emplace_back(i); }
    for (auto& elem : hat2) { elem = 0; }
    auto itHat2End = hat2.end();
    for (auto it = hat2.begin(); it != itHat2End; ++it) { assert (*it == 0); }
    for (auto it = hat2.begin(); it != itHat2End; ++it) { *it = 5; }
    for (auto it = hat2.begin(); it != itHat2End; ++it) { assert (*it == 5); }

    // const simpleContainers::HashedArrayTree<int> hat2Const = hat2;
    // for (auto& elem : hat2Const) { elem = 0; }   // fails compilation as expected
    // for (auto it = hat2Const.begin(); it != hat2Const.end(); ++it) {
    //     assert (*it == 0);   // fails compilation as expected
    // }

    // test bidirection

    simpleContainers::HashedArrayTree<int> hat3(16);
    for (int i = 0; i < 8; ++i) { hat3.emplace_back(i); } // fill half
    assert(hat3.size() == 8 && hat3.capacity() == 16);

    auto itHat3 = hat3.begin();         assert(*itHat3 == 0);
    ++itHat3;                           assert(*itHat3 == 1);
    itHat3++;                           assert(*itHat3 == 2);
    itHat3 += 6;                        assert(itHat3 == hat3.end());
    --itHat3;                           assert(*itHat3 == 7);
    itHat3--;                           assert(*itHat3 == 6);
    itHat3 -= 6;                        assert(itHat3 == hat3.begin());

    for (int i = 8; i < 16; ++i) { hat3.emplace_back(i); }    // fill entire, without old value overwrite
    assert(hat3.size() == 16 && hat3.capacity() == 16);
    assert(hat3.full());

    itHat3 = hat3.begin();         assert(*itHat3 == 0);
    itHat3++;                      assert(*itHat3 == 1);
    ++itHat3;                      assert(*itHat3 == 2);
    itHat3 += 13;                  assert(*itHat3 == 15);
    ++itHat3;                      assert(itHat3 == hat3.end());
    itHat3--;                      assert(*itHat3 == 15);
    --itHat3;                      assert(*itHat3 == 14);
    itHat3 -= 14;                  assert(itHat3 == hat3.begin());

    // testing random access for iterators

    std::vector<int> hat3Expected = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    assert(hat3.get_as_vector() == hat3Expected);

    auto hat3rait1 = hat3.begin();      assert(*hat3rait1 == 0);
    assert(hat3.begin() + 16 == hat3.end());
    hat3rait1 += 3;                     assert(*hat3rait1 == 3);
    hat3rait1 += (-1);                  assert(*hat3rait1 == 2);
    assert(hat3rait1 + 1 == 1 + hat3rait1 && *(hat3rait1 + 1) == *(1 + hat3rait1));
    hat3rait1 -= 1;                     assert(*hat3rait1 == 1);
    
    assert(hat3rait1[-1] == 0);         assert(*(hat3rait1 + (-1)) == 0);
    assert(hat3rait1[0] == 1);          assert(*(hat3rait1 + 0) == 1);
    assert(hat3rait1[1] == 2);          assert(*(hat3rait1 + 1) == 2);

    hat3rait1 = hat3.begin();
    auto hat3rait2 = hat3.end();
    assert(static_cast<size_t>(hat3rait2 - hat3rait1) == hat3.size() && hat3rait2 == hat3rait1 + (hat3rait2 - hat3rait1));
    assert((hat3rait1 - hat3rait2) == (-1) * static_cast<int>(hat3.size()) && hat3rait1 == hat3rait2 + (hat3rait1 - hat3rait2));
    hat3rait1 += 1;
    auto hat3rait3 = hat3rait1 + 1;
    hat3rait2 -= 1;
    assert (hat3rait2 - hat3rait1 == 14);
    assert(hat3rait1 < hat3rait2 && hat3rait2 - hat3rait1 > 0);
    assert(!(hat3rait2 < hat3rait1));
    assert(hat3rait1 < hat3rait3 && hat3rait3 < hat3rait2 && hat3rait1 < hat3rait2);
    assert(hat3rait2 > hat3rait1);
    assert(hat3rait1 <= hat3rait2 && (hat3rait1 <= hat3rait2) == !(hat3rait1 > hat3rait2));
    assert(hat3rait2 >= hat3rait1 && (hat3rait2 >= hat3rait1) == !(hat3rait2 < hat3rait1));
}
