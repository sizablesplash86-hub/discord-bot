cmake_minimum_required(VERSION 3.14)

project(default C)



# Set C++ standard

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)



# Include FetchContent to download Concord (Cogmasters/concord)

include(FetchContent)



# Fetch Concord from GitHub

FetchContent_Declare(
    concord
    GIT_REPOSITORY https://github.com/Cogmasters/concord.git
    GIT_TAG        dev
)


FetchContent_MakeAvailable(concord)



# Gather all source files in the src/ directory

file(GLOB_RECURSE SOURCES "src/*.c")



# Create your executable (replace 'my_project' with your desired binary name)

add_executable(default ${SOURCES})



# Link Concord to your project

# (Note: Adjust the target name 'concord' if their CMakeLists exposes a different library target name)

find_package(CURL REQUIRED)

# ... FetchContent and add_executable ...

target_link_libraries(default PRIVATE discord CURL::libcurl)

target_link_libraries(default PRIVATE discord)



# Include directories if needed for your src files to find headers


target_include_directories(default PRIVATE src)
