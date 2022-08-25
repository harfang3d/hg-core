cmake_minimum_required(VERSION 3.16.2)

file(READ version.txt HG_VERSION)
string(STRIP ${HG_VERSION} HG_VERSION)

project(harfang_core VERSION ${HG_VERSION} LANGUAGES CXX C HOMEPAGE_URL "https://harfang3d.com")
set(CMAKE_CXX_STANDARD 98)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

option(HG_ENABLE_COVERAGE "enable code coverage" OFF)

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