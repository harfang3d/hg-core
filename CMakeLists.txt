cmake_minimum_required(VERSION 3.16.2)

file(READ version.txt HG_VERSION)
string(STRIP ${HG_VERSION} HG_VERSION)

project(harfang_core VERSION ${HG_VERSION} LANGUAGES CXX C HOMEPAGE_URL "https://harfang3d.com")

if ((CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC"))
	set(CMAKE_CXX_STANDARD 11)
else()
	set(CMAKE_CXX_STANDARD 98)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

option(HG_ENABLE_COVERAGE "enable code coverage" OFF)
option(HG_ENABLE_PVS_MISRA "enable MISRA compliance run" OFF)

set(HG_ENGINE_BACKEND SOKOL_GLCORE33 CACHE STRING "Graphics backend (default: SOKOL_GLCORE33)")
set_property(CACHE HG_ENGINE_BACKEND PROPERTY STRINGS SOKOL_GLCORE33 SOKOL_GLES2 SOKOL_GLES3 SOKOL_D3D11 SOKOL_METAL SOKOL_WGPU SOKOL_DUMMY_BACKEND)

if(HG_ENABLE_COVERAGE)
	include(CodeCoverage)
	set(HG_ENGINE_BACKEND "SOKOL_DUMMY_BACKEND")
	append_coverage_compiler_flags()
endif()

if(WIN32)
	if(MSVC)
		add_compile_options(/Zc:__cplusplus)
	endif()

	add_compile_definitions(GLFW_WIN32=1)
endif()
add_compile_options(-pedantic)
add_compile_definitions(__STDC_WANT_LIB_EXT1__=1)

add_library(dummy_backend INTERFACE)

if(HG_ENGINE_BACKEND STREQUAL "SOKOL_GLCORE33")
	find_package(OpenGL REQUIRED)
	add_library(hg::gfx_backend ALIAS OpenGL::GL)
elseif(HG_ENGINE_BACKEND STREQUAL "SOKOL_GLES2")
	find_package(OpenGLES3 REQUIRED)
	add_library(hg::gfx_backend ALIAS OpenGL::GLES3)
elseif(HG_ENGINE_BACKEND STREQUAL "SOKOL_GLES3")
	find_package(OpenGLES2 REQUIRED)
	add_library(hg::gfx_backend ALIAS OpenGL::GLES2)
elseif(HG_ENGINE_BACKEND STREQUAL "SOKOL_D3D11")
	# [todo]
	add_library(hg::gfx_backend ALIAS dummy_backend)
elseif(HG_ENGINE_BACKEND STREQUAL "SOKOL_METAL")
	# [todo]
	add_library(hg::gfx_backend ALIAS dummy_backend)
elseif(HG_ENGINE_BACKEND STREQUAL "SOKOL_WGPU")
	# [todo]
	add_library(hg::gfx_backend ALIAS dummy_backend)
elseif(HG_ENGINE_BACKEND STREQUAL "SOKOL_DUMMY_BACKEND")
	add_library(hg::gfx_backend ALIAS dummy_backend)
endif()

# add_compile_options(-pedantic)
add_subdirectory(foundation)
add_subdirectory(engine)
add_subdirectory(tests)

if(NOT HG_ENGINE_BACKEND STREQUAL "SOKOL_DUMMY_BACKEND")
	add_subdirectory(app_glfw)
	add_subdirectory(samples)
endif()

#
if(HG_ENABLE_PVS_MISRA)
	include(FetchContent)
	FetchContent_Declare(PVS_CMakeModule GIT_REPOSITORY "https://github.com/viva64/pvs-studio-cmake-module.git" GIT_TAG "master")
	FetchContent_MakeAvailable(PVS_CMakeModule)
	include("${pvs_cmakemodule_SOURCE_DIR}/PVS-Studio.cmake")

	pvs_studio_add_target(TARGET foundation.analyze ALL FORMAT sarif-vscode ANALYZE foundation MODE MISRA:1,2 LOG foundation.sarif CONVERTER_ARGS -m misra)
	pvs_studio_add_target(TARGET engine.analyze ALL FORMAT sarif-vscode ANALYZE engine MODE MISRA:1,2 LOG engine.sarif CONVERTER_ARGS -m misra)
endif()
