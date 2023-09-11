#include <iostream>
#include <cassert>

#include "simpleContainers/simpleRingBuffer.hpp"

#include "someTestClass.hpp"

int main() {
    // construction tests

    simpleContainers::RingBuffer<SomeClass> rb1; // default ctor
    assert(rb1.getCurrentCapacity() == simpleContainers::RingBuffer<SomeClass>::defaultInitialCapacity);
    assert(rb1.getCurrentSize() == 0);

    simpleContainers::RingBuffer<SomeClass> rb2 = rb1;  // copy ctor
    assert(rb2.getCurrentCapacity() == rb1.getCurrentCapacity());
    assert(rb2.getCurrentSize() == rb1.getCurrentSize());

    simpleContainers::RingBuffer<SomeClass> rb3{rb1};  // copy ctor
    assert(rb3.getCurrentCapacity() == rb1.getCurrentCapacity());
    assert(rb3.getCurrentSize() == rb1.getCurrentSize());

    rb3 = rb2; // copy assignment
    assert(rb3.getCurrentCapacity() == rb2.getCurrentCapacity());
    assert(rb3.getCurrentSize() == rb2.getCurrentSize());

    std::size_t tmpRb1InitialCapacity = 5;
    simpleContainers::RingBuffer<SomeClass> tmpRb1{tmpRb1InitialCapacity}; // capacity ctor
    assert(tmpRb1.getCurrentCapacity() == tmpRb1InitialCapacity);
    assert(tmpRb1.getCurrentSize() == 0);

    simpleContainers::RingBuffer<SomeClass> rb4 = std::move(tmpRb1); // move ctor
    assert(rb4.getCurrentCapacity() == tmpRb1InitialCapacity && tmpRb1.getCurrentCapacity() == 0);
    assert(rb4.getCurrentSize() == 0 && tmpRb1.getCurrentSize() == 0);

    simpleContainers::RingBuffer<SomeClass> rb5 = simpleContainers::RingBuffer<SomeClass>{}; // default ctor for temporary, move ctor for rb5, dtor for temporary
    assert(rb5.getCurrentCapacity() == simpleContainers::RingBuffer<SomeClass>::defaultInitialCapacity);
    assert(rb5.getCurrentSize() == 0);

    rb5 = std::move(rb4); // move assignment
    assert(rb5.getCurrentCapacity() == tmpRb1InitialCapacity && rb4.getCurrentCapacity() == 0);
    assert(rb5.getCurrentSize() == 0 && rb4.getCurrentSize() == 0);
    
    std::cout << "=================" << std::endl;
    // insertion test

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
        std::cout << "-------\n";
        rb7.push(SomeClass{i});
        std::cout << "-------\n";
    }
    std::cout << "------------------------------------------------------\n";

    simpleContainers::RingBuffer<SomeClass> rb8(10);
    for (int i = 0; i < 20; ++i) {
        std::cout << "-------\n";
        SomeClass scTmp{i};
        rb8.push(scTmp);
        std::cout << "-------\n";
    }
    std::cout << "------------------------------------------------------\n";

    simpleContainers::RingBuffer<SomeClass> rb9(10);
    SomeClass sc1{5};
    for (int i = 0; i < 20; ++i) {
        std::cout << "-------\n";
        rb9.push(sc1);
        std::cout << "-------\n";
    }
    std::cout << "------------------------------------------------------\n";

    simpleContainers::RingBuffer<SomeClass> rb10(10);
    for (int i = 0; i < 20; ++i) {
        std::cout << "-------\n";
        rb10.emplace(i);
        std::cout << "-------\n";
    }
    
    std::cout << "=================" << std::endl;

    return 0;
}
