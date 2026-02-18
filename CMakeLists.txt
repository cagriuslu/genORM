cmake_minimum_required(VERSION 3.21)

option(BUILD_TESTS "Build tests" OFF)
option(SQLITE_HEADER_DIR "Directory containing Sqlite headers to compile against. If left empty, Sqlite will be built.")

project(genORM-cxx C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

if(SQLITE_HEADER_DIR)
    add_library(sqlite3 INTERFACE)
    target_include_directories(sqlite3 INTERFACE ${SQLITE_HEADER_DIR})
else()
    add_subdirectory(ext/sqlite)
endif()

add_library(genORM-cxx lib-cxx/src/genORM.cc)
target_include_directories(genORM-cxx PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib-cxx/include>
    $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib-cxx/include>
)
target_link_libraries(genORM-cxx PRIVATE sqlite3)

install(TARGETS genORM-cxx ARCHIVE DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/install/lib)
install(FILES lib-cxx/include/genORM/genORM.h DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/install/include/genORM)

if(BUILD_TESTS)
    add_subdirectory(ext/googletest)
    add_executable(test test-cxx/test.cc test-cxx/testproj.orm.cc)
    target_link_libraries(test PRIVATE libgenORM-cxx)
    target_link_libraries(test PRIVATE GTest::gtest_main)
    target_link_libraries(test PRIVATE sqlite3)
endif()
