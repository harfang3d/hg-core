// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include <fmt/format.h>

#include "app_glfw/app_glfw.h"

#include "foundation/profiler.h"
#include "foundation/projection.h"

#include "engine/assets.h"
#include "engine/scene.h"

using namespace hg;

//
void SubmitSceneToPipeline(const Scene &scene) {

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
		const Mat44 proj = ComputePerspectiveProjectionMatrix(0.1f, 1000.f, 3.2f, ComputeAspectRatioX(640.f, 480.f));
		const Mat4 view = InverseFast(TransformationMat4(pos, rot));



		scene.Update(0);




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
