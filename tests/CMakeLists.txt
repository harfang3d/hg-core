set(TEST_FOUNDATION_SRCS
	foundation/os.cpp
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
)

add_executable(test_foundation test_foundation.cpp utils.cpp ${TEST_FOUNDATION_SRCS})
target_link_libraries(test_foundation PUBLIC foundation)
target_include_directories(test_foundation PUBLIC extern/acutest)

if(HG_ENABLE_COVERAGE)
	 set(COVERAGE_EXCLUDES
		"foundation/assert.*" 
		"foundation/extern/*" 
		"engine/*" 
		"app_glfw/*" 
		"samples/*" 
		"tests/*" 
		"/usr/*"
	 )
	setup_target_for_coverage_lcov(
		NAME foundation_coverage
		BASE_DIR "${PROJECT_SOURCE_DIR}"
		EXECUTABLE test_foundation)
endif()
