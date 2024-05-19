set(SC_SIMPLE_CONTAINERS_HEADERS 
    "simpleRingBuffer.hpp"
    "simpleHashedArrayTree.hpp"
)
set(SC_SIMPLE_CONTAINERS_INCLUDES  "./")

add_library(simpleContainers INTERFACE)
target_include_directories(simpleContainers INTERFACE ${SC_SIMPLE_CONTAINERS_INCLUDES})
