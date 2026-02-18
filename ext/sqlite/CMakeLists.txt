include(FetchContent)
FetchContent_Declare(
    sqlite3_source
    URL ${CMAKE_CURRENT_LIST_DIR}/sqlite-amalgamation-3510200.zip
)
FetchContent_GetProperties(sqlite3_source)

if(NOT sqlite3_source_POPULATED)
    FetchContent_MakeAvailable(sqlite3_source)
    add_library(sqlite3 STATIC ${sqlite3_source_SOURCE_DIR}/sqlite3.c)
    target_include_directories(sqlite3 PUBLIC ${sqlite3_source_SOURCE_DIR})
endif()
