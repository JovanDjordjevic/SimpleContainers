#include <algorithm>
#include <iostream>
#include <iterator>
#include <random>
#include <sstream>
#include <string>

#include "simpleContainers/simpleRingBuffer.hpp"

// A ring buffer is a structure of size N that is used to hold the last N data entris of some operation.

// This example covers some basic usage, and not all supported functionality is shown
// For more details on available methods see either the generated documentation, simpleRingBuffer.hpp file 
// itself or some of the test cases in simpleRingBufferTest.cpp

template <typename T>
void printRingBuffer(const simpleContainers::RingBuffer<T>& rb1) {
    std::cout << "ring buffer contains: { ";
    for (auto& elem : rb1) { std::cout << elem << " "; }
    std::cout << "}" << std::endl;
    return;
}

int main() {
    // For example, a random number is generated every second, and we are interested in the last 10 generated numbers

    // construct ring buffer with capacity of 5
    simpleContainers::RingBuffer<unsigned int> rb1(10);

    std::cout << "rb1 size: " << rb1.size() << " rb1 capacity: " << rb1.capacity() << std::endl;

    // setup random number generation
    std::random_device device;
    std::mt19937 generator{device()};
    std::uniform_int_distribution<std::mt19937::result_type> distr(0, 100); // distribution in range [0, 100]

    // notice that after the 10th iteration, the oldest elements are no longer there, and new elements are inserted
    for (int i = 0; i < 20; ++i) {
        // "sleep 1s"
        rb1.push_back(distr(generator)); // an emplace_back method also exists, similar behavior to a std::vector
        printRingBuffer(rb1);
    }

    std::cout << "------------------------------------------------------------------------------------" << std::endl;

    // The capacity of a ring buffer can increase or decrease.
    std::cout << "rb1 can currently hold a maximum of " << rb1.capacity() << " elements" << std::endl;

    // If size is decreased from N to M, then the oldest N-M elements will be dropped.
    rb1.change_capacity(5);
    std::cout << "rb1 can currently hold a maximum of " << rb1.capacity() << " elements" << std::endl;
    printRingBuffer(rb1);

    // If size is increased from N to M then no old elements will be dropped for the next M-N insertions (untill the ring buffer is filled again)
    rb1.change_capacity(15);
    std::cout << "rb1 can currently hold a maximum of " << rb1.capacity() << " elements" << std::endl;
    printRingBuffer(rb1);

    // The entire contents of a ring buffer can also be cleared
    rb1.clear();
    std::cout << "rb1 is empty: " << std::boolalpha << rb1.empty() << std::endl;

    std::cout << "------------------------------------------------------------------------------------" << std::endl;

    simpleContainers::RingBuffer<std::string> rb2{"str1", "str2", "str3", "str4", "str5"};

    // ring buffer supports subscript operators and .at() method similar to a vector (element at index 0 is the oldest element)
    std::cout << rb2[0] << " ... " << rb2[4] << std::endl;
    std::cout << rb2.at(1) << " " << rb2.at(2) << " " << rb2.at(3) << std::endl;

    // Ring buffer implements a random access iterator and const iterator so a for-each style iteration is possilbe
    printRingBuffer(rb2);

    // Ring buffer can be used with a lot of stl algorithms
    // Ring buffer iterators are random access so sorting is possible if needed

    std::rotate(rb2.begin(), rb2.begin() + 2, rb2.end());
    printRingBuffer(rb2);

    std::shuffle(rb2.begin(), rb2.end(), generator);
    printRingBuffer(rb2);

    std::sort(rb2.begin(), rb2.end());
    printRingBuffer(rb2);

    // RingBuffer can be used with a std::back_inserter
    std::string input = "1 2 3 4 5 6 7 8 9 10";
    std::istringstream iss{input};

    simpleContainers::RingBuffer<int> rb3(5);
    std::copy(std::istream_iterator<int>(iss), std::istream_iterator<int>(), std::back_inserter(rb3));
    printRingBuffer(rb3);

    return 0;
}
