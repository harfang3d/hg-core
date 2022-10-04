// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include <fmt/format.h>

#include "app_glfw/app_glfw.h"

#include "foundation/clock.h"
#include "foundation/profiler.h"
#include "foundation/projection.h"

#include "engine/assets.h"
#include "engine/scene.h"

using namespace hg;

//
sg_pipeline MakePipeline(
	const VertexLayout &vertex_layout, sg_shader shader, const ShaderLayout &shader_layout, uint8_t index_type_size, const RenderState &render_state) {
	sg_pipeline_desc pipeline_desc;

	memset(&pipeline_desc, 0, sizeof(sg_pipeline_desc));

	if (index_type_size == 4) {
		pipeline_desc.index_type = SG_INDEXTYPE_UINT32;
	} else if (index_type_size == 2) {
		pipeline_desc.index_type = SG_INDEXTYPE_UINT16;
	}

	pipeline_desc.shader = shader;

	//
	const uint64_t depth_test = render_state.state & RENDER_STATE_DEPTH_TEST_MASK;

	if (depth_test == RENDER_STATE_DEPTH_TEST_EQUAL) {
		pipeline_desc.depth.compare = SG_COMPAREFUNC_EQUAL;
	} else if (depth_test == RENDER_STATE_DEPTH_TEST_NOTEQUAL) {
		pipeline_desc.depth.compare = SG_COMPAREFUNC_NOT_EQUAL;
	} else if (depth_test == RENDER_STATE_DEPTH_TEST_LESS) {
		pipeline_desc.depth.compare = SG_COMPAREFUNC_LESS;
	} else if (depth_test == RENDER_STATE_DEPTH_TEST_LEQUAL) {
		pipeline_desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
	} else if (depth_test == RENDER_STATE_DEPTH_TEST_GREATER) {
		pipeline_desc.depth.compare = SG_COMPAREFUNC_GREATER;
	} else if (depth_test == RENDER_STATE_DEPTH_TEST_GEQUAL) {
		pipeline_desc.depth.compare = SG_COMPAREFUNC_GREATER_EQUAL;
	} else if (depth_test == RENDER_STATE_DEPTH_TEST_NEVER) {
		pipeline_desc.depth.compare = SG_COMPAREFUNC_NEVER;
	} else if (depth_test == RENDER_STATE_DEPTH_TEST_ALWAYS) {
		pipeline_desc.depth.compare = SG_COMPAREFUNC_ALWAYS;
	}

	pipeline_desc.depth.write_enabled = (render_state.state & RENDER_STATE_WRITE_Z) ? true : false;

	//
	pipeline_desc.alpha_to_coverage_enabled = (render_state.state & RENDER_STATE_BLEND_ALPHA_TO_COVERAGE) ? true : false;

	//
	const uint64_t cull_mask = render_state.state & RENDER_STATE_CULL_MASK;

	if (cull_mask == RENDER_STATE_CULL_CW) {
		pipeline_desc.cull_mode = SG_CULLMODE_FRONT;
	} else if (cull_mask == RENDER_STATE_CULL_CCW) {
		pipeline_desc.cull_mode = SG_CULLMODE_BACK;
	}

	pipeline_desc.face_winding = (render_state.state & RENDER_STATE_FRONT_CCW) ? SG_FACEWINDING_CCW : SG_FACEWINDING_CW;

	//
	const uint64_t write_mask = render_state.state & RENDER_STATE_WRITE_MASK;

	pipeline_desc.colors[0].write_mask = sg_color_mask(write_mask & 0xF);

	//
	const uint64_t blend_equation = render_state.state & RENDER_STATE_BLEND_EQUATION_MASK;

	const uint64_t blend_equation_rgb = blend_equation & 0x7;
	const uint64_t blend_equation_a = (blend_equation >> 3) & 0x7;

	sg_blend_state &blend_state = pipeline_desc.colors[0].blend;

	if (blend_equation_rgb == RENDER_STATE_BLEND_EQUATION_ADD) {
		blend_state.op_rgb = SG_BLENDOP_ADD;
	} else if (blend_equation_rgb == RENDER_STATE_BLEND_EQUATION_SUB) {
		blend_state.op_rgb = SG_BLENDOP_SUBTRACT;
	} else if (blend_equation_rgb == RENDER_STATE_BLEND_EQUATION_REVSUB) {
		blend_state.op_rgb = SG_BLENDOP_REVERSE_SUBTRACT;
	} else {
		//
	}

	if (blend_equation_a == RENDER_STATE_BLEND_EQUATION_ADD) {
		blend_state.op_alpha = SG_BLENDOP_ADD;
	} else if (blend_equation_a == RENDER_STATE_BLEND_EQUATION_SUB) {
		blend_state.op_alpha = SG_BLENDOP_SUBTRACT;
	} else if (blend_equation_a == RENDER_STATE_BLEND_EQUATION_REVSUB) {
		blend_state.op_alpha = SG_BLENDOP_REVERSE_SUBTRACT;
	} else {
		//
	}

	blend_state.enabled = (blend_equation != 0) ? true : false;

	//
	const uint64_t blend = render_state.state & RENDER_STATE_BLEND_MASK;

	const uint64_t src_rgb = blend & 0xF;
	const uint64_t dst_rgb = (blend >> 4) & 0xF;
	const uint64_t src_alpha = (blend >> 8) & 0xF;
	const uint64_t dst_alpha = (blend >> (4 + 8)) & 0xF;

	if (src_rgb == RENDER_STATE_BLEND_ZERO) {
		blend_state.src_factor_rgb = SG_BLENDFACTOR_ZERO;
	} else if (src_rgb == RENDER_STATE_BLEND_ONE) {
		blend_state.src_factor_rgb = SG_BLENDFACTOR_ONE;
	} else if (src_rgb == RENDER_STATE_BLEND_SRC_COLOR) {
		blend_state.src_factor_rgb = SG_BLENDFACTOR_SRC_COLOR;
	} else if (src_rgb == RENDER_STATE_BLEND_INV_SRC_COLOR) {
		blend_state.src_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_COLOR;
	}

	if (dst_rgb == RENDER_STATE_BLEND_ZERO) {
		blend_state.dst_factor_rgb = SG_BLENDFACTOR_ZERO;
	} else if (dst_rgb == RENDER_STATE_BLEND_ONE) {
		blend_state.dst_factor_rgb = SG_BLENDFACTOR_ONE;
	} else if (dst_rgb == RENDER_STATE_BLEND_DST_COLOR) {
		blend_state.dst_factor_rgb = SG_BLENDFACTOR_SRC_COLOR;
	} else if (dst_rgb == RENDER_STATE_BLEND_INV_SRC_COLOR) {
		blend_state.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_DST_COLOR;
	}

	if (src_alpha == RENDER_STATE_BLEND_ZERO) {
		blend_state.src_factor_alpha = SG_BLENDFACTOR_ZERO;
	} else if (src_alpha == RENDER_STATE_BLEND_ONE) {
		blend_state.src_factor_alpha = SG_BLENDFACTOR_ONE;
	} else if (src_alpha == RENDER_STATE_BLEND_SRC_ALPHA) {
		blend_state.src_factor_alpha = SG_BLENDFACTOR_SRC_ALPHA;
	} else if (src_alpha == RENDER_STATE_BLEND_INV_SRC_ALPHA) {
		blend_state.src_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
	}

	if (dst_alpha == RENDER_STATE_BLEND_ZERO) {
		blend_state.dst_factor_alpha = SG_BLENDFACTOR_ZERO;
	} else if (dst_alpha == RENDER_STATE_BLEND_ONE) {
		blend_state.dst_factor_alpha = SG_BLENDFACTOR_ONE;
	} else if (dst_alpha == RENDER_STATE_BLEND_DST_ALPHA) {
		blend_state.dst_factor_alpha = SG_BLENDFACTOR_SRC_ALPHA;
	} else if (dst_alpha == RENDER_STATE_BLEND_INV_DST_ALPHA) {
		blend_state.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_DST_ALPHA;
	}

	//
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
		BoundDisplayList &bound_list = obj.bound_lists[i];

		const size_t mat_idx = mdl.mats[i];
		Material &mat = obj.materials[mat_idx];

		if (resources.programs.IsValidRef(mat.program)) {
			bound_list.valid = true;

			const PipelineProgram &pipeline_shader = resources.programs.Get_unsafe_(mat.program.ref.idx);
			SetupShaderUniformData(pipeline_shader.shader, mat.uniform_data);

			memset(&bound_list.bindings, 0, sizeof(sg_bindings));

			bound_list.bindings.index_buffer = list.index_buffer;
			bound_list.bindings.vertex_buffers[0] = list.vertex_buffer;

			bound_list.pipeline = MakePipeline(
				mdl.vtx_layout, pipeline_shader.shader.shader, pipeline_shader.shader.layout, list.index_type_size, mat.state); // MUST BE REMADE ON SHADER CHANGE!

			for (int k = 0; k < SG_NUM_SHADER_STAGES; ++k) {
				sg_image *img = (k == 0) ? bound_list.bindings.vs_images : bound_list.bindings.fs_images;

				for (int j = 0; j < SG_MAX_SHADERSTAGE_IMAGES; ++j) {
					const std::string &shader_image_name = pipeline_shader.shader.images[k][j].name;

					if (!shader_image_name.empty()) { // shader expects a texture
						std::map<std::string, Material::Texture>::const_iterator i = mat.textures.find(shader_image_name);

						if (i != mat.textures.end()) {
							if (resources.textures.IsValidRef(i->second.texture)) {
								const Texture &tex = resources.textures.Get_unsafe_(i->second.texture.ref.idx);
								img[j].id = tex.image.id;
							} else {
								bound_list.valid = false;
							}
						} else {
							bound_list.valid = false;
						}
					}
				}
			}
		} else {
			bound_list.valid = false;
		}
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

Mat44 Mat44_to_sokol(const Mat44 &m) {
	return Transpose(m);
}

void DrawObject(Scene::Object_ &obj, PipelineResources &resources, const Mat44 &mvp) {
	if (!resources.models.IsValidRef(obj.model))
		return;

	const Model &mdl = resources.models.Get(obj.model);
	const size_t list_count = mdl.lists.size();

	for (size_t i = 0; i < list_count; ++i) {
		const DisplayList &list = mdl.lists[i];
		const BoundDisplayList &bound_list = obj.bound_lists[i];

		if (bound_list.valid) {
			const uint16_t mat_idx = mdl.mats[i];
			Material &mat = obj.materials[mat_idx];

			const PipelineProgram &program = resources.programs.Get(mat.program);

			//
			sg_apply_pipeline(bound_list.pipeline);
			sg_apply_bindings(bound_list.bindings);

			// TODO set uniform values
			const int mvp_idx = GetUniformDataIndex("u_mtx.mvp", program.shader);
			SetUniformDataValue(mat.uniform_data, mvp_idx, Mat44_to_sokol(mvp));

			//
			sg_range range;
			range.ptr = GetUniformDataPtr(mat.uniform_data);
			range.size = GetUniformDataSize(mat.uniform_data);
			sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, range);

			sg_draw(0, list.element_count, 1);
		}
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

	// Node node = scene.GetNodeEx("scene_range:Car01_Body/Car01_Body_LOD0");
	// Scene::Object_ &obj = scene.objects[node.GetObject().ref.idx];

	const std::vector<Node> objects = scene.GetAllNodesWithComponent(NCI_Object);

	for (std::vector<Node>::const_iterator it = objects.begin(); it != objects.end(); ++it) {
		Scene::Object_ &obj = scene.objects[it->GetObject().ref.idx];
		BindObject(obj, resources);
	}

	// default pass action (clear to grey)
	sg_pass_action pass_action = {0};

	const std::vector<SceneAnimRef> scene_anims = scene.GetSceneAnims();

	if (!scene_anims.empty()) {
		const SceneAnimRef scene_anim_ref = scene_anims[0];
		scene.PlayAnim(scene_anim_ref, ALM_Loop);
	}

	reset_clock();

	while (!glfwWindowShouldClose(win)) {
		const time_ns dt_clock = tick_clock();

		scene.Update(dt_clock);

		int width, height;
		glfwGetFramebufferSize(win, &width, &height);

		const ViewState view_state = scene.ComputeCurrentCameraViewState(ComputeAspectRatioX(width, height));

		// SubmitSceneToPipeline(scene, view_state, resources);

		const Mat44 vp = view_state.proj * view_state.view;

		//
		sg_begin_default_pass(&pass_action, width, height);

		for (std::vector<Node>::const_iterator it = objects.begin(); it != objects.end(); ++it) {
			Scene::Object_ &obj = scene.objects[it->GetObject().ref.idx];

			const Mat4 world = it->GetTransform().GetWorld();
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
