project(tfr-impl)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-march=native)
endif()

include_directories(src)
file(GLOB_RECURSE SOURCES "src/*.*")
add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_link_libraries(${PROJECT_NAME} tfr-core)
