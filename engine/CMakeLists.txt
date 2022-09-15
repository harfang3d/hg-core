# mikktspace
add_library(mikktspace STATIC extern/mikktspace/mikktspace.c extern/mikktspace/mikktspace.h)
target_include_directories(mikktspace PUBLIC extern/mikktspace)

# miniz
add_library(miniz STATIC extern/miniz/miniz.c extern/miniz/miniz.h)
target_include_directories(miniz PUBLIC extern/miniz)

# engine
set(ENGINE_HDRS
	anim.h
	assets.h
	assets_rw_interface.h
	create_model.h
	file_format.h
	fps_controller.h
	geometry.h
	json.h
	load_dds.h
	model_builder.h
	node.h
	picture.h
	render_pipeline.h
	resource_cache.h
	scene.h
	vertex.h
	
)

set(ENGINE_SRCS
	anim.cpp
	anim_load_binary.cpp
	anim_load_json.cpp
	assets.cpp
	assets_rw_interface.cpp
	component.cpp
	create_model.cpp
	fps_controller.cpp
	geometry.cpp
	json.cpp
	load_dds.cpp
	model_builder.cpp
	node.cpp
	picture.cpp
	render_pipeline.cpp
	scene.cpp
	scene_load_binary.cpp
	scene_load_json.cpp
	vertex.cpp
)

add_library(engine STATIC ${ENGINE_SRCS} ${ENGINE_HDRS})
target_link_libraries(engine PUBLIC foundation mikktspace miniz hg::gfx_backend)
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} extern/sokol extern/stb)
target_compile_definitions(engine PUBLIC ${HG_ENGINE_BACKEND})
set_target_properties(engine 
	PROPERTIES 
		CXX_STANDARD 98
		CXX_EXTENSIONS OFF
)
