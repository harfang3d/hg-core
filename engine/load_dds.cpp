// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "engine/load_dds.h"

#include "foundation/log.h"
#include "foundation/math.h"
#include "foundation/profiler.h"

#include <fmt/format.h>
#include <vector>

namespace hg {

// from ddraw.h
#define DDSD_CAPS 0x00000001L // default
#define DDSD_HEIGHT 0x00000002L
#define DDSD_WIDTH 0x00000004L
#define DDSD_PITCH 0x00000008L
#define DDSD_PIXELFORMAT 0x00001000L
#define DDSD_LINEARSIZE 0x00080000L

#define DDPF_ALPHAPIXELS 0x00000001L
#define DDPF_FOURCC 0x00000004l
#define DDPF_RGB 0x00000040L

struct DDPIXELFORMAT {
	uint32_t dwSize;
	uint32_t dwFlags;
	uint32_t dwFourCC;

	uint32_t dwRGBBitCount;

	uint32_t dwRBitMask;
	uint32_t dwGBitMask;
	uint32_t dwBBitMask;
	uint32_t dwRGBAlphaBitMask;
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

#ifndef MAKEFOURCC
#	define MAKEFOURCC(a,b,c,d) \
(static_cast<uint32_t>((a)) | (static_cast<uint32_t>((b)) << 8) | (static_cast<uint32_t>((c)) << 16) | (static_cast<uint32_t>((d)) << 24))
#endif // MAKEFOURCC

//
static inline sg_pixel_format FourCCPixelFormat(uint32_t fourcc) {
	return (fourcc == MAKEFOURCC('E', 'T', 'C', '2') ? SG_PIXELFORMAT_ETC2_RGB8 : _SG_PIXELFORMAT_NUM);
}

//
Texture LoadDDS(const Reader &ir, const Handle &h, const std::string &name) {
	ProfilerPerfSection section("LoadDDS", name);

	Texture tex;
	tex.image.id = SG_INVALID_ID;

	const uint32_t magic = Read<uint32_t>(ir, h);
	if (magic == MAKEFOURCC('D','D','S',' ')) {
		DDSURFACEDESC2 header;
		ir.read(h, &header, sizeof(DDSURFACEDESC2));

		if (header.dwSize == 124) {

			log(fmt::format("Load DDS '{}'", name));
			log(fmt::format("    Size: {}x{} Mips: {}", header.dwWidth, header.dwHeight, header.dwMipMapCount));

			if (header.ddpfPixelFormat.dwFlags & DDPF_RGB) {
				size_t pitch = header.dwWidth * header.ddpfPixelFormat.dwRGBBitCount / 8;

				if (header.dwFlags & DDSD_PITCH) {
					pitch = header.dwPitchOrLinearSize;
				}

				if (header.ddpfPixelFormat.dwRGBBitCount == 32) {
					assert((header.ddpfPixelFormat.dwRBitMask == 0x00ff0000U && header.ddpfPixelFormat.dwBBitMask == 0x000000ffU) ||
						   (header.ddpfPixelFormat.dwRBitMask == 0x000000ffU && header.ddpfPixelFormat.dwBBitMask == 0x00ff0000U));
					assert(header.ddpfPixelFormat.dwGBitMask == 0x0000ff00U);

					if (header.ddpfPixelFormat.dwFlags & DDPF_ALPHAPIXELS) {
						assert(header.ddpfPixelFormat.dwRGBAlphaBitMask == 0xff000000U);
					}
					std::vector<uint8_t> rgba;
					rgba.resize(header.dwWidth * header.dwHeight * 4);

					uint8_t *dst = &rgba[0];

					std::vector<uint8_t> line(pitch * 4);
					for (size_t y = 0; y < header.dwHeight; ++y) {
						uint8_t *src = &line[0];
						ir.read(h, src, pitch);

						if (header.ddpfPixelFormat.dwRBitMask == 0x00ff0000U) {
							for (size_t x = 0; x < header.dwWidth; ++x, dst += 4, src += 4) {
								dst[0] = src[2]; // R
								dst[1] = src[1]; // G
								dst[2] = src[0]; // B
								dst[3] = src[3]; // A
							}
						} else {
							for (size_t x = 0; x < header.dwWidth; ++x, dst += 4, src += 4) {
								dst[0] = src[0]; // R
								dst[1] = src[1]; // G
								dst[2] = src[1]; // B
								dst[3] = src[3]; // A
							}
						}
					}
					
					sg_image_desc desc;
					memset(&desc, 0, sizeof(sg_image_desc));
					desc.width = header.dwWidth;
					desc.height = header.dwHeight;
					desc.pixel_format = SG_PIXELFORMAT_RGBA8;
					desc.min_filter = SG_FILTER_LINEAR;
					desc.mag_filter = SG_FILTER_LINEAR;
					desc.num_mipmaps = 1;
					desc.data.subimage[0][0].ptr = &rgba[0];
					desc.data.subimage[0][0].size = rgba.size();
					tex.image = sg_make_image(&desc);
				} else {
					warn(fmt::format("    Unsupported bit count: {}", header.ddpfPixelFormat.dwRGBBitCount));
				}
			} else if (header.ddpfPixelFormat.dwFlags & DDPF_FOURCC) {
				log(fmt::format("    FourCC: {}{}{}{}", static_cast<char>(header.ddpfPixelFormat.dwFourCC),
					static_cast<char>(header.ddpfPixelFormat.dwFourCC >> 8), static_cast<char>(header.ddpfPixelFormat.dwFourCC >> 16),
					static_cast<char>(header.ddpfPixelFormat.dwFourCC >> 24)));

				const size_t size = ir.size(h);
				const size_t remaining = size - ir.tell(h);

				std::vector<uint8_t> payload(remaining);
				ir.read(h, payload.data(), remaining);

				const sg_pixel_format fmt = FourCCPixelFormat(header.ddpfPixelFormat.dwFourCC);

				if (fmt != _SG_PIXELFORMAT_NUM) {
					sg_image_desc desc;
					memset(&desc, 0, sizeof(sg_image_desc));
					desc.width = header.dwWidth;
					desc.height = header.dwHeight;
					desc.pixel_format = fmt;
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
				}
			} else {
				warn("    Invalid header!");
			}
		}
	}
	return tex;
}

} // namespace hg
