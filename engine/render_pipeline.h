// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#pragma once

#include "engine/picture.h"
#include "engine/resource_cache.h"

#include "foundation/cext.h"
#include "foundation/color.h"
#include "foundation/data.h"
#include "foundation/frustum.h"
#include "foundation/generational_vector_list.h"
#include "foundation/matrix4.h"
#include "foundation/matrix44.h"
#include "foundation/minmax.h"
#include "foundation/rw_interface.h"
#include "foundation/time.h"
#include "foundation/vector2.h"

#include <deque>
#include <functional>
#include <map>
#include <rapidjson/document.h>
#include <sokol_gfx.h>
#include <string>
#include <vector>

#if _WIN32
#undef GetObject // stop Windows.h from polluting global namespace
#endif

namespace hg {

class Data;

static const int max_skinned_model_matrix_count = 32; // make sure this stays in sync with bgfx_shader.sh

// from BGFX
#define RENDER_STATE_WRITE_R uint64_t(0x0000000000000001) // Enable R write.
#define RENDER_STATE_WRITE_G uint64_t(0x0000000000000002) // Enable G write.
#define RENDER_STATE_WRITE_B uint64_t(0x0000000000000004) // Enable B write.
#define RENDER_STATE_WRITE_A uint64_t(0x0000000000000008) // Enable alpha write.
#define RENDER_STATE_WRITE_Z uint64_t(0x0000004000000000) // Enable depth write.

#define RENDER_STATE_WRITE_RGB (RENDER_STATE_WRITE_R | RENDER_STATE_WRITE_G | RENDER_STATE_WRITE_B)
#define RENDER_STATE_WRITE_MASK (RENDER_STATE_WRITE_RGB | RENDER_STATE_WRITE_A | RENDER_STATE_WRITE_Z)

#define RENDER_STATE_DEPTH_TEST_LESS uint64_t(0x0000000000000010) // Enable depth test, less.
#define RENDER_STATE_DEPTH_TEST_LEQUAL uint64_t(0x0000000000000020) // Enable depth test, less or equal.
#define RENDER_STATE_DEPTH_TEST_EQUAL uint64_t(0x0000000000000030) // Enable depth test, equal.
#define RENDER_STATE_DEPTH_TEST_GEQUAL uint64_t(0x0000000000000040) // Enable depth test, greater or equal.
#define RENDER_STATE_DEPTH_TEST_GREATER uint64_t(0x0000000000000050) // Enable depth test, greater.
#define RENDER_STATE_DEPTH_TEST_NOTEQUAL uint64_t(0x0000000000000060) // Enable depth test, not equal.
#define RENDER_STATE_DEPTH_TEST_NEVER uint64_t(0x0000000000000070) // Enable depth test, never.
#define RENDER_STATE_DEPTH_TEST_ALWAYS uint64_t(0x0000000000000080) // Enable depth test, always.
#define RENDER_STATE_DEPTH_TEST_SHIFT 4 // Depth test state bit shift
#define RENDER_STATE_DEPTH_TEST_MASK uint64_t(0x00000000000000f0) // Depth test state bit mask

#define RENDER_STATE_BLEND_ZERO uint64_t(0x0000000000001000) // 0, 0, 0, 0
#define RENDER_STATE_BLEND_ONE uint64_t(0x0000000000002000) // 1, 1, 1, 1
#define RENDER_STATE_BLEND_SRC_COLOR uint64_t(0x0000000000003000) // Rs, Gs, Bs, As
#define RENDER_STATE_BLEND_INV_SRC_COLOR uint64_t(0x0000000000004000) // 1-Rs, 1-Gs, 1-Bs, 1-As
#define RENDER_STATE_BLEND_SRC_ALPHA uint64_t(0x0000000000005000) // As, As, As, As
#define RENDER_STATE_BLEND_INV_SRC_ALPHA uint64_t(0x0000000000006000) // 1-As, 1-As, 1-As, 1-As
#define RENDER_STATE_BLEND_DST_ALPHA uint64_t(0x0000000000007000) // Ad, Ad, Ad, Ad
#define RENDER_STATE_BLEND_INV_DST_ALPHA uint64_t(0x0000000000008000) // 1-Ad, 1-Ad, 1-Ad ,1-Ad
#define RENDER_STATE_BLEND_DST_COLOR uint64_t(0x0000000000009000) // Rd, Gd, Bd, Ad
#define RENDER_STATE_BLEND_INV_DST_COLOR uint64_t(0x000000000000a000) // 1-Rd, 1-Gd, 1-Bd, 1-Ad
#define RENDER_STATE_BLEND_SRC_ALPHA_SAT uint64_t(0x000000000000b000) // f, f, f, 1; f = min(As, 1-Ad)
#define RENDER_STATE_BLEND_FACTOR uint64_t(0x000000000000c000) // Blend factor
#define RENDER_STATE_BLEND_INV_FACTOR uint64_t(0x000000000000d000) // 1-Blend factor
#define RENDER_STATE_BLEND_SHIFT 12 // Blend state bit shift
#define RENDER_STATE_BLEND_MASK uint64_t(0x000000000ffff000) // Blend state bit mask

#define RENDER_STATE_BLEND_EQUATION_ADD uint64_t(0x0000000000000000) // Blend add: src + dst.
#define RENDER_STATE_BLEND_EQUATION_SUB uint64_t(0x0000000010000000) // Blend subtract: src - dst.
#define RENDER_STATE_BLEND_EQUATION_REVSUB uint64_t(0x0000000020000000) // Blend reverse subtract: dst - src.
#define RENDER_STATE_BLEND_EQUATION_MIN uint64_t(0x0000000030000000) // Blend min: min(src, dst).
#define RENDER_STATE_BLEND_EQUATION_MAX uint64_t(0x0000000040000000) // Blend max: max(src, dst).
#define RENDER_STATE_BLEND_EQUATION_SHIFT 28 // Blend equation bit shift
#define RENDER_STATE_BLEND_EQUATION_MASK uint64_t(0x00000003f0000000) // Blend equation bit mask

#define RENDER_STATE_CULL_CW uint64_t(0x0000001000000000) // Cull clockwise triangles.
#define RENDER_STATE_CULL_CCW uint64_t(0x0000002000000000) // Cull counter-clockwise triangles.
#define RENDER_STATE_CULL_SHIFT 36 // Culling mode bit shift
#define RENDER_STATE_CULL_MASK uint64_t(0x0000003000000000) // Culling mode bit mask

#define RENDER_STATE_ALPHA_REF_SHIFT 40 // Alpha reference bit shift
#define RENDER_STATE_ALPHA_REF_MASK uint64_t(0x0000ff0000000000) // Alpha reference bit mask
#define RENDER_STATE_ALPHA_REF(v) (((uint64_t)(v) << RENDER_STATE_ALPHA_REF_SHIFT) & RENDER_STATE_ALPHA_REF_MASK)

#define RENDER_STATE_PT_TRISTRIP uint64_t(0x0001000000000000) // Tristrip.
#define RENDER_STATE_PT_LINES uint64_t(0x0002000000000000) // Lines.
#define RENDER_STATE_PT_LINESTRIP uint64_t(0x0003000000000000) // Line strip.
#define RENDER_STATE_PT_POINTS uint64_t(0x0004000000000000) // Points.
#define RENDER_STATE_PT_SHIFT 48 // Primitive type bit shift
#define RENDER_STATE_PT_MASK uint64_t(0x0007000000000000) // Primitive type bit mask

#define RENDER_STATE_POINT_SIZE_SHIFT 52 // Point size bit shift
#define RENDER_STATE_POINT_SIZE_MASK uint64_t(0x00f0000000000000) // Point size bit mask
#define RENDER_STATE_POINT_SIZE(v) (((uint64_t)(v) << RENDER_STATE_POINT_SIZE_SHIFT) & RENDER_STATE_POINT_SIZE_MASK)

#define RENDER_STATE_MSAA uint64_t(0x0100000000000000) // Enable MSAA rasterization.
#define RENDER_STATE_LINEAA uint64_t(0x0200000000000000) // Enable line AA rasterization.
#define RENDER_STATE_CONSERVATIVE_RASTER uint64_t(0x0400000000000000) // Enable conservative rasterization.
#define RENDER_STATE_NONE uint64_t(0x0000000000000000) // No state.
#define RENDER_STATE_FRONT_CCW uint64_t(0x0000008000000000) // Front counter-clockwise (default is clockwise).
#define RENDER_STATE_BLEND_INDEPENDENT uint64_t(0x0000000400000000) // Enable blend independent.
#define RENDER_STATE_BLEND_ALPHA_TO_COVERAGE uint64_t(0x0000000800000000) // Enable alpha to coverage.

#define RENDER_STATE_DEFAULT                                                                                                                                   \
	(RENDER_STATE_WRITE_RGB | RENDER_STATE_WRITE_A | RENDER_STATE_WRITE_Z | RENDER_STATE_DEPTH_TEST_LESS | RENDER_STATE_CULL_CW | RENDER_STATE_MSAA)

#define RENDER_STATE_MASK uint64_t(0xffffffffffffffff) // State bit mask

enum UniformType {
	UT_Sampler, // Sampler
	UT_End, // Reserved, do not use

	UT_Vec4, // 4 floats vector
	UT_Mat3, // 3x3 matrix
	UT_Mat4, // 4x4 matrix

	UT_Count
};

//
struct RenderState {
	RenderState() : state(RENDER_STATE_DEFAULT), rgba(0) {}

	uint64_t state;
	uint32_t rgba;
};

//
struct ViewState {
	Frustum frustum;
	Mat44 proj;
	Mat4 view;
};

ViewState ComputeOrthographicViewState(const Mat4 &world, float size, float znear, float zfar, const Vec2 &aspect_ratio, const Vec2 &offset = Vec2::Zero);
ViewState ComputePerspectiveViewState(const Mat4 &world, float fov, float znear, float zfar, const Vec2 &aspect_ratio, const Vec2 &offset = Vec2::Zero);

Mat4 ComputeBillboardMat4(const Vec3 &pos, const ViewState &view_state, const Vec3 &scale = Vec3::One);

//
struct Window;

#if 0

bool RenderInit(Window *window, bgfx::RendererType::Enum type, bgfx::CallbackI *callback = nullptr);
bool RenderInit(Window *window, bgfx::CallbackI *callback = nullptr);

Window *RenderInit(int width, int height, bgfx::RendererType::Enum type, uint32_t reset_flags = 0,
	bgfx::TextureFormat::Enum format = bgfx::TextureFormat::Count, uint32_t debug_flags = 0, bgfx::CallbackI *callback = nullptr);
Window *RenderInit(int width, int height, uint32_t reset_flags = 0, bgfx::TextureFormat::Enum format = bgfx::TextureFormat::Count, uint32_t debug_flags = 0,
	bgfx::CallbackI *callback = nullptr);
Window *RenderInit(const std::string &window_title, int width, int height, bgfx::RendererType::Enum type, uint32_t reset_flags = 0,
	bgfx::TextureFormat::Enum format = bgfx::TextureFormat::Count, uint32_t debug_flags = 0, bgfx::CallbackI *callback = nullptr);
Window *RenderInit(const std::string &window_title, int width, int height, uint32_t reset_flags = 0, bgfx::TextureFormat::Enum format = bgfx::TextureFormat::Count,
	uint32_t debug_flags = 0, bgfx::CallbackI *callback = nullptr);

void RenderShutdown();

bool IsRenderUp();

/// Fit the backbuffer to the specified window client area dimensions, return true if resizing was carried out.
bool RenderResetToWindow(Window *win, int &width, int &height, uint32_t reset_flags = 0);

void SetView2D(bgfx::ViewId id, int x, int y, int res_x, int res_y, float znear = -1.f, float zfar = 1.f,
	uint16_t clear_flags = BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, const Color &clear_color = Color::Black, float depth = 1.f, uint8_t stencil = 0,
	bool y_up = false);
void SetViewPerspective(bgfx::ViewId id, int x, int y, int res_x, int res_y, const Mat4 &world, float znear = 0.01f, float zfar = 1000.f,
	float zoom_factor = 1.8, uint16_t clear_flags = BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, const Color &clear_color = Color::Black, float depth = 1.f,
	uint8_t stencil = 0, const Vec2 &aspect_ratio = {});
void SetViewOrthographic(bgfx::ViewId id, int x, int y, int res_x, int res_y, const Mat4 &world, float znear = 0.01f, float zfar = 1000.f, float size = 1.f,
	uint16_t clear_flags = BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, const Color &clear_color = Color::Black, float depth = 1.f, uint8_t stencil = 0,
	const Vec2 &aspect_ratio = {});

//
bgfx::ProgramHandle LoadProgram(const Reader &ir, const ReadProvider &ip, const std::string &vs_name, const std::string &fs_name, bool silent = false);
bgfx::ProgramHandle LoadProgram(const Reader &ir, const ReadProvider &ip, const std::string &name, bool silent = false);
bgfx::ProgramHandle LoadComputeProgram(const Reader &ir, const ReadProvider &ip, const std::string &cs_name, bool silent = false);

bgfx::ProgramHandle LoadProgramFromFile(const std::string &vs_path, const std::string &fs_path, bool silent = false);
bgfx::ProgramHandle LoadProgramFromAssets(const std::string &vs_name, const std::string &fs_name, bool silent = false);
bgfx::ProgramHandle LoadProgramFromFile(const std::string &path, bool silent = false);
bgfx::ProgramHandle LoadProgramFromAssets(const std::string &name, bool silent = false);
bgfx::ProgramHandle LoadComputeProgramFromFile(const std::string &cs_path, bool silent = false);
bgfx::ProgramHandle LoadComputeProgramFromAssets(const std::string &cs_name, bool silent = false);

//
std::vector<bgfx::ShaderHandle> GetProgramShaders(bgfx::ProgramHandle prg_h);

#endif

//
rapidjson::Value LoadResourceMeta(const Reader &ir, const ReadProvider &ip, const std::string &name);
rapidjson::Value LoadResourceMetaFromFile(const std::string &path);
rapidjson::Value LoadResourceMetaFromAssets(const std::string &name);

bool SaveResourceMetaToFile(const std::string &path, const rapidjson::Value &meta);

//
struct PipelineInfo {
	std::string name;
	std::vector<std::vector<std::string> > configs;
};

struct PipelineResources;

//
enum PipelineProgramFeature {
	OptionalBaseColorOpacityMap, // USE_BASE_COLOR_OPACITY_MAP
	OptionalOcclusionRoughnessMetalnessMap, // USE_OCCLUSION_ROUGHNESS_METALNESS_MAP

	OptionalDiffuseMap, // USE_DIFFUSE_MAP
	OptionalSpecularMap, // USE_SPECULAR_MAP
	OptionalLightMap, // USE_LIGHT_MAP
	OptionalSelfMap, // USE_SELF_MAP
	OptionalOpacityMap, // USE_OPACITY_MAP
	OptionalAmbientMap, // USE_AMBIENT_MAP
	OptionalReflectionMap, // USE_REFLECTION_MAP
	OptionalNormalMap, // USE_NORMAL_MAP

	NormalMapInWorldSpace, // NORMAL_MAP_IN_WORLD_SPACE

	DiffuseUV1, // DIFFUSE_UV_CHANNEL=N
	SpecularUV1, // SPECULAR_UV_CHANNEL=N
	AmbientUV1, // AMBIENT_UV_CHANNEL=N

	OptionalSkinning, // ENABLE_SKINNING
	OptionalAlphaCut, // ENABLE_ALPHA_CUT

	Count
};

std::vector<PipelineProgramFeature> LoadPipelineProgramFeatures(
	const Reader &ir, const ReadProvider &ip, const std::string &name, bool &success, bool silent = false);
std::vector<PipelineProgramFeature> LoadPipelineProgramFeaturesFromFile(const std::string &path, bool &success, bool silent = false);
std::vector<PipelineProgramFeature> LoadPipelineProgramFeaturesFromAssets(const std::string &name, bool &success, bool silent = false);

//
enum VertexAttribute { VA_Position, VA_Normal, VA_Tangent, VA_Bitangent, VA_Color, VA_BoneIndices, VA_BoneWeights, VA_UV0, VA_UV1, VA_Count };

struct ShaderLayout {
	ShaderLayout() {
		for (size_t i = 0; i < SG_MAX_VERTEX_ATTRIBUTES; ++i) {
			attrib[i] = VA_Count;
		}
	}

	uint8_t attrib[SG_MAX_VERTEX_ATTRIBUTES]; // VertexAttribute
};

struct VertexLayout {
	VertexLayout();

	void Set(VertexAttribute semantic, sg_vertex_format format, size_t offset);

	bool Has(VertexAttribute attr) const {
		return attrib[attr].format != SG_VERTEXFORMAT_INVALID;
	}

	sg_vertex_format GetFormat(VertexAttribute attr) const {
		return static_cast<sg_vertex_format>(attrib[attr].format);
	}

	size_t GetOffset(VertexAttribute attr) const {
		return static_cast<size_t>(attrib[attr].offset);
	}

	size_t GetStride() const {
		return stride;
	}

	void SetStride(uint8_t stride_) {
		stride = stride_;
	}

	void PackVertex(VertexAttribute semantic, const float *in, size_t in_count, int8_t *out) const;
	void PackVertex(VertexAttribute semantic, const uint8_t *in, size_t in_count, int8_t *out) const;

	struct Attrib {
		uint8_t format; // sg_vertex_format
		uint8_t offset; // offset
	};

private:
	Attrib attrib[VA_Count];
	uint8_t stride;
};

//
VertexLayout VertexLayoutPosFloatNormFloat();
VertexLayout VertexLayoutPosFloatNormUInt8();
VertexLayout VertexLayoutPosFloatColorFloat();
VertexLayout VertexLayoutPosFloatColorUInt8();
VertexLayout VertexLayoutPosFloatTexCoord0UInt8();
VertexLayout VertexLayoutPosFloatNormUInt8TexCoord0UInt8();

//
void FillPipelineLayout(const VertexLayout &vertex_layout, const ShaderLayout &shader_layout, sg_layout_desc &layout, size_t buffer_index = 0);

//
sg_buffer MakeIndexBuffer(const void *data, size_t size);
sg_buffer MakeVertexBuffer(const void *data, size_t size);

//
struct PipelineProgram;
struct Material;
struct Texture;
struct Model;

typedef ResourceRef<PipelineProgram> PipelineProgramRef;
typedef ResourceRef<Material> MaterialRef;
typedef ResourceRef<Texture> TextureRef;
typedef ResourceRef<Model> ModelRef;

static const PipelineProgramRef InvalidPipelineProgramRef;
static const MaterialRef InvalidMaterialRef;
static const TextureRef InvalidTextureRef;
static const ModelRef InvalidModelRef;

//
static const float default_shadow_bias = 0.0001F;
static const Vec4 default_pssm_split = Vec4(10.F, 50.F, 100.F, 500.F);

#if 1

//
struct ShaderUniform {
	ShaderUniform() : type(SG_UNIFORMTYPE_INVALID) {}

	std::string name;
	sg_uniform_type type;
};

struct ShaderUniforms {
	ShaderUniforms() : layout(_SG_UNIFORMLAYOUT_DEFAULT) {}

	ShaderUniform uniform[SG_MAX_UB_MEMBERS];
	sg_uniform_layout layout;
};

struct Shader { // 20B
	Shader() {
		shader.id = SG_INVALID_ID;
	}

	sg_shader shader;

	ShaderLayout layout; // attributes
	ShaderUniforms uniforms; // uniforms
};

Shader LoadShader(const Reader &ir, const ReadProvider &ip, const std::string &name, bool silent = false);
Shader LoadShaderFromFile(const std::string &path, bool silent = false);
Shader LoadShaderFromAssets(const std::string &name, bool silent = false);

struct PipelineProgram {
	Shader shader;
};

/*
struct TextureUniform {
	TextureUniform() : channel(0xff) {}
	//	bgfx::UniformHandle handle;
	TextureRef tex_ref;
	uint8_t channel;
};

struct Vec4Uniform {
	Vec4Uniform() : value(1.f, 1.f, 1.f, 1.f), is_color(false) {}
	//	bgfx::UniformHandle handle;
	Vec4 value;
	bool is_color;
};
*/

#if 0
struct PipelineProgram {
	std::vector<PipelineProgramFeature> features; // features imply associated uniforms
	std::vector<TextureUniform> texture_uniforms; // naked texture uniforms
	std::vector<Vec4Uniform> vec4_uniforms; // naked vec4/color uniforms
	std::vector<ProgramHandle> programs; // program variants (dependent on feature set)

	std::string name;
	PipelineInfo pipeline;
	ReadProvider ip;
	Reader ir;
};

PipelineProgram LoadPipelineProgram(
	const Reader &ir, const ReadProvider &ip, const std::string &name, PipelineResources &resources, const PipelineInfo &pipeline, bool silent = false);
PipelineProgram LoadPipelineProgramFromFile(const std::string &path, PipelineResources &resources, const PipelineInfo &pipeline, bool silent = false);
PipelineProgram LoadPipelineProgramFromAssets(const std::string &name, PipelineResources &resources, const PipelineInfo &pipeline, bool silent = false);

void Destroy(PipelineProgram &pipeline_program);

//
bool LoadPipelineProgramUniforms(const Reader &ir, const ReadProvider &ip, const std::string &name, std::vector<TextureUniform> &texs,
	std::vector<Vec4Uniform> &vecs, PipelineResources &resources, bool silent = false);
bool LoadPipelineProgramUniformsFromFile(
	const std::string &path, std::vector<TextureUniform> &texs, std::vector<Vec4Uniform> &vecs, PipelineResources &resources, bool silent = false);
bool LoadPipelineProgramUniformsFromAssets(
	const std::string &name, std::vector<TextureUniform> &texs, std::vector<Vec4Uniform> &vecs, PipelineResources &resources, bool silent = false);
#endif

//
struct DisplayList { // 4B
	size_t element_count;
	sg_buffer index_buffer;
	sg_buffer vertex_buffer;
	std::vector<uint16_t> bones_table;
};

/// Create an empty texture.
/// @see CreateTextureFromPicture and UpdateTextureFromPicture.
// Texture CreateTexture(int width, int height, const std::string &name, uint64_t flags, bgfx::TextureFormat::Enum format = bgfx::TextureFormat::RGBA8);
/// Create a texture from a picture.
/// @see Picture, CreateTexture and UpdateTextureFromPicture.
// Texture CreateTextureFromPicture(const Picture &pic, const std::string &name, uint64_t flags, bgfx::TextureFormat::Enum format = bgfx::TextureFormat::RGBA8);

void UpdateTextureFromPicture(Texture &tex, const Picture &pic);

uint64_t LoadTextureFlags(const Reader &ir, const ReadProvider &ip, const std::string &name, bool silent = false);
uint64_t LoadTextureFlagsFromFile(const std::string &path, bool silent = false);
uint64_t LoadTextureFlagsFromAssets(const std::string &name, bool silent = false);

struct TextureInfo {};

Texture LoadTexture(const Reader &ir, const ReadProvider &ip, const std::string &name, bool silent = false);
Texture LoadTextureFromFile(const std::string &path, bool silent = false);
Texture LoadTextureFromAssets(const std::string &name, bool silent = false);

void Destroy(Texture &texture);

struct Texture { // 8B
	sg_image image;
};

// inline Texture MakeTexture(bgfx::TextureHandle handle, uint64_t flags = BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE) { return {flags, handle}; }

//
struct UniformSetValue { // burn it with fire, complete insanity
	UniformSetValue() : count(1) {}
	UniformSetValue(const UniformSetValue &v);
	UniformSetValue &operator=(const UniformSetValue &v);
	~UniformSetValue();

	// bgfx::UniformHandle uniform = BGFX_INVALID_HANDLE;
	std::vector<float> value;
	uint16_t count;
};

UniformSetValue MakeUniformSetValue(const std::string &name, float v, uint16_t count = 1);
UniformSetValue MakeUniformSetValue(const std::string &name, const Vec2 &v, uint16_t count = 1);
UniformSetValue MakeUniformSetValue(const std::string &name, const Vec3 &v, uint16_t count = 1);
UniformSetValue MakeUniformSetValue(const std::string &name, const Vec4 &v, uint16_t count = 1);
UniformSetValue MakeUniformSetValue(const std::string &name, const Mat3 &mtx, uint16_t count = 1);
UniformSetValue MakeUniformSetValue(const std::string &name, const Mat4 &mtx, uint16_t count = 1);
UniformSetValue MakeUniformSetValue(const std::string &name, const Mat44 &mtx, uint16_t count = 1);

struct UniformSetTexture { // burn it with fire, complete insanity
	UniformSetTexture() : stage(0) {}
	UniformSetTexture(const UniformSetTexture &v);
	UniformSetTexture &operator=(const UniformSetTexture &v);
	~UniformSetTexture();

	// bgfx::UniformHandle uniform = BGFX_INVALID_HANDLE;
	Texture texture;
	uint8_t stage;
};

UniformSetTexture MakeUniformSetTexture(const std::string &name, const Texture &texture, uint8_t stage);

//
struct UniformData { // stored in material, links to Shader.uniforms
	UniformData() {
		for (size_t i = 0; i < SG_MAX_UB_MEMBERS; ++i) {
			offset[i] = 0;
		}
	}

	uint16_t offset[SG_MAX_UB_MEMBERS];
	std::vector<int8_t> data;
};

int GetUniformDataIndex(const std::string &name, const Shader &shader);

template <typename T> T GetUniformDataValue(const UniformData &data, const int index) {
	return *reinterpret_cast<T *>(&data.data[data.offset[index]]);
}

template <typename T> void SetUniformDataValue(UniformData &data, const int index, const T &value) {
	*reinterpret_cast<T *>(&data.data[data.offset[index]]) = value;
}

const void *GetUniformDataPtr(const UniformData &data);
size_t GetUniformDataSize(const UniformData &data);

void SetupShaderUniformData(const Shader &shader, UniformData &data);

//
static const int MF_EnableSkinning = 0x01;
static const int MF_DiffuseUV1 = 0x02;
static const int MF_SpecularUV1 = 0x04;
static const int MF_AmbientUV1 = 0x08;
static const int MF_NormalMapInWorldSpace = 0x10;
static const int MF_EnableAlphaCut = 0x20;

struct Material { // 56B
	Material() : variant_idx(0), flags(0) {}

	PipelineProgramRef program;
	uint32_t variant_idx;

	struct Value {
		Value() : type(UT_Vec4), count(1), idx(-1) {}

		UniformType type;
		std::vector<float> value;
		uint16_t count;
		int16_t idx;
	};

	std::map<std::string, Value> values;

	struct Texture {
		Texture() : channel(0), idx(-1) {}

		TextureRef texture;
		uint8_t channel;
		int8_t idx;
	};

	std::map<std::string, Texture> textures;

	RenderState state;
	uint8_t flags;

	UniformData uniform_data;
};

Material CreateMaterial(PipelineProgramRef prg);
Material CreateMaterial(PipelineProgramRef prg, const std::string &value_name, const Vec4 &value);
Material CreateMaterial(PipelineProgramRef prg, const std::string &value_name_0, const Vec4 &value_0, const std::string &value_name_1, const Vec4 &value_1);

void SetMaterialProgram(Material &mat, PipelineProgramRef prg);

void SetMaterialValue(Material &mat, const std::string &name, float v);
void SetMaterialValue(Material &mat, const std::string &name, const Vec2 &v);
void SetMaterialValue(Material &mat, const std::string &name, const Vec3 &v);
void SetMaterialValue(Material &mat, const std::string &name, const Vec4 &v);
void SetMaterialValue(Material &mat, const std::string &name, const Mat3 &m);
void SetMaterialValue(Material &mat, const std::string &name, const Mat4 &m);
void SetMaterialValue(Material &mat, const std::string &name, const Mat44 &m);

void SetMaterialTexture(Material &mat, const std::string &name, TextureRef tex, uint8_t stage);
bool SetMaterialTextureRef(Material &mat, const std::string &name, TextureRef tex);

TextureRef GetMaterialTexture(Material &mat, const std::string &name);
std::vector<std::string> GetMaterialTextures(Material &mat);

std::vector<std::string> GetMaterialValues(Material &mat);

enum FaceCulling { FC_Disabled, FC_Clockwise, FC_CounterClockwise };

FaceCulling GetMaterialFaceCulling(const Material &mat);
void SetMaterialFaceCulling(Material &mat, FaceCulling mode);

enum DepthTest { DT_Less, DT_LessEqual, DT_Equal, DT_GreaterEqual, DT_Greater, DT_NotEqual, DT_Never, DT_Always, DT_Disabled };

DepthTest GetMaterialDepthTest(const Material &mat);
void SetMaterialDepthTest(Material &mat, DepthTest test);

/// Control the compositing mode used to draw primitives.
enum BlendMode { BM_Additive, BM_Alpha, BM_Darken, BM_Lighten, BM_Multiply, BM_Opaque, BM_Screen, BM_LinearBurn, BM_Undefined };

BlendMode GetMaterialBlendMode(const Material &mat);
void SetMaterialBlendMode(Material &mat, BlendMode mode);

void GetMaterialWriteRGBA(const Material &m_, bool &write_r, bool &write_g, bool &write_b, bool &write_a);
void SetMaterialWriteRGBA(Material &m_, bool write_r, bool write_g, bool write_b, bool write_a);

bool GetMaterialWriteZ(const Material &m);
void SetMaterialWriteZ(Material &m, bool enable);

bool GetMaterialNormalMapInWorldSpace(const Material &m);
void SetMaterialNormalMapInWorldSpace(Material &m, bool enable);

bool GetMaterialDiffuseUsesUV1(const Material &m);
void SetMaterialDiffuseUsesUV1(Material &m, bool enable);
bool GetMaterialSpecularUsesUV1(const Material &m);
void SetMaterialSpecularUsesUV1(Material &m, bool enable);
bool GetMaterialAmbientUsesUV1(const Material &m);
void SetMaterialAmbientUsesUV1(Material &m, bool enable);

bool GetMaterialSkinning(const Material &m);
void SetMaterialSkinning(Material &m, bool enable);
bool GetMaterialAlphaCut(const Material &m);
void SetMaterialAlphaCut(Material &m, bool enable);

/// Compute a render state to control subsequent render calls culling mode, blending mode, Z mask, etc... The same render state can be used by different render
/// calls.
/// @see DrawLines, DrawTriangles and DrawModel.
RenderState ComputeRenderState(BlendMode blend, bool write_z, bool write_r = true, bool write_g = true, bool write_b = true, bool write_a = true);
RenderState ComputeRenderState(BlendMode blend, DepthTest test = DT_Less, FaceCulling culling = FC_Clockwise, bool write_z = true, bool write_r = true,
	bool write_g = true, bool write_b = true, bool write_a = true);

//
struct Model { // 96B (+heap)
	Model() : tri_count(0) {}

	uint32_t tri_count;
	VertexLayout vtx_layout;

	std::vector<DisplayList> lists;
	std::vector<MinMax> bounds; // minmax/list
	std::vector<uint16_t> mats; // material/list
	std::vector<Mat4> bind_pose; // bind pose matrices
};

Model LoadModel(const Reader &ir, const Handle &h, const std::string &name, bool silent = false);
Model LoadModelFromFile(const std::string &path, bool silent = false);
Model LoadModelFromAssets(const std::string &name, bool silent = false);

size_t GetModelMaterialCount(const Model &model);

//
void Destroy(Model &model);
void Destroy(Material &material);
void Destroy(PipelineProgram &pipeline_shader);

//
struct TextureLoad {
	Reader ir;
	ReadProvider ip;
	TextureRef ref;
};

struct ModelLoad {
	Reader ir;
	ReadProvider ip;
	ModelRef ref;
};

struct PipelineResources {
	PipelineResources() : programs(Destroy), textures(Destroy), materials(Destroy), models(Destroy) {}
	~PipelineResources() {
		DestroyAll();
	}

	ResourceCache<PipelineProgram> programs;
	ResourceCache<Texture> textures;
	ResourceCache<Material> materials;
	ResourceCache<Model> models;

	std::deque<TextureLoad> texture_loads;
	std::deque<ModelLoad> model_loads;

	void DestroyAll();
};

//
size_t ProcessTextureLoadQueue(PipelineResources &resources, time_ns t_budget = time_from_ms(4), bool silent = false);

TextureRef QueueLoadTexture(const Reader &ir, const ReadProvider &ip, const std::string &name, uint64_t flags, PipelineResources &resources);
TextureRef QueueLoadTextureFromFile(const std::string &path, uint64_t flags, PipelineResources &resources);
TextureRef QueueLoadTextureFromAssets(const std::string &name, uint64_t flags, PipelineResources &resources);

TextureRef SkipLoadOrQueueTextureLoad(
	const Reader &ir, const ReadProvider &ip, const std::string &path, PipelineResources &resources, bool queue_load, bool do_not_load, bool silent = false);

//
size_t ProcessModelLoadQueue(PipelineResources &resources, time_ns t_budget = time_from_ms(4), bool silent = false);
ModelRef QueueLoadModel(const Reader &ir, const ReadProvider &ip, const std::string &name, PipelineResources &resources);

ModelRef QueueLoadModelFromFile(const std::string &path, PipelineResources &resources);
ModelRef QueueLoadModelFromAssets(const std::string &name, PipelineResources &resources);

ModelRef SkipLoadOrQueueModelLoad(
	const Reader &ir, const ReadProvider &ip, const std::string &path, PipelineResources &resources, bool queue_load, bool do_not_load, bool silent = false);

//
Material LoadMaterial(const rapidjson::Value &js, const Reader &deps_ir, const ReadProvider &deps_ip, PipelineResources &resources,
	const PipelineInfo &pipeline, bool queue_texture_loads, bool do_not_load_resources, bool silent = false);
Material LoadMaterial(const Reader &ir, const Handle &h, const Reader &deps_ir, const ReadProvider &deps_ip, PipelineResources &resources,
	const PipelineInfo &pipeline, bool queue_texture_loads, bool do_not_load_resources, bool silent = false);
Material LoadMaterialFromFile(const std::string &path, PipelineResources &resources, const PipelineInfo &pipeline, bool queue_texture_loads,
	bool do_not_load_resources, bool silent = false);
Material LoadMaterialFromAssets(const std::string &path, PipelineResources &resources, const PipelineInfo &pipeline, bool queue_texture_loads,
	bool do_not_load_resources, bool silent = false);

bool SaveMaterial(rapidjson::Document &jd, rapidjson::Value &js, const Material &mat, const PipelineResources &resources);
bool SaveMaterial(const Material &mat, const Writer &iw, const Handle &h, const PipelineResources &resources);
bool SaveMaterialToFile(const std::string &path, const Material &m, const PipelineResources &resources);

//
PipelineProgramRef LoadPipelineProgramRef(
	const Reader &ir, const ReadProvider &ip, const std::string &name, PipelineResources &resources, const PipelineInfo &pipeline, bool silent = false);
PipelineProgramRef LoadPipelineProgramRefFromFile(const std::string &path, PipelineResources &resources, const PipelineInfo &pipeline, bool silent = false);
PipelineProgramRef LoadPipelineProgramRefFromAssets(const std::string &name, PipelineResources &resources, const PipelineInfo &pipeline, bool silent = false);

ModelRef LoadModel(const Reader &ir, const ReadProvider &ip, const std::string &path, PipelineResources &resources, bool silent = false);
ModelRef LoadModelFromFile(const std::string &path, PipelineResources &resources, bool silent = false);
ModelRef LoadModelFromAssets(const std::string &path, PipelineResources &resources, bool silent = false);

/*
struct TextureMeta {
	TextureMeta() : flags(0) {}
	uint64_t flags;
};

TextureMeta LoadTextureMeta(const Reader &ir, const ReadProvider &ip, const std::string &name, bool silent = false);
TextureMeta LoadTextureMetaFromFile(const std::string &path, bool silent = false);
TextureMeta LoadTextureMetaFromAssets(const std::string &name, bool silent = false);
*/

TextureRef LoadTexture(const Reader &ir, const ReadProvider &ip, const std::string &path, PipelineResources &resources, bool silent = false);
TextureRef LoadTextureFromFile(const std::string &path, PipelineResources &resources, bool silent = false);
TextureRef LoadTextureFromAssets(const std::string &path, PipelineResources &resources, bool silent = false);

/// Capture a texture content to a Picture. Return the frame counter at which the capture will be complete.
/// A Picture object can be accessed by the CPU.
/// This function is asynchronous and its result will not be available until the returned frame counter is equal or greater to the frame counter returned by
/// Frame.
uint32_t CaptureTexture(const PipelineResources &resources, const TextureRef &t, Picture &pic);

MaterialRef LoadMaterialRef(const Reader &ir, const Handle &h, const std::string &path, const Reader &deps_ir, const ReadProvider &deps_ip,
	PipelineResources &resources, const PipelineInfo &pipeline, bool queue_texture_loads, bool do_not_load_resources, bool silent = false);
MaterialRef LoadMaterialRefFromFile(const std::string &path, PipelineResources &resources, const PipelineInfo &pipeline, bool queue_texture_loads,
	bool do_not_load_resources, bool silent = false);
MaterialRef LoadMaterialRefFromAssets(const std::string &path, PipelineResources &resources, const PipelineInfo &pipeline, bool queue_texture_loads,
	bool do_not_load_resources, bool silent = false);

//
size_t GetQueuedResourceCount(const PipelineResources &res);
size_t ProcessLoadQueues(PipelineResources &res, time_ns t_budget = time_from_ms(4), bool silent = false);

//
std::vector<int> GetMaterialPipelineProgramFeatureStates(const Material &mat, const std::vector<PipelineProgramFeature> &features);
std::string GetPipelineProgramVariantName(const std::string &name, const std::vector<PipelineProgramFeature> &features, const std::vector<int> &states);

int GetPipelineProgramFeatureStateCount(PipelineProgramFeature feat);

int GetPipelineProgramVariantCount(const std::vector<PipelineProgramFeature> &feats);
int GetPipelineProgramVariantIndex(const std::vector<PipelineProgramFeature> &feats, const std::vector<int> &states);

void UpdateMaterialPipelineProgramVariant(Material &mat, const PipelineResources &resources);

void CreateMissingMaterialProgramValues(Material &mat, PipelineResources &resources, const Reader &ir, const ReadProvider &ip);
void CreateMissingMaterialProgramValuesFromFile(Material &mat, PipelineResources &resources);
void CreateMissingMaterialProgramValuesFromAssets(Material &mat, PipelineResources &resources);

/// Compute a sorting key to control the rendering order of a display list, `view_depth` is expected in view space.
uint32_t ComputeSortKey(float view_depth);

/// Compute a sorting key to control the rendering order of a display list.
uint32_t ComputeSortKeyFromWorld(const Vec3 &T, const Mat4 &view);
uint32_t ComputeSortKeyFromWorld(const Vec3 &T, const Mat4 &view, const Mat4 &model);

// FIXSOKOL
// void DrawDisplayList(bgfx::ViewId view_id, bgfx::IndexBufferHandle idx, bgfx::VertexBufferHandle vtx, bgfx::ProgramHandle prg,
//	const std::vector<UniformSetValue> &values = {}, const std::vector<UniformSetTexture> &textures = {}, RenderState state = {}, uint32_t depth = 0);

/// Draw a model to the specified view.
/// @see UniformSetValueList and UniformSetTextureList to pass uniform values to the shader program.
// FIXSOKOL
// void DrawModel(bgfx::ViewId view_id, const Model &mdl, bgfx::ProgramHandle prg, const std::vector<UniformSetValue> &values,
//	const std::vector<UniformSetTexture> &textures, const Mat4 *mtxs, size_t mtx_count = 1, RenderState state = {}, uint32_t depth = 0);

//
struct ModelDisplayList { // 16B
	const Material *mat; // 8
	uint32_t mtx_idx; // 4
	uint16_t mdl_idx; // 2
	uint16_t lst_idx; // 2
};

void CullModelDisplayLists(const Frustum &frustum, std::vector<ModelDisplayList> &display_lists, const std::vector<Mat4> &mtxs, const PipelineResources &res);

/*
void DrawModelDisplayLists(bgfx::ViewId view_id, const std::vector<ModelDisplayList> &display_lists, uint8_t pipeline_config_idx,
	const std::vector<UniformSetValue> &values, const std::vector<UniformSetTexture> &textures, const std::vector<Mat4> &mtxs, const PipelineResources &res);
void DrawModelDisplayLists(bgfx::ViewId view_id, const std::vector<ModelDisplayList> &display_lists, const std::vector<uint32_t> &depths,
	uint8_t pipeline_config_idx, const std::vector<UniformSetValue> &values, const std::vector<UniformSetTexture> &textures, const std::vector<Mat4> &mtxs,
	const PipelineResources &res);
void DrawModelDisplayLists(bgfx::ViewId view_id, const std::vector<ModelDisplayList> &display_lists, uint8_t pipeline_config_idx,
	const std::vector<UniformSetValue> &values, const std::vector<UniformSetTexture> &textures, const std::vector<Mat4> &mtxs,
	const std::vector<Mat4> &prv_mtxs, const PipelineResources &res);
*/

//
struct SkinnedModelDisplayList { // 782B
	const Material *mat; // 8
	uint32_t mtx_idxs[max_skinned_model_matrix_count]; // 384
	uint32_t bones_idxs[max_skinned_model_matrix_count]; // 384
	uint16_t bone_count; // 2
	uint16_t mdl_idx; // 2
	uint16_t lst_idx; // 2
};

/*
void DrawSkinnedModelDisplayLists(bgfx::ViewId view_id, const std::vector<SkinnedModelDisplayList> &display_lists, uint8_t pipeline_config_idx,
	const std::vector<UniformSetValue> &values, const std::vector<UniformSetTexture> &textures, const std::vector<Mat4> &mtxs, const PipelineResources &res);
void DrawSkinnedModelDisplayLists(bgfx::ViewId view_id, const std::vector<SkinnedModelDisplayList> &display_lists, const std::vector<uint32_t> &depths,
	uint8_t pipeline_config_idx, const std::vector<UniformSetValue> &values, const std::vector<UniformSetTexture> &textures, const std::vector<Mat4> &mtxs,
	const PipelineResources &res);
void DrawSkinnedModelDisplayLists(bgfx::ViewId view_id, const std::vector<SkinnedModelDisplayList> &display_lists, uint8_t pipeline_config_idx,
	const std::vector<UniformSetValue> &values, const std::vector<UniformSetTexture> &textures, const std::vector<Mat4> &mtxs,
	const std::vector<Mat4> &prv_mtxs, const PipelineResources &res);
*/

//
typedef std::vector<uint16_t> Indices;

struct Vertices {
	Vertices(const VertexLayout &layout, size_t count);

	const VertexLayout &GetDecl() const {
		return layout;
	}

	Vertices &Begin(size_t i);
	Vertices &SetPos(const Vec3 &pos);
	Vertices &SetNormal(const Vec3 &normal);
	Vertices &SetTangent(const Vec3 &tangent);
	Vertices &SetBinormal(const Vec3 &binormal);
	Vertices &SetTexCoord0(const Vec2 &uv);
	Vertices &SetTexCoord1(const Vec2 &uv);
	Vertices &SetTexCoord2(const Vec2 &uv);
	Vertices &SetTexCoord3(const Vec2 &uv);
	Vertices &SetTexCoord4(const Vec2 &uv);
	Vertices &SetTexCoord5(const Vec2 &uv);
	Vertices &SetTexCoord6(const Vec2 &uv);
	Vertices &SetTexCoord7(const Vec2 &uv);
	Vertices &SetColor0(const Color &color);
	Vertices &SetColor1(const Color &color);
	Vertices &SetColor2(const Color &color);
	Vertices &SetColor3(const Color &color);
	void End(bool validate = false);

	void Clear();

	void Reserve(size_t count);
	void Resize(size_t count);

	const void *GetData() const {
		return data.data();
	}

	size_t GetSize() const {
		return data.size();
	}

	size_t GetCount() const {
		return data.size() / layout.GetStride();
	}

	size_t GetCapacity() const {
		return data.capacity() / layout.GetStride();
	}

private:
	VertexLayout layout;
	std::vector<char> data;

	int idx;
	uint32_t vtx_attr_flag;
};

//
void SetTransform(const Mat4 &world);
void SetUniforms(const std::vector<UniformSetValue> &values, const std::vector<UniformSetTexture> &textures);

// FIXSOKOL
#if 0
/// Draw a list of lines to the specified view.
/// @see UniformSetValueList and UniformSetTextureList to pass uniform values to the shader program.
void DrawLines(bgfx::ViewId view_id, const Vertices &vtx, bgfx::ProgramHandle prg, RenderState = {}, uint32_t depth = 0);
void DrawLines(bgfx::ViewId view_id, const Vertices &vtx, bgfx::ProgramHandle prg, const std::vector<UniformSetValue> &values,
	const std::vector<UniformSetTexture> &textures, RenderState = {}, uint32_t depth = 0);
void DrawLines(bgfx::ViewId view_id, const Indices &idx, const Vertices &vtx, bgfx::ProgramHandle prg, const std::vector<UniformSetValue> &values,
	const std::vector<UniformSetTexture> &textures, RenderState = {}, uint32_t depth = 0);
/*!
	Draw a list of sprites to the specified view.
	@see UniformSetValueList and UniformSetTextureList to pass uniform values to the shader program.
	@note This function prepares the sprite on the CPU before submitting them all to the GPU as a single draw call.
*/
void DrawSprites(bgfx::ViewId view_id, const Mat3 &inv_view_R, bgfx::VertexLayout &decl, const std::vector<Vec3> &pos, const Vec2 &size,
	bgfx::ProgramHandle prg, RenderState state = {}, uint32_t depth = 0);

/// Draw a list of triangles to the specified view.
/// @see UniformSetValueList and UniformSetTextureList to pass uniform values to the shader program.
void DrawTriangles(bgfx::ViewId view_id, const Vertices &vtx, bgfx::ProgramHandle prg, RenderState = {}, uint32_t depth = 0);
void DrawTriangles(bgfx::ViewId view_id, const Vertices &vtx, bgfx::ProgramHandle prg, const std::vector<UniformSetValue> &values,
	const std::vector<UniformSetTexture> &textures, RenderState = {}, uint32_t depth = 0);
void DrawTriangles(bgfx::ViewId view_id, const Indices &idx, const Vertices &vtx, bgfx::ProgramHandle prg, const std::vector<UniformSetValue> &values,
	const std::vector<UniformSetTexture> &textures, RenderState = {}, uint32_t depth = 0);
void DrawSprites(bgfx::ViewId view_id, const Mat3 &inv_view_R, bgfx::VertexLayout &decl, const std::vector<Vec3> &pos, const Vec2 &size,
	bgfx::ProgramHandle prg, const std::vector<UniformSetValue> &values, const std::vector<UniformSetTexture> &textures, RenderState state = {},
	uint32_t depth = 0);
#endif

//
#if 0
struct Pipeline {
	std::map<std::string, bgfx::TextureHandle> textures;
	std::map<std::string, bgfx::FrameBufferHandle> framebuffers;

	std::vector<UniformSetValue> uniform_values;
	std::vector<UniformSetTexture> uniform_textures;
};

void DestroyPipeline(Pipeline &pipeline);

//
struct FrameBuffer {
	bgfx::FrameBufferHandle handle = BGFX_INVALID_HANDLE;
};

FrameBuffer CreateFrameBuffer(const Texture &color, const Texture &depth, const std::string &name);
FrameBuffer CreateFrameBuffer(bgfx::TextureFormat::Enum color_format, bgfx::TextureFormat::Enum depth_format, int aa, const std::string &name);
FrameBuffer CreateFrameBuffer(
	int width, int height, bgfx::TextureFormat::Enum color_format, bgfx::TextureFormat::Enum depth_format, int aa, const std::string &name);

Texture GetColorTexture(FrameBuffer &frameBuffer);
Texture GetDepthTexture(FrameBuffer &frameBuffer);

/// Destroy a frame buffer and its resources.
void DestroyFrameBuffer(FrameBuffer &frameBuffer);

bool CreateFullscreenQuad(bgfx::TransientIndexBuffer &idx, bgfx::TransientVertexBuffer &vtx);

bimg::ImageContainer *LoadImage(const Reader &ir, const ReadProvider &ip, const std::string &name);
bimg::ImageContainer *LoadImageFromFile(const std::string &name);
bimg::ImageContainer *LoadImageFromAssets(const std::string &name);
void UpdateTextureFromImage(Texture &tex, bimg::ImageContainer *img, bool auto_delete = true);
#endif

#endif

} // namespace hg
