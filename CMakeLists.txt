cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)
project(bit_mixer_evaluation)
link_directories(/usr/local/lib/)
add_subdirectory(core/)


if (NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Release")
	message("Development mode.")
	target_compile_definitions(mixer_eval-core PRIVATE MIXER_DEVELOPMENT=1)
endif()