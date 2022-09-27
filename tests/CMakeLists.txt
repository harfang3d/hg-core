set(TEST_FOUNDATION_SRCS
	foundation/cext.cpp
	foundation/os.cpp
	foundation/pack_float.cpp
	foundation/math.cpp
	foundation/rand.cpp
	foundation/units.cpp
	foundation/string.cpp
	foundation/path_tools.cpp
	foundation/log.cpp
	foundation/vec2.cpp
	foundation/vec3.cpp
	foundation/vec4.cpp
	foundation/mat3.cpp
	foundation/mat4.cpp
	foundation/mat44.cpp
	foundation/color.cpp
	foundation/minmax.cpp
	foundation/plane.cpp
	foundation/projection.cpp
	foundation/obb.cpp
	foundation/easing.cpp
	foundation/quaternion.cpp
	foundation/frustum.cpp
	foundation/vector_list.cpp
	foundation/generational_vector_list.cpp
	foundation/intrusive_shared_ptr_st.cpp
	foundation/file.cpp
	foundation/dir.cpp
	foundation/data.cpp
	foundation/rw_interface.cpp
	foundation/data_rw_interface.cpp
	foundation/file_rw_interface.cpp
	foundation/clock.cpp
	foundation/cmd_line.cpp
)

set(TEST_ENGINE_SRCS
	engine/vertex_layout.cpp
	engine/anim.cpp
	engine/node.cpp
	engine/picture.cpp
	engine/resource_cache.cpp
	engine/render_pipeline.cpp
	engine/scene.cpp
	engine/load_dds.cpp
)

add_executable(tests tests.cpp utils.cpp utils.h ${TEST_FOUNDATION_SRCS} ${TEST_ENGINE_SRCS})
target_link_libraries(tests PUBLIC engine foundation)
if(NOT HG_ENGINE_BACKEND STREQUAL "SOKOL_DUMMY_BACKEND")
	target_link_libraries(tests PUBLIC app_glfw)
endif()
target_include_directories(tests PUBLIC extern/acutest)

add_custom_command(TARGET tests
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data ${PROJECT_BINARY_DIR}/data
	COMMENT "Copy test assets"
)

if(HG_ENABLE_COVERAGE)
	set(COVERAGE_EXCLUDES
		"foundation/assert.*" 
		"foundation/profiler.*" 
		"foundation/extern/*" 
		"engine/extern/*" 
		"app_glfw/*" 
		"samples/*" 
		"tests/*" 
		"/usr/*"
		"/c/msys64/*"
	)

	setup_target_for_coverage_lcov(
		NAME coverage
		BASE_DIR "${PROJECT_SOURCE_DIR}"
		EXECUTABLE tests)
endif()
