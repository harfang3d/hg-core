// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#pragma once

namespace hg {

enum ShaderType {
	VertexShader = 0,
	FragmentShader,
	ComputeShader
};

enum ShaderLang {
	GLSLTarget = 0,
	HLSLTarget
};

enum ShaderUniformType {
	InvalidUniformType = -1,
	UniformFloat = 0,
	UniformFloat2,
	UniformFloat3,
	UniformFloat4,
	UniformInt,
	UniformInt2,
	UniformInt3,
	UniformInt4,
	UniformMat3,
	UniformMat4
	// [todo] bool, double, uint, matXY ?
};

enum ShaderImageType {
	InvalidImageType = -1,
    Image2D = 0,
    ImageCube,
    Image3D,
    ImageArray
};

enum ShaderImageBaseType {
	InvalidImageBaseType = -1,
	ImageFloat = 0,
	ImageByte,
	ImageShort,
	ImageInt,
	ImageUByte,
	ImageUShort,
	ImageUInt
};

} // namespace hg