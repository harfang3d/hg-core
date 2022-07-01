// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include <acutest.h>
#include <fmt/format.h>

#include "foundation/matrix44.h"
#include "foundation/projection.h"
#include "foundation/time.h"
#include "foundation/vector3.h"

#include "engine/fps_controller.h"
#include "engine/render_pipeline.h"
#include "engine/geometry.h"

#define SOKOL_GFX_IMPL
#define SOKOL_GLCORE33
#include <sokol_gfx.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace hg;

//----------------------------------------------

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

/*
void SetView2D(bgfx::ViewId id, int x, int y, int res_x, int res_y, float znear = -1.f, float zfar = 1.f,
	uint16_t clear_flags = BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, const Color &clear_color = Color::Black, float depth = 1.f, uint8_t stencil = 0,
	bool y_up = false);
void SetViewPerspective(bgfx::ViewId id, int x, int y, int res_x, int res_y, const Mat4 &world, float znear = 0.01f, float zfar = 1000.f,
	float zoom_factor = 1.8, uint16_t clear_flags = BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, const Color &clear_color = Color::Black, float depth = 1.f,
	uint8_t stencil = 0, const Vec2 &aspect_ratio = {});
void SetViewOrthographic(bgfx::ViewId id, int x, int y, int res_x, int res_y, const Mat4 &world, float znear = 0.01f, float zfar = 1000.f, float size = 1.f,
	uint16_t clear_flags = BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, const Color &clear_color = Color::Black, float depth = 1.f, uint8_t stencil = 0,
	const Vec2 &aspect_ratio = {});
*/

//----------------------------------------------
static bool key_up = false, key_down = false, key_left = false, key_right = false;

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	bool v = false;
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
		v = true;

	if (key == GLFW_KEY_W)
		key_up = v;
	if (key == GLFW_KEY_S)
		key_down = v;
	if (key == GLFW_KEY_A)
		key_left = v;
	if (key == GLFW_KEY_D)
		key_right = v;
}

//

//
sg_pipeline MakePipeline(const VertexLayout &vtx_layout, sg_shader shader) {
	sg_pipeline_desc pipeline_desc;

	memset(&pipeline_desc, 0, sizeof(sg_pipeline_desc));
	pipeline_desc.shader = shader;
	vtx_layout.FillLayoutDesc(pipeline_desc.layout);

	return sg_make_pipeline(&pipeline_desc);
}

//
sg_buffer MakeVertexBuffer(const void *data, size_t size) {
	sg_buffer_desc buffer_desc;

	memset(&buffer_desc, 0, sizeof(sg_buffer_desc));
	buffer_desc.data.ptr = data;
	buffer_desc.data.size = size;

	return sg_make_buffer(&buffer_desc);
}

//

//
void test_init() {
	GLFWwindow *win = RenderInit(640, 480, "Sokol Triangle GLFW");

	glfwSetKeyCallback(win, key_callback);

	// a vertex buffer (position/color)
	const float vertices[] = {0.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f};
	sg_buffer vbuf = MakeVertexBuffer(vertices, sizeof(vertices));

	// a shader
	sg_shader_desc shader_desc;

	typedef struct {
		Mat44 mvp;
	} shader_vs_params;

	memset(&shader_desc, 0, sizeof(sg_shader_desc));

	shader_desc.vs.source = "#version 330\n"
							"uniform mat4 mvp;\n"
							"layout(location=0) in vec4 position;\n"
							"layout(location=1) in vec4 color0;\n"
							"out vec4 color;\n"
							"void main() {\n"
							"	gl_Position = mvp * position;\n"
							"	color = color0;\n"
							"}\n";

	shader_desc.vs.uniform_blocks[0].size = sizeof(shader_vs_params);
	shader_desc.vs.uniform_blocks[0].uniforms[0].name = "mvp";
	shader_desc.vs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_MAT4;

	shader_desc.fs.source = "#version 330\n"
							"in vec4 color;\n"
							"out vec4 frag_color;\n"
							"void main() {\n"
							"	frag_color = color;\n"
							"}\n";

	sg_shader shd = sg_make_shader(&shader_desc);

	// a pipeline state object (default render states are fine for triangle)
	VertexLayout vtx_layout;
	vtx_layout.AddAttrib(0, SG_VERTEXFORMAT_FLOAT3);
	vtx_layout.AddAttrib(1, SG_VERTEXFORMAT_FLOAT4);

	sg_pipeline pip = MakePipeline(vtx_layout, shd);

	// resource bindings
	sg_bindings bind;
	memset(&bind, 0, sizeof(sg_bindings));
	bind.vertex_buffers[0] = vbuf;

	// default pass action (clear to grey)
	sg_pass_action pass_action = {0};

	// draw loop
	shader_vs_params vs_params;

	//
	double old_xpos = 0, old_ypos = 0;
	Vec3 pos(0, 0, 1), rot(0, 0, 0);

	while (!glfwWindowShouldClose(win)) {
		double xpos, ypos;
		glfwGetCursorPos(win, &xpos, &ypos);
		const double xdelta = xpos - old_xpos, ydelta = ypos - old_ypos;
		old_xpos = xpos;
		old_ypos = ypos;

		fmt::print("dx={} dy={} up={} down={} left={} right={}\n", xdelta, ydelta, key_up, key_down, key_left, key_right);

		//
		FpsController(key_up, key_down, key_left, key_right, true, float(-xdelta), float(-ydelta), pos, rot, 2.f, time_from_ms(16));

		const Mat44 proj = ComputePerspectiveProjectionMatrix(0.1f, 1000.f, 3.2f, ComputeAspectRatioX(640.f, 480.f));
		const Mat4 view = InverseFast(TransformationMat4(pos, rot));

		vs_params.mvp = Transpose(proj * view);

		//
		int cur_width, cur_height;
		glfwGetFramebufferSize(win, &cur_width, &cur_height);

		sg_begin_default_pass(&pass_action, cur_width, cur_height);

		sg_apply_pipeline(pip);
		sg_apply_bindings(bind);
		{
			sg_range range;
			range.ptr = &vs_params;
			range.size = sizeof(vs_params);
			sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, range);
		}
		sg_draw(0, 3, 1);

		sg_end_pass();

		sg_commit();
		glfwSwapBuffers(win);
		glfwPollEvents();
	}

	RenderShutdown();
}

//
TEST_LIST = {
	{"Init", test_init},

	{NULL, NULL},
};
