// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "engine/load_dds.h"

#include "foundation/log.h"
#include "foundation/math.h"
#include "foundation/profiler.h"

#include <fmt/format.h>
#include <vector>

namespace hg {

// from ddraw.h
#define DDSD_CAPS 0x00000001l // default
#define DDSD_HEIGHT 0x00000002l
#define DDSD_WIDTH 0x00000004l
#define DDSD_PITCH 0x00000008l
#define DDSD_PIXELFORMAT 0x00001000l
#define DDSD_LINEARSIZE 0x00080000l

#define DDPF_ALPHAPIXELS 0x00000001l
#define DDPF_FOURCC 0x00000004l
#define DDPF_RGB 0x00000040l

struct DDPIXELFORMAT {
	uint32_t dwSize;
	uint32_t dwFlags;
	uint32_t dwFourCC;

	union {
		uint32_t dwRGBBitCount;
		uint32_t dwYUVBitCount;
		uint32_t dwZBufferBitDepth;
		uint32_t dwAlphaBitDepth;
	};

	union {
		uint32_t dwRBitMask;
		uint32_t dwYBitMask;
	};

	union {
		uint32_t dwGBitMask;
		uint32_t dwUBitMask;
	};

	union {
		uint32_t dwBBitMask;
		uint32_t dwVBitMask;
	};

	union {
		uint32_t dwRGBAlphaBitMask;
		uint32_t dwYUVAlphaBitMask;
	};
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

//
static sg_pixel_format FourCCPixelFormat(uint32_t fourcc) {
	if (fourcc == '2CTE') {
		return SG_PIXELFORMAT_ETC2_RGB8;
	}

	return _SG_PIXELFORMAT_NUM;
}

//
Texture LoadDDS(const Reader &ir, const Handle &h, const std::string &name) {
	ProfilerPerfSection section("LoadDDS", name);

	Texture tex;
	tex.image.id = SG_INVALID_ID;

	const uint32_t magic = Read<uint32_t>(ir, h);
	if (magic != ' SDD')
		return tex;

	DDSURFACEDESC2 header;
	ir.read(h, &header, sizeof(DDSURFACEDESC2));

	if (header.dwSize != 124)
		return tex;

	log(fmt::format("Load DDS '{}'", name));
	log(fmt::format("    Size: {}x{} Mips: {}", header.dwWidth, header.dwHeight, header.dwMipMapCount));

	if (header.ddpfPixelFormat.dwFlags & DDPF_RGB) {
		size_t pitch = header.dwWidth * header.ddpfPixelFormat.dwRGBBitCount / 8;

		if (header.dwFlags & DDSD_PITCH)
			pitch = header.dwPitchOrLinearSize;

		if (header.ddpfPixelFormat.dwRGBBitCount == 32) {
			assert(header.ddpfPixelFormat.dwRBitMask == 0x00ff0000);
			assert(header.ddpfPixelFormat.dwGBitMask == 0x0000ff00);
			assert(header.ddpfPixelFormat.dwBBitMask == 0x000000ff);

			if (header.ddpfPixelFormat.dwFlags & DDPF_ALPHAPIXELS)
				assert(header.ddpfPixelFormat.dwRGBAlphaBitMask == 0xff000000);

			std::vector<uint8_t> rgba;
			rgba.resize(header.dwWidth * header.dwHeight * 4);

			uint8_t *dst = &rgba[0];

			std::vector<uint8_t> line(pitch * 4);
			for (size_t y = 0; y < header.dwHeight; ++y) {
				uint8_t *src = &line[0];
				ir.read(h, src, pitch);

				for (size_t x = 0; x < header.dwWidth; ++x, dst += 4, src += 4) {
					dst[0] = src[2]; // R
					dst[1] = src[1]; // G
					dst[2] = src[0]; // B
					dst[3] = src[3]; // A
				}
			}
		} else {
			warn(fmt::format("    Unsupported bit count: {}", header.ddpfPixelFormat.dwRGBBitCount));
		}
	} else if (header.ddpfPixelFormat.dwFlags & DDPF_FOURCC) {
		log(fmt::format("    FourCC: {}", (const char *)(&header.ddpfPixelFormat.dwFourCC)));

		const size_t size = ir.size(h);
		const size_t remaining = size - ir.tell(h);

		std::vector<uint8_t> payload(remaining);
		ir.read(h, payload.data(), remaining);

		sg_image_desc desc;
		memset(&desc, 0, sizeof(sg_image_desc));
		desc.width = header.dwWidth;
		desc.height = header.dwHeight;
		desc.pixel_format = FourCCPixelFormat(header.ddpfPixelFormat.dwFourCC);
		desc.min_filter = SG_FILTER_LINEAR;
		desc.mag_filter = SG_FILTER_LINEAR;
		desc.num_mipmaps = header.dwMipMapCount;

		uint8_t *mip_data = payload.data();
		for (int i = 0, w = desc.width, h = desc.height; i < desc.num_mipmaps; ++i, w /= 2, h /= 2) {
			const size_t mip_size = Max(8, (w * h) / 2);
			desc.data.subimage[0][i].ptr = mip_data;
			desc.data.subimage[0][i].size = mip_size;
			mip_data += mip_size;
		}

		tex.image = sg_make_image(&desc);
	} else {
		warn("    Invalid header!");
	}

	return tex;
}

} // namespace hg
