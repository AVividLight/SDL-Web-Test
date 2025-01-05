cmake_minimum_required(VERSION 3.28)
project(demo)

find_package(SDL3 QUIET)
if(NOT SDL3_FOUND)
	set(SDL_SHARED FALSE CACHE BOOL "Prefer static linking")
	include(FetchContent)
	set(SDL_STATIC TRUE CACHE BOOL "Build a SDL static library (if available)")
	FetchContent_Declare(
		SDL
		GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
		GIT_TAG preview-3.1.6
		GIT_SHALLOW TRUE
		GIT_PROGRESS TRUE
	)
	FetchContent_MakeAvailable(SDL)
	set_property(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/_deps/sdl-src" PROPERTY EXCLUDE_FROM_ALL TRUE)
endif()

add_executable(demo main.cpp)

target_link_libraries(demo PRIVATE SDL3::SDL3-static)
