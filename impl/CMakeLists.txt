project(mixer-impl)
file(GLOB_RECURSE SOURCES "src/*.*")
add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_link_libraries(${PROJECT_NAME} mixer-core)
