cmake_minimum_required(VERSION 3.28)
project(demo)

set(BUILD_SHARED_LIBS FALSE)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

set(SDL_SHARED FALSE CACHE BOOL "Prefer static linking")
set(SDL_STATIC TRUE CACHE BOOL "Build a SDL static library (if available)")
message("Fetching SDL3...")
FetchContent_Declare(
	SDL
	GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
	GIT_TAG preview-3.1.6
	GIT_SHALLOW TRUE
	GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(SDL)
set_property(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/_deps/sdl-src" PROPERTY EXCLUDE_FROM_ALL TRUE)

message("Fetching SDL3 TTF...")
set(SDL_SHARED FALSE CACHE BOOL "Prefer static linking")
set(SDL_STATIC TRUE CACHE BOOL "Build a SDL static library (if available)")
set(SDLTTF_VENDORED TRUE)
FetchContent_Declare(
	SDL_TTF
	GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
	GIT_TAG f2b4d9aaa143a39f4bacf3b417f67ae6d5b31b40
	GIT_SHALLOW TRUE
	GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(SDL_TTF)
set_property(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/_deps/sdl_ttf-src" PROPERTY EXCLUDE_FROM_ALL TRUE)


add_executable(demo main.cpp)

target_link_libraries(demo PRIVATE SDL3::SDL3-static SDL3_ttf::SDL3_ttf-static)
