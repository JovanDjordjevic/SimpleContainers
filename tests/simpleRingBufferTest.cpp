#include <cassert>
#include <iostream>
#include <string>
#include <utility>

#include "simpleContainers/simpleRingBuffer.hpp"

#include "someTestClass.hpp"

int main() {
    std::cout << "================= TESTING RING BUFFER CONSTRUCTION FOR CLASS TYPE =================" << std::endl;

    simpleContainers::RingBuffer<SomeClass> rb1; // default ctor
    assert(rb1.capacity() == simpleContainers::RingBuffer<SomeClass>::defaultInitialCapacity);
    assert(rb1.size() == 0);

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
    simpleContainers::RingBuffer<SomeClass> tmpRb1{tmpRb1InitialCapacity}; // capacity ctor
    assert(tmpRb1.capacity() == tmpRb1InitialCapacity);
    assert(tmpRb1.size() == 0);

    simpleContainers::RingBuffer<SomeClass> rb4 = std::move(tmpRb1); // move ctor
    assert(rb4.capacity() == tmpRb1InitialCapacity && tmpRb1.capacity() == 0);
    assert(rb4.size() == 0 && tmpRb1.size() == 0);

    simpleContainers::RingBuffer<SomeClass> rb5 = simpleContainers::RingBuffer<SomeClass>{}; // default ctor for temporary, move ctor for rb5, dtor for temporary
    assert(rb5.capacity() == simpleContainers::RingBuffer<SomeClass>::defaultInitialCapacity);
    assert(rb5.size() == 0);

    rb5 = std::move(rb4); // move assignment
    assert(rb5.capacity() == tmpRb1InitialCapacity && rb4.capacity() == 0);
    assert(rb5.size() == 0 && rb4.size() == 0);

    std::cout << "================= TESTING RING BUFFER MEMBER FUNCTIONS =================" << std::endl;

    std::vector<SomeClass> tmpVec;
    assert(rb5.max_size() == tmpVec.max_size());

    // swap
    simpleContainers::RingBuffer<std::string> rbSwap1(5);
    for (int i = 0; i < 5; ++i) { rbSwap1.emplace(std::to_string(i)); }
    std::vector<std::string> rbSwap1ExpectedContents{"0", "1", "2", "3", "4"};
    assert(rbSwap1.getElementsInInsertionOrder() == rbSwap1ExpectedContents);
    simpleContainers::RingBuffer<std::string> rbSwap1Cpy = rbSwap1;
    assert(rbSwap1 == rbSwap1Cpy);

    std::cout << "------------------------------------------------------" << std::endl;

    simpleContainers::RingBuffer<std::string> rbSwap2(5);
    for (int i = 5; i < 10; ++i) { rbSwap2.emplace(std::to_string(i)); }
    std::vector<std::string> rbSwap2ExpectedContents{"5", "6", "7", "8", "9"};
    assert(rbSwap2.getElementsInInsertionOrder() == rbSwap2ExpectedContents);
    simpleContainers::RingBuffer<std::string> rbSwap2Cpy = rbSwap2;
    assert(rbSwap2 == rbSwap2Cpy);

    std::cout << "------------------------------------------------------" << std::endl;

    assert(rbSwap1 != rbSwap2);

    std::cout << "------------------------------------------------------" << std::endl;

    rbSwap1.swap(rbSwap2);
    assert(rbSwap1 != rbSwap2);
    assert(rbSwap1.getElementsInInsertionOrder() == rbSwap2ExpectedContents);
    assert(rbSwap2.getElementsInInsertionOrder() == rbSwap1ExpectedContents);
    assert(rbSwap1 != rbSwap1Cpy && rbSwap1 == rbSwap2Cpy);
    assert(rbSwap2 != rbSwap2Cpy && rbSwap2 == rbSwap1Cpy);

    std::cout << "------------------------------------------------------" << std::endl;

    std::swap(rbSwap1, rbSwap2);
    assert(rbSwap1 != rbSwap2);
    assert(rbSwap1.getElementsInInsertionOrder() == rbSwap1ExpectedContents);
    assert(rbSwap2.getElementsInInsertionOrder() == rbSwap2ExpectedContents);
    assert(rbSwap1 == rbSwap1Cpy && rbSwap1 != rbSwap2Cpy);
    assert(rbSwap2 == rbSwap2Cpy && rbSwap2 != rbSwap1Cpy);

    
    assert(rbSwap1.full() == true);
    rbSwap1.clear();
    assert(rbSwap1.empty() == true);

    std::cout << "------------------------------------------------------" << std::endl;

    simpleContainers::RingBuffer<int> rbResize(5);
    assert(rbResize.empty() && rbResize.capacity() == 5);
    for (int i = 1; i < 10; ++i) { rbResize.emplace(i); }

    std::vector<int> rbResizeExpectedResult = {5, 6, 7, 8, 9};
    assert(rbResize.getElementsInInsertionOrder() == rbResizeExpectedResult);
    assert(rbResize.full());

    rbResize.changeCapacity(5); // should cause no change
    assert(rbResize.getElementsInInsertionOrder() == rbResizeExpectedResult);
    assert(rbResize.full());

    rbResize.changeCapacity(10); // should expand the capacity, and internally reorder 
    assert(rbResize.getElementsInInsertionOrder() == rbResizeExpectedResult);
    assert(rbResize.capacity() == 10 && !rbResize.empty() && !rbResize.full());

    for (int i = 1; i < 10; ++i) { rbResize.emplace(i); }
    rbResizeExpectedResult = {9, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    assert(rbResize.getElementsInInsertionOrder() == rbResizeExpectedResult);
    assert(rbResize.full());
    rbResize.emplace(10);
    rbResizeExpectedResult = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    assert(rbResize.getElementsInInsertionOrder() == rbResizeExpectedResult);
    assert(rbResize.full());

    rbResize.changeCapacity(5); // should reduce the capacity, keep only 5 last inserted elements  
    rbResizeExpectedResult = {6, 7, 8, 9, 10};
    assert(rbResize.getElementsInInsertionOrder() == rbResizeExpectedResult);
    assert(rbResize.full());
    rbResize.emplace(11);
    rbResizeExpectedResult = {7, 8, 9, 10, 11};
    assert(rbResize.getElementsInInsertionOrder() == rbResizeExpectedResult);
    assert(rbResize.full());

    std::cout << "================= TESTING RING BUFFER INSERTION =================" << std::endl;

    simpleContainers::RingBuffer<int> rb6(5);

    for(int i = 0; i < 50; ++i) {
        rb6.push(i);

        if (i == 3) {
            std::vector<int> expectedResult = {0, 1, 2, 3};
            assert(rb6.getElementsInInsertionOrder() == expectedResult);
        }
        else if (i == 4) {
            std::vector<int> expectedResult = {0, 1, 2, 3, 4};
            assert(rb6.getElementsInInsertionOrder() == expectedResult);
        }
        else if (i == 5) {
            std::vector<int> expectedResult = {1, 2, 3, 4, 5};  // true order at this time in the container is {5, 1, 2, 3, 4}
            assert(rb6.getElementsInInsertionOrder() == expectedResult);
        }
        else if (i == 11) {
            std::vector<int> expectedResult = {7, 8, 9, 10, 11};  // true order at this time in the container is {10, 11, 7, 8, 9};
            assert(rb6.getElementsInInsertionOrder() == expectedResult);
        }
        else if (i == 22) {
            std::vector<int> expectedResult = {18, 19, 20, 21, 22};  // true order at this time in the container is {20, 21, 22, 18, 19};
            assert(rb6.getElementsInInsertionOrder() == expectedResult);
        }
        else if (i == 38) {
            std::vector<int> expectedResult = {34, 35, 36, 37, 38};  // true order at this time in the container is {35, 36, 37, 38, 34};
            assert(rb6.getElementsInInsertionOrder() == expectedResult);
        }
        else if (i == 45) {
            std::vector<int> expectedResult = {41, 42, 43, 44, 45};  // true order at this time in the container is {41, 42, 43, 44, 45};
            assert(rb6.getElementsInInsertionOrder() == expectedResult);
        }
    }

    simpleContainers::RingBuffer<SomeClass> rb7(10);
    for (int i = 0; i < 20; ++i) {
        std::cout << "-------" << std::endl;
        rb7.push(SomeClass{i});
        std::cout << "-------" << std::endl;
    }
    std::cout << "------------------------------------------------------" << std::endl;

    simpleContainers::RingBuffer<SomeClass> rb8(10);
    for (int i = 0; i < 20; ++i) {
        std::cout << "-------" << std::endl;
        SomeClass scTmp{i};
        rb8.push(scTmp);
        std::cout << "-------" << std::endl;
    }
    std::cout << "------------------------------------------------------" << std::endl;

    simpleContainers::RingBuffer<SomeClass> rb9(10);
    SomeClass sc1{5};
    for (int i = 0; i < 20; ++i) {
        std::cout << "-------" << std::endl;
        rb9.push(sc1);
        std::cout << "-------" << std::endl;
    }
    std::cout << "------------------------------------------------------" << std::endl;

    simpleContainers::RingBuffer<SomeClass> rb10(10);
    for (int i = 0; i < 25; ++i) {
        std::cout << "-------" << std::endl;
        rb10.emplace(i);
        std::cout << "-------" << std::endl;
    }

    simpleContainers::RingBuffer<SomeClass> rb10Cpy = rb10;
    assert(rb10 == rb10Cpy);
    assert(rb9 != rb10);
    
    std::cout << "================= TESTING RING BUFFER ITERATOR =================" << std::endl;

    simpleContainers::RingBuffer<int> rb11(10);
    assert(rb11.empty() && (rb11.empty() == !rb1.full()));
    assert(rb11.begin() == rb11.end());

    std::cout << "------------------------------------------------------" << std::endl;

    int elemCnt = 0;
    for(auto& elem : rb11) { ++elemCnt; }
    assert(elemCnt == rb11.size());

    std::cout << "------------------------------------------------------" << std::endl;

    for (int i = 0; i < 5; ++i) { rb11.emplace(i); }

    elemCnt = 0;
    for(auto& elem : rb11) {
        ++elemCnt;
        // std::cout << elem << std::endl;
    }
    assert(elemCnt == 5 && rb11.size() == 5);
    assert(*(rb11.begin()) == 0);

    std::cout << "------------------------------------------------------" << std::endl;

    for (int i = 5; i < 10; ++i) {
        rb11.emplace(i); 
    }

    elemCnt = 0;
    for(auto& elem : rb11) { 
        ++elemCnt;
        // std::cout << elem << std::endl;
    }
    assert(elemCnt == 10);
    assert(rb11.full());
    assert(*(rb11.begin()) == 0);

    std::cout << "------------------------------------------------------" << std::endl;

    for (int i = 10; i < 15; ++i) {
        rb11.emplace(i); 
    }

    elemCnt = 0;
    for(auto& elem : rb11) { 
        ++elemCnt;
    }
    assert(elemCnt == 10);
    assert(*(rb11.begin()) == 5);
    std::vector<int> expectedResult = {5, 6, 7, 8, 9, 10, 11, 12, 13, 14};  // true order at this time in the container is {10, 11, 12, 13, 14, 5, 6, 7, 8, 9};
    assert(rb11.getElementsInInsertionOrder() == expectedResult);

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

    elemCnt = 0;
    for (auto i = rb11.cbegin(); i != rb11.cend(); ++i) {
        ++ elemCnt;
    }
    assert(elemCnt == 10);

    const simpleContainers::RingBuffer<int> rb11Const = rb11;
    elemCnt = 0;
    for (auto& elem : rb11Const) {
        ++ elemCnt;
    }
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

    auto tmpIt = rb11.cbegin();
    tmpIt = rb11.begin();
        
    std::cout << "------------------------------------------------------" << std::endl;

    simpleContainers::RingBuffer<int> rb12;
    for (int i = 0; i < 500; ++i) { rb12.emplace(i); }
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


    return 0;
}
