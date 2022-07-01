cmake_minimum_required(VERSION 3.0)

project(meshoptimizer VERSION 0.17 LANGUAGES CXX)

set(SOURCES
	src/meshoptimizer.h
	src/allocator.cpp
	src/clusterizer.cpp
	src/indexcodec.cpp
	src/indexgenerator.cpp
	src/overdrawanalyzer.cpp
	src/overdrawoptimizer.cpp
	src/simplifier.cpp
	src/spatialorder.cpp
	src/stripifier.cpp
	src/vcacheanalyzer.cpp
	src/vcacheoptimizer.cpp
	src/vertexcodec.cpp
	src/vertexfilter.cpp
	src/vfetchanalyzer.cpp
	src/vfetchoptimizer.cpp
)

if(MSVC)
	add_compile_options(/W4 /WX)
else()
	add_compile_options(-Wall -Wextra -Wshadow -Wno-missing-field-initializers -Werror)
endif()

add_library(meshoptimizer STATIC ${SOURCES})
target_include_directories(meshoptimizer INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>")
