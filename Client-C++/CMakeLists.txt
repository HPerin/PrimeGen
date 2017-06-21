cmake_minimum_required(VERSION 3.7)
project(PrimeGen)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_COMPILER "/usr/gcc-4.9.4/bin/g++")

set(BOOST_ROOT /usr/boost-1.59.0/)
find_package(Boost REQUIRED system)

file(GLOB SOURCE_FILES "${CMAKE_SOURCE_DIR}/src/*")
add_executable(PrimeGen ${SOURCE_FILES} src/Manager.h src/Block.h)
target_link_libraries(PrimeGen ${Boost_LIBRARIES} -lpthread)
target_include_directories(PrimeGen PRIVATE ${CMAKE_SOURCE_DIR}/lib ${Boost_INCLUDE_DIRS})
