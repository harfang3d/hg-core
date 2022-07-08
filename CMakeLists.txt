cmake_minimum_required(VERSION 3.16.2)

file(READ version.txt HG_VERSION)
string(STRIP ${HG_VERSION} HG_VERSION)

#
project(harfang_core VERSION ${HG_VERSION} LANGUAGES CXX C HOMEPAGE_URL "https://harfang3d.com")
set(CMAKE_CXX_STANDARD 98)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

option(HG_ENABLE_COVERAGE "enable code coverage" OFF)

if(HG_ENABLE_COVERAGE)
    include(CodeCoverage)
    append_coverage_compiler_flags()
endif()

if(WIN32)
	if(MSVC)
		add_compile_options(/Zc:__cplusplus)
	endif()
	add_compile_definitions(GLFW_WIN32=1)
endif()

# add_compile_options(-pedantic)

add_subdirectory(foundation)
add_subdirectory(engine)
add_subdirectory(tests)

add_subdirectory(app_glfw)
add_subdirectory(samples)
