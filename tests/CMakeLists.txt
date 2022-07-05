add_executable(test_foundation test_foundation.cpp)
target_link_libraries(test_foundation PUBLIC foundation)
target_include_directories(test_foundation PUBLIC extern/acutest)

add_executable(test_render_pipeline test_render_pipeline.cpp)
target_link_libraries(test_render_pipeline PUBLIC engine)
target_include_directories(test_render_pipeline PUBLIC extern/acutest)

if(UNIX OR MSYS AND NOT APPLE AND NOT EMSCRIPTEN)
	find_package(OpenGL)

	if(OPENGL_FOUND)
		target_link_libraries(test_render_pipeline PUBLIC OpenGL::GL)
	else()
		find_package(OpenGLES3)

		if(NOT OPENGLES3_FOUND)
			find_package(OpenGLES2 REQUIRED)

			if(OPENGLES2_FOUND)
				set(OPENGLES_LIBRARIES ${OPENGLES2_LIBRARIES})
				set(OPENGLES_INCLUDE_DIRS ${OPENGLES2_INCLUDE_DIRS})
			endif()
		else()
			set(OPENGLES_LIBRARIES ${OPENGLES3_LIBRARIES})
			set(OPENGLES_INCLUDE_DIRS ${OPENGLES3_INCLUDE_DIRS})
		endif()

		find_package(EGL REQUIRED)
		target_include_directories(test_render_pipeline PRIVATE ${OPENGLES_INCLUDE_DIRS})
		target_link_libraries(test_render_pipeline PUBLIC ${OPENGLES_LIBRARY} ${EGL_LIBRARIES})
	endif()
endif()
