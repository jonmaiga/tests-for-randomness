project(mixer-impl)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-march=native)
endif()

include_directories(src)
file(GLOB_RECURSE SOURCES "src/*.*")
add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_link_libraries(${PROJECT_NAME} mixer-core)
