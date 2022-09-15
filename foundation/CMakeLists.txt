# fmt::format
set(FMT_USE_CPP11 OFF CACHE BOOL "")
add_subdirectory(extern/fmt)

# xxhash
add_library(xxhash STATIC extern/xxhash/xxhash.c extern/xxhash/xxhash.h)
target_include_directories(xxhash PUBLIC extern/xxhash)

# foundation
set(FOUNDATION_HDRS
	assert.h
	axis.h
	cext.h
	clock.h
	cmd_line.h
	color.h
	data.h
	data_rw_interface.h
	dir.h
	easing.h
	file.h
	file_rw_interface.h
	frustum.h
	generational_vector_list.h
	intrusive_shared_ptr_st.h
	log.h
	math.h
	matrix3.h
	matrix4.h
	matrix44.h
	minmax.h
	obb.h
	os.h
	pack_float.h
	path_tools.h
	plane.h
	profiler.h
	projection.h
	quaternion.h
	rand.h
	rw_interface.h
	rotation_order.h
	seek_mode.h
	string.h
	time.h
	unit.h
	vector_list.h
	vector2.h
	vector3.h
	vector4.h
)

set(FOUNDATION_SRCS
	assert.cpp
	clock.cpp
	cmd_line.cpp
	color.cpp
	data.cpp
	data_rw_interface.cpp
	dir.cpp
	easing.cpp
	file.cpp
	file_rw_interface.cpp
	frustum.cpp
	log.cpp
	math.cpp
	matrix3.cpp
	matrix4.cpp
	matrix44.cpp
	minmax.cpp
	obb.cpp
	os.cpp
	path_tools.cpp
	plane.cpp
	profiler.cpp
	projection.cpp
	quaternion.cpp
	rand.cpp
	rw_interface.cpp
	string.cpp
	time.cpp
	unit.cpp
	vector2.cpp
	vector3.cpp
	vector4.cpp
)

add_library(foundation STATIC ${FOUNDATION_SRCS} ${FOUNDATION_HDRS})
target_include_directories(foundation PUBLIC ${CMAKE_SOURCE_DIR} extern/utf8-cpp extern/rapidjson extern/srombauts-shared_ptr)
target_compile_definitions(foundation PUBLIC RAPIDJSON_HAS_STDSTRING=1)
target_link_libraries(foundation PUBLIC fmt xxhash)
set_target_properties(foundation 
	PROPERTIES 
		PUBLIC_HEADER "${FOUNDATION_HDRS}"
		CXX_STANDARD 98
		CXX_EXTENSIONS OFF
)
