// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#pragma once

#include <stdint.h>
#include <string>

#include <foundation/color.h>

namespace hg {

enum PictureFormat { PF_None, PF_RGB24, PF_RGBA32, PF_RGBA32F, PF_Last };

int size_of(PictureFormat format);
size_t GetChannelCount(PictureFormat format);

class Picture { // 16 bytes
public:
	Picture();
	Picture(const Picture &pic);
	Picture(uint16_t width, uint16_t height, PictureFormat format);
	Picture(void *data, uint16_t width, uint16_t height, PictureFormat format);
	~Picture();

	Picture &operator=(const Picture &pic);

	uint16_t GetWidth() const { return w; }
	uint16_t GetHeight() const { return h; }

	PictureFormat GetFormat() const { return f; }

	uint8_t *GetData() const { return d; }

	/// Set picture data, do not take ownership of the data.
	void SetData(void *data, uint16_t width, uint16_t height, PictureFormat format);
	/// Allocate and copy to picture data.
	void CopyData(const void *data, uint16_t width, uint16_t height, PictureFormat format);

	bool HasDataOwnership() const { return has_ownership != 0; }
	void TakeDataOwnership();

	void Clear();

#if __cplusplus >= 201103L
	Picture(Picture &&pic) noexcept;
	Picture &operator=(Picture &&pic) noexcept;
#endif

private:
	uint16_t w, h;
	PictureFormat f;
	uint8_t has_ownership;

	uint8_t *d;
};

Picture MakePictureView(void *data, uint16_t width, uint16_t height, PictureFormat format);
Picture MakePicture(const void *data, uint16_t width, uint16_t height, PictureFormat format);

//
Picture Crop(const Picture &picture, uint16_t width, uint16_t height);
Picture Resize(const Picture &picture, uint16_t width, uint16_t height);

//
Color GetPixelRGBA(const Picture &pic, uint16_t x, uint16_t y);
void SetPixelRGBA(Picture &pic, uint16_t x, uint16_t y, const Color &col);

// I/O
bool LoadJPG(Picture &pic, const std::string &path);
bool LoadPNG(Picture &pic, const std::string &path);
bool LoadGIF(Picture &pic, const std::string &path);
bool LoadPSD(Picture &pic, const std::string &path);
bool LoadTGA(Picture &pic, const std::string &path);
bool LoadBMP(Picture &pic, const std::string &path);

bool LoadPicture(Picture &pic, const std::string &path);

bool SavePNG(const Picture &pic, const std::string &path);
bool SaveTGA(const Picture &pic, const std::string &path);
bool SaveBMP(const Picture &pic, const std::string &path);
bool SaveBC6H(const Picture &pic, const std::string &path, bool fast);
bool SaveBC7(const Picture &pic, const std::string &path, bool fast);
bool SaveHDR(const Picture &pic, const std::string &path);

} // namespace hg
