// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <string>

namespace hg {

bool RenderInit(GLFWwindow *win);
GLFWwindow *RenderInit(int width, int height, const std::string &title);

void RenderShutdown();

} // namespace hg
