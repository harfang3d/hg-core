// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include <fmt/format.h>

#include "app_glfw/app_glfw.h"

#include "foundation/profiler.h"
#include "foundation/projection.h"

#include "engine/assets.h"
#include "engine/scene.h"

using namespace hg;

//











//
sg_pipeline MakePipeline(const VertexLayout &vertex_layout, sg_shader shader, const ShaderLayout &shader_layout) {
	sg_pipeline_desc pipeline_desc;

	memset(&pipeline_desc, 0, sizeof(sg_pipeline_desc));
	pipeline_desc.index_type = SG_INDEXTYPE_UINT16;
	pipeline_desc.shader = shader;

	pipeline_desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
	pipeline_desc.depth.write_enabled = true;
	pipeline_desc.cull_mode = SG_CULLMODE_FRONT;

	FillPipelineLayout(vertex_layout, shader_layout, pipeline_desc.layout);

	return sg_make_pipeline(&pipeline_desc);
}

//
void BindObject(Scene::Object_ &obj, const PipelineResources &resources) {
	if (!resources.models.IsValidRef(obj.model))
		return;

	const Model &mdl = resources.models.Get_unsafe_(obj.model.ref.idx);

	obj.bound_lists.resize(mdl.lists.size());

	for (size_t i = 0; i < mdl.lists.size(); ++i) {
		const DisplayList &list = mdl.lists[i];
		const size_t mat_idx = mdl.mats[i];

		Material &mat = obj.materials[mat_idx];

		if (!resources.programs.IsValidRef(mat.program))
			continue;

		BoundDisplayList &bound_list = obj.bound_lists[i];

		const PipelineProgram &pipeline_shader = resources.programs.Get(mat.program);
		SetupShaderUniformData(pipeline_shader.shader, mat.uniform_data);


		memset(&bound_list.bindings, 0, sizeof(sg_bindings));

		bound_list.bindings.index_buffer = list.index_buffer;
		bound_list.bindings.vertex_buffers[0] = list.vertex_buffer;

		bound_list.pipeline = MakePipeline(mdl.vtx_layout, pipeline_shader.shader.shader, pipeline_shader.shader.layout); // MUST BE REMADE ON SHADER CHANGE!
	}
}




//
void SubmitSceneToPipeline(const Scene &scene, const ViewState &view_state, const PipelineResources &resources) {
	const std::vector<Node> all_nodes = scene.GetAllNodes();

	for (std::vector<Node>::const_iterator i = all_nodes.begin(); i != all_nodes.end(); ++i) {
		const Node &node = *i;

		if (!node.HasTransform() || !node.HasObject())
			continue;

		const Object obj = node.GetObject();
		const ModelRef mdl_ref = obj.GetModelRef();

		if (!resources.models.IsValidRef(mdl_ref))
			continue;

		const Model &mdl = resources.models.Get(mdl_ref);



		const size_t list_count = mdl.lists.size();

		for (size_t i = 0; i < list_count; ++i) {
			const DisplayList &display_list = mdl.lists[i];
			const uint16_t mat_idx = mdl.mats[i];

			const Material &mat = obj.GetMaterial(mat_idx);


//




		}




//		for (std::vector<DisplayList>::const_iterator _idx = mdl.lists.begin())









	}

}




Mat44 Mat44_to_sokol(const Mat44 &m) { return Transpose(m); }

void DrawObject(Scene::Object_ &obj, PipelineResources &resources, const Mat44 &mvp) {
	if (!resources.models.IsValidRef(obj.model))
		return;

	const Model &mdl = resources.models.Get(obj.model);
	const size_t list_count = mdl.lists.size();

	for (size_t i = 0; i < list_count; ++i) {
		const DisplayList &list = mdl.lists[i];
		const BoundDisplayList &bound_list = obj.bound_lists[i];

		const uint16_t mat_idx = mdl.mats[i];
		Material &mat = obj.materials[mat_idx];

		const PipelineProgram &program = resources.programs.Get(mat.program);

		//
		sg_apply_pipeline(bound_list.pipeline);
		sg_apply_bindings(bound_list.bindings);

		// TODO set uniform values
		const int mvp_idx = GetUniformDataIndex("mvp", program.shader);
		SetUniformDataValue(mat.uniform_data, mvp_idx, Mat44_to_sokol(mvp));

		//
		sg_range range;
		range.ptr = GetUniformDataPtr(mat.uniform_data);
		range.size = GetUniformDataSize(mat.uniform_data);
		sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, range);

		sg_draw(0, list.element_count, 1);
	}
}










int main(int narg, const char **args) {
	const size_t vertex_layout_size = sizeof(VertexLayout); // 288 -> 9
	const size_t shader_layout_size = sizeof(ShaderLayout); // 64 -> 16

	if (narg < 3)
		return -1;

	GLFWwindow *win = RenderInit(640, 480, "Harfang Core - Scene Viewer");

	Scene scene;
	PipelineResources resources;
	PipelineInfo info;

	AddAssetsFolder(args[1]);

	LoadSceneContext ctx;
//	LoadSceneJsonFromAssets(args[2], scene, resources, info, ctx);
	LoadSceneBinaryFromAssets(args[2], scene, resources, info, ctx);

	PrintProfilerFrame(EndProfilerFrame());

	Vec3 pos(0, 2, -5), rot(0, 0, 0);




//Node node = scene.GetNodeEx("scene_range:Car01_Body/Car01_Body_LOD0");
//Scene::Object_ &obj = scene.objects[node.GetObject().ref.idx];

	const auto objects = scene.GetAllNodesWithComponent(NCI_Object);

	for (auto &node : objects) {
		Scene::Object_ &obj = scene.objects[node.GetObject().ref.idx];
		BindObject(obj, resources);
	}


	// default pass action (clear to grey)
	sg_pass_action pass_action = {0};


	while (!glfwWindowShouldClose(win)) {
		scene.Update(0);

/*
		const Mat4 camera_world = TransformationMat4(pos, rot);
		const Mat44 projection = ComputePerspectiveProjectionMatrix(0.1f, 1000.f, 3.2f, ComputeAspectRatioX(640.f, 480.f));

		ViewState view_state;
		view_state.proj = projection;
		view_state.view = InverseFast(camera_world);
		view_state.frustum = MakeFrustum(projection, camera_world);
*/

		const ViewState view_state = scene.ComputeCurrentCameraViewState(ComputeAspectRatioX(640.f, 480.f));

		// SubmitSceneToPipeline(scene, view_state, resources);

		const Mat44 vp = view_state.proj * view_state.view;



		//
		int cur_width, cur_height;
		glfwGetFramebufferSize(win, &cur_width, &cur_height);

		sg_begin_default_pass(&pass_action, cur_width, cur_height);



		for (auto &node : objects) {
			Scene::Object_ &obj = scene.objects[node.GetObject().ref.idx];

			const Mat4 world = node.GetTransform().GetWorld();
			const Mat44 mvp = vp * world;

			DrawObject(obj, resources, mvp);
		}





		sg_end_pass();

		sg_commit();

		glfwSwapBuffers(win);
		glfwPollEvents();
	}

	PrintProfilerFrame(EndProfilerFrame());

	RenderShutdown();
	return 0;
}
