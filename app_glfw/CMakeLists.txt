# GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "" FORCE)
set(GLFW_INSTALL OFF CACHE INTERNAL "" FORCE)

add_subdirectory(extern/glfw-3.3.7)

#
add_library(app_glfw STATIC app_glfw.cpp app_glfw.h)
target_link_libraries(app_glfw PUBLIC glfw engine)
