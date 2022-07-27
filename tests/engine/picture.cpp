// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#define TEST_NO_MAIN
#include "acutest.h"

#include "foundation/cext.h"

#include "engine/picture.h"

#include "foundation/math.h"

using namespace hg;

void test_picture() {
	TEST_CHECK(size_of(PF_None) == 0);
	TEST_CHECK(size_of(PF_RGB24) == 3);
	TEST_CHECK(size_of(PF_RGBA32) == 4);
	TEST_CHECK(size_of(PF_RGBA32F) == 16);
	TEST_CHECK(size_of(PF_Last) == 0);

	TEST_CHECK(GetChannelCount(PF_None) == 0);
	TEST_CHECK(GetChannelCount(PF_RGB24) == 3);
	TEST_CHECK(GetChannelCount(PF_RGBA32) == 4);
	TEST_CHECK(GetChannelCount(PF_RGBA32F) == 4);
	TEST_CHECK(GetChannelCount(PF_Last) == 0);

	{
		Picture pic0;
		TEST_CHECK(pic0.GetWidth() == 0);
		TEST_CHECK(pic0.GetHeight() == 0);
		TEST_CHECK(pic0.GetFormat() == PF_None);
		TEST_CHECK(pic0.GetData() == nullptr);
		TEST_CHECK(pic0.HasDataOwnership() == false);
		TEST_CHECK(IsValid(pic0) == false);

		SetPixelRGBA(pic0, 0, 0, Color::Yellow);
		TEST_CHECK(GetPixelRGBA(pic0, 0, 0) == Color::Zero);

		pic0.TakeDataOwnership();
		TEST_CHECK(pic0.HasDataOwnership() == false);
				
		TEST_CHECK(SavePNG(pic0, "picture.png") == false);
		TEST_CHECK(SaveTGA(pic0, "picture.tga") == false);
		TEST_CHECK(SaveBMP(pic0, "picture.bmp") == false);
		TEST_CHECK(SaveHDR(pic0, "picture.hdr") == false);
#if 0																					// [todo]
		TEST_CHECK(SaveBC6H(pic0, "picture_bc6h.dds", true) == false);
		TEST_CHECK(SaveBC7(pic0, "picture_bc7.dds", true) == false);
#endif

		TEST_CHECK(LoadPNG(pic0, "unknown.png") == false);
		TEST_CHECK(LoadTGA(pic0, "unknown.tga") == false);
		TEST_CHECK(LoadBMP(pic0, "unknown.bmp") == false);
		TEST_CHECK(LoadGIF(pic0, "unknown.gif") == false);
		TEST_CHECK(LoadPSD(pic0, "unknown.psd") == false);
		TEST_CHECK(LoadJPG(pic0, "unknown.jpg") == false);
	}

	{ 
		Picture pic0(16, 16, PF_RGB24);
		TEST_CHECK(pic0.GetWidth() == 16);
		TEST_CHECK(pic0.GetHeight() == 16);
		TEST_CHECK(pic0.GetFormat() == PF_RGB24);
		TEST_CHECK(pic0.GetData() != nullptr);
		TEST_CHECK(pic0.HasDataOwnership() == true);
		TEST_CHECK(IsValid(pic0) == true);

		SetPixelRGBA(pic0, 7, 9, Color::Orange);
		Color c = GetPixelRGBA(pic0, 7, 9);
		TEST_CHECK(TestEqual(c.r, Color::Orange.r, 1.f / 255.f) == true);
		TEST_CHECK(TestEqual(c.g, Color::Orange.g, 1.f / 255.f) == true);
		TEST_CHECK(TestEqual(c.b, Color::Orange.b, 1.f / 255.f) == true);
		TEST_CHECK(c.a == 0.f);
		TEST_CHECK(GetPixelRGBA(pic0, 20, 8) == Color::Zero);

		pic0.Clear();
		TEST_CHECK(IsValid(pic0) == false);

		uint16_t w = 12;
		uint16_t h = 8;
		PictureFormat fmt = PF_RGBA32;
		uint8_t *data = new uint8_t[w * h * size_of(fmt)];
		pic0 = MakePictureView(data, w, h, fmt);
		TEST_CHECK(IsValid(pic0) == true);
		TEST_CHECK(pic0.HasDataOwnership() == false);
		TEST_CHECK(pic0.GetData() == data);
		TEST_CHECK(pic0.GetWidth() == w);
		TEST_CHECK(pic0.GetHeight() == h);
		TEST_CHECK(pic0.GetFormat() == fmt);
		
		pic0.TakeDataOwnership();
		TEST_CHECK(pic0.HasDataOwnership() == true);
		TEST_CHECK(pic0.GetData() != data);
		TEST_CHECK(pic0.GetWidth() == w);
		TEST_CHECK(pic0.GetHeight() == h);
		TEST_CHECK(pic0.GetFormat() == fmt);

		pic0 = MakePicture(data, w, h, fmt);
		TEST_CHECK(pic0.HasDataOwnership() == true);
		TEST_CHECK(pic0.GetData() != data);
		TEST_CHECK(pic0.GetWidth() == w);
		TEST_CHECK(pic0.GetHeight() == h);
		TEST_CHECK(pic0.GetFormat() == fmt);

		delete[] data;
	}

	{ 
		uint16_t w = 24;
		uint16_t h = 24;
		PictureFormat fmt = PF_RGBA32F;
		uint8_t *data = new uint8_t[w * h * size_of(fmt)];
		
		Picture pic0(data, w, h, fmt);
		TEST_CHECK(pic0.GetWidth() == w);
		TEST_CHECK(pic0.GetHeight() == h);
		TEST_CHECK(pic0.GetFormat() == fmt);
		TEST_CHECK(pic0.GetData() == data);
		TEST_CHECK(pic0.HasDataOwnership() == false);

		Picture pic1(pic0);
		TEST_CHECK(pic1.GetWidth() == pic0.GetWidth());
		TEST_CHECK(pic1.GetHeight() == pic0.GetHeight());
		TEST_CHECK(pic1.GetFormat() == pic0.GetFormat());
		TEST_CHECK(pic1.GetData() == pic0.GetData());
		TEST_CHECK(pic1.HasDataOwnership() == false);

		pic1.TakeDataOwnership();
		TEST_CHECK(pic1.GetData() != pic0.GetData());
		TEST_CHECK(pic1.HasDataOwnership() == true);

		Picture pic2(pic1);
		TEST_CHECK(pic2.GetWidth() == pic2.GetWidth());
		TEST_CHECK(pic2.GetHeight() == pic2.GetHeight());
		TEST_CHECK(pic2.GetFormat() == pic2.GetFormat());
		TEST_CHECK(pic2.GetData() != pic1.GetData());
		TEST_CHECK(pic2.HasDataOwnership() == true);
	}
}