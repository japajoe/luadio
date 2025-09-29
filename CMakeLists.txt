cmake_minimum_required(VERSION 3.15)
project(luadio)

set(CMAKE_CXX_STANDARD 20)

# GLFW settings
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")

# miniaudioex settings
set(MINIAUDIOEX_BUILD_SHARED OFF CACHE BOOL "Build miniaudioex shared library")
if(UNIX AND NOT APPLE)
	set(MINIAUDIOEX_BUILD_PLATFORM "linux_x86_64")
elseif(APPLE)
	set(MINIAUDIOEX_BUILD_PLATFORM "mac")
elseif(WIN32)
	set(MINIAUDIOEX_BUILD_PLATFORM "windows")
endif()

# luajit settings
set(BUILDMODE "static")
set(LUAJIT_OPTION_BUILD_MODE "static")

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g0")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -g0")
endif()

file(GLOB_RECURSE SOURCES src/*.cpp src/*.c)

include_directories(
    "${PROJECT_SOURCE_DIR}/include/"
	"${PROJECT_SOURCE_DIR}/include/core"
	"${PROJECT_SOURCE_DIR}/include/embedded"
	"${PROJECT_SOURCE_DIR}/include/external/glad"
	"${PROJECT_SOURCE_DIR}/include/external/imgui"
	"${PROJECT_SOURCE_DIR}/include/external/lua"
	"${PROJECT_SOURCE_DIR}/include/external/stb"
	"${PROJECT_SOURCE_DIR}/include/modules"
	"${PROJECT_SOURCE_DIR}/include/system"
)

add_subdirectory(libs/glfw)
add_subdirectory(libs/miniaudioex)
add_subdirectory(libs/LuaJIT-cmake)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE glfw miniaudioex liblua-static)
