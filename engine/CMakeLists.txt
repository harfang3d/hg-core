# GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "" FORCE)
set(GLFW_INSTALL OFF CACHE INTERNAL "" FORCE)

add_subdirectory(extern/glfw-3.3.7)

# meshoptimizer
add_subdirectory(extern/meshoptimizer)

# engine
set(ENGINE_HDRS
	assets.h
	assets_rw_interface.h
	create_model.h
	file_format.h
	fps_controller.h
	geometry.h
	model_builder.h
	picture.h
	render_pipeline.h
	resource_cache.h
	vertex.h
	extern/mikktspace/mikktspace.h
)

set(ENGINE_SRCS

	# assets.cpp
	# assets_rw_interface.cpp
	create_model.cpp
	fps_controller.cpp
	geometry.cpp
	model_builder.cpp
	picture.cpp
	render_pipeline.cpp
	vertex.cpp
	extern/mikktspace/mikktspace.c
)

add_library(engine STATIC ${ENGINE_SRCS} ${ENGINE_HDRS})
target_link_libraries(engine PUBLIC foundation glfw meshoptimizer)
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} extern/sokol extern/stb extern/mikktspace)
