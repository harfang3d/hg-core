// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include <fmt/format.h>

#include "engine/assets.h"
#include "engine/scene.h"

#define SOKOL_GFX_IMPL
#define SOKOL_GLCORE33
#include <sokol_gfx.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace hg;

//
int main(int narg, const char **args) {
	if (narg < 3)
		return -1;

	Scene scene;
	PipelineResources resources;
	PipelineInfo info;

	AddAssetsFolder(args[1]);

	LoadSceneContext ctx;
	LoadSceneJsonFromAssets(args[2], scene, resources, info, ctx);

	return 0;
}
