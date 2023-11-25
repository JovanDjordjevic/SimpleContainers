# SimpleContainers

SimpleContainers is a collection of header-only implementations of some useful containers and data structures without external dependencies.
Use C++11 or later standard.

Tested on the following compilers:

- gcc version 9.4
- clang version 10.0
- MSVC version 19.32

Currently implemented containers and structures:

- **RingBuffer\<T\>** - a container that holds only the last **N** inserted elements of type **T** (overwrites the oldest element whenever a new one is inserted when maximum capacity is reached) (at this time, **T** = bool is not supported)

Usage examples can be found in the [examples](https://github.com/JovanDjordjevic/SimpleContainers/blob/main/examples) folder

Doxygen generated documentation can be found [here](https://jovandjordjevic.github.io/SimpleContainers/html/index.html)
