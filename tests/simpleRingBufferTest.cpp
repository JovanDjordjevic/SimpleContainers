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

int main() {
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

    std::cout << "================= TESTING RING BUFFER MEMBER FUNCTIONS =================" << std::endl;

    std::vector<SomeClass> tmpVecMaxSize;
    assert(rb5.max_size() == tmpVecMaxSize.max_size());

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

    std::cout << "================= TESTING RING BUFFER INSERTION =================" << std::endl;

    simpleContainers::RingBuffer<int> rb6(5);

    for(int i = 0; i < 50; ++i) {
        rb6.push_back(i);

        if (i == 3) {
            std::vector<int> expectedResult = {0, 1, 2, 3};
            assert(rb6.get_elements() == expectedResult);
        }
        else if (i == 4) {
            std::vector<int> expectedResult = {0, 1, 2, 3, 4};
            assert(rb6.get_elements() == expectedResult);
        }
        else if (i == 5) {
            std::vector<int> expectedResult = {1, 2, 3, 4, 5};  // true order at this time in the container is {5, 1, 2, 3, 4}
            assert(rb6.get_elements() == expectedResult);
        }
        else if (i == 11) {
            std::vector<int> expectedResult = {7, 8, 9, 10, 11};  // true order at this time in the container is {10, 11, 7, 8, 9};
            assert(rb6.get_elements() == expectedResult);
        }
        else if (i == 22) {
            std::vector<int> expectedResult = {18, 19, 20, 21, 22};  // true order at this time in the container is {20, 21, 22, 18, 19};
            assert(rb6.get_elements() == expectedResult);
        }
        else if (i == 38) {
            std::vector<int> expectedResult = {34, 35, 36, 37, 38};  // true order at this time in the container is {35, 36, 37, 38, 34};
            assert(rb6.get_elements() == expectedResult);
        }
        else if (i == 45) {
            std::vector<int> expectedResult = {41, 42, 43, 44, 45};  // true order at this time in the container is {41, 42, 43, 44, 45};
            assert(rb6.get_elements() == expectedResult);
        }
    }

    simpleContainers::RingBuffer<SomeClass> rb7(10);
    for (int i = 0; i < 20; ++i) {
        std::cout << "-------" << std::endl;
        rb7.push_back(SomeClass{i});
        std::cout << "-------" << std::endl;
    }
    std::cout << "------------------------------------------------------" << std::endl;

    simpleContainers::RingBuffer<SomeClass> rb8(10);
    for (int i = 0; i < 20; ++i) {
        std::cout << "-------" << std::endl;
        SomeClass scTmp{i};
        rb8.push_back(scTmp);
        std::cout << "-------" << std::endl;
    }
    std::cout << "------------------------------------------------------" << std::endl;

    simpleContainers::RingBuffer<SomeClass> rb9(10);
    SomeClass sc1{5};
    for (int i = 0; i < 20; ++i) {
        std::cout << "-------" << std::endl;
        rb9.push_back(sc1);
        std::cout << "-------" << std::endl;
    }
    std::cout << "------------------------------------------------------" << std::endl;

    simpleContainers::RingBuffer<SomeClass> rb10(10);
    for (int i = 0; i < 25; ++i) {
        std::cout << "-------" << std::endl;
        rb10.emplace_back(i);
        std::cout << "-------" << std::endl;
    }

    simpleContainers::RingBuffer<SomeClass> rb10Cpy = rb10;
    assert(rb10 == rb10Cpy);
    assert(rb9 != rb10);
    
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

    std::cout << "================= TESTING RING BUFFER ITERATORS =================" << std::endl;

    simpleContainers::RingBuffer<int> rb11(10);
    assert(rb11.empty() && (rb11.empty() == !rb1.full()));
    assert(rb11.begin() == rb11.end());

    std::cout << "------------------------------------------------------" << std::endl;

    size_t elemCnt = 0;
    std::for_each(rb11.begin(), rb11.end(), [&elemCnt](const int){ ++elemCnt; });
    assert(elemCnt == rb11.size());

    std::cout << "------------------------------------------------------" << std::endl;

    for (int i = 0; i < 5; ++i) { rb11.emplace_back(i); }

    elemCnt = 0;
    std::for_each(rb11.begin(), rb11.end(), [&elemCnt](const int){ ++elemCnt; });
    assert(elemCnt == 5 && rb11.size() == 5);
    assert(*(rb11.begin()) == 0);

    std::cout << "------------------------------------------------------" << std::endl;

    for (int i = 5; i < 10; ++i) {
        rb11.emplace_back(i); 
    }

    elemCnt = 0;
    std::for_each(rb11.begin(), rb11.end(), [&elemCnt](const int){ ++elemCnt; });

    assert(elemCnt == 10);
    assert(rb11.full());
    assert(*(rb11.begin()) == 0);

    std::cout << "------------------------------------------------------" << std::endl;

    for (int i = 10; i < 15; ++i) {
        rb11.emplace_back(i); 
    }

    elemCnt = 0;
    std::for_each(rb11.begin(), rb11.end(), [&elemCnt](const int){ ++elemCnt; });

    assert(elemCnt == 10);
    assert(*(rb11.begin()) == 5);
    std::vector<int> expectedResult = {5, 6, 7, 8, 9, 10, 11, 12, 13, 14};  // true order at this time in the container is {10, 11, 12, 13, 14, 5, 6, 7, 8, 9};
    assert(rb11.get_elements() == expectedResult);

    auto itRb11Find1 = std::find(rb11.begin(), rb11.end(), 8);
    assert(itRb11Find1 != rb11.end() && *itRb11Find1 == 8);
    auto itRb11Find2 = std::find(rb11.begin(), rb11.end(), 12);
    assert(itRb11Find2 != rb11.end() && *itRb11Find2 == 12);

    itRb11Find1.swap(itRb11Find2);
    assert(*itRb11Find2 == 8 && *itRb11Find1 == 12);

    std::swap(itRb11Find1, itRb11Find2);
    assert(*itRb11Find1 == 8 && *itRb11Find2 == 12);

    std::iter_swap(itRb11Find1, itRb11Find2);
    assert(*itRb11Find1 == 12 && *itRb11Find2 == 8);
    expectedResult = {5, 6, 7, 12, 9, 10, 11, 8, 13, 14};
    assert(rb11.get_elements() == expectedResult);

    // test multipass
    simpleContainers::RingBuffer<int>::iterator itRb11Begin = rb11.begin();
    simpleContainers::RingBuffer<int>::iterator itRb11BeginCpy = itRb11Begin;
    auto rb11Item1 = *itRb11Begin;
    assert(rb11Item1 == 5);
    ++itRb11Begin;
    itRb11Begin++;
    rb11Item1 = *itRb11Begin;
    assert(rb11Item1 == 7);
    auto rb11Item2 = *itRb11BeginCpy;
    assert(rb11Item2 == 5);

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
    for (auto i = rb11.cbegin(); i != rb11.cend(); ++i) {
        ++ elemCnt;
    }
    assert(elemCnt == 10);

    const simpleContainers::RingBuffer<int> rb11Const = rb11;
    elemCnt = 0;
    std::for_each(rb11.begin(), rb11.end(), [&elemCnt](const int){ ++elemCnt; });
    assert(elemCnt == 10);

    std::cout << "------------------------------------------------------" << std::endl;

    simpleContainers::RingBuffer<int>::const_iterator itRb11Cbegin = rb11.cbegin();
    simpleContainers::RingBuffer<int>::const_iterator itRb11CbeginCpy = itRb11Cbegin;
    auto rb11Citem1 = *itRb11Cbegin;
    assert(rb11Citem1 == 5);
    ++itRb11Cbegin;
    itRb11Cbegin++;
    rb11Citem1 = *itRb11Cbegin;
    assert(rb11Citem1 == 7);
    auto rb11Citem2 = *itRb11CbeginCpy;
    assert(rb11Citem2 == 5);

    std::cout << "------------------------------------------------------" << std::endl;

    // create const from non const iterator
    simpleContainers::RingBuffer<int>::const_iterator rb11ConstItFromNonConstIt = rb11.begin();
    assert(*rb11ConstItFromNonConstIt == *rb11.begin());

    auto tmpIt = rb11.cbegin();
    tmpIt = rb11.begin();
        
    std::cout << "------------------------------------------------------" << std::endl;

    simpleContainers::RingBuffer<int> rb12;
    for (int i = 0; i < 500; ++i) { rb12.emplace_back(i); }
    for (auto& elem : rb12) { elem = 0; }
    auto itRb12End = rb12.end();
    for (auto it = rb12.begin(); it != itRb12End; ++it) { assert (*it == 0); }
    for (auto it = rb12.begin(); it != itRb12End; ++it) { *it = 5; }
    for (auto it = rb12.begin(); it != itRb12End; ++it) { assert (*it == 5); }

    // const simpleContainers::RingBuffer<int> rb12Const = rb12;
    // for (auto& elem : rb12Const) { elem = 0; }
    // for (auto it = rb12Const.begin(); it != rb12Const.end(); ++it) {
    //     assert (*it == 0);   // fails compilation as expected
    // }

    std::cout << "------------------------------------------------------" << std::endl;
    // test bidirection
    simpleContainers::RingBuffer<int> rb13(5);
    for (int i = 1; i < 4; ++i) { rb13.emplace_back(i); } // fill half
    assert(rb13.size() == 3 && rb13.capacity() == 5);

    auto itRb13 = rb13.begin();         assert(*itRb13 == 1);
    ++itRb13;                           assert(*itRb13 == 2);
    itRb13++;                           assert(*itRb13 == 3);
    ++itRb13;                           assert(itRb13 == rb13.end());
    --itRb13;                           assert(*itRb13 == 3);
    itRb13--;                           assert(*itRb13 == 2);
    --itRb13;                           assert(itRb13 == rb13.begin());

    for (int i = 4; i < 6; ++i) { rb13.emplace_back(i); }    // fill entire, without old value overwrite
    assert(rb13.size() == 5 && rb13.capacity() == 5);

    itRb13 = rb13.begin();         assert(*itRb13 == 1);
    itRb13++;                      assert(*itRb13 == 2);
    ++itRb13;                      assert(*itRb13 == 3);
    itRb13++;                      assert(*itRb13 == 4);
    ++itRb13;                      assert(*itRb13 == 5);
    ++itRb13;                      assert(itRb13 == rb13.end());
    itRb13--;                      assert(*itRb13 == 5);
    --itRb13;                      assert(*itRb13 == 4);
    itRb13--;                      assert(*itRb13 == 3);
    --itRb13;                      assert(*itRb13 == 2);
    --itRb13;                      assert(itRb13 == rb13.begin());

    for (int i = 6; i < 8; ++i) { rb13.emplace_back(i); }    // overwrite old values
    assert(rb13.size() == 5 && rb13.capacity() == 5);

    itRb13 = rb13.begin();         assert(*itRb13 == 3);
    itRb13++;                      assert(*itRb13 == 4);
    ++itRb13;                      assert(*itRb13 == 5);
    itRb13++;                      assert(*itRb13 == 6);
    ++itRb13;                      assert(*itRb13 == 7);
    ++itRb13;                      assert(itRb13 == rb13.end());
    itRb13--;                      assert(*itRb13 == 7);
    --itRb13;                      assert(*itRb13 == 6);
    itRb13--;                      assert(*itRb13 == 5);
    --itRb13;                      assert(*itRb13 == 4);
    --itRb13;                      assert(itRb13 == rb13.begin());

    std::cout << "------------------------------------------------------" << std::endl;
    // testing random access for iterators

    std::vector<int> rb13Expected{3, 4, 5, 6, 7};
    assert (rb13.get_elements() == rb13Expected); // actual order in vector {6, 7, 3, 4, 5}

    auto rb13rait1 = rb13.begin();      assert(*rb13rait1 == 3);
    assert(rb13.begin() + 5 == rb13.end());
    rb13rait1 += 3;                     assert(*rb13rait1 == 6);
    rb13rait1 += (-1);                  assert(*rb13rait1 == 5);
    assert(rb13rait1 + 1 == 1 + rb13rait1 && *(rb13rait1 + 1) == *(1 + rb13rait1));
    rb13rait1 -= 1;                     assert(*rb13rait1 == 4);
    
    assert(rb13rait1[-1] == 3);         assert(*(rb13rait1 + (-1)) == 3);
    assert(rb13rait1[0] == 4);          assert(*(rb13rait1 + 0) == 4);
    assert(rb13rait1[1] == 5);          assert(*(rb13rait1 + 1) == 5);

    rb13rait1 = rb13.begin();
    auto rb13rait2 = rb13.end();
    assert(static_cast<size_t>(rb13rait2 - rb13rait1) == rb13.size() && rb13rait2 == rb13rait1 + (rb13rait2 - rb13rait1));
    assert((rb13rait1 - rb13rait2) == (-1) * static_cast<int>(rb13.size()) && rb13rait1 == rb13rait2 + (rb13rait1 - rb13rait2));
    rb13rait1 += 1;
    auto rb13rait3 = rb13rait1 + 1;
    rb13rait2 -= 1;
    assert (rb13rait2 - rb13rait1 == 3);
    assert(rb13rait1 < rb13rait2 && rb13rait2 - rb13rait1 > 0);
    assert(!(rb13rait2 < rb13rait1));
    assert(rb13rait1 < rb13rait3 && rb13rait3 < rb13rait2 && rb13rait1 < rb13rait2);
    assert(rb13rait2 > rb13rait1);
    assert(rb13rait1 <= rb13rait2 && (rb13rait1 <= rb13rait2) == !(rb13rait1 > rb13rait2));
    assert(rb13rait2 >= rb13rait1 && (rb13rait2 >= rb13rait1) == !(rb13rait2 < rb13rait1));

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

    std::cout << "================= TESTING IN STL ALGORITHMS =================" << std::endl;

    simpleContainers::RingBuffer<long> rb14{262, 3426, -123, 552, -91, 251, 673};
    assert(rb14.size() == 7 && rb14.capacity() == 7);

    assert(std::all_of(rb14.cbegin(), rb14.cend(), [](long i) { return i != 0; }));
    assert(std::any_of(rb14.begin(), rb14.end(), [](long i) { return i % 2 == 0; }));
    assert(std::count(rb14.cbegin(), rb14.cend(), 552) == 1);
    assert(std::find(rb14.begin(), rb14.end(), -91) != rb14.end());

    simpleContainers::RingBuffer<long> rb14Filtered;
    assert(rb14Filtered.empty() && rb14Filtered.capacity() == simpleContainers::RingBuffer<long>::defaultInitialCapacity);
    std::copy_if(rb14.begin(), rb14.end(), std::back_inserter(rb14Filtered), [](long x) { return x < 0; });

    auto rb14Cpy = rb14;
    std::fill(std::begin(rb14Cpy), std::end(rb14Cpy), 5);
    assert(static_cast<size_t>(std::count(rb14Cpy.cbegin(), rb14Cpy.cend(), 5)) == rb14Cpy.size());

    std::transform(std::begin(rb14Cpy), std::end(rb14Cpy), std::begin(rb14Cpy), [](long x) { return x * 2; });
    assert(static_cast<size_t>(std::count(rb14Cpy.cbegin(), rb14Cpy.cend(), 10)) == rb14Cpy.size());

    rb14Cpy = rb14;
    std::replace_if(std::begin(rb14Cpy), std::end(rb14Cpy), [](long x) { return x > 1000; }, 1000);
    assert(std::count(rb14Cpy.cbegin(), rb14Cpy.cend(), 1000) == 1);

    // std::for_each(rb14Cpy.begin(), rb14Cpy.end(), [](long elem){ std::cout << elem << std::endl; });
    auto itRemoveRb14 = std::remove_if(rb14Cpy.begin(), rb14Cpy.end(), [](long elem){ return elem > 0; });
    assert(itRemoveRb14 == rb14Cpy.begin() + 2);
    assert(*rb14Cpy.begin() == -123 && *(rb14Cpy.begin() + 1) == -91);
    // std::for_each(rb14Cpy.begin(), rb14Cpy.end(), [](long elem){ std::cout << elem << std::endl; });

    std::vector<long> rb14Expected{262, 3426, -123, 552, -91, 251, 673};
    assert(rb14.get_elements() == rb14Expected);
    std::reverse(rb14.begin(), rb14.end());
    rb14Expected = {673, 251, -91, 552, -123, 3426, 262};
    assert(rb14.get_elements() == rb14Expected);

    auto itRb14Find = std::find(rb14.begin(), rb14.end(), -91);
    assert(itRb14Find != rb14.end());
    std::rotate(rb14.begin(), itRb14Find, rb14.end());
    rb14Expected = {-91, 552, -123, 3426, 262, 673, 251};
    assert(rb14.get_elements() == rb14Expected);

    rb14Cpy = rb14;
    std::random_device rd;
    std::mt19937 generator(rd());
    // std::cout << "before shuffle" << std::endl;
    // std::for_each(rb14Cpy.begin(), rb14Cpy.end(), [](long elem){ std::cout << elem << std::endl; });
    std::shuffle(rb14Cpy.begin(), rb14Cpy.end(), generator);
    // std::cout << "after shuffle" << std::endl;
    // std::for_each(rb14Cpy.begin(), rb14Cpy.end(), [](long elem){ std::cout << elem << std::endl; });

    rb14Cpy = rb14;
    auto itRb14Partition = std::stable_partition(rb14Cpy.begin(), rb14Cpy.end(), [](long x) {return x % 2 == 0;});
    assert(*itRb14Partition == -91); // to suppress the itRb14Partition unused warning
    rb14Expected = {552, 3426, 262, -91, -123, 673, 251};
    assert(rb14Cpy.get_elements() == rb14Expected);

    auto itRb14LowerBound = std::lower_bound(rb14Cpy.begin(), rb14Cpy.end(), 673);
    assert(itRb14LowerBound != rb14Cpy.end());

    assert(!std::is_sorted(rb14Cpy.begin(), rb14Cpy.end()));
    // std::cout << "before sort" << std::endl;
    // std::for_each(rb14Cpy.begin(), rb14Cpy.end(), [](long elem){ std::cout << elem << std::endl; });
    std::sort(rb14Cpy.begin(), rb14Cpy.end());
    // std::cout << "after sort" << std::endl;
    // std::for_each(rb14Cpy.begin(), rb14Cpy.end(), [](long elem){ std::cout << elem << std::endl; });
    assert(std::is_sorted(rb14Cpy.begin(), rb14Cpy.end()));

    auto itRb14Min = std::min_element(rb14Cpy.begin(), rb14Cpy.end());
    auto itRb14Max = std::max_element(rb14Cpy.begin(), rb14Cpy.end());
    assert(itRb14Min != rb14Cpy.end() && *itRb14Min == -123);
    assert(itRb14Max != rb14Cpy.end() && *itRb14Max == 3426);

    assert(4950 == std::accumulate(std::begin(rb14Cpy), std::end(rb14Cpy), long(0), [](long acc, long x) { return acc + x; }));

    std::cout << "------------------------------------------------------" << std::endl;

    return 0;
}
