cmake_minimum_required(VERSION 3.21)

project(genORM-cxx C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

add_library(libgenORM-cxx lib-cxx/src/genORM.cc)
target_include_directories(libgenORM-cxx PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib-cxx/include)
target_link_libraries(libgenORM-cxx PRIVATE sqlite3)

add_subdirectory(ext/googletest)
add_subdirectory(ext/sqlite)

add_executable(test test-cxx/test.cc test-cxx/testproj.orm.cc)
target_link_libraries(test PRIVATE libgenORM-cxx)
target_link_libraries(test PRIVATE GTest::gtest_main)
target_link_libraries(test PRIVATE sqlite3)
