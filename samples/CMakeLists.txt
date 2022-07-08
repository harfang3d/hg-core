add_executable(geo_viewer geo_viewer.cpp)
target_link_libraries(geo_viewer PUBLIC app_glfw engine)

add_executable(scn_viewer scn_viewer.cpp)
target_link_libraries(scn_viewer PUBLIC app_glfw engine)

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

		target_include_directories(geo_viewer PRIVATE ${OPENGLES_INCLUDE_DIRS})
		target_link_libraries(geo_viewer PUBLIC ${OPENGLES_LIBRARY} ${EGL_LIBRARIES})

		target_include_directories(scn_viewer PRIVATE ${OPENGLES_INCLUDE_DIRS})
		target_link_libraries(scn_viewer PUBLIC ${OPENGLES_LIBRARY} ${EGL_LIBRARIES})
		endif()
endif()
