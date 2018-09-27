cmake_minimum_required(VERSION 3.0)
project(chariort C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11 -Wstrict-prototypes -Wmissing-prototypes -Wall -Wextra")
set(CMAKE_C_COMPILER "gcc")


# Enable librt linking on Linux but not OS X
if(APPLE)
    set(LIBRT "")
else(APPLE)
    set(LIBRT "rt")
endif(APPLE)


# Fix include directories
include_directories(${CMAKE_SOURCE_DIR}/include/)
set(INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/include/)
if(APPLE)
    include_directories(${CMAKE_SOURCE_DIR}/linux-include/include/)
    include_directories(${CMAKE_SOURCE_DIR}/linux-include/include/uapi/)
    include_directories(${CMAKE_SOURCE_DIR}/linux-include/arch/arm/include/)
    include_directories(${CMAKE_SOURCE_DIR}/linux-include/arch/arm/include/uapi/)
    set(INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/linux-include/include/)
    set(INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/linux-include/include/uapi/)
    set(INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/linux-include/arch/arm/include/)
    set(INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/linux-include/arch/arm/include/uapi/)
    add_definitions(-D__CLION__)
endif(APPLE)


add_subdirectory(libs)
add_subdirectory(tests)


# Target to SCP to and make on Zynq MITX
add_custom_target(
        zynq-mitx
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMAND rsync -druvh CMakeLists.txt include libs tests zynq-mitx:/root/chariort
        COMMAND ssh zynq-mitx 'cd /root/chariort && cmake . && make'
)

# Target to SCP to and make on ZCU102
add_custom_target(
        zcu102
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMAND rsync -druvh CMakeLists.txt include libs tests zcu102:/home/russell/chariort
        COMMAND ssh zcu102 'cd /home/russell/chariort && cmake . && make'
)
