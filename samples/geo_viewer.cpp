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

#include "engine/assets.h"
#include "engine/scene.h"

#include "app_glfw/app_glfw.h"

using namespace hg;

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
sg_pipeline MakePipeline(const VertexLayout &vertex_layout, sg_shader shader, const ShaderLayout &shader_layout) {
	sg_pipeline_desc pipeline_desc;

	memset(&pipeline_desc, 0, sizeof(sg_pipeline_desc));
	pipeline_desc.index_type = SG_INDEXTYPE_UINT16;
	pipeline_desc.shader = shader;
	pipeline_desc.primitive_type = SG_PRIMITIVETYPE_TRIANGLES;

	pipeline_desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
	pipeline_desc.depth.write_enabled = true;
	pipeline_desc.cull_mode = SG_CULLMODE_BACK;

	FillPipelineLayout(vertex_layout, shader_layout, pipeline_desc.layout);

	return sg_make_pipeline(&pipeline_desc);
}

//

//
struct RenderMaterial {
	UniformData data;
	sg_pipeline pipeline;
};

void SetupRenderMaterial(RenderMaterial &render_mat, const VertexLayout &vtx_layout, const Shader &shader) {
	render_mat.pipeline = MakePipeline(vtx_layout, shader.shader, shader.layout);
	SetupShaderUniformData(shader, render_mat.data);
}

//
class LString {
	LString();
	LString(const char *s);
	LString(const LString &s);
	LString(const std::string &s);
	~LString();

	void clear();

	const char *c_str() const { return str; }

	void operator=(const char *s);
	void operator=(const std::string &s);

private:
	void set(const char *s);

	char *str;
};

LString::LString() { str = nullptr; }
LString::LString(const char *s) { set(s); }
LString::LString(const LString &s) { set(s.c_str()); }
LString::LString(const std::string &s) { set(s.c_str()); }

LString::~LString() { clear(); }

void LString::operator=(const char *s) {
	clear();
	set(s);
}

void LString::operator=(const std::string &s) {
	clear();
	set(s.c_str());
}

void LString::clear() { delete[] str; }

void LString::set(const char *s) {
	if (s) {
		const size_t len = strlen(s);
		str = new char[len + 1];
		strncpy(str, s, len);
	}
}




//




//
int main(int narg, const char **args) {
	const size_t string_size = sizeof(std::string);
	const size_t lstring_size = sizeof(LString);

	const size_t shader_size = sizeof(Shader);
	const size_t uniform_buffer_data_size = sizeof(UniformData);

	GLFWwindow *win = RenderInit(640, 480, "Harfang Core - Geometry Viewer");

	glfwSetKeyCallback(win, key_callback);

	// a vertex buffer (position/color)
	const float vertices[] = {0.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f};
	sg_buffer vbuf = MakeVertexBuffer(vertices, sizeof(vertices));

	// a shader
	Shader shader = LoadShaderFromFile("dummy");

	// a pipeline state object (default render states are fine for triangle)
	const Geometry geo = LoadGeometryFromFile("D:/assemble-demo-assets/range_rover_simple/project/car_rangerover/Car01_Body_LOD0.geo");
#if 0
	VertexLayout vtx_layout;
	vtx_layout.Set(VA_Normal, SG_VERTEXFORMAT_FLOAT3);
	vtx_layout.Set(VA_Position, SG_VERTEXFORMAT_FLOAT3);
#else
	const VertexLayout vtx_layout = ComputeGeometryVertexLayout(geo);
#endif
	Model geo_to_mdl = GeometryToModel(geo, vtx_layout);
//	Model mdl = CreateSphereModel(vtx_layout, 1.f, 32, 16);

	Model compiled_mdl = LoadModelFromFile("D:/assemble-demo-assets/range_rover_simple/project_compiled/car_rangerover/Car01_Body_LOD0.geo");

	Model mdl = compiled_mdl;



	//
	sg_pipeline pip = MakePipeline(mdl.vtx_layout, shader.shader, shader.layout);

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
	UniformData uniform_data;
	SetupShaderUniformData(shader, uniform_data);

	const int mvp_idx = GetUniformDataIndex("mvp", shader);

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

		SetUniformDataValue(uniform_data, mvp_idx, Transpose(proj * view));

		//
		int cur_width, cur_height;
		glfwGetFramebufferSize(win, &cur_width, &cur_height);

		sg_begin_default_pass(&pass_action, cur_width, cur_height);

		sg_apply_pipeline(pip);
		sg_apply_bindings(bind);
		{
			sg_range range;
			range.ptr = GetUniformDataPtr(uniform_data);
			range.size = GetUniformDataSize(uniform_data);
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
