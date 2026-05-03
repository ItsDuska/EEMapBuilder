#cmake_minimum_required(VERSION 3.20)
#project(EEMapBuilder LANGUAGES CXX)

#set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

#file(GLOB_RECURSE SOURCES
#   src/*.cpp
#)

#add_executable(EEMapBuilder ${SOURCES})


#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#find_package(PkgConfig REQUIRED)
#pkg_check_modules(SDL3 REQUIRED sdl3)

#find_package(SDL3 REQUIRED CONFIG)
#target_link_libraries(EEMapBuilder PRIVATE SDL3::SDL3)


#target_include_directories(EEMapBuilder PRIVATE ${SDL3_INCLUDE_DIRS})
#target_link_libraries(EEMapBuilder PRIVATE ${SDL3_LIBRARIES})

#target_compile_options(EEMapBuilder PRIVATE ${SDL3_CFLAGS_OTHER})

cmake_minimum_required(VERSION 3.20)
project(EEMapBuilder LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
				src/*.cpp
)

add_executable(EEMapBuilder ${SOURCES})

target_include_directories(EEMapBuilder
				PRIVATE
								${CMAKE_CURRENT_SOURCE_DIR}/src
)

find_package(SDL3 REQUIRED CONFIG)
target_link_libraries(EEMapBuilder PRIVATE SDL3::SDL3)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${SOURCES})
