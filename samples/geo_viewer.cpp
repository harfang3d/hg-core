// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include <fmt/format.h>

#include "foundation/matrix44.h"
#include "foundation/projection.h"
#include "foundation/time.h"
#include "foundation/vector3.h"

#include "engine/create_model.h"
#include "engine/fps_controller.h"
#include "engine/geometry.h"
#include "engine/render_pipeline.h"

#include "engine/scene.h"
#include "engine/assets.h"

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
sg_pipeline MakePipeline(const VertexLayout &vertex_layout, sg_shader shader, const ShaderLayout &shader_layout) {
	sg_pipeline_desc pipeline_desc;

	memset(&pipeline_desc, 0, sizeof(sg_pipeline_desc));
	pipeline_desc.index_type = SG_INDEXTYPE_UINT32;
	pipeline_desc.shader = shader;

	pipeline_desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
	pipeline_desc.depth.write_enabled = true;
	pipeline_desc.cull_mode = SG_CULLMODE_BACK;

	FillPipelineLayout(vertex_layout, shader_layout, pipeline_desc.layout);

	return sg_make_pipeline(&pipeline_desc);
}

//
int main(int narg, const char **args) {
	GLFWwindow *win = RenderInit(640, 480, "Sokol Triangle GLFW");

	glfwSetKeyCallback(win, key_callback);

	// a vertex buffer (position/color)
	const float vertices[] = {0.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f};
	sg_buffer vbuf = MakeVertexBuffer(vertices, sizeof(vertices));

	// a shader
	sg_shader_desc shader_desc;

	struct shader_vs_params {
		Mat44 mvp;
	};

	memset(&shader_desc, 0, sizeof(sg_shader_desc));

	shader_desc.vs.source = "#version 330\n"
							"uniform mat4 mvp;\n"
							"layout (location=0) in vec4 a_position;\n"
							"layout (location=1) in vec3 a_normal;\n"
							"out vec3 i_normal;\n"
							"void main() {\n"
							"	gl_Position = mvp * a_position;\n"
							"	i_normal = a_normal;\n"
							"}\n";

	shader_desc.vs.uniform_blocks[0].size = sizeof(shader_vs_params);
	shader_desc.vs.uniform_blocks[0].uniforms[0].name = "mvp";
	shader_desc.vs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_MAT4;

	shader_desc.fs.source = "#version 330\n"
							"in vec3 i_normal;\n"
							"out vec4 o_color;\n"
							"void main() {\n"
							"	float k = normalize(i_normal).z;\n"
							"	o_color = vec4(k, k, k, 1);\n"
							"}\n";

	sg_shader shd = sg_make_shader(&shader_desc);

	ShaderLayout shd_layout;
	shd_layout.attrib[0] = VA_Position;
	shd_layout.attrib[1] = VA_Normal;

	// a pipeline state object (default render states are fine for triangle)
/*
	VertexLayout mdl_layout;
	mdl_layout.Add(VA_Normal, SG_VERTEXFORMAT_FLOAT3);
	mdl_layout.Add(VA_Position, SG_VERTEXFORMAT_FLOAT3);
	mdl_layout.End();
*/

	const Geometry geo = LoadGeometryFromFile(args[1]);

	// Model mdl = CreateSphereModel(layout, 1.f, 32, 16);
	const VertexLayout mdl_layout = ComputeGeometryVertexLayout(geo);
	Model mdl = GeometryToModel(geo, mdl_layout);

	//
	sg_pipeline pip = MakePipeline(mdl_layout, shd, shd_layout);

	// resource bindings
	sg_bindings bind;
	memset(&bind, 0, sizeof(sg_bindings));

	const DisplayList &list = mdl.lists[0];
	bind.index_buffer = list.index_buffer;
	bind.vertex_buffers[0] = list.vertex_buffer;

	// bind.vertex_buffers[0] = vbuf;

	// default pass action (clear to grey)
	sg_pass_action pass_action = {0};

	// draw loop
	shader_vs_params vs_params;

	//
	double old_xpos = 0, old_ypos = 0;
	Vec3 pos(0, 1.5, -4), rot(0, 0, 0);

	while (!glfwWindowShouldClose(win)) {
		double xpos, ypos;
		glfwGetCursorPos(win, &xpos, &ypos);
		const double xdelta = 0 /*xpos - old_xpos*/, ydelta = 0 /*ypos - old_ypos*/;
		old_xpos = xpos;
		old_ypos = ypos;

		fmt::print("dx={} dy={} up={} down={} left={} right={}\n", xdelta, ydelta, key_up, key_down, key_left, key_right);

		//
		FpsController(key_up, key_down, key_left, key_right, true, float(xdelta), float(ydelta), pos, rot, 2.f, time_from_ms(16));

		const Mat44 proj = /* ScaleMat4(Vec3(1, -1, 1)) **/ ComputePerspectiveProjectionMatrix(0.1f, 1000.f, 3.2f, ComputeAspectRatioX(640.f, 480.f));
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
		sg_draw(0, list.element_count, 1);

		sg_end_pass();

		sg_commit();
		glfwSwapBuffers(win);
		glfwPollEvents();
	}

	RenderShutdown();
	return 0;
}
