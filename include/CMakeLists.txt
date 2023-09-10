set(SIMPLE_CONTAINERS_HEADERS "simpleRingBuffer.hpp")
set(SIMPLE_CONTAINERS_INCLUDES  "./")

add_library(simpleContainers INTERFACE)
target_include_directories(simpleContainers INTERFACE ${SIMPLE_CONTAINERS_INCLUDES})
