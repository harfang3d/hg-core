// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include <fmt/format.h>

#include "app_glfw/app_glfw.h"

#include "foundation/profiler.h"

#include "engine/assets.h"
#include "engine/scene.h"

using namespace hg;

//
int main(int narg, const char **args) {
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

	RenderShutdown();
	return 0;
}
