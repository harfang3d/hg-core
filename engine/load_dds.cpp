// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "engine/load_dds.h"

#include "foundation/log.h"
#include "foundation/math.h"
#include "foundation/profiler.h"

#include <fmt/format.h>
#include <vector>

namespace hg {

#ifndef D3DFMT_A16B16G16R16
#	define D3DFMT_A16B16G16R16 36
#endif
#ifndef D3DFMT_Q16W16V16U16
#	define D3DFMT_Q16W16V16U16 110
#endif
#ifndef D3DFMT_R16F
#	define D3DFMT_R16F 111
#endif
#ifndef D3DFMT_G16R16F
#	define D3DFMT_G16R16F 112
#endif
#ifndef D3DFMT_A16B16G16R16F
#	define D3DFMT_A16B16G16R16F 113
#endif
#ifndef D3DFMT_R32F
#	define D3DFMT_R32F 114
#endif
#ifndef D3DFMT_G32R32F
#	define D3DFMT_G32R32F 115
#endif
#ifndef D3DFMT_A32B32G32R32F 
#	define D3DFMT_A32B32G32R32F 116
#endif

// from ddraw.h
#define DDSD_CAPS 0x00000001L // default
#define DDSD_HEIGHT 0x00000002L
#define DDSD_WIDTH 0x00000004L
#define DDSD_PITCH 0x00000008L
#define DDSD_PIXELFORMAT 0x00001000L
#define DDSD_LINEARSIZE 0x00080000L

#define DDSD_VOLUME 0x00800000L

#define DDSD_CUBEMAP 0x00000200L

#define DDPF_ALPHAPIXELS 0x00000001L
#define DDPF_FOURCC 0x00000004L
#define DDPF_RGB 0x00000040L


struct DDPIXELFORMAT {
	uint32_t dwSize;
	uint32_t dwFlags;
	uint32_t dwFourCC;

	uint32_t dwRGBBitCount;

	uint32_t dwRBitMask;
	uint32_t dwGBitMask;
	uint32_t dwBBitMask;
	uint32_t dwABitMask;
}
#ifdef __GNUG__
__attribute__((packed))
#endif
;

struct DDSCAPS2 {
	uint32_t dwCaps1;
	uint32_t dwCaps2;
	uint32_t Reserved[2];
}
#ifdef __GNUG__
__attribute__((packed))
#endif
;

struct DDSURFACEDESC2 {
	uint32_t dwSize;
	uint32_t dwFlags;
	uint32_t dwHeight;
	uint32_t dwWidth;
	uint32_t dwPitchOrLinearSize;
	uint32_t dwDepth;
	uint32_t dwMipMapCount;
	uint32_t dwReserved1[11];
	DDPIXELFORMAT ddpfPixelFormat;
	DDSCAPS2 ddsCaps;
	uint32_t dwReserved2;
}
#ifdef __GNUG__
__attribute__((packed))
#endif
;

enum DXGI_FORMAT {
	DXGI_FORMAT_UNKNOWN = 0,
	DXGI_FORMAT_R32G32B32A32_TYPELESS = 1,
	DXGI_FORMAT_R32G32B32A32_FLOAT = 2,
	DXGI_FORMAT_R32G32B32A32_UINT = 3,
	DXGI_FORMAT_R32G32B32A32_SINT = 4,
	DXGI_FORMAT_R32G32B32_TYPELESS = 5,
	DXGI_FORMAT_R32G32B32_FLOAT = 6,
	DXGI_FORMAT_R32G32B32_UINT = 7,
	DXGI_FORMAT_R32G32B32_SINT = 8,
	DXGI_FORMAT_R16G16B16A16_TYPELESS = 9,
	DXGI_FORMAT_R16G16B16A16_FLOAT = 10,
	DXGI_FORMAT_R16G16B16A16_UNORM = 11,
	DXGI_FORMAT_R16G16B16A16_UINT = 12,
	DXGI_FORMAT_R16G16B16A16_SNORM = 13,
	DXGI_FORMAT_R16G16B16A16_SINT = 14,
	DXGI_FORMAT_R32G32_TYPELESS = 15,
	DXGI_FORMAT_R32G32_FLOAT = 16,
	DXGI_FORMAT_R32G32_UINT = 17,
	DXGI_FORMAT_R32G32_SINT = 18,
	DXGI_FORMAT_R32G8X24_TYPELESS = 19,
	DXGI_FORMAT_D32_FLOAT_S8X24_UINT = 20,
	DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
	DXGI_FORMAT_X32_TYPELESS_G8X24_UINT = 22,
	DXGI_FORMAT_R10G10B10A2_TYPELESS = 23,
	DXGI_FORMAT_R10G10B10A2_UNORM = 24,
	DXGI_FORMAT_R10G10B10A2_UINT = 25,
	DXGI_FORMAT_R11G11B10_FLOAT = 26,
	DXGI_FORMAT_R8G8B8A8_TYPELESS = 27,
	DXGI_FORMAT_R8G8B8A8_UNORM = 28,
	DXGI_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
	DXGI_FORMAT_R8G8B8A8_UINT = 30,
	DXGI_FORMAT_R8G8B8A8_SNORM = 31,
	DXGI_FORMAT_R8G8B8A8_SINT = 32,
	DXGI_FORMAT_R16G16_TYPELESS = 33,
	DXGI_FORMAT_R16G16_FLOAT = 34,
	DXGI_FORMAT_R16G16_UNORM = 35,
	DXGI_FORMAT_R16G16_UINT = 36,
	DXGI_FORMAT_R16G16_SNORM = 37,
	DXGI_FORMAT_R16G16_SINT = 38,
	DXGI_FORMAT_R32_TYPELESS = 39,
	DXGI_FORMAT_D32_FLOAT = 40,
	DXGI_FORMAT_R32_FLOAT = 41,
	DXGI_FORMAT_R32_UINT = 42,
	DXGI_FORMAT_R32_SINT = 43,
	DXGI_FORMAT_R24G8_TYPELESS = 44,
	DXGI_FORMAT_D24_UNORM_S8_UINT = 45,
	DXGI_FORMAT_R24_UNORM_X8_TYPELESS = 46,
	DXGI_FORMAT_X24_TYPELESS_G8_UINT = 47,
	DXGI_FORMAT_R8G8_TYPELESS = 48,
	DXGI_FORMAT_R8G8_UNORM = 49,
	DXGI_FORMAT_R8G8_UINT = 50,
	DXGI_FORMAT_R8G8_SNORM = 51,
	DXGI_FORMAT_R8G8_SINT = 52,
	DXGI_FORMAT_R16_TYPELESS = 53,
	DXGI_FORMAT_R16_FLOAT = 54,
	DXGI_FORMAT_D16_UNORM = 55,
	DXGI_FORMAT_R16_UNORM = 56,
	DXGI_FORMAT_R16_UINT = 57,
	DXGI_FORMAT_R16_SNORM = 58,
	DXGI_FORMAT_R16_SINT = 59,
	DXGI_FORMAT_R8_TYPELESS = 60,
	DXGI_FORMAT_R8_UNORM = 61,
	DXGI_FORMAT_R8_UINT = 62,
	DXGI_FORMAT_R8_SNORM = 63,
	DXGI_FORMAT_R8_SINT = 64,
	DXGI_FORMAT_A8_UNORM = 65,
	DXGI_FORMAT_R1_UNORM = 66,
	DXGI_FORMAT_R9G9B9E5_SHAREDEXP = 67,
	DXGI_FORMAT_R8G8_B8G8_UNORM = 68,
	DXGI_FORMAT_G8R8_G8B8_UNORM = 69,
	DXGI_FORMAT_BC1_TYPELESS = 70,
	DXGI_FORMAT_BC1_UNORM = 71,
	DXGI_FORMAT_BC1_UNORM_SRGB = 72,
	DXGI_FORMAT_BC2_TYPELESS = 73,
	DXGI_FORMAT_BC2_UNORM = 74,
	DXGI_FORMAT_BC2_UNORM_SRGB = 75,
	DXGI_FORMAT_BC3_TYPELESS = 76,
	DXGI_FORMAT_BC3_UNORM = 77,
	DXGI_FORMAT_BC3_UNORM_SRGB = 78,
	DXGI_FORMAT_BC4_TYPELESS = 79,
	DXGI_FORMAT_BC4_UNORM = 80,
	DXGI_FORMAT_BC4_SNORM = 81,
	DXGI_FORMAT_BC5_TYPELESS = 82,
	DXGI_FORMAT_BC5_UNORM = 83,
	DXGI_FORMAT_BC5_SNORM = 84,
	DXGI_FORMAT_B5G6R5_UNORM = 85,
	DXGI_FORMAT_B5G5R5A1_UNORM = 86,
	DXGI_FORMAT_B8G8R8A8_UNORM = 87,
	DXGI_FORMAT_B8G8R8X8_UNORM = 88,
	DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89,
	DXGI_FORMAT_B8G8R8A8_TYPELESS = 90,
	DXGI_FORMAT_B8G8R8A8_UNORM_SRGB = 91,
	DXGI_FORMAT_B8G8R8X8_TYPELESS = 92,
	DXGI_FORMAT_B8G8R8X8_UNORM_SRGB = 93,
	DXGI_FORMAT_BC6H_TYPELESS = 94,
	DXGI_FORMAT_BC6H_UF16 = 95,
	DXGI_FORMAT_BC6H_SF16 = 96,
	DXGI_FORMAT_BC7_TYPELESS = 97,
	DXGI_FORMAT_BC7_UNORM = 98,
	DXGI_FORMAT_BC7_UNORM_SRGB = 99,
	DXGI_FORMAT_AYUV = 100,
	DXGI_FORMAT_Y410 = 101,
	DXGI_FORMAT_Y416 = 102,
	DXGI_FORMAT_NV12 = 103,
	DXGI_FORMAT_P010 = 104,
	DXGI_FORMAT_P016 = 105,
	DXGI_FORMAT_420_OPAQUE = 106,
	DXGI_FORMAT_YUY2 = 107,
	DXGI_FORMAT_Y210 = 108,
	DXGI_FORMAT_Y216 = 109,
	DXGI_FORMAT_NV11 = 110,
	DXGI_FORMAT_AI44 = 111,
	DXGI_FORMAT_IA44 = 112,
	DXGI_FORMAT_P8 = 113,
	DXGI_FORMAT_A8P8 = 114,
	DXGI_FORMAT_B4G4R4A4_UNORM = 115,
	DXGI_FORMAT_P208 = 130,
	DXGI_FORMAT_V208 = 131,
	DXGI_FORMAT_V408 = 132
};

struct DDSHEADERDXT10 {
	uint32_t dxgiFormat;
	uint32_t resourceDimension;
	uint32_t miscFlag;
	uint32_t arraySize;
	uint32_t miscFlags2;
} 
#ifdef __GNUG__
__attribute__((packed))
#endif
;

#ifndef MAKEFOURCC
#	define MAKEFOURCC(a,b,c,d) \
(static_cast<uint32_t>((a)) | (static_cast<uint32_t>((b)) << 8) | (static_cast<uint32_t>((c)) << 16) | (static_cast<uint32_t>((d)) << 24))
#endif // MAKEFOURCC

#define FOURCC_DX10 MAKEFOURCC('D','X','1','0')

#define FOURCC_ETC2_RGB8   MAKEFOURCC('E', 'T', 'C', '2')
#define FOURCC_ETC2_RGBA8  MAKEFOURCC('E', 'T', 'C', 'A')
#define FOURCC_ETC2_RGB8A1 MAKEFOURCC('E', 'T', 'C', 'P')
#define FOURCC_ETC2_RG11   MAKEFOURCC('E', 'A', 'C', '2')

#define FOURCC_DXT1 MAKEFOURCC('D', 'X', 'T', '1')
#define FOURCC_DXT2 MAKEFOURCC('D', 'X', 'T', '2')
#define FOURCC_DXT3 MAKEFOURCC('D', 'X', 'T', '3')
#define FOURCC_DXT4 MAKEFOURCC('D', 'X', 'T', '4')
#define FOURCC_DXT5 MAKEFOURCC('D', 'X', 'T', '5')

#define FOURCC_BC1  MAKEFOURCC('B', 'C', '1', ' ')
#define FOURCC_BC2  MAKEFOURCC('B', 'C', '2', ' ')
#define FOURCC_BC3  MAKEFOURCC('B', 'C', '3', ' ')
#define FOURCC_BC4  MAKEFOURCC('B', 'C', '4', ' ')
#define FOURCC_BC4U MAKEFOURCC('B', 'C', '4', 'U')
#define FOURCC_BC4S MAKEFOURCC('B', 'C', '4', 'S')
#define FOURCC_BC5  MAKEFOURCC('B', 'C', '5', ' ')
#define FOURCC_BC5U MAKEFOURCC('B', 'C', '5', 'U')
#define FOURCC_BC5S MAKEFOURCC('B', 'C', '5', 'S')

// Converts FourCC code to SOKOL pixel format.
static inline sg_pixel_format FromFourCC(uint32_t fourcc) {
	sg_pixel_format fmt;
	switch (fourcc) { 
		case FOURCC_ETC2_RGB8:
			fmt = SG_PIXELFORMAT_ETC2_RGB8;
			break;
		case FOURCC_BC1:
		case FOURCC_DXT1:
			fmt = SG_PIXELFORMAT_BC1_RGBA;
			break;
		case FOURCC_BC2:
		case FOURCC_DXT2:
		case FOURCC_DXT3:
			fmt = SG_PIXELFORMAT_BC2_RGBA;
			break;
		case FOURCC_BC3:
		case FOURCC_DXT4:
		case FOURCC_DXT5:
			fmt = SG_PIXELFORMAT_BC3_RGBA;
			break;
		case FOURCC_BC4:
		case FOURCC_BC4U:
			fmt = SG_PIXELFORMAT_BC4_R;
			break;
		case FOURCC_BC4S:
			fmt = SG_PIXELFORMAT_BC4_RSN;
			break;
		case FOURCC_BC5:
		case FOURCC_BC5U:
			fmt = SG_PIXELFORMAT_BC5_RG;
			break;
		case FOURCC_BC5S:
			fmt = SG_PIXELFORMAT_BC5_RGSN;
			break;
		case D3DFMT_A16B16G16R16:
			fmt = SG_PIXELFORMAT_RGBA16;
			break;
		case D3DFMT_Q16W16V16U16:
			fmt = SG_PIXELFORMAT_RGBA16SN;
			break;
		case D3DFMT_R16F:
			fmt = SG_PIXELFORMAT_R16F;
			break;
		case D3DFMT_G16R16F:
			fmt = SG_PIXELFORMAT_RG16F;
			break;
		case D3DFMT_A16B16G16R16F:
			fmt = SG_PIXELFORMAT_RGBA16F;
			break;
		case D3DFMT_R32F:
			fmt = SG_PIXELFORMAT_R32F;
			break;
		case D3DFMT_G32R32F:
			fmt = SG_PIXELFORMAT_RG32F;
			break;
		case D3DFMT_A32B32G32R32F:
			fmt = SG_PIXELFORMAT_RGBA32F;
			break;
		default:
			fmt = SG_PIXELFORMAT_NONE;
			break;
	}
	return fmt;
}

// Converts DXGI to SOKOL pixel format.
static inline sg_pixel_format FromDXGIPixelFormat(uint32_t in) {
	sg_pixel_format fmt;
	switch (in) {
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
			fmt = SG_PIXELFORMAT_RGBA32F;
			break;
		case DXGI_FORMAT_R32G32B32A32_UINT:
			fmt = SG_PIXELFORMAT_RGBA32UI;
			break;
		case DXGI_FORMAT_R32G32B32A32_SINT:
			fmt = SG_PIXELFORMAT_RGBA32SI;
			break;
		case DXGI_FORMAT_R16G16B16A16_TYPELESS:
			fmt = SG_PIXELFORMAT_RGBA16;
			break;
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
			fmt = SG_PIXELFORMAT_RGBA16F;
			break;
		case DXGI_FORMAT_R16G16B16A16_UINT:
			fmt = SG_PIXELFORMAT_RGBA16UI;
			break;
		case DXGI_FORMAT_R16G16B16A16_SNORM:
			fmt = SG_PIXELFORMAT_RGBA16SN;
			break;
		case DXGI_FORMAT_R16G16B16A16_SINT:
			fmt = SG_PIXELFORMAT_RGBA16SI;
			break;
		case DXGI_FORMAT_R32G32_FLOAT:
			fmt = SG_PIXELFORMAT_RG32F;
			break;
		case DXGI_FORMAT_R32G32_UINT:
			fmt = SG_PIXELFORMAT_RG32UI;
			break;
		case DXGI_FORMAT_R32G32_SINT:
			fmt = SG_PIXELFORMAT_RG32SI;
			break;
		case DXGI_FORMAT_R10G10B10A2_TYPELESS:
			fmt = SG_PIXELFORMAT_RGB10A2;
			break;
		case DXGI_FORMAT_R11G11B10_FLOAT:
			fmt = SG_PIXELFORMAT_RG11B10F;
			break;
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:
			fmt = SG_PIXELFORMAT_RGBA8;
			break;
		case DXGI_FORMAT_R8G8B8A8_UINT:
			fmt = SG_PIXELFORMAT_RGBA8UI;
			break;
		case DXGI_FORMAT_R8G8B8A8_SINT:
			fmt = SG_PIXELFORMAT_RGBA8SI;
			break;
		case DXGI_FORMAT_R8G8B8A8_SNORM:
			fmt = SG_PIXELFORMAT_RGBA8SN;
			break;
		case DXGI_FORMAT_R16G16_TYPELESS:
			fmt = SG_PIXELFORMAT_RG16;
			break;
		case DXGI_FORMAT_R16G16_FLOAT:
			fmt = SG_PIXELFORMAT_RG16F;
			break;
		case DXGI_FORMAT_R16G16_UINT:
			fmt = SG_PIXELFORMAT_RG16UI;
			break;
		case DXGI_FORMAT_R16G16_SNORM:
			fmt = SG_PIXELFORMAT_RG16SN;
			break;
		case DXGI_FORMAT_R16G16_SINT:
			fmt = SG_PIXELFORMAT_RG16SI;
			break;
		case DXGI_FORMAT_R32_FLOAT:
			fmt = SG_PIXELFORMAT_R32F;
			break;
		case DXGI_FORMAT_R32_UINT:
			fmt = SG_PIXELFORMAT_R32UI;
			break;
		case DXGI_FORMAT_R32_SINT:
			fmt = SG_PIXELFORMAT_R32SI;
			break;
		case DXGI_FORMAT_D32_FLOAT:
			fmt = SG_PIXELFORMAT_DEPTH;
			break;
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
			fmt = SG_PIXELFORMAT_DEPTH_STENCIL;
			break;
		case DXGI_FORMAT_R8G8_TYPELESS:
			fmt = SG_PIXELFORMAT_RG8;
			break;
		case DXGI_FORMAT_R8G8_UINT:
			fmt = SG_PIXELFORMAT_RG8UI;
			break;
		case DXGI_FORMAT_R8G8_SNORM:
			fmt = SG_PIXELFORMAT_RG8SN;
			break;
		case DXGI_FORMAT_R8G8_SINT:
			fmt = SG_PIXELFORMAT_RG8SI;
			break;
		case DXGI_FORMAT_R16_TYPELESS:
			fmt = SG_PIXELFORMAT_R16;
			break;
		case DXGI_FORMAT_R16_FLOAT:
			fmt = SG_PIXELFORMAT_R16F;
			break;
		case DXGI_FORMAT_R16_UINT:
			fmt = SG_PIXELFORMAT_R16UI;
			break;
		case DXGI_FORMAT_R16_SNORM:
			fmt = SG_PIXELFORMAT_R16SN;
			break;
		case DXGI_FORMAT_R16_SINT:
			fmt = SG_PIXELFORMAT_R16SI;
			break;
		case DXGI_FORMAT_R8_TYPELESS:
			fmt = SG_PIXELFORMAT_R8;
			break;
		case DXGI_FORMAT_R8_UINT:
			fmt = SG_PIXELFORMAT_R8UI;
			break;
		case DXGI_FORMAT_R8_SNORM:
			fmt = SG_PIXELFORMAT_R8SN;
			break;
		case DXGI_FORMAT_R8_SINT:
			fmt = SG_PIXELFORMAT_R8SI;
			break;
		case DXGI_FORMAT_BC1_TYPELESS:
		case DXGI_FORMAT_BC1_UNORM:
			fmt = SG_PIXELFORMAT_BC1_RGBA;
			break;
		case DXGI_FORMAT_BC2_TYPELESS:
		case DXGI_FORMAT_BC2_UNORM:
			fmt = SG_PIXELFORMAT_BC2_RGBA;
			break;
		case DXGI_FORMAT_BC3_TYPELESS:
		case DXGI_FORMAT_BC3_UNORM:
			fmt = SG_PIXELFORMAT_BC3_RGBA;
			break;
		case DXGI_FORMAT_BC4_TYPELESS:
		case DXGI_FORMAT_BC4_UNORM:
			fmt = SG_PIXELFORMAT_BC4_R;
			break;
		case DXGI_FORMAT_BC4_SNORM:
			fmt = SG_PIXELFORMAT_BC4_RSN;
			break;
		case DXGI_FORMAT_BC5_TYPELESS:
		case DXGI_FORMAT_BC5_UNORM:
			fmt = SG_PIXELFORMAT_BC5_RG;
			break;
		case DXGI_FORMAT_BC5_SNORM:
			fmt = SG_PIXELFORMAT_BC5_RGSN;
			break;
		case DXGI_FORMAT_BC6H_SF16:
			fmt = SG_PIXELFORMAT_BC6H_RGBF;
			break;
		case DXGI_FORMAT_BC6H_UF16:
			fmt = SG_PIXELFORMAT_BC6H_RGBUF;
			break;
		case DXGI_FORMAT_BC7_UNORM:
		case DXGI_FORMAT_BC7_TYPELESS:
			fmt = SG_PIXELFORMAT_BC7_RGBA;
			break;
		// unsupported formats
#if 0
		case DXGI_FORMAT_BC7_UNORM_SRGB:
		case DXGI_FORMAT_BC6H_TYPELESS :
		case DXGI_FORMAT_B8G8R8A8_UNORM:
		case DXGI_FORMAT_B8G8R8X8_UNORM:
		case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
		case DXGI_FORMAT_B8G8R8A8_TYPELESS:
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		case DXGI_FORMAT_B8G8R8X8_TYPELESS:
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
		case DXGI_FORMAT_B5G5R5A1_UNORM:
		case DXGI_FORMAT_B5G6R5_UNORM:
		case DXGI_FORMAT_AYUV:
		case DXGI_FORMAT_Y410:
		case DXGI_FORMAT_Y416:
		case DXGI_FORMAT_NV12:
		case DXGI_FORMAT_P010:
		case DXGI_FORMAT_P016:
		case DXGI_FORMAT_420_OPAQUE:
		case DXGI_FORMAT_YUY2:
		case DXGI_FORMAT_Y210:
		case DXGI_FORMAT_Y216:
		case DXGI_FORMAT_NV11:
		case DXGI_FORMAT_AI44:
		case DXGI_FORMAT_IA44:
		case DXGI_FORMAT_P8:
		case DXGI_FORMAT_A8P8:
		case DXGI_FORMAT_B4G4R4A4_UNORM:
		case DXGI_FORMAT_P208:
		case DXGI_FORMAT_V208:
		case DXGI_FORMAT_V408:
		case DXGI_FORMAT_BC3_UNORM_SRGB:
		case DXGI_FORMAT_BC2_UNORM_SRGB:
		case DXGI_FORMAT_BC1_UNORM_SRGB:
		case DXGI_FORMAT_G8R8_G8B8_UNORM:
		case DXGI_FORMAT_R8G8_B8G8_UNORM:
		case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
		case DXGI_FORMAT_R1_UNORM:
		case DXGI_FORMAT_A8_UNORM:
		case DXGI_FORMAT_R8_UNORM:
		case DXGI_FORMAT_R16_UNORM:
		case DXGI_FORMAT_D16_UNORM:
		case DXGI_FORMAT_R8G8_UNORM:
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_R24G8_TYPELESS:
		case DXGI_FORMAT_R32_TYPELESS:
		case DXGI_FORMAT_R16G16_UNORM:
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_R10G10B10A2_UINT:
		case DXGI_FORMAT_R10G10B10A2_UNORM:
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		case DXGI_FORMAT_R32G8X24_TYPELESS:
		case DXGI_FORMAT_R32G32_TYPELESS:
		case DXGI_FORMAT_R16G16B16A16_UNORM:
		case DXGI_FORMAT_R32G32B32_TYPELESS:
		case DXGI_FORMAT_R32G32B32_FLOAT:
		case DXGI_FORMAT_R32G32B32_UINT:
		case DXGI_FORMAT_R32G32B32_SINT:
#endif
		default:
			fmt = SG_PIXELFORMAT_NONE;
			break;
	}
	return fmt;
}
	
// 
static inline size_t BytesPerPixel(sg_pixel_format fmt, int width, int height) {
	size_t size;
	if (((fmt >= SG_PIXELFORMAT_BC1_RGBA) && (fmt <= SG_PIXELFORMAT_BC7_RGBA)) || (fmt == SG_PIXELFORMAT_ETC2_RGB8)) {
		size_t block_size;
		if ((fmt == SG_PIXELFORMAT_BC1_RGBA) || (fmt == SG_PIXELFORMAT_BC4_R) || (fmt == SG_PIXELFORMAT_BC4_RSN) || (fmt == SG_PIXELFORMAT_ETC2_RGB8)) {
			block_size = 8;
		} else {
			block_size = 16;
		}
		size = Max(1, (width + 3) / 4) * Max(1, (height + 3) / 4) * block_size;
	} else {
		int bpp;
		if ((fmt >= SG_PIXELFORMAT_R8) && (fmt <= SG_PIXELFORMAT_R8SI)) {
			bpp = 8;
		} else if ((fmt >= SG_PIXELFORMAT_R16) && (fmt <= SG_PIXELFORMAT_RG8SI)) {
			bpp = 16;
		} else if (((fmt >= SG_PIXELFORMAT_R32UI) && (fmt <= SG_PIXELFORMAT_RG11B10F)) || (fmt == SG_PIXELFORMAT_DEPTH_STENCIL) || (fmt == SG_PIXELFORMAT_DEPTH)) {
			bpp = 32;
		} else if ((fmt >= SG_PIXELFORMAT_RG32UI) && (fmt <= SG_PIXELFORMAT_RGBA16F)) {
			bpp = 64;
		} else if ((fmt >= SG_PIXELFORMAT_RGBA32UI) && (fmt <= SG_PIXELFORMAT_RGBA32F)) {
			bpp = 128;
		} else {
			bpp = 0;
		}

		uint32_t row_bytes = (width * bpp + 7) / 8;
		size = height * row_bytes;
	}
	return size;
}
//
Texture LoadDDS(const Reader &ir, const Handle &h, const std::string &name) {
	ProfilerPerfSection section("LoadDDS", name);
	
	sg_image_desc desc;
	memset(&desc, 0, sizeof(sg_image_desc));

	Texture tex;
	tex.image.id = SG_INVALID_ID;

	const uint32_t magic = Read<uint32_t>(ir, h);
	if (magic != MAKEFOURCC('D', 'D', 'S', ' ')) {
		warn("    Invalid DDS file");
	} else {
		DDSURFACEDESC2 header;
		if (ir.read(h, &header, sizeof(DDSURFACEDESC2)) != sizeof(DDSURFACEDESC2)) {
			warn("    Failed to read header");
		} else if (header.dwSize != 124) {
			warn("    Invalid header");
		} else {
			desc.pixel_format = SG_PIXELFORMAT_NONE;
			desc.min_filter = SG_FILTER_LINEAR;
			desc.mag_filter = SG_FILTER_LINEAR;
			desc.width = header.dwWidth;
			desc.height = header.dwHeight;
			desc.num_mipmaps = header.dwMipMapCount;
			if (desc.num_mipmaps == 0) {
				desc.num_mipmaps = 1;
			}

			// check for DXT10 extended header.
			if ((header.ddpfPixelFormat.dwFlags & DDPF_FOURCC) && (header.ddpfPixelFormat.dwFourCC == FOURCC_DX10)) {
				DDSHEADERDXT10 ext_header;
				if (ir.read(h, &ext_header, sizeof(DDSHEADERDXT10)) != sizeof(DDSHEADERDXT10)) {
					warn("    Failed to read extented header");
				} else {
					desc.pixel_format = FromDXGIPixelFormat(ext_header.dxgiFormat);
					if (ext_header.resourceDimension == 3) {
						if (ext_header.miscFlag & 0x4U) {
							desc.type = SG_IMAGETYPE_CUBE;
						} else {
							desc.type = SG_IMAGETYPE_2D;
						}
					} else if ((ext_header.resourceDimension == 4) && (header.dwFlags & DDSD_VOLUME)) {
						desc.type = SG_IMAGETYPE_3D;
					} else {
						warn("    Invalid image type");
					}
				}
			} else {
				if (header.ddpfPixelFormat.dwFlags & DDPF_FOURCC) {
					desc.pixel_format = FromFourCC(header.ddpfPixelFormat.dwFourCC);
				} else if ((header.ddpfPixelFormat.dwFlags & DDPF_RGB) && (header.ddpfPixelFormat.dwRGBBitCount == 32)) {
					if ((header.ddpfPixelFormat.dwGBitMask == 0x0000ff00U) &&
						((header.ddpfPixelFormat.dwRBitMask | header.ddpfPixelFormat.dwBBitMask) == 0x00ff00ffU)) {
						if (header.ddpfPixelFormat.dwFlags & DDPF_ALPHAPIXELS) {
							assert(header.ddpfPixelFormat.dwABitMask == 0xff000000U);
						}
						if (header.ddpfPixelFormat.dwRBitMask == 0x000000ffU) {
							desc.pixel_format = SG_PIXELFORMAT_RGBA8;
						} else {
							desc.pixel_format = SG_PIXELFORMAT_BGRA8;
						}
					} else if ((header.ddpfPixelFormat.dwRBitMask == 0xffffffffU) && (header.ddpfPixelFormat.dwGBitMask == 0x00000000U) &&
							   (header.ddpfPixelFormat.dwBBitMask == 0x00000000U) && (header.ddpfPixelFormat.dwABitMask == 0x00000000U)) {
						desc.pixel_format = SG_PIXELFORMAT_R32F;
					}
				}
				if (desc.type == 0) {
					if (header.dwFlags & DDSD_VOLUME) {
						desc.type = SG_IMAGETYPE_3D;
					} else if (header.ddsCaps.dwCaps2 & DDSD_CUBEMAP) {
						desc.type = SG_IMAGETYPE_CUBE;
					} else {
						desc.type = SG_IMAGETYPE_2D;
					}
				}
			}

			if (desc.pixel_format == SG_PIXELFORMAT_NONE) {
				warn("    Invalid or unsupported pixel format");
			} else if(desc.type) {
				if (desc.type == SG_IMAGETYPE_3D) {
					desc.num_slices = header.dwDepth;
				} else {
					desc.num_slices = 1;
				}
				
				log(fmt::format("Load DDS '{}'", name));
				log(fmt::format("    Size: {}x{}x{} Mips: {}", desc.width, desc.height, desc.num_slices, desc.num_mipmaps));
				// [todo] format / image type

				const size_t size = ir.size(h);
				const size_t remaining = size - ir.tell(h);
				
				std::vector<uint8_t> payload(remaining);
				if (ir.read(h, payload.data(), remaining) != remaining) {
					warn("    Failed to read image data");
				} else {
					const size_t side_count = (desc.type != SG_IMAGETYPE_CUBE) ? 1 : 6;
					const uint8_t *ptr = payload.data();
					for (size_t j = 0; j < side_count; j++) {
						int w = desc.width;
						int h = desc.height;
						int d = desc.num_slices;

						for (size_t i = 0; i < desc.num_mipmaps; i++) {
							const size_t bytes_per_pixel = BytesPerPixel(desc.pixel_format, w, h);
							const size_t size = d * bytes_per_pixel;

							desc.data.subimage[j][i].ptr = ptr;
							desc.data.subimage[j][i].size = size;

							ptr += size;

							w >>= 1;
							h >>= 1;
							d >>= 1;
							if (!w) {
								w = 1;
							}
							if (!h) {
								h = 1;
							}
							if (!d) {
								d = 1;
							}
						}
					}
					if (desc.pixel_format == SG_PIXELFORMAT_BGRA8) {
						// BGRA8 may not be supported by some backends (namely OpenGL).
						desc.pixel_format = SG_PIXELFORMAT_RGBA8;
						for (size_t j = 0; j < side_count; j++) {
							for (size_t i = 0; i < desc.num_mipmaps; i++) {
								uint8_t *ptr = (uint8_t*)desc.data.subimage[j][i].ptr;
								for (size_t k = 0; k < desc.data.subimage[j][i].size; k+=4, ptr+=4) {
									uint8_t v = ptr[0];
									ptr[0] = ptr[2];
									ptr[2] = v;
								}
							}
						}
					}
					tex.image = sg_make_image(&desc);
				}
			}
		}
	}
	return tex;
}

} // namespace hg
