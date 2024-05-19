#include <algorithm>
#include <cassert>
#include <exception>
#include <iostream>
#include <numeric>
#include <random>
#include <set>
#include <string>
#include <utility>

#include "simpleContainers/simpleRingBuffer.hpp"

#include "someTestClass.hpp"
#include "someTemplateTestClass.hpp"
#include "someAllocatorClass.hpp"

void test_ring_buffer_construction();
void test_ring_buffer_member_functions();
void test_ring_buffer_insertion();
void test_ring_buffer_iterators();
void test_ring_buffer_in_stl_containers();
void test_ring_buffer_in_stl_algorithms();

int main() {
    test_ring_buffer_construction();
    test_ring_buffer_member_functions();
    test_ring_buffer_insertion();
    test_ring_buffer_iterators();
    test_ring_buffer_in_stl_containers();
    test_ring_buffer_in_stl_algorithms();
    return 0;
}

void test_ring_buffer_construction() {
    std::cout << "================= TESTING RING BUFFER CONSTRUCTION =================" << std::endl;

    simpleContainers::RingBuffer<SomeClass> rb1; // default ctor
    assert(rb1.capacity() == simpleContainers::RingBuffer<SomeClass>::defaultInitialCapacity);
    assert(rb1.size() == 0);

    SomeAllocatorClass<SomeClass> customAlloc;
    simpleContainers::RingBuffer<SomeClass, SomeAllocatorClass<SomeClass>> rb1CustomAlloc(5, customAlloc); // default ctor with custom allocator
    assert(rb1CustomAlloc.capacity() == 5);
    assert(rb1CustomAlloc.size() == 0);
    // assert(rb1CustomAlloc.get_allocator() == customAlloc);

    simpleContainers::RingBuffer<SomeClass> rb2 = rb1;  // copy ctor
    assert(rb2.capacity() == rb1.capacity());
    assert(rb2.size() == rb1.size());

    simpleContainers::RingBuffer<SomeClass> rb3{rb1};  // copy ctor
    assert(rb3.capacity() == rb1.capacity());
    assert(rb3.size() == rb1.size());

    rb3 = rb2; // copy assignment
    assert(rb3.capacity() == rb2.capacity());
    assert(rb3.size() == rb2.size());

    std::size_t tmpRb1InitialCapacity = 5;
    simpleContainers::RingBuffer<SomeClass> tmpRb1(tmpRb1InitialCapacity); // capacity ctor
    assert(tmpRb1.capacity() == tmpRb1InitialCapacity);
    assert(tmpRb1.size() == 0);

    // fails assert as expected
    // simpleContainers::RingBuffer<SomeClass> rbInvalidCapacity(0); // capacity ctor
    // assert(rbInvalidCapacity.capacity() == simpleContainers::RingBuffer<SomeClass>::defaultInitialCapacity);
    // assert(rbInvalidCapacity.size() == 0);

    std::vector<int> tmpVec{1, 2, 3, 4, 5};
    simpleContainers::RingBuffer<int> rbFromVec(tmpVec);    // ctor from vector
    assert(rbFromVec.size() == tmpVec.size() && rbFromVec.capacity() == tmpVec.size());

    simpleContainers::RingBuffer<double> rbFromInitialierList{1.0, 2.0, 3.0, 4.0, 5.0};     // ctor from initializer list
    assert(rbFromVec.size() == 5 && rbFromVec.capacity() == 5);

    simpleContainers::RingBuffer<char> rbFill(5, 'a');      // fill ctor
    assert(rbFill.size() == 5 && rbFill.capacity() == 5);
    std::vector<char> rbFillExpected(5, 'a');
    assert(rbFill.get_elements() == rbFillExpected);

    simpleContainers::RingBuffer<double> rbFromRbIterators1(tmpVec.begin(), tmpVec.end());   // iterator pair ctor
    assert(rbFromRbIterators1.size() == tmpVec.size() && rbFromRbIterators1.capacity() == tmpVec.capacity());

    simpleContainers::RingBuffer<double> rbFromRbIterators2(rbFromRbIterators1.begin(), rbFromRbIterators1.end());   // iterator pair ctor
    assert(rbFromRbIterators2.size() == rbFromRbIterators1.size() && rbFromRbIterators2.capacity() == rbFromRbIterators1.capacity());

    simpleContainers::RingBuffer<SomeClass> rb4 = std::move(tmpRb1); // move ctor
    assert(rb4.capacity() == tmpRb1InitialCapacity);
    assert(rb4.size() == 0);

    simpleContainers::RingBuffer<SomeClass> rb5 = simpleContainers::RingBuffer<SomeClass>{}; // default ctor for temporary, move ctor for rb5, dtor for temporary
    assert(rb5.capacity() == simpleContainers::RingBuffer<SomeClass>::defaultInitialCapacity);
    assert(rb5.size() == 0);

    rb5 = std::move(rb4); // move assignment
    assert(rb5.capacity() == tmpRb1InitialCapacity);
    assert(rb5.size() == 0);
}

void test_ring_buffer_member_functions() {
    std::cout << "================= TESTING RING BUFFER MEMBER FUNCTIONS =================" << std::endl;

    simpleContainers::RingBuffer<SomeClass> rb1;
    std::vector<SomeClass> tmpVecMaxSize;
    assert(rb1.max_size() == tmpVecMaxSize.max_size());

    // swap
    simpleContainers::RingBuffer<std::string> rbSwap1(5);
    for (int i = 0; i < 5; ++i) { rbSwap1.emplace_back(std::to_string(i)); }
    std::vector<std::string> rbSwap1ExpectedContents{"0", "1", "2", "3", "4"};
    assert(rbSwap1.get_elements() == rbSwap1ExpectedContents);
    simpleContainers::RingBuffer<std::string> rbSwap1Cpy = rbSwap1;
    assert(rbSwap1 == rbSwap1Cpy);

    std::cout << "------------------------------------------------------" << std::endl;

    simpleContainers::RingBuffer<std::string> rbSwap2(5);
    for (int i = 5; i < 10; ++i) { rbSwap2.emplace_back(std::to_string(i)); }
    std::vector<std::string> rbSwap2ExpectedContents{"5", "6", "7", "8", "9"};
    assert(rbSwap2.get_elements() == rbSwap2ExpectedContents);
    simpleContainers::RingBuffer<std::string> rbSwap2Cpy = rbSwap2;
    assert(rbSwap2 == rbSwap2Cpy);

    std::cout << "------------------------------------------------------" << std::endl;

    assert(rbSwap1 != rbSwap2);

    std::cout << "------------------------------------------------------" << std::endl;

    rbSwap1.swap(rbSwap2);
    assert(rbSwap1 != rbSwap2);
    assert(rbSwap1.get_elements() == rbSwap2ExpectedContents);
    assert(rbSwap2.get_elements() == rbSwap1ExpectedContents);
    assert(rbSwap1 != rbSwap1Cpy && rbSwap1 == rbSwap2Cpy);
    assert(rbSwap2 != rbSwap2Cpy && rbSwap2 == rbSwap1Cpy);

    std::cout << "------------------------------------------------------" << std::endl;

    std::swap(rbSwap1, rbSwap2);
    assert(rbSwap1 != rbSwap2);
    assert(rbSwap1.get_elements() == rbSwap1ExpectedContents);
    assert(rbSwap2.get_elements() == rbSwap2ExpectedContents);
    assert(rbSwap1 == rbSwap1Cpy && rbSwap1 != rbSwap2Cpy);
    assert(rbSwap2 == rbSwap2Cpy && rbSwap2 != rbSwap1Cpy);

    
    assert(rbSwap1.full() == true);
    rbSwap1.clear();
    assert(rbSwap1.empty() == true);

    std::cout << "------------------------------------------------------" << std::endl;

    simpleContainers::RingBuffer<int> rbResize(5);
    assert(rbResize.empty() && rbResize.capacity() == 5);
    for (int i = 1; i < 10; ++i) { rbResize.emplace_back(i); }

    std::vector<int> rbResizeExpectedResult = {5, 6, 7, 8, 9};
    assert(rbResize.get_elements() == rbResizeExpectedResult);
    assert(rbResize.full());

    // fails assert as expected
    // rbResize.change_capacity(0);
    // assert(rbResize.get_elements() == rbResizeExpectedResult);
    // assert(rbResize.full());

    rbResize.change_capacity(5); // should cause no change
    assert(rbResize.get_elements() == rbResizeExpectedResult);
    assert(rbResize.full());

    rbResize.change_capacity(10); // should expand the capacity, and internally reorder 
    assert(rbResize.get_elements() == rbResizeExpectedResult);
    assert(rbResize.capacity() == 10 && !rbResize.empty() && !rbResize.full());

    for (int i = 1; i < 10; ++i) { rbResize.emplace_back(i); }
    rbResizeExpectedResult = {9, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    assert(rbResize.get_elements() == rbResizeExpectedResult);
    assert(rbResize.full());
    rbResize.emplace_back(10);
    rbResizeExpectedResult = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    assert(rbResize.get_elements() == rbResizeExpectedResult);
    assert(rbResize.full());

    rbResize.change_capacity(5); // should reduce the capacity, keep only 5 last inserted elements  
    rbResizeExpectedResult = {6, 7, 8, 9, 10};
    assert(rbResize.get_elements() == rbResizeExpectedResult);
    assert(rbResize.full());
    rbResize.emplace_back(11);
    rbResizeExpectedResult = {7, 8, 9, 10, 11};
    assert(rbResize.get_elements() == rbResizeExpectedResult);
    assert(rbResize.full());

    std::cout << "------------------------------------------------------" << std::endl;

    // test erasure when ring buffer is not full
    simpleContainers::RingBuffer<int> rbErase1(20);
    for (int i = 0; i < 10; ++i) { rbErase1.emplace_back(i); }
    assert(rbErase1.size() == 10 && rbErase1.capacity() == 20);

    auto rbErase1It = rbErase1.erase(rbErase1.begin() + 3);
    std::vector<int> rbErase1Expected{0, 1, 2, 4, 5, 6, 7, 8, 9};
    assert(rbErase1.get_elements() == rbErase1Expected);
    assert(*rbErase1It == 4);

    rbErase1.emplace_back(10);
    rbErase1Expected = {0, 1, 2, 4, 5, 6, 7, 8, 9, 10};
    assert(rbErase1.get_elements() == rbErase1Expected);

    rbErase1It = rbErase1.erase(rbErase1.end() - 1);
    rbErase1Expected = {0, 1, 2, 4, 5, 6, 7, 8, 9};
    assert(rbErase1.get_elements() == rbErase1Expected);
    assert(rbErase1It == rbErase1.end());

    rbErase1.emplace_back(11);
    rbErase1Expected = {0, 1, 2, 4, 5, 6, 7, 8, 9, 11};
    assert(rbErase1.get_elements() == rbErase1Expected);

    rbErase1It = rbErase1.erase(rbErase1.begin() + 4, rbErase1.begin() + 7);
    rbErase1Expected = {0, 1, 2, 4, 8, 9, 11};
    assert(rbErase1.get_elements() == rbErase1Expected);
    assert(*rbErase1It == 8);

    rbErase1It = rbErase1.erase(rbErase1.end() - 3, rbErase1.end());
    rbErase1Expected = {0, 1, 2, 4};
    assert(rbErase1.get_elements() == rbErase1Expected);
    assert(rbErase1It == rbErase1.end());

    rbErase1.emplace_back(12);
    rbErase1Expected = {0, 1, 2, 4, 12};
    assert(rbErase1.get_elements() == rbErase1Expected);

    std::cout << "------------------------------------------------------" << std::endl;

    // test erasure when ring buffer is full and next insertion index is somewhere in the middle of the internal vector
    simpleContainers::RingBuffer<int> rbErase2Original(10);
    for (int i = 0; i < 15; ++i) { rbErase2Original.emplace_back(i); }
    assert(rbErase2Original.size() == rbErase2Original.capacity());
    
    simpleContainers::RingBuffer<int> rbErase2 = rbErase2Original;

    std::vector<int> rbErase2Expected{5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
    assert(rbErase2.get_elements() == rbErase2Expected);

    auto rbErase2It = rbErase2.erase(rbErase2.begin() + 6);
    rbErase2Expected = {5, 6, 7, 8, 9, 10, 12, 13, 14};
    assert(rbErase2.get_elements() == rbErase2Expected);
    assert(*rbErase2It == 12);

    for(int i = 15; i < 17; ++i) { rbErase2.push_back(i); }
    rbErase2Expected = {6, 7, 8, 9, 10, 12, 13, 14, 15, 16};
    assert(rbErase2.get_elements() == rbErase2Expected);

    rbErase2It = rbErase2.erase(rbErase2.end() - 1);
    rbErase2Expected = {6, 7, 8, 9, 10, 12, 13, 14, 15};
    assert(rbErase2.get_elements() == rbErase2Expected);
    assert(rbErase2It == rbErase2.end());
    rbErase2It = rbErase2.erase(rbErase2.end() - 1);
    rbErase2Expected = {6, 7, 8, 9, 10, 12, 13, 14};
    assert(rbErase2.get_elements() == rbErase2Expected);
    assert(rbErase2It == rbErase2.end());

    rbErase2 = rbErase2Original;
    rbErase2It = rbErase2.erase(rbErase2.begin() + 2, rbErase2.begin() + 8);
    rbErase2Expected = {5, 6, 13, 14};
    assert(rbErase2.get_elements() == rbErase2Expected);
    assert(*rbErase2It == 13);
    
    rbErase2.emplace_back(15);
    rbErase2Expected = {5, 6, 13, 14, 15};
    assert(rbErase2.get_elements() == rbErase2Expected);

    rbErase2It = rbErase2.erase(rbErase2.begin() + 3, rbErase2.end());
    rbErase2Expected = {5, 6, 13};
    assert(rbErase2.get_elements() == rbErase2Expected);
    assert(rbErase2It == rbErase2.end());

    rbErase2It = rbErase2.erase(rbErase2.begin(), rbErase2.end());
    assert(rbErase2.empty());
    assert(rbErase2It == rbErase2.end());

    
    std::cout << "------------------------------------------------------" << std::endl;

    // test subscript operators
    simpleContainers::RingBuffer<int> rbSubscript(6);
    assert(rbSubscript.empty() && rbSubscript.capacity() == 6);

    // std::cout << rbSubscript[0] << std::endl;       // fails at runtime as expected (but only on msvc?)
    // std::cout << rbSubscript.at(0) << std::endl;    // fails at runtime as expected

    for (int i = 0; i < 6; ++i) { rbSubscript.emplace_back(i); }
    
    std::vector<int> rbSubscriptExpected{0, 1, 2, 3, 4, 5};
    assert(rbSubscript.get_elements() == rbSubscriptExpected);
    assert(rbSubscript[0] == 0 && rbSubscript[1] == 1 && rbSubscript[2] == 2
        && rbSubscript[3] == 3 && rbSubscript[4] == 4 && rbSubscript[5] == 5);
    assert(rbSubscript.at(0) == 0 && rbSubscript.at(1) == 1 && rbSubscript.at(2) == 2
        && rbSubscript.at(3) == 3 && rbSubscript.at(4) == 4 && rbSubscript.at(5) == 5);

    for (int i = 6; i < 9; ++i) { rbSubscript.emplace_back(i); }
    rbSubscriptExpected = {3, 4, 5, 6, 7, 8};       // actual order in container {6, 7, 8, 3, 4, 5}
    assert(rbSubscript.get_elements() == rbSubscriptExpected);
    assert(rbSubscript[0] == 3 && rbSubscript[1] == 4 && rbSubscript[2] == 5
        && rbSubscript[3] == 6 && rbSubscript[4] == 7 && rbSubscript[5] == 8);
    assert(rbSubscript.at(0) == 3 && rbSubscript.at(1) == 4 && rbSubscript.at(2) == 5
        && rbSubscript.at(3) == 6 && rbSubscript.at(4) == 7 && rbSubscript.at(5) == 8);

    rbSubscript[2] = 0;
    rbSubscript[3] = 0;
    rbSubscript.at(4) = 0;
    rbSubscriptExpected = {3, 4, 0, 0, 0, 8};       // actual order in container {0, 0, 8, 3, 4, 0}
    assert(rbSubscript.get_elements() == rbSubscriptExpected);
}

void test_ring_buffer_insertion() {
    std::cout << "================= TESTING RING BUFFER INSERTION =================" << std::endl;

    simpleContainers::RingBuffer<int> rb1(5);

    for(int i = 0; i < 50; ++i) {
        rb1.push_back(i);

        if (i == 3) {
            std::vector<int> expectedResult = {0, 1, 2, 3};
            assert(rb1.get_elements() == expectedResult);
        }
        else if (i == 4) {
            std::vector<int> expectedResult = {0, 1, 2, 3, 4};
            assert(rb1.get_elements() == expectedResult);
        }
        else if (i == 5) {
            std::vector<int> expectedResult = {1, 2, 3, 4, 5};  // true order at this time in the container is {5, 1, 2, 3, 4}
            assert(rb1.get_elements() == expectedResult);
        }
        else if (i == 11) {
            std::vector<int> expectedResult = {7, 8, 9, 10, 11};  // true order at this time in the container is {10, 11, 7, 8, 9};
            assert(rb1.get_elements() == expectedResult);
        }
        else if (i == 22) {
            std::vector<int> expectedResult = {18, 19, 20, 21, 22};  // true order at this time in the container is {20, 21, 22, 18, 19};
            assert(rb1.get_elements() == expectedResult);
        }
        else if (i == 38) {
            std::vector<int> expectedResult = {34, 35, 36, 37, 38};  // true order at this time in the container is {35, 36, 37, 38, 34};
            assert(rb1.get_elements() == expectedResult);
        }
        else if (i == 45) {
            std::vector<int> expectedResult = {41, 42, 43, 44, 45};  // true order at this time in the container is {41, 42, 43, 44, 45};
            assert(rb1.get_elements() == expectedResult);
        }
    }

    simpleContainers::RingBuffer<SomeClass> rb2(10);
    for (int i = 0; i < 20; ++i) {
        std::cout << "-------" << std::endl;
        rb2.push_back(SomeClass{i});
        std::cout << "-------" << std::endl;
    }
    std::cout << "------------------------------------------------------" << std::endl;

    simpleContainers::RingBuffer<SomeClass> rb3(10);
    for (int i = 0; i < 20; ++i) {
        std::cout << "-------" << std::endl;
        SomeClass scTmp{i};
        rb3.push_back(scTmp);
        std::cout << "-------" << std::endl;
    }
    std::cout << "------------------------------------------------------" << std::endl;

    simpleContainers::RingBuffer<SomeClass> rb4(10);
    SomeClass sc1{5};
    for (int i = 0; i < 20; ++i) {
        std::cout << "-------" << std::endl;
        rb4.push_back(sc1);
        std::cout << "-------" << std::endl;
    }
    std::cout << "------------------------------------------------------" << std::endl;

    simpleContainers::RingBuffer<SomeClass> rb5(10);
    for (int i = 0; i < 25; ++i) {
        std::cout << "-------" << std::endl;
        rb5.emplace_back(i);
        std::cout << "-------" << std::endl;
    }

    simpleContainers::RingBuffer<SomeClass> rb5Cpy = rb5;
    assert(rb5 == rb5Cpy);
    assert(rb4 != rb5);
    
    // test for tempalte classes

    simpleContainers::RingBuffer<SomeTemplateClass<std::string>> rbWithTemplateClass;
    assert(rbWithTemplateClass.capacity() == simpleContainers::RingBuffer<SomeTemplateClass<std::string>>::defaultInitialCapacity);
    rbWithTemplateClass.push_back(SomeTemplateClass<std::string>{"someStr1"});
    rbWithTemplateClass.emplace_back("someStr2");
    assert(rbWithTemplateClass.size() == 2);
    std::vector<SomeTemplateClass<std::string>> rbWithTemplateClassExpected{SomeTemplateClass<std::string>{"someStr1"}, SomeTemplateClass<std::string>{"someStr2"}};
    assert(rbWithTemplateClass.get_elements() == rbWithTemplateClassExpected);

    // test comparison operators

    simpleContainers::RingBuffer<int> rbCmp1{1, 2, 3};
    assert(rbCmp1 == rbCmp1);
    assert(rbCmp1 >= rbCmp1);
    assert(rbCmp1 <= rbCmp1);
    assert(!(rbCmp1 < rbCmp1));
    assert(!(rbCmp1 > rbCmp1));
    simpleContainers::RingBuffer<int> rbCmp2{1, 2, 3};
    assert(rbCmp1 == rbCmp2);
    assert(rbCmp1 <= rbCmp2);
    assert(rbCmp1 >= rbCmp2);
    assert(!(rbCmp1 < rbCmp2));
    assert(!(rbCmp1 > rbCmp2));

    rbCmp2 = {2, 3, 4, 5};
    assert(rbCmp1 != rbCmp2);
    assert(rbCmp1 < rbCmp2 && !(rbCmp2 < rbCmp1));
    assert(rbCmp1 <= rbCmp2);
    
    simpleContainers::RingBuffer<int> rbCmp3{5, 6, 7};
    assert(rbCmp1 < rbCmp2);
    assert(rbCmp2 < rbCmp3);
    assert(rbCmp1 < rbCmp3);

    rbCmp2 = {1, 2, 3};
    rbCmp3 = {1, 2, 3};
    assert(rbCmp1 == rbCmp2);
    assert(rbCmp2 == rbCmp3);
    assert(rbCmp1 == rbCmp3);
}

void test_ring_buffer_iterators() {
    std::cout << "================= TESTING RING BUFFER ITERATORS =================" << std::endl;

    simpleContainers::RingBuffer<int> rb1(10);
    assert(rb1.empty() && (rb1.empty() == !rb1.full()));
    assert(rb1.begin() == rb1.end());

    std::cout << "------------------------------------------------------" << std::endl;

    size_t elemCnt = 0;
    std::for_each(rb1.begin(), rb1.end(), [&elemCnt](const int){ ++elemCnt; });
    assert(elemCnt == rb1.size());

    std::cout << "------------------------------------------------------" << std::endl;

    for (int i = 0; i < 5; ++i) { rb1.emplace_back(i); }

    elemCnt = 0;
    std::for_each(rb1.begin(), rb1.end(), [&elemCnt](const int){ ++elemCnt; });
    assert(elemCnt == 5 && rb1.size() == 5);
    assert(*(rb1.begin()) == 0);

    std::cout << "------------------------------------------------------" << std::endl;

    for (int i = 5; i < 10; ++i) {
        rb1.emplace_back(i); 
    }

    elemCnt = 0;
    std::for_each(rb1.begin(), rb1.end(), [&elemCnt](const int){ ++elemCnt; });

    assert(elemCnt == 10);
    assert(rb1.full());
    assert(*(rb1.begin()) == 0);

    std::cout << "------------------------------------------------------" << std::endl;

    for (int i = 10; i < 15; ++i) {
        rb1.emplace_back(i); 
    }

    elemCnt = 0;
    std::for_each(rb1.begin(), rb1.end(), [&elemCnt](const int){ ++elemCnt; });

    assert(elemCnt == 10);
    assert(*(rb1.begin()) == 5);
    std::vector<int> expectedResult = {5, 6, 7, 8, 9, 10, 11, 12, 13, 14};  // true order at this time in the container is {10, 11, 12, 13, 14, 5, 6, 7, 8, 9};
    assert(rb1.get_elements() == expectedResult);

    auto itrb1Find1 = std::find(rb1.begin(), rb1.end(), 8);
    assert(itrb1Find1 != rb1.end() && *itrb1Find1 == 8);
    auto itrb1Find2 = std::find(rb1.begin(), rb1.end(), 12);
    assert(itrb1Find2 != rb1.end() && *itrb1Find2 == 12);

    itrb1Find1.swap(itrb1Find2);
    assert(*itrb1Find2 == 8 && *itrb1Find1 == 12);

    std::swap(itrb1Find1, itrb1Find2);
    assert(*itrb1Find1 == 8 && *itrb1Find2 == 12);

    std::iter_swap(itrb1Find1, itrb1Find2);
    assert(*itrb1Find1 == 12 && *itrb1Find2 == 8);
    expectedResult = {5, 6, 7, 12, 9, 10, 11, 8, 13, 14};
    assert(rb1.get_elements() == expectedResult);

    // test multipass
    simpleContainers::RingBuffer<int>::iterator itrb1Begin = rb1.begin();
    simpleContainers::RingBuffer<int>::iterator itrb1BeginCpy = itrb1Begin;
    auto rb1Item1 = *itrb1Begin;
    assert(rb1Item1 == 5);
    ++itrb1Begin;
    itrb1Begin++;
    rb1Item1 = *itrb1Begin;
    assert(rb1Item1 == 7);
    auto rb1Item2 = *itrb1BeginCpy;
    assert(rb1Item2 == 5);

    std::cout << "------------------------------------------------------" << std::endl;
    // const iteration

    simpleContainers::RingBuffer<int> rbEmpty(10);
    assert(rbEmpty.empty() && rbEmpty.capacity() == 10);
    simpleContainers::RingBuffer<int>::const_iterator itRbEmpty = rbEmpty.begin();
    assert(itRbEmpty == rbEmpty.end());

    rbEmpty.emplace_back(1);
    itRbEmpty = rbEmpty.begin();
    assert(itRbEmpty != rbEmpty.end() && *itRbEmpty == 1);

    elemCnt = 0;
    for (auto i = rb1.cbegin(); i != rb1.cend(); ++i) {
        ++ elemCnt;
    }
    assert(elemCnt == 10);

    const simpleContainers::RingBuffer<int> rb1Const = rb1;
    elemCnt = 0;
    std::for_each(rb1.begin(), rb1.end(), [&elemCnt](const int){ ++elemCnt; });
    assert(elemCnt == 10);

    std::cout << "------------------------------------------------------" << std::endl;

    simpleContainers::RingBuffer<int>::const_iterator itrb1Cbegin = rb1.cbegin();
    simpleContainers::RingBuffer<int>::const_iterator itrb1CbeginCpy = itrb1Cbegin;
    auto rb1Citem1 = *itrb1Cbegin;
    assert(rb1Citem1 == 5);
    ++itrb1Cbegin;
    itrb1Cbegin++;
    rb1Citem1 = *itrb1Cbegin;
    assert(rb1Citem1 == 7);
    auto rb1Citem2 = *itrb1CbeginCpy;
    assert(rb1Citem2 == 5);

    std::cout << "------------------------------------------------------" << std::endl;

    // create const from non const iterator
    simpleContainers::RingBuffer<int>::const_iterator rb1ConstItFromNonConstIt = rb1.begin();
    assert(*rb1ConstItFromNonConstIt == *rb1.begin());

    auto tmpIt = rb1.cbegin();
    tmpIt = rb1.begin();
        
    std::cout << "------------------------------------------------------" << std::endl;

    simpleContainers::RingBuffer<int> rb2;
    for (int i = 0; i < 500; ++i) { rb2.emplace_back(i); }
    for (auto& elem : rb2) { elem = 0; }
    auto itrb2End = rb2.end();
    for (auto it = rb2.begin(); it != itrb2End; ++it) { assert (*it == 0); }
    for (auto it = rb2.begin(); it != itrb2End; ++it) { *it = 5; }
    for (auto it = rb2.begin(); it != itrb2End; ++it) { assert (*it == 5); }

    // const simpleContainers::RingBuffer<int> rb2Const = rb2;
    // for (auto& elem : rb2Const) { elem = 0; }
    // for (auto it = rb2Const.begin(); it != rb2Const.end(); ++it) {
    //     assert (*it == 0);   // fails compilation as expected
    // }

    std::cout << "------------------------------------------------------" << std::endl;
    // test bidirection
    simpleContainers::RingBuffer<int> rb3(5);
    for (int i = 1; i < 4; ++i) { rb3.emplace_back(i); } // fill half
    assert(rb3.size() == 3 && rb3.capacity() == 5);

    auto itrb3 = rb3.begin();         assert(*itrb3 == 1);
    ++itrb3;                           assert(*itrb3 == 2);
    itrb3++;                           assert(*itrb3 == 3);
    ++itrb3;                           assert(itrb3 == rb3.end());
    --itrb3;                           assert(*itrb3 == 3);
    itrb3--;                           assert(*itrb3 == 2);
    --itrb3;                           assert(itrb3 == rb3.begin());

    for (int i = 4; i < 6; ++i) { rb3.emplace_back(i); }    // fill entire, without old value overwrite
    assert(rb3.size() == 5 && rb3.capacity() == 5);

    itrb3 = rb3.begin();         assert(*itrb3 == 1);
    itrb3++;                      assert(*itrb3 == 2);
    ++itrb3;                      assert(*itrb3 == 3);
    itrb3++;                      assert(*itrb3 == 4);
    ++itrb3;                      assert(*itrb3 == 5);
    ++itrb3;                      assert(itrb3 == rb3.end());
    itrb3--;                      assert(*itrb3 == 5);
    --itrb3;                      assert(*itrb3 == 4);
    itrb3--;                      assert(*itrb3 == 3);
    --itrb3;                      assert(*itrb3 == 2);
    --itrb3;                      assert(itrb3 == rb3.begin());

    for (int i = 6; i < 8; ++i) { rb3.emplace_back(i); }    // overwrite old values
    assert(rb3.size() == 5 && rb3.capacity() == 5);

    itrb3 = rb3.begin();         assert(*itrb3 == 3);
    itrb3++;                      assert(*itrb3 == 4);
    ++itrb3;                      assert(*itrb3 == 5);
    itrb3++;                      assert(*itrb3 == 6);
    ++itrb3;                      assert(*itrb3 == 7);
    ++itrb3;                      assert(itrb3 == rb3.end());
    itrb3--;                      assert(*itrb3 == 7);
    --itrb3;                      assert(*itrb3 == 6);
    itrb3--;                      assert(*itrb3 == 5);
    --itrb3;                      assert(*itrb3 == 4);
    --itrb3;                      assert(itrb3 == rb3.begin());

    std::cout << "------------------------------------------------------" << std::endl;
    // testing random access for iterators

    std::vector<int> rb3Expected{3, 4, 5, 6, 7};
    assert (rb3.get_elements() == rb3Expected); // actual order in vector {6, 7, 3, 4, 5}

    auto rb3rait1 = rb3.begin();      assert(*rb3rait1 == 3);
    assert(rb3.begin() + 5 == rb3.end());
    rb3rait1 += 3;                     assert(*rb3rait1 == 6);
    rb3rait1 += (-1);                  assert(*rb3rait1 == 5);
    assert(rb3rait1 + 1 == 1 + rb3rait1 && *(rb3rait1 + 1) == *(1 + rb3rait1));
    rb3rait1 -= 1;                     assert(*rb3rait1 == 4);
    
    assert(rb3rait1[-1] == 3);         assert(*(rb3rait1 + (-1)) == 3);
    assert(rb3rait1[0] == 4);          assert(*(rb3rait1 + 0) == 4);
    assert(rb3rait1[1] == 5);          assert(*(rb3rait1 + 1) == 5);

    rb3rait1 = rb3.begin();
    auto rb3rait2 = rb3.end();
    assert(static_cast<size_t>(rb3rait2 - rb3rait1) == rb3.size() && rb3rait2 == rb3rait1 + (rb3rait2 - rb3rait1));
    assert((rb3rait1 - rb3rait2) == (-1) * static_cast<int>(rb3.size()) && rb3rait1 == rb3rait2 + (rb3rait1 - rb3rait2));
    rb3rait1 += 1;
    auto rb3rait3 = rb3rait1 + 1;
    rb3rait2 -= 1;
    assert (rb3rait2 - rb3rait1 == 3);
    assert(rb3rait1 < rb3rait2 && rb3rait2 - rb3rait1 > 0);
    assert(!(rb3rait2 < rb3rait1));
    assert(rb3rait1 < rb3rait3 && rb3rait3 < rb3rait2 && rb3rait1 < rb3rait2);
    assert(rb3rait2 > rb3rait1);
    assert(rb3rait1 <= rb3rait2 && (rb3rait1 <= rb3rait2) == !(rb3rait1 > rb3rait2));
    assert(rb3rait2 >= rb3rait1 && (rb3rait2 >= rb3rait1) == !(rb3rait2 < rb3rait1));
}

void test_ring_buffer_in_stl_containers() {
    std::cout << "================= TESTING IN STL CONTAINERS =================" << std::endl;

    std::vector<simpleContainers::RingBuffer<SomeClass>> vectorOfRingBuffers;
    assert(vectorOfRingBuffers.size() == 0);
    vectorOfRingBuffers.resize(5);
    assert(vectorOfRingBuffers.size() == 5);
    for (auto& vecRb : vectorOfRingBuffers) {
        assert(vecRb.size() == 0 && vecRb.capacity() == simpleContainers::RingBuffer<SomeClass>::defaultInitialCapacity);
    }

    for (auto& vecRb : vectorOfRingBuffers) {
        std::cout << "-------" << std::endl;
        for (int i = 0; i < 5; ++i) {
            vecRb.emplace_back(SomeClass{i});
        }
        std::cout << "-------" << std::endl;
    }

    for (auto& vecRb : vectorOfRingBuffers) {
        assert(vecRb.size() == 5 && vecRb.capacity() == simpleContainers::RingBuffer<SomeClass>::defaultInitialCapacity);
    }

    std::set<simpleContainers::RingBuffer<SomeTemplateClass<int>>> setOfRingBuffers;
    assert(setOfRingBuffers.empty());
    simpleContainers::RingBuffer<SomeTemplateClass<int>> setRb1(5);
    for (int i = 0; i < 5; ++i) { setRb1.emplace_back(SomeTemplateClass<int>{i}); }
    setOfRingBuffers.emplace(setRb1);

    simpleContainers::RingBuffer<SomeTemplateClass<int>> setRb2 = setRb1;
    for (int i = 5; i < 10; ++i) { setRb2.emplace_back(SomeTemplateClass<int>{i}); }
    setOfRingBuffers.emplace(setRb2);

    auto setOfRingBuffersIt = setOfRingBuffers.begin();
    assert(*setOfRingBuffersIt == setRb1);
    ++setOfRingBuffersIt;
    assert(*setOfRingBuffersIt == setRb2);

    simpleContainers::RingBuffer<SomeTemplateClass<int>> setRb3 = {0, 0, 0};
    setRb3.change_capacity(2);
    setOfRingBuffers.insert(setRb3);

    setOfRingBuffersIt = setOfRingBuffers.begin();
    assert(*setOfRingBuffersIt == setRb3);
    ++setOfRingBuffersIt;
    assert(*setOfRingBuffersIt == setRb1);
    ++setOfRingBuffersIt;
    assert(*setOfRingBuffersIt == setRb2);
}

void test_ring_buffer_in_stl_algorithms() {
    std::cout << "================= TESTING IN STL ALGORITHMS =================" << std::endl;

    simpleContainers::RingBuffer<long> rb1{262, 3426, -123, 552, -91, 251, 673};
    assert(rb1.size() == 7 && rb1.capacity() == 7);

    assert(std::all_of(rb1.cbegin(), rb1.cend(), [](long i) { return i != 0; }));
    assert(std::any_of(rb1.begin(), rb1.end(), [](long i) { return i % 2 == 0; }));
    assert(std::count(rb1.cbegin(), rb1.cend(), 552) == 1);
    assert(std::find(rb1.begin(), rb1.end(), -91) != rb1.end());

    simpleContainers::RingBuffer<long> rb1Filtered;
    assert(rb1Filtered.empty() && rb1Filtered.capacity() == simpleContainers::RingBuffer<long>::defaultInitialCapacity);
    std::copy_if(rb1.begin(), rb1.end(), std::back_inserter(rb1Filtered), [](long x) { return x < 0; });

    auto rb1Cpy = rb1;
    std::fill(std::begin(rb1Cpy), std::end(rb1Cpy), 5);
    assert(static_cast<size_t>(std::count(rb1Cpy.cbegin(), rb1Cpy.cend(), 5)) == rb1Cpy.size());

    std::transform(std::begin(rb1Cpy), std::end(rb1Cpy), std::begin(rb1Cpy), [](long x) { return x * 2; });
    assert(static_cast<size_t>(std::count(rb1Cpy.cbegin(), rb1Cpy.cend(), 10)) == rb1Cpy.size());

    rb1Cpy = rb1;
    std::replace_if(std::begin(rb1Cpy), std::end(rb1Cpy), [](long x) { return x > 1000; }, 1000);
    assert(std::count(rb1Cpy.cbegin(), rb1Cpy.cend(), 1000) == 1);

    // std::for_each(rb1Cpy.begin(), rb1Cpy.end(), [](long elem){ std::cout << elem << std::endl; });
    auto itRemoverb1 = std::remove_if(rb1Cpy.begin(), rb1Cpy.end(), [](long elem){ return elem > 0; });
    assert(itRemoverb1 == rb1Cpy.begin() + 2);
    assert(*rb1Cpy.begin() == -123 && *(rb1Cpy.begin() + 1) == -91);
    // std::for_each(rb1Cpy.begin(), rb1Cpy.end(), [](long elem){ std::cout << elem << std::endl; });

    std::vector<long> rb1Expected{262, 3426, -123, 552, -91, 251, 673};
    assert(rb1.get_elements() == rb1Expected);
    std::reverse(rb1.begin(), rb1.end());
    rb1Expected = {673, 251, -91, 552, -123, 3426, 262};
    assert(rb1.get_elements() == rb1Expected);

    auto itrb1Find = std::find(rb1.begin(), rb1.end(), -91);
    assert(itrb1Find != rb1.end());
    std::rotate(rb1.begin(), itrb1Find, rb1.end());
    rb1Expected = {-91, 552, -123, 3426, 262, 673, 251};
    assert(rb1.get_elements() == rb1Expected);

    rb1Cpy = rb1;
    std::random_device rd;
    std::mt19937 generator(rd());
    // std::cout << "before shuffle" << std::endl;
    // std::for_each(rb1Cpy.begin(), rb1Cpy.end(), [](long elem){ std::cout << elem << std::endl; });
    std::shuffle(rb1Cpy.begin(), rb1Cpy.end(), generator);
    // std::cout << "after shuffle" << std::endl;
    // std::for_each(rb1Cpy.begin(), rb1Cpy.end(), [](long elem){ std::cout << elem << std::endl; });

    rb1Cpy = rb1;
    auto itrb1Partition = std::stable_partition(rb1Cpy.begin(), rb1Cpy.end(), [](long x) {return x % 2 == 0;});
    assert(*itrb1Partition == -91); // to suppress the itrb1Partition unused warning
    rb1Expected = {552, 3426, 262, -91, -123, 673, 251};
    assert(rb1Cpy.get_elements() == rb1Expected);

    auto itrb1LowerBound = std::lower_bound(rb1Cpy.begin(), rb1Cpy.end(), 673);
    assert(itrb1LowerBound != rb1Cpy.end());

    assert(!std::is_sorted(rb1Cpy.begin(), rb1Cpy.end()));
    // std::cout << "before sort" << std::endl;
    // std::for_each(rb1Cpy.begin(), rb1Cpy.end(), [](long elem){ std::cout << elem << std::endl; });
    std::sort(rb1Cpy.begin(), rb1Cpy.end());
    // std::cout << "after sort" << std::endl;
    // std::for_each(rb1Cpy.begin(), rb1Cpy.end(), [](long elem){ std::cout << elem << std::endl; });
    assert(std::is_sorted(rb1Cpy.begin(), rb1Cpy.end()));

    auto itrb1Min = std::min_element(rb1Cpy.begin(), rb1Cpy.end());
    auto itrb1Max = std::max_element(rb1Cpy.begin(), rb1Cpy.end());
    assert(itrb1Min != rb1Cpy.end() && *itrb1Min == -123);
    assert(itrb1Max != rb1Cpy.end() && *itrb1Max == 3426);

    assert(4950 == std::accumulate(std::begin(rb1Cpy), std::end(rb1Cpy), long(0), [](long acc, long x) { return acc + x; }));
}
