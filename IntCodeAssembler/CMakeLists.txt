cmake_minimum_required(VERSION 3.0)

project(icc.elf)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (MSVC)
string(CONCAT CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}" " /GL")
string(CONCAT CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}" " /LTCG")
endif (MSVC)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
message("Set variables")

add_executable(icc.elf
	src/main.cpp
	
	src/compile.hpp
	src/compile.cpp
	src/file.hpp
	src/file.cpp
	src/util.hpp
	src/util.cpp
	src/err.hpp
	src/err.cpp
)

message("Added sources")

INSTALL(TARGETS icc.elf DESTINATION "./bin")

message("Build done")
