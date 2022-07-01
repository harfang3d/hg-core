cmake_minimum_required(VERSION 3.16.2)

file(READ version.txt HG_VERSION)
string(STRIP ${HG_VERSION} HG_VERSION)

#
project(harfang_core VERSION ${HG_VERSION} LANGUAGES CXX C HOMEPAGE_URL "https://harfang3d.com")
set(CMAKE_CXX_STANDARD 98)

if(WIN32)
	add_compile_definitions(GLFW_WIN32=1)
endif()

# fmt::format
set(FMT_USE_CPP11 OFF CACHE BOOL "")
add_subdirectory(extern/fmt)

# foundation
set(FOUNDATION_HDRS
foundation/assert.h
foundation/axis.h
foundation/cext.h
foundation/clock.h
foundation/color.h
foundation/data.h
foundation/data_rw_interface.h
foundation/dir.h
foundation/file.h
foundation/file_rw_interface.h
foundation/frustum.h
foundation/generational_vector_list.h
foundation/intrusive_shared_ptr_st.h
foundation/log.h
foundation/math.h
foundation/matrix3.h
foundation/matrix4.h
foundation/matrix44.h
foundation/minmax.h
foundation/obb.h
foundation/os.h
foundation/pack_float.h
foundation/path_tools.h
foundation/plane.h
foundation/projection.h
foundation/quaternion.h
foundation/rand.h
foundation/rw_interface.h
foundation/rotation_order.h
foundation/seek_mode.h
foundation/string.h
foundation/time.h
foundation/unit.h
foundation/vector_list.h
foundation/vector2.h
foundation/vector3.h
foundation/vector4.h
)

set(FOUNDATION_SRCS
foundation/assert.cpp
foundation/clock.cpp
foundation/color.cpp
foundation/data.cpp
foundation/data_rw_interface.cpp
foundation/dir.cpp
foundation/file.cpp
foundation/file_rw_interface.cpp
foundation/frustum.cpp
foundation/log.cpp
foundation/math.cpp
foundation/matrix3.cpp
foundation/matrix4.cpp
foundation/matrix44.cpp
foundation/minmax.cpp
foundation/obb.cpp
foundation/os.cpp
foundation/path_tools.cpp
foundation/plane.cpp
foundation/projection.cpp
foundation/quaternion.cpp
foundation/rand.cpp
foundation/rw_interface.cpp
foundation/string.cpp
foundation/time.cpp
foundation/unit.cpp
foundation/vector2.cpp
foundation/vector3.cpp
foundation/vector4.cpp
)

add_library(foundation STATIC ${FOUNDATION_SRCS} ${FOUNDATION_HDRS})
target_include_directories(foundation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} extern/utf8-cpp)
target_link_libraries(foundation PUBLIC fmt)

# GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "" FORCE)
set(GLFW_INSTALL OFF CACHE INTERNAL "" FORCE)

add_subdirectory(extern/glfw-3.3.7)

# simpleson
add_library(simpleson STATIC extern/simpleson/json.cpp extern/simpleson/json.h)
target_include_directories(foundation PUBLIC extern/simpleson)

# meshoptimizer
add_subdirectory(engine/extern/meshoptimizer)

# engine
set(ENGINE_HDRS
engine/assets.h
engine/assets_rw_interface.h
engine/create_model.h
engine/file_format.h
engine/fps_controller.h
engine/geometry.h
engine/model_builder.h
engine/picture.h
engine/render_pipeline.h
engine/resource_cache.h
engine/vertex.h
engine/extern/mikktspace/mikktspace.h
)

set(ENGINE_SRCS
#engine/assets.cpp
#engine/assets_rw_interface.cpp
engine/create_model.cpp
engine/fps_controller.cpp
engine/geometry.cpp
engine/model_builder.cpp
engine/picture.cpp
engine/render_pipeline.cpp
engine/vertex.cpp
engine/extern/mikktspace/mikktspace.c
)

add_library(engine STATIC ${ENGINE_SRCS} ${ENGINE_HDRS})
target_link_libraries(engine PUBLIC foundation fmt glfw simpleson meshoptimizer)
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} extern/sokol extern/stb engine/extern/mikktspace)

# unit tests
add_executable(test_foundation tests/test_foundation.cpp)
target_link_libraries(test_foundation PUBLIC foundation)
target_include_directories(test_foundation PUBLIC extern/acutest)

add_executable(test_render_pipeline tests/test_render_pipeline.cpp)
target_link_libraries(test_render_pipeline PUBLIC engine)
target_include_directories(test_render_pipeline PUBLIC extern/acutest)
