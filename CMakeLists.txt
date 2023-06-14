cmake_minimum_required(VERSION 3.13.4)
project(teclang)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_CXX_STANDARD 17)
set(BUILD_ARCH "-m64")
set(TARGET_64 on)

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# Now build our tools
file(GLOB CORE_SOURCES core/*.cpp core/debug/*.cpp)
file(GLOB PARSER_SOURCES
        parsers/*.cpp
        parsers/statements/*.cpp
        parsers/expressions/*.cpp)
file(GLOB STRUCTURE_SOURCES
        structure/*.cpp
        structure/constants/*.cpp
        structure/operations/*.cpp
        structure/types/*.cpp
        structure/statements/*.cpp
        structure/expressions/*.cpp)

add_executable(teclang ${CORE_SOURCES} ${PARSER_SOURCES} ${STRUCTURE_SOURCES})

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs ${LLVM_TARGETS_TO_BUILD} core native)

# Link against LLVM libraries
target_link_libraries(teclang ${llvm_libs})
