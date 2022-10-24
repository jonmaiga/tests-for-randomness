project(mixer_eval-core)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

file(GLOB_RECURSE SOURCES "src/*.*")
set(SOURCE ${SOURCES})
add_executable(${PROJECT_NAME} ${SOURCE} "src/util/fileutil.cpp" "src/util/fileutil.h")
target_link_libraries(${PROJECT_NAME})