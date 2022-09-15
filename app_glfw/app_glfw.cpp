// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "app_glfw.h"

#include <sokol_gfx.h>

#include <cstring>

namespace hg {

bool RenderInit(GLFWwindow *win) {
	sg_desc desc;
	memset(&desc, 0, sizeof(sg_desc));
	sg_setup(&desc);

	return true;
}

GLFWwindow *RenderInit(int width, int height, const std::string &title) {
	glfwInit();

#if defined(SOKOL_GLCORE33) || defined(SOKOL_GLES2) || defined(SOKOL_GLES3) // any GL
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
	GLFWwindow *win = glfwCreateWindow(640, 480, title.c_str(), 0, 0);
	glfwMakeContextCurrent(win);
	glfwSwapInterval(1);
#else
#error "Unsupported graphic API"
#endif

	RenderInit(win);
	return win;
}

void RenderShutdown() {
	sg_shutdown();
	glfwTerminate();
}

} // namespace hg
