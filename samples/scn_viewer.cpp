// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include <fmt/format.h>

#include "app_glfw/app_glfw.h"

#include "foundation/profiler.h"
#include "foundation/projection.h"

#include "engine/assets.h"
#include "engine/scene.h"

using namespace hg;

//






struct BoundDisplayList { // links DisplayList to object and its materials
	sg_pipeline pipeline; // links list attributes to shader inputs
	sg_bindings bindings; // links list buffers and material resources to shader inputs
};

struct BoundModel { // links Model to Object and its Materials
	std::vector<BoundDisplayList> bound_lists;
};





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
void BindDisplayList(const DisplayList &list, BoundDisplayList &bound_list, const VertexLayout &vtx_layout, sg_shader shader, const ShaderLayout &shader_layout) {
	memset(&bound_list.bindings, 0, sizeof(sg_bindings));

	bound_list.bindings.index_buffer = list.index_buffer;
	bound_list.bindings.vertex_buffers[0] = list.vertex_buffer;

	//
	bound_list.pipeline = MakePipeline(vtx_layout, shader, shader_layout);
}



//
/*
void BindModel(const Model &mdl, BoundModel &bound_mdl) {
	bound_mdl.bound_lists.resize(mdl.lists.size());

	for (size_t i = 0; i < mdl.lists.size(); ++i)
		BindDisplayList(mdl.lists[i], bound_mdl.bound_lists[i]);




}
*/



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





void TU() {
	hg::Scene scene;
	hg::Node node = scene.CreateNode();

	assert(node.IsValid() == true);
	scene.DestroyNode(node);
	assert(node.IsValid() == false);

	assert(node.HasTransform() == false);
	assert(node.HasCamera() == false);
	assert(node.HasLight() == false);
	assert(node.HasObject() == false);
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

	while (!glfwWindowShouldClose(win)) {
		const Mat4 camera_world = TransformationMat4(pos, rot);
		const Mat44 projection = ComputePerspectiveProjectionMatrix(0.1f, 1000.f, 3.2f, ComputeAspectRatioX(640.f, 480.f));



		scene.Update(0);


		ViewState view_state;
		view_state.proj = projection;
		view_state.view = InverseFast(camera_world);
		view_state.frustum = MakeFrustum(projection, camera_world);

		SubmitSceneToPipeline(scene, view_state, resources);


/*
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
*/

		glfwSwapBuffers(win);
		glfwPollEvents();
	}

	PrintProfilerFrame(EndProfilerFrame());

	RenderShutdown();
	return 0;
}
