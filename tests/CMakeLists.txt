set(TEST_FOUNDATION_SRCS
	foundation/math.cpp
	foundation/units.cpp
	foundation/string.cpp
	foundation/path_tools.cpp
	foundation/vec2.cpp
	foundation/vec3.cpp
	foundation/vec4.cpp
	foundation/mat3.cpp
	foundation/mat4.cpp
	foundation/mat44.cpp
)

add_executable(test_foundation test_foundation.cpp ${TEST_FOUNDATION_SRCS})
target_link_libraries(test_foundation PUBLIC foundation)
target_include_directories(test_foundation PUBLIC extern/acutest)
