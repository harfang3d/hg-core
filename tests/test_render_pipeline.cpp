// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include <acutest.h>
#include <fmt/format.h>
#define SOKOL_GFX_IMPL
#define SOKOL_GLCORE33
#include <sokol_gfx.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "foundation/time.h"

using namespace hg;

void test_init() {
}

void test_init_ref() {
	// create window and GL context via GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow *w = glfwCreateWindow(640, 480, "Sokol Triangle GLFW", 0, 0);
	glfwMakeContextCurrent(w);
	glfwSwapInterval(1);

	// setup sokol_gfx
	sg_desc desc;
	memset(&desc, 0, sizeof(sg_desc));
	sg_setup(&desc);

	// a vertex buffer (position/color)
	const float vertices[] = {0.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f};

	sg_buffer_desc buffer_desc;
	memset(&buffer_desc, 0, sizeof(sg_buffer_desc));
	buffer_desc.data = SG_RANGE(vertices);
	sg_buffer vbuf = sg_make_buffer(&buffer_desc);

	// a shader
	sg_shader_desc shader_desc;

	memset(&shader_desc, 0, sizeof(sg_shader_desc));
	shader_desc.vs.source = "#version 330\n"
							"layout(location=0) in vec4 position;\n"
							"layout(location=1) in vec4 color0;\n"
							"out vec4 color;\n"
							"void main() {\n"
							"  gl_Position = position;\n"
							"  color = color0;\n"
							"}\n";
	shader_desc.fs.source = "#version 330\n"
							"in vec4 color;\n"
							"out vec4 frag_color;\n"
							"void main() {\n"
							"  frag_color = color;\n"
							"}\n";

	sg_shader shd = sg_make_shader(&shader_desc);

	// a pipeline state object (default render states are fine for triangle)
	sg_pipeline_desc pipeline_desc;

	memset(&pipeline_desc, 0, sizeof(sg_pipeline_desc));
	pipeline_desc.shader = shd;
	pipeline_desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;
	pipeline_desc.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT4;

	sg_pipeline pip = sg_make_pipeline(&pipeline_desc);

	// resource bindings
	sg_bindings bind;
	memset(&bind, 0, sizeof(sg_bindings));
	bind.vertex_buffers[0] = vbuf;

	// default pass action (clear to grey)
	sg_pass_action pass_action = {0};

	// draw loop
	while (!glfwWindowShouldClose(w)) {
		int cur_width, cur_height;
		glfwGetFramebufferSize(w, &cur_width, &cur_height);
		sg_begin_default_pass(&pass_action, cur_width, cur_height);
		sg_apply_pipeline(pip);
		sg_apply_bindings(&bind);
		sg_draw(0, 3, 1);
		sg_end_pass();
		sg_commit();
		glfwSwapBuffers(w);
		glfwPollEvents();
	}

	// cleanup
	sg_shutdown();
	glfwTerminate();
}

TEST_LIST = {
	{"Init", test_init},

	{NULL, NULL},
};
