cmake_minimum_required(VERSION 3.16.2)

file(READ version.txt HG_VERSION)
string(STRIP ${HG_VERSION} HG_VERSION)

project(harfang_core VERSION ${HG_VERSION} LANGUAGES CXX C HOMEPAGE_URL "https://harfang3d.com")
set(CMAKE_CXX_STANDARD 98)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

option(HG_ENABLE_COVERAGE "enable code coverage" OFF)
option(HG_ENABLE_PVS_MISRA "enable MISRA compliance run" OFF)

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

add_compile_definitions(__STDC_WANT_LIB_EXT1__=1)

# add_compile_options(-pedantic)
add_subdirectory(foundation)
add_subdirectory(engine)
add_subdirectory(tests)

add_subdirectory(app_glfw)
add_subdirectory(samples)

#
if(HG_ENABLE_PVS_MISRA)
	include(FetchContent)
	FetchContent_Declare(PVS_CMakeModule GIT_REPOSITORY "https://github.com/viva64/pvs-studio-cmake-module.git" GIT_TAG "master")
	FetchContent_MakeAvailable(PVS_CMakeModule)
	include("${pvs_cmakemodule_SOURCE_DIR}/PVS-Studio.cmake")

	pvs_studio_add_target(TARGET foundation.analyze ALL FORMAT sarif-vscode ANALYZE foundation MODE MISRA:1,2 LOG foundation.sarif CONVERTER_ARGS -m misra)
endif()
