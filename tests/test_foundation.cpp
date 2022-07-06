// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.
#include <math.h>
#include <utf8.h>

#include "acutest.h"

#include "foundation/math.h"
#include "foundation/unit.h"
#include "foundation/string.h"
#include "foundation/path_tools.h"

#include "foundation/time.h"
#include "foundation/clock.h"
#include "foundation/vector2.h"
#include "foundation/vector3.h"
#include "foundation/vector4.h"
#include "foundation/log.h"

#include <fmt/format.h>

using namespace hg;

//
void test_math() {
	TEST_CHECK(Abs(0) == 0);
	TEST_CHECK(Abs( 1000) == 1000);
	TEST_CHECK(Abs(-1000) == 1000);
	TEST_CHECK(Abs( 2.f) == 2.f);
	TEST_CHECK(Abs(-2.f) == 2.f);

	TEST_CHECK(Min(0, 0) == 0);
	TEST_CHECK(Min(10, 40) == 10);
	TEST_CHECK(Min(60, 3) == 3);
	TEST_CHECK(Min(-1, 4) == -1);
	TEST_CHECK(Min(-999, -3) == -999);
	TEST_CHECK(Min(-12, -34) == -34);
	TEST_CHECK(Min(1.0/3.0, 1.0/4.0) == 1.0/4.0);
	TEST_CHECK(Min(55.5555f, 48.33333f) == 48.33333f);
	TEST_CHECK(Min(-55.5555f,-48.33333f) == -55.5555f);
	TEST_CHECK(Min(10.0051, 10.005) == 10.005);

	TEST_CHECK(Min(22.22224, 22.22226, 22.222265) == 22.22224);
	TEST_CHECK(Min(0.00003, -0.00001, 0.0004) == -0.00001);
	TEST_CHECK(Min(5.55755, 5.550015, 5.54975) == 5.54975);

	TEST_CHECK(Max(0, 0) == 0);
	TEST_CHECK(Max(10, 40) == 40);
	TEST_CHECK(Max(60, 3) == 60);
	TEST_CHECK(Max(-1, 4) == 4);
	TEST_CHECK(Max(-999, -3) == -3);
	TEST_CHECK(Max(-12, -34) == -12);
	TEST_CHECK(Max(1.0 / 3.0, 1.0 / 4.0) == 1.0 / 3.0);
	TEST_CHECK(Max(55.5555f, 48.33333f) == 55.5555f);
	TEST_CHECK(Max(-55.5555f, -48.33333f) == -48.33333f);
	TEST_CHECK(Max(10.0051, 10.005) == 10.0051);

	TEST_CHECK(Max(22.22224, 22.22226, 22.222265) == 22.222265);
	TEST_CHECK(Max(0.00003, 0.0004, -0.00001) == 0.0004);
	TEST_CHECK(Max(5.55755, 5.550015, 5.54975) == 5.55755);

	TEST_CHECK(Clamp(0, -1, 1) == 0);
	TEST_CHECK(Clamp(1, -2, 2) == 1);
	TEST_CHECK(Clamp(2, -1, 1) == 1);
	TEST_CHECK(Clamp(-2, -1, 1) == -1);
	TEST_CHECK(Clamp(0.0, -1.02, 1.415) == 0.0);
	TEST_CHECK(Clamp(-1.2, -1.02, 1.415) == -1.02);
	TEST_CHECK(Clamp(1.4152, -1.02, 1.415) == 1.415);
	TEST_CHECK(Clamp(-33.3333, -9.4, 13.5) == -9.4);
	TEST_CHECK(Clamp(999.9999, -77.002, 2.715) == 2.715);
	TEST_CHECK(Clamp(1.10333, -2.11111, 2.323232) == 1.10333);

	TEST_CHECK(Lerp(-4.f, 5.f, 0.f) == -4.f);
	TEST_CHECK(Lerp(-4.f, 5.f, 1.f) == 5.f);
	TEST_CHECK(Lerp(9.f, 3.f, 0.f) == 9.f);
	TEST_CHECK(Lerp(9.f, 3.f, 1.f) == 3.f);
	TEST_CHECK(Lerp(-11.f, 11.f, 0.5f) == 0.f);

	TEST_CHECK(Sqrt(0.f) == 0.f);
	TEST_CHECK(Sqrt(1.f) == 1.f);
	TEST_CHECK(Sqrt(25.f) == 5.f);
	TEST_CHECK(Sqrt(9.f) == 3.f);
	TEST_CHECK(Sqrt(2.f) == 1.41421356237309504880f);

	TEST_CHECK(TestEqual(0.f, 0.f));
	TEST_CHECK(TestEqual(0.f, 1.f) == false);
	TEST_CHECK(TestEqual(99.999998f, 99.999998f));
	TEST_CHECK(TestEqual(17.353535f, 17.353535f));
	TEST_CHECK(TestEqual(17.3535352f, 17.3535351f));
	TEST_CHECK(TestEqual(0.12547f, 0.125721f, 0.001));
	TEST_CHECK(TestEqual(0.1259999f, 0.1281111f, 0.001) == false);

	TEST_CHECK(EqualZero(0.f));
	TEST_CHECK(EqualZero(0.000001f));
	TEST_CHECK(EqualZero(-0.000001f));
	TEST_CHECK(EqualZero(0.000002f) == false);
	TEST_CHECK(EqualZero(0.0001678f, 0.0002f));

	TEST_CHECK(Ceil(1.f) == 1.f);
	TEST_CHECK(Ceil(-1.f) == -1.f);
	TEST_CHECK(Ceil(1.5125f) == 2.f);
	TEST_CHECK(Ceil(1.99999f) == 2.f);
	TEST_CHECK(Ceil(1.49999f) == 2.f);
	TEST_CHECK(Ceil(1.11111f) == 2.f);
	TEST_CHECK(Ceil(-2.11111f) == -2.f);
	TEST_CHECK(Ceil(-0.5f) == 0.f);

	TEST_CHECK(Floor(1.f) == 1.f);
	TEST_CHECK(Floor(-1.f) == -1.f);
	TEST_CHECK(Floor(1.5125f) == 1.f);
	TEST_CHECK(Floor(1.99999f) == 1.f);
	TEST_CHECK(Floor(1.49999f) == 1.f);
	TEST_CHECK(Floor(1.11111f) == 1.f);
	TEST_CHECK(Floor(-2.11111f) == -3.f);
	TEST_CHECK(Floor(-0.5f) == -1.f);

	TEST_CHECK(Mod(301.f) == 0.f);
	TEST_CHECK(TestEqual(Mod(0.5555f), 0.5555f));
	TEST_CHECK(TestEqual(Mod(-99.125f), -0.125f));
	
	TEST_CHECK(Frac(4.f) == 0.f);
	TEST_CHECK(TestEqual(Frac(11.5555f), 0.5555f));
	TEST_CHECK(TestEqual(Frac(-8.125f), -0.125f));

	TEST_CHECK(RangeAdjust(0.f, -1.f, 1.f, -2.f, 1.f) == -0.5f);
	TEST_CHECK(RangeAdjust(0.5f, 0.f, 1.f, -1.f, 1.f) == 0.f);

	TEST_CHECK(TestEqual(Quantize(5.f, 4.f), 4.f));
	TEST_CHECK(TestEqual(Quantize(6.f, 9.f), 0.f));
	TEST_CHECK(TestEqual(Quantize(32.561f, 3.f), 30.f));

	TEST_CHECK(TestEqual(Wrap(2.5f, 0.f, 1.f), 0.5f));
	TEST_CHECK(TestEqual(Wrap(4.5, 2.5, 5.25), 4.5f));
	TEST_CHECK(TestEqual(Wrap(1.7, 0.5, 0.725), 0.575f));
	TEST_CHECK(TestEqual(Wrap(HalfPi, -Pi, Pi), HalfPi));
	TEST_CHECK(TestEqual(Wrap(Pi, -Pi, Pi), Pi));
	TEST_CHECK(TestEqual(Wrap(1.0, 0.0, 1.0), 1.0));
	TEST_CHECK(TestEqual(Wrap(2.f, 0.f, 1.f), 1.f));
	TEST_CHECK(TestEqual(Wrap(-7, -6, -3), -4));
	TEST_CHECK(TestEqual(Wrap(28,  6,  3), 4));
	
	TEST_CHECK(IsFinite(32755.f) == true);
	TEST_CHECK(IsFinite(-9999.999f) == true);
	TEST_CHECK(IsFinite(log(0.0)) == false);
	
	TEST_CHECK(getPOT(256) == 256);
	TEST_CHECK(getPOT(220) == 256);
	TEST_CHECK(getPOT(1960) == 2048);
	TEST_CHECK(getPOT(-140) == 1);

	TEST_CHECK(isPOT(512));
	TEST_CHECK(isPOT(1080) == false);
	TEST_CHECK(isPOT(-256) == false);

	TEST_CHECK(TestEqual(Sin(0.f), 0.f));
	TEST_CHECK(TestEqual(Sin(Pi / 6.f), 1.f / 2.f));
	TEST_CHECK(TestEqual(Sin(Pi / 4.f), float(sqrt(2.0) / 2.0)));
	TEST_CHECK(TestEqual(Sin(Pi / 3.f), float(sqrt(3.0) / 2.0)));
	TEST_CHECK(TestEqual(Sin(HalfPi), 1.f));
	TEST_CHECK(TestEqual(Sin(Pi), 0.f));
	TEST_CHECK(TestEqual(Sin(TwoPi), 0.f));
	TEST_CHECK(TestEqual(Sin(3.f * Pi / 2.f), -1.f));
	TEST_CHECK(TestEqual(Sin(-HalfPi), -1.f));
	TEST_CHECK(TestEqual(Sin(-1.5f), -Sin(1.5f)));
	TEST_CHECK(TestEqual(Sin(1.5f - Pi), -Sin(1.5f)));
	TEST_CHECK(TestEqual(Sin(0.4f + Pi), -Sin(0.4f)));

	TEST_CHECK(TestEqual(ASin(0.f), 0.f));
	TEST_CHECK(TestEqual(ASin(0.5f), Pi / 6.f));
	TEST_CHECK(TestEqual(ASin(float(sqrt(2.0) / 2.0)), Pi / 4.f));
	TEST_CHECK(TestEqual(ASin(float(sqrt(3.0) / 2.0)), Pi / 3.f));
	TEST_CHECK(TestEqual(ASin(1.f), HalfPi));
	TEST_CHECK(TestEqual(ASin(-1.f), -HalfPi));
	TEST_CHECK(TestEqual(ASin(-0.7f), -ASin(0.7f)));

	TEST_CHECK(TestEqual(Cos(0.f), 1.f));
	TEST_CHECK(TestEqual(Cos(Pi / 6.f), float(sqrt(3.0) / 2.f)));
	TEST_CHECK(TestEqual(Cos(Pi / 4.f), float(sqrt(2.0) / 2.0)));
	TEST_CHECK(TestEqual(Cos(Pi / 3.f), 1.0 / 2.0));
	TEST_CHECK(TestEqual(Cos(HalfPi), 0.f));
	TEST_CHECK(TestEqual(Cos(Pi),-1.f));
	TEST_CHECK(TestEqual(Cos(TwoPi), 1.f));
	TEST_CHECK(TestEqual(Cos(3.f * Pi / 2.f), 0.f));
	TEST_CHECK(TestEqual(Cos(-1.5f), Cos(1.5f)));
	TEST_CHECK(TestEqual(Cos(1.5f - Pi), -Cos(1.5f)));
	TEST_CHECK(TestEqual(Cos(2.6f - Pi), -Cos(2.6f)));

	TEST_CHECK(TestEqual(Cos(0.7f + HalfPi),-Sin(0.7f)));
	TEST_CHECK(TestEqual(Cos(2.5f - HalfPi), Sin(2.5f)));
	TEST_CHECK(TestEqual(Sin(0.3f + HalfPi), Cos(0.3f)));
	TEST_CHECK(TestEqual(Sin(1.8f - HalfPi),-Cos(1.8f)));

	TEST_CHECK(TestEqual(ACos(1.f), 0.f));
	TEST_CHECK(TestEqual(ACos(float(sqrt(3.0) / 2.f)), Pi / 6.f));
	TEST_CHECK(TestEqual(ACos(float(sqrt(2.0) / 2.0)), Pi / 4.f));
	TEST_CHECK(TestEqual(ACos(1.0 / 2.0), Pi / 3.f));
	TEST_CHECK(TestEqual(ACos(0.f), HalfPi));
	TEST_CHECK(TestEqual(ACos(-1.f), Pi));

	TEST_CHECK(TestEqual(ACos(-0.7f), HalfPi + ASin(0.7f)));

	TEST_CHECK(TestEqual(Tan(0.f), 0.f));
	TEST_CHECK(TestEqual(Tan(Pi / 6.f), float(sqrt(3.0) / 3.0)));
	TEST_CHECK(TestEqual(Tan(Pi / 4.f), 1.f));
	TEST_CHECK(TestEqual(Tan(Pi / 3.f), float(sqrt(3.0))));
	TEST_CHECK(TestEqual(Tan(-1.4f), -Tan(1.4f)));
	TEST_CHECK(TestEqual(Tan(0.6f + Pi), Tan(0.6f)));

	TEST_CHECK(TestEqual(ATan(0.f), 0.f));
	TEST_CHECK(TestEqual(ATan(float(sqrt(3.0) / 3.0)), Pi / 6.f));
	TEST_CHECK(TestEqual(ATan(1.f), Pi / 4.f));
	TEST_CHECK(TestEqual(ATan(float(sqrt(3.0))), Pi / 3.f));
	TEST_CHECK(TestEqual(ATan(-1.4f), -ATan(1.4f)));
	TEST_CHECK(TestEqual(ATan(1.f/0.4f) + ATan(0.4f), HalfPi));

	TEST_CHECK(TestEqual(LinearInterpolate(-1.f, 1.f, 0.5f), 0.f));
	TEST_CHECK(TestEqual(LinearInterpolate(102, 204, 0), 102));
	TEST_CHECK(TestEqual(LinearInterpolate(102, 204, 1), 204));
	TEST_CHECK(TestEqual(LinearInterpolate(16, 32, 0.5), 24));
	TEST_CHECK(TestEqual(LinearInterpolate(-5.f, 5.f, 0.1f), -4.f));
	TEST_CHECK(TestEqual(LinearInterpolate(-5.f, 5.f, 0.9f), 4.f));
	TEST_CHECK(TestEqual(LinearInterpolate( 8.f, 9.f, 1.1f), 9.1f));
	TEST_CHECK(TestEqual(LinearInterpolate( 6.f, 8.f,-0.1f), 5.8f));

	TEST_CHECK(TestEqual(CosineInterpolate(-1.f, 1.f, 0.0f),-1.f));
	TEST_CHECK(TestEqual(CosineInterpolate(-1.f, 1.f, 1.0f), 1.f));
	TEST_CHECK(TestEqual(CosineInterpolate( 1.f, 3.f, 0.5f), 2.f));
	TEST_CHECK(TestEqual(CosineInterpolate( 2.f, 4.f, 2.0f), 2.f));
	TEST_CHECK(TestEqual(CosineInterpolate( 2.f, 4.f, 3.0f), 4.f));
	TEST_CHECK(TestEqual(CosineInterpolate(-2.f, 2.f, 2.5f), 0.f));
	TEST_CHECK(TestEqual(CosineInterpolate(-2.f, 2.f, 1.f / 3.f), -1.f));
	TEST_CHECK(TestEqual(CosineInterpolate(-2.f, 2.f, 7.f / 3.f), -1.f));
	TEST_CHECK(TestEqual(CosineInterpolate(-2.f, 2.f, 4.f / 3.f), 1.f));
	
	const float tab[5] = {-2.f,-1.f,0.f,1.f,2.f};
	TEST_CHECK(TestEqual(LinearInterpolateArray(5, tab, 0.0f),-2.0f));
	TEST_CHECK(TestEqual(LinearInterpolateArray(5, tab, 1.0f), 2.0f));
	TEST_CHECK(TestEqual(LinearInterpolateArray(5, tab, 0.5f), 0.0f));
	TEST_CHECK(TestEqual(LinearInterpolateArray(5, tab, 0.25f),-1.0f));
	TEST_CHECK(TestEqual(LinearInterpolateArray(5, tab, 0.75f), 1.0f));
	TEST_CHECK(TestEqual(LinearInterpolateArray(5, tab, 0.125f),-1.5f));
	TEST_CHECK(TestEqual(LinearInterpolateArray(5, tab, 0.875f), 1.5f));
}

// 
void test_units() {
	TEST_CHECK(TestEqual(Deg(60.f), Pi/3.f));
	TEST_CHECK(TestEqual(Deg(90.f), HalfPi));
	TEST_CHECK(TestEqual(Deg(120.f), TwoPi / 3.f));
	TEST_CHECK(TestEqual(Deg(180.f), Pi));
	TEST_CHECK(TestEqual(Deg(315.f), 7.f*Pi/4.f));
	TEST_CHECK(TestEqual(Deg(360.f), TwoPi));

	TEST_CHECK(TestEqual(DegreeToRadian(60.f), Pi / 3.f));
	TEST_CHECK(TestEqual(DegreeToRadian(90.f), HalfPi));
	TEST_CHECK(TestEqual(DegreeToRadian(120.f), TwoPi / 3.f));
	TEST_CHECK(TestEqual(DegreeToRadian(180.f), Pi));
	TEST_CHECK(TestEqual(DegreeToRadian(315.f), 7.f * Pi / 4.f));
	TEST_CHECK(TestEqual(DegreeToRadian(360.f), TwoPi));

	TEST_CHECK(TestEqual(RadianToDegree(Pi / 3.f), 60.f));
	TEST_CHECK(TestEqual(RadianToDegree(HalfPi), 90.f));
	TEST_CHECK(TestEqual(RadianToDegree(TwoPi / 3.f), 120.f));
	TEST_CHECK(TestEqual(RadianToDegree(Pi), 180.f));
	TEST_CHECK(TestEqual(RadianToDegree(7.f * Pi / 4.f), 315.f));
	TEST_CHECK(TestEqual(RadianToDegree(TwoPi), 360.f));
	
	TEST_CHECK(TestEqual(Csec(1.0f), 0.01f));
	TEST_CHECK(TestEqual(Csec(100.f), 1.f));

	TEST_CHECK(TestEqual(Ms(1.0f), 0.001f));
	TEST_CHECK(TestEqual(Ms(1000.f), 1.f));

	TEST_CHECK(TestEqual(Ton(2.f), 2000.f));
	TEST_CHECK(TestEqual(Ton(0.001f), 1.f));

	TEST_CHECK(TestEqual(Kg(2.f), 2.f));

	TEST_CHECK(TestEqual(G(2.f), 0.002f));
	TEST_CHECK(TestEqual(G(1000.f), 1.f));

	TEST_CHECK(TestEqual(Km(1.f), 1000.f));
	TEST_CHECK(TestEqual(Km(0.001f), 1.f));

	TEST_CHECK(TestEqual(Mtr(1.f), 1.f));

	TEST_CHECK(TestEqual(Cm(1.f), 0.01f));
	TEST_CHECK(TestEqual(Cm(100.f), 1.f));

	TEST_CHECK(TestEqual(Mm(1.f), 0.001f));
	TEST_CHECK(TestEqual(Mm(1000.f), 1.f));
	
	TEST_CHECK(TestEqual(Inch(1.f), 0.0254f));
	TEST_CHECK(TestEqual(Inch(39.3701f), 1.f));
	
	TEST_CHECK(TestEqual(KB(1), 1024));
	TEST_CHECK(TestEqual(KB(1024), MB(1)));

	TEST_CHECK(FormatMemorySize(512) == "512B");
	TEST_CHECK(FormatMemorySize(KB(1)) == "1.0KB");
	TEST_CHECK(FormatMemorySize(MB(1)) == "1.0MB");
	TEST_CHECK(FormatMemorySize(MB(1024)) == "1.0GB");
	TEST_CHECK(FormatMemorySize(MB(1024*1024)) == "1.0TB");
	TEST_CHECK(FormatMemorySize(KB(1) + 512) == "1.5KB");
	TEST_CHECK(FormatMemorySize(MB(2048+768)) == "2.7GB");
	TEST_CHECK(FormatMemorySize(MB(1) + KB(512)) == "1.5MB");

	TEST_CHECK(FormatCount(1) == "1");
	TEST_CHECK(FormatCount(10) == "10");
	TEST_CHECK(FormatCount(1000) == "1.0K");
	TEST_CHECK(FormatCount(12345) == "12.3K");
	TEST_CHECK(FormatCount(2000000) == "2.0M");
	TEST_CHECK(FormatCount(5555555) == "5.5M");

	TEST_CHECK(FormatDistance(Mm(1.f)) == "1.0mm");
	TEST_CHECK(FormatDistance(Mm(8.2f)) == "8.2mm");
	TEST_CHECK(FormatDistance(Cm(1.f)) == "10.0mm");
	TEST_CHECK(FormatDistance(Cm(53.5f)) == "53.5cm");
	TEST_CHECK(FormatDistance(45.3f) == "45.3m");
	TEST_CHECK(FormatDistance(Km(1.f)) == "1.0km");
	TEST_CHECK(FormatDistance(Km(12.345)) == "12.3km");

	TEST_CHECK(FormatTime(time_from_sec(1) + time_from_ms(100)) == "1 sec 100 ms");
	TEST_CHECK(FormatTime(time_from_min(30) + time_from_sec(26) + time_from_ms(3)) == "30 min 26 sec 3 ms");
	TEST_CHECK(FormatTime(time_from_hour(2) + time_from_min(15) + time_from_sec(30) + time_from_ms(222)) == "2 hour 15 min 30 sec 222 ms");
}

//
void test_string() {
	TEST_CHECK(to_lower('A') == 'a');
	TEST_CHECK(to_lower('G') == 'g');
	TEST_CHECK(to_lower('z') == 'z');
	TEST_CHECK(to_lower('e') == 'e');
	TEST_CHECK(to_lower('0') == '0');
	TEST_CHECK(to_lower('!') == '!');

	TEST_CHECK(case_sensitive_eq_func('I', 'I'));
	TEST_CHECK(case_sensitive_eq_func('f', 'f'));
	TEST_CHECK(case_sensitive_eq_func('F', 'f') == false);
	TEST_CHECK(case_sensitive_eq_func('y', 'A') == false);
	TEST_CHECK(case_sensitive_eq_func('A', 'a') == false);
	TEST_CHECK(case_sensitive_eq_func('_', '_'));
	TEST_CHECK(case_sensitive_eq_func('!', '=') == false);

	TEST_CHECK(case_insensitive_eq_func('J', 'J'));
	TEST_CHECK(case_insensitive_eq_func('o', 'o'));
	TEST_CHECK(case_insensitive_eq_func('H', 'h'));
	TEST_CHECK(case_insensitive_eq_func('b', 'B'));
	TEST_CHECK(case_insensitive_eq_func('w', 'L') == false);
	TEST_CHECK(case_insensitive_eq_func('U', 'e') == false);
	TEST_CHECK(case_insensitive_eq_func('*', '*'));
	TEST_CHECK(case_insensitive_eq_func('$', '~') == false);

	TEST_CHECK(starts_with("starts_with", "start"));
	TEST_CHECK(starts_with("StArTs_WiTh", "start") == false);
	TEST_CHECK(starts_with("cAsE_SeNsItIviTy::insensitive", "CaSe_sEnSiTiViTy:", case_sensitivity::insensitive));
	TEST_CHECK(starts_with("Lorem ipsum dolor sit amet", "ipsum", case_sensitivity::insensitive) == false);

	TEST_CHECK(ends_with("starts_with", "_with"));
	TEST_CHECK(ends_with("StArTs_WiTh", "_WIth") == false);
	TEST_CHECK(ends_with("cAsE_SeNsItIviTy::iNsEnSiTiVe", ":InSEnsITiVE", case_sensitivity::insensitive));
	TEST_CHECK(ends_with("Lorem ipsum dolor sit amet", " sit amet", case_sensitivity::insensitive));
	TEST_CHECK(ends_with("Lorem ipsum dolor sit amet", " amet sit", case_sensitivity::insensitive) == false);

	std::string str = "bobcat dogfights a catfish over a hotdog";
	TEST_CHECK(replace_all(str, "dog", "cat") == 2);
	TEST_CHECK(str == "bobcat catfights a catfish over a hotcat");
	TEST_CHECK(replace_all(str, "cat", "DOG") == 4);
	TEST_CHECK(str == "bobDOG DOGfights a DOGfish over a hotDOG");

	std::vector<std::string> list(4);
	list[0] = "bobcat";
	list[1] = "catfish";
	list[2] = "hotdog";
	list[3] = "dogfish";
	TEST_CHECK(split("bobcat  , catfish,hotdog , dogfish ", ",", " ") == list);
	TEST_CHECK(split("*bobcat*||*catfish*||*hotdog*||*dogfish*", "||", "*") == list);
	
	TEST_CHECK(lstrip("Thopha saccata") == "Thopha saccata");
	TEST_CHECK(lstrip("     Baorisa hieroglyphica") == "Baorisa hieroglyphica");
	TEST_CHECK(lstrip("\t\t    \tStigmodera cancellata", " \t") == "Stigmodera cancellata");
	TEST_CHECK(lstrip(" - Stigmodera cancellata", " ") != "Stigmodera cancellata");

	TEST_CHECK(rstrip("Selenocosmia crassipes") == "Selenocosmia crassipes");
	TEST_CHECK(rstrip("Agrias claudina    ") == "Agrias claudina");
	TEST_CHECK(rstrip("Mormolyce phyllodes...;;-;..-_-", "_.-;") == "Mormolyce phyllodes");
	TEST_CHECK(rstrip("Phymateus viridipes\n\n ", " \t") != "Stigmodera cancellata");

	TEST_CHECK(strip("Ornithoptera euphorion") == "Ornithoptera euphorion");
	TEST_CHECK(strip("    Phyllium bioculatum        ") == "Phyllium bioculatum");
	TEST_CHECK(strip("\"0o. .o0\" Eupholus schoenherrii \"0o. .o0\"", "0.\"o ") == "Eupholus schoenherrii");
	TEST_CHECK(strip("<:_Chrysis ruddii= />", "<:/>") == "_Chrysis ruddii= ");

	TEST_CHECK(lstrip_space("Myrmecia brevinoda") == "Myrmecia brevinoda");
	TEST_CHECK(lstrip_space("\n\t\t  Rhachoepalpus metallicus\r\n") == "Rhachoepalpus metallicus\r\n");
	TEST_CHECK(lstrip_space("\r\n\t* Julodis cirrosa") != "Julodis cirrosa");

	TEST_CHECK(rstrip_space("Endoxyla cinerea") == "Endoxyla cinerea");
	TEST_CHECK(rstrip_space("Alaruasa violacea    \t \t  \r\n") == "Alaruasa violacea");
	TEST_CHECK(rstrip_space("Dynastes hercule    \t .\t  \r\n") != "Dynastes hercule");

	TEST_CHECK(strip_space("Phellus olgae") == "Phellus olgae");
	TEST_CHECK(strip_space("\t\t\tProtaetia affinis   \t\t   \r\n ") == "Protaetia affinis");
	TEST_CHECK(strip_space(" * Phobaeticus serratipes\r\n_ ") != "Phobaeticus serratipes");

	TEST_CHECK(reduce("   The Irish   damselfly or  crescent bluet (Coenagrion      lunulatum) is a damselfly found in  northern  Europe and Asia  to "
					  "north-eastern  China      ") ==
			   "The Irish damselfly or crescent bluet (Coenagrion lunulatum) is a damselfly found in northern Europe and Asia to north-eastern China");
	TEST_CHECK(reduce("The Arctic bluet (Coenagrion johanssoni) is found in Northern Europe, and east through Asia as far as the Amur River") ==
			   "The Arctic bluet (Coenagrion johanssoni) is found in Northern Europe, and east through Asia as far as the Amur River");
	TEST_CHECK(reduce("\t  Pyrrhosoma nymphula can reach\ta body length\r\n   of 33-36 millimetres (1.3-1.4 in).\r\n\r\n\r\n", "-=-", " \r\n\t") ==
			   "Pyrrhosoma-=-nymphula-=-can-=-reach-=-a-=-body-=-length-=-of-=-33-36-=-millimetres-=-(1.3-1.4-=-in).");

	const char *club_tailed_dragonflies[] = {
		"Yellow-legged",
		"Club-tailed",
		"Green club-tailed",
		"Green-eyed hook-tailed",
	};
	std::string v = join(club_tailed_dragonflies, club_tailed_dragonflies + 4, " dragonfly, ", "dragonfly.");
	TEST_CHECK(join(club_tailed_dragonflies, club_tailed_dragonflies + 4, " Dragonfly\n") ==
			   "Yellow-legged Dragonfly\nClub-tailed Dragonfly\nGreen club-tailed Dragonfly\nGreen-eyed hook-tailed");
	TEST_CHECK(join(club_tailed_dragonflies, club_tailed_dragonflies + 1, " // ") == "Yellow-legged");

	const char *path[] = {
		"e:",
		"hg-core",
		"foundation",
		"string",
		"h",
	};
	TEST_CHECK(join(path, path + 5, "/", ".") == "e:/hg-core/foundation/string.h");
	TEST_CHECK(join(path, path + 1, "\\", ".") == "e:");
	
	TEST_CHECK(contains("CTA TTA TTA ACA AGA AGT ATA GTA GAA AAC GGA GCT GGA ACA GGT TGA ACT GTT TAT CCT CCT CTT TCA TCT AAT ATT", "GGA"));
	TEST_CHECK(contains("GCC CAT AGA GGA GCT TCT GTT GAT TTA GCT ATT TTT TCT CTT CAT TTA GCT GGA ATT TCT TCC ATC CTA GGA GCA GTA ", "ATA") == false);
	TEST_CHECK(contains("", "ATT") == false);
	TEST_CHECK(contains("AGT TTA GTT ACT CAA CGT", "") == true);

	const char blueberry_jam_ansi_raw[] = { 0x42, 0x6C, 0xE5, 0x62, 0xE6, 0x72, 0x73, 0x79, 0x6C, 0x74, 0x65, 0x74, 0xF8, 0x79, 0x00 };
	const wchar_t blueberry_jam_utf16_raw[] = {0x0042, 0x006c, 0x00e5, 0x0062, 0x00e6, 0x0072, 0x0073, 0x0079, 0x006c, 0x0074, 0x0065, 0x0074, 0x00f8, 0x0079, 0x0000};
	const char32_t blueberry_jam_utf32_raw[] = {0x00000042, 0x0000006c, 0x000000e5, 0x00000062, 0x000000e6, 0x00000072u, 0x00000073, 0x00000079, 0x0000006c,
		0x00000074, 0x00000065, 0x00000074, 0x000000f8, 0x00000079, 0x00000000};
	
	const std::string blueberry_jam_ansi(blueberry_jam_ansi_raw);
	const std::string blueberry_jam_utf8 = "\x42\x6C\xC3\xA5\x62\xC3\xA6\x72\x73\x79\x6C\x74\x65\x74\xC3\xB8\x79";
	const std::wstring blueberry_jam_utf16(blueberry_jam_utf16_raw);
	const std::u32string blueberry_jam_utf32(blueberry_jam_utf32_raw);
	
	TEST_CHECK(utf32_to_utf8(blueberry_jam_utf32) == blueberry_jam_utf8);
	TEST_CHECK(utf8_to_utf32(blueberry_jam_utf8) == blueberry_jam_utf32);
	TEST_CHECK(wchar_to_utf8(blueberry_jam_utf16) == blueberry_jam_utf8);
	TEST_CHECK(utf8_to_wchar(blueberry_jam_utf8) == blueberry_jam_utf16);

	TEST_CHECK(ansi_to_utf8(blueberry_jam_ansi) == blueberry_jam_utf8);
	TEST_CHECK(ansi_to_wchar(blueberry_jam_ansi) == blueberry_jam_utf16);

	const char32_t blueberry_jam_utf32_invalid_cp_raw[] = {0x00000042, 0x0000006c, 0x000000e5, 0x00000062, 0x000000e6, 0x00000072u, 0x00000073, 0x00000079,
		0x0000006c, 0x00000074, 0x00000065, 0x00000074, 0x000000f8, 0x00000079, 0x0000ffff};
	const std::string blueberry_jam_utf8_invalid = "\x42\x6C\xC3\xA5\x62\xC3\xa0\xa1\xA6\x72\x73\x79\xf0\x28\x8c\x28\x6C\x74\x65\x74\xC3\xB8\x79";
	const std::u32string blueberry_jam_utf32_invalid_cp(blueberry_jam_utf32_invalid_cp_raw);
		
	TEST_EXCEPTION(utf32_to_utf8(blueberry_jam_utf32_invalid_cp), utf8::invalid_code_point);
	TEST_EXCEPTION(utf8_to_utf32(blueberry_jam_utf8_invalid), utf8::invalid_utf8);
	TEST_EXCEPTION(utf8_to_wchar(blueberry_jam_utf8_invalid), utf8::invalid_utf8);

	const std::string s_low = "lorem ipsum dolor sit amet, consectetur adipiscing elit";
	std::string s;
	s = "Lorem Ipsum doloR sit AMET, consECtETur adipISCINg eliT";
	TEST_CHECK(tolower(s) == s_low);
	tolower_inplace(s);
	TEST_CHECK(s == s_low);

	s = s_low;
	TEST_CHECK(tolower(s) == s_low);
	tolower_inplace(s);
	TEST_CHECK(s == s_low);

	const std::string s_lo_20_36 = "Lorem Ipsum doloR sit amet, consecteTur adipISCINg eliT";
	s = "Lorem Ipsum doloR sit AMET, CONsecTETur adipISCINg eliT";
	TEST_CHECK(tolower(s, 20, 36) == s_lo_20_36);
	tolower_inplace(s, 20, 36);
	TEST_CHECK(s == s_lo_20_36);

	const std::string s_up = "LOREM IPSUM DOLOR SIT AMET, CONSECTETUR ADIPISCING ELIT";
	s = "Lorem Ipsum doloR sit AMET, consECtETur adipISCINg eliT";
	TEST_CHECK(toupper(s) == s_up);
	toupper_inplace(s);
	TEST_CHECK(s == s_up);

	s = s_up;
	TEST_CHECK(toupper(s) == s_up);
	toupper_inplace(s);
	TEST_CHECK(s == s_up);

	const std::string s_up_20_36 = "Lorem Ipsum doloR siT AMET, CONSECTETur adipISCINg eliT";
	s = "Lorem Ipsum doloR sit Amet, CONsecTETur adipISCINg eliT";
	TEST_CHECK(toupper(s, 20, 36) == s_up_20_36);
	toupper_inplace(s, 20, 36);
	TEST_CHECK(s == s_up_20_36);

	const std::string in = "At vero eos et accusamus et iusto odio dignissimos";
	TEST_CHECK(slice(in, 15) == "accusamus et iusto odio dignissimos");
	TEST_CHECK(slice(in, 3, 8) == "vero eos");
	TEST_CHECK(slice(in, 8, 120) == "eos et accusamus et iusto odio dignissimos");
	TEST_CHECK(slice(in, -16) == "odio dignissimos");
	TEST_CHECK(slice(in, -35, 9) == "accusamus");
	TEST_CHECK(slice(in, -200) == in);
	TEST_CHECK(slice(in, -200, 7) == "At vero");
	TEST_CHECK(slice(in, 25, -17) == "et iusto");
	TEST_CHECK(slice(in, -16, -6) == "odio digni");
	TEST_CHECK(slice(in, 0, -128) == "");

	TEST_CHECK(left(in, 12) == "At vero eos ");
	TEST_CHECK(left(in, -23) == "At vero eos et accusamus et");
	TEST_CHECK(left(in, -99) == "");
	TEST_CHECK(left(in, 111) == in);

	TEST_CHECK(right(in, 22) == "iusto odio dignissimos");
	TEST_CHECK(right(in, -25) == "et iusto odio dignissimos");
	TEST_CHECK(right(in, 64) == in);

	const std::string eol_linux = "Ingredients\n"
								  "  * 2 cups granulated sugar\n"
								  "  * 1 1/2 cups fresh lemon juice (about 6 to 8 lemons)\n"
								  "  * 5 cups water\n";
	const std::string eol_windows = "Ingredients\r\n"
									"  * 2 cups granulated sugar\r\n"
									"  * 1 1/2 cups fresh lemon juice (about 6 to 8 lemons)\r\n"
									"  * 5 cups water\r\n";

	const std::string eol_src = "Ingredients\r\n"
								"  * 2 cups granulated sugar\n"
								"  * 1 1/2 cups fresh lemon juice (about 6 to 8 lemons)\r\n"
								"  * 5 cups water\n";
	s = eol_src;
	normalize_eol(s);
	TEST_CHECK(s == eol_linux);
	normalize_eol(s, EOLWindows);
	TEST_CHECK(s == eol_windows);

	s = "coffee please!";
	normalize_eol(s);
	TEST_CHECK(s == "coffee please!");
	s = "ice cold tea";
	normalize_eol(s, EOLWindows);
	TEST_CHECK(s == "ice cold tea");

	TEST_CHECK(strip_prefix("epipaleolithic", "epi") == "paleolithic");
	TEST_CHECK(strip_prefix("paleopaleozoic", "paleo") == "paleozoic");
	TEST_CHECK(strip_prefix("neolithic", "meso") == "neolithic");
	TEST_CHECK(strip_prefix("chalcolithic", "") == "chalcolithic");
	TEST_CHECK(strip_prefix("", "paleo") == "");

	TEST_CHECK(strip_suffix("walliserops trifurcatus", "trifurcatus") == "walliserops ");
	TEST_CHECK(strip_suffix("dimetrodon.jpg.jpg", ".jpg") == "dimetrodon.jpg");
	TEST_CHECK(strip_suffix("conolichas angustus", "triconicus") == "conolichas angustus");
	TEST_CHECK(strip_suffix("buenellus higginsi", "") == "buenellus higginsi");
	TEST_CHECK(strip_suffix("", "plicatus") == "");

	const std::string word_wrap_in = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nunc sagittis orci porta massa iaculis efficitur.";
	const std::string word_wrap_30 = "Lorem ipsum dolor sit amet, consectetur\n"
									 "adipiscing elit. Nunc sagittis\n"
									 "orci porta massa iaculis efficitur.";
	const std::string word_wrap_26_sp4 = "Lorem ipsum dolor sit amet,\n"
										 "    consectetur adipiscing elit.\n"
										 "    Nunc sagittis orci porta massa\n"
										 "    iaculis efficitur.";
	TEST_CHECK(word_wrap(word_wrap_in, 30) == word_wrap_30);
	TEST_CHECK(word_wrap(word_wrap_in, 26, 4, ' ') == word_wrap_26_sp4);
	TEST_CHECK(word_wrap(word_wrap_in, word_wrap_in.size()) == word_wrap_in);

	TEST_CHECK(name_to_path("user@home/readme first!") == "user-home-readme-first-");
	TEST_CHECK(name_to_path("random_filename_00") == "random_filename_00");

	TEST_CHECK(pad_left("Heoclisis fulva", 9, '.') == "Heoclisis fulva");
	TEST_CHECK(pad_left("Megaloprepus caerulatus", 28, '#') == "#####Megaloprepus caerulatus");
	TEST_CHECK(pad_left("Acanthacorydalis fruhstorferi", 33) == "    Acanthacorydalis fruhstorferi");
	
	TEST_CHECK(pad_left("Valanga irregularis", 11, '|') == "Valanga irregularis");
	TEST_CHECK(pad_right("Goliathus regius", 23, ':') == "Goliathus regius:::::::");
	TEST_CHECK(pad_right("Macrodontia cervicornis", 27) == "Macrodontia cervicornis    ");	
}

//
void test_path_tools() {
	TEST_CHECK(IsPathAbsolute("") == false);
#if WIN32
	TEST_CHECK(IsPathAbsolute("C:\\Windows\\System32\\svchost.exe"));
	TEST_CHECK(IsPathAbsolute(".git/refs/heads/main") == false);
#else
	TEST_CHECK(IsPathAbsolute("/usr/bin/bash"));
	TEST_CHECK(IsPathAbsolute("~/.ssh/known_hosts") == false);
#endif

	TEST_CHECK(PathToDisplay("The_Hidden_Fortress") == "The Hidden Fortress");
	TEST_CHECK(PathToDisplay("When the Last Sword Is Drawn") == "When the Last Sword Is Drawn");
	TEST_CHECK(PathToDisplay("") == "");

	TEST_CHECK(NormalizePath("The_Hidden_Fortress") == "The_Hidden_Fortress");
	TEST_CHECK(NormalizePath("When the Last Sword Is Drawn") == "When_the_Last_Sword_Is_Drawn");
	TEST_CHECK(NormalizePath("") == "");

	TEST_CHECK(FactorizePath("./here/../here/there/../../here") == "./here");
	TEST_CHECK(FactorizePath("a/b/c/../d/e/../../f/../../..") == "");
	TEST_CHECK(FactorizePath("/p/a/t/h") == "/p/a/t/h");

#if WIN32
	TEST_CHECK(CleanPath("C:/User/Default/My Documents/My Videos\\..\\..\\AppData\\Roaming\\hg_app\\./.config") == "c:/User/Default/AppData/Roaming/hg_app/.config");
	TEST_CHECK(CleanPath("\\\\printers\\laserjet\\jobs") == "\\\\printers/laserjet/jobs");
#endif
	TEST_CHECK(CleanPath("/home/user0001/../../home/../home/user0000/.config/app/hg.cfg") == "/home/user0000/.config/app/hg.cfg");
	TEST_CHECK(CleanPath("") == "");
	TEST_CHECK(CleanPath("Lorem ipsum dolor sit amet, consectetur") == "Lorem ipsum dolor sit amet, consectetur");

	TEST_CHECK(CleanFileName("<movie>\"1080p\"cyber city render\\final?\\.mp4") == "_movie__1080p_cyber city render_final__.mp4");
	TEST_CHECK(CleanFileName("render_pass_0000-pbr-no-shadows.png") == "render_pass_0000-pbr-no-shadows.png");
	TEST_CHECK(CleanFileName("") == "");

	TEST_CHECK(CutFilePath("/usr/local/bin/deltree.exe") == "deltree.exe");
	TEST_CHECK(CutFilePath("/proc/sys/Device/00000032") == "00000032");
	TEST_CHECK(CutFilePath("c:\\Users\\6502\\Documents\\") == "");
	TEST_CHECK(CutFilePath("Readme.md") == "Readme.md");
	TEST_CHECK(CutFilePath("") == "");

	TEST_CHECK(GetFilePath("/usr/local/bin/deltree.exe") == "/usr/local/bin/");
	TEST_CHECK(GetFilePath("/proc/sys/Device/00000032") == "/proc/sys/Device/");
	TEST_CHECK(GetFilePath("c:\\Users\\6502\\Documents\\") == "c:\\Users\\6502\\Documents\\");
	TEST_CHECK(GetFilePath("image.jpg") == "./");
	TEST_CHECK(GetFilePath("") == "./");
	
	TEST_CHECK(CutFileName("/usr/local/bin/deltree.exe") == "/usr/local/bin/");
	TEST_CHECK(CutFileName("/proc/sys/Device/00000032") == "/proc/sys/Device/");
	TEST_CHECK(CutFileName("c:\\Users\\6502\\Documents\\") == "c:\\Users\\6502\\Documents\\");
	TEST_CHECK(CutFileName("Readme.md") == "");
	TEST_CHECK(CutFileName("") == "");
	
	TEST_CHECK(GetFileName("/usr/local/bin/deltree.exe") == "deltree");
	TEST_CHECK(GetFileName(".app-config.json") == ".app-config");
	TEST_CHECK(GetFileName("/proc/sys/Device/00000032") == "00000032");
	TEST_CHECK(GetFileName("c:\\Users\\6502\\Documents\\") == "");
	TEST_CHECK(GetFileName("") == "");

	TEST_CHECK(CutFileExtension("/usr/local/bin/deltree.exe") == "/usr/local/bin/deltree");
	TEST_CHECK(CutFileExtension("c:\\Users\\6502\\Documents\\masm") == "c:\\Users\\6502\\Documents\\masm");
	TEST_CHECK(CutFileExtension("/usr/local/bin/") == "/usr/local/bin/");

	TEST_CHECK(HasFileExtension("c:\\Windows\\System32\\bootcfg.exe"));
	TEST_CHECK(HasFileExtension("/usr/bin/grep") == false);
	TEST_CHECK(HasFileExtension("git_commit.msg"));
	TEST_CHECK(HasFileExtension("") == false);

	TEST_CHECK(GetFileExtension("c:\\Windows\\System32\\bootcfg.exe") == "exe");
	TEST_CHECK(GetFileExtension("/usr/bin/grep") == "");
	TEST_CHECK(GetFileExtension("git_commit.msg") == "msg");
	TEST_CHECK(GetFileExtension("") == "");

	TEST_CHECK(PathStartsWith("/usr/local/bin/dummy", "/usr/"));
	TEST_CHECK(PathStartsWith("/tmp/abc/def/ghi/../../012/../012/../../345/core", "/usr/bin/../../tmp/345"));
	TEST_CHECK(PathStartsWith("/home/user000/.config/../../user001/Documents/../.config", "/home/user001/Documents/../Download/../../user000/") == false);
	TEST_CHECK(PathStartsWith("/usr/bin", ""));
	TEST_CHECK(PathStartsWith("", "./"));

	TEST_CHECK(PathStripPrefix("/usr/local/bin/dummy", "/usr") == "local/bin/dummy");
	TEST_CHECK(PathStripPrefix("/etc/X11/screen", "/usr") == "etc/X11/screen");
	TEST_CHECK(PathStripPrefix("./.config/app.json", "") == ".config/app.json");

	TEST_CHECK(PathStripSuffix("/usr/local/bin/dummy", "bin/dummy/") == "/usr/local");
	TEST_CHECK(PathStripSuffix("/etc/X11/screen", "display") == "/etc/X11/screen");
	TEST_CHECK(PathStripSuffix("/home/user000/.config/app.json", "") == "/home/user000/.config/app.json");

#if WIN32
	TEST_CHECK(PathJoin("/usr/local/bin/dummy", "../../../bin/bash") == "usr/bin/bash");
#else
	TEST_CHECK(PathJoin("/usr/local/bin/dummy", "../../../bin/bash") == "/usr/bin/bash");
#endif
	std::vector<std::string> elements(6);
	elements[0] = "001";
	elements[1] = "002";
	elements[2] = "003";
	elements[3] = "004";
	elements[4] = "005";
	elements[5] = "006";
	TEST_CHECK(PathJoin(elements) == "001/002/003/004/005/006");
	
	TEST_CHECK(SwapFileExtension("image.png", "pcx") == "image.pcx");
	TEST_CHECK(SwapFileExtension("config", "json") == "config.json");
	TEST_CHECK(SwapFileExtension("~/.config", "xml") == "~/.xml");
	TEST_CHECK(SwapFileExtension("/usr/bin/top", "") == "/usr/bin/top");
}

//
void test_clock_update() {
	reset_clock();

	TEST_CHECK(get_clock() == 0);
	TEST_CHECK(get_clock_dt() == 1);

	tick_clock();
	sleep_for(time_from_ms(16));
	tick_clock();

	TEST_CHECK(time_to_us(get_clock_dt()) >= 15000);

	for (int n = 0; n < 16; ++n) {
		sleep_for(time_from_ms(16));
		tick_clock();
	}
}

//
void test_vec2() {
	{
		Vec3 v3(0.f, 1.f, -1.f);
		Vec2 vf(v3);
		TEST_CHECK(TestEqual(vf.x, 0.f));
		TEST_CHECK(TestEqual(vf.y, 1.f));
		iVec2 vi(v3);
		TEST_CHECK(vi.x == 0);
		TEST_CHECK(vi.y == 1);
	}
	{
		Vec4 v4(1.f, 2.f, 3.f, 4.f);
		Vec2 vf(v4);
		TEST_CHECK(TestEqual(vf.x, 1.f));
		TEST_CHECK(TestEqual(vf.y, 2.f));
		iVec2 vi(v4);
		TEST_CHECK(vi.x == 1);
		TEST_CHECK(vi.y == 2);
	}
	{
		Vec2 u(0.5f,-0.5f);
		Vec2 v(1.f, -1.f);
		Vec2 w = u + v;
		TEST_CHECK(TestEqual(w.x, 1.5f));
		TEST_CHECK(TestEqual(w.y,-1.5f));
	}
	{
		iVec2 u(1,-1);
		iVec2 v(2, 6);
		iVec2 w = u + v;
		TEST_CHECK(w.x == 3);
		TEST_CHECK(w.y == 5);
	}
	{
		Vec2 u(0.5f,-0.5f);
		Vec2 v = u + 0.25f;
		TEST_CHECK(TestEqual(v.x, 0.75f));
		TEST_CHECK(TestEqual(v.y,-0.25f));
	}
	{
		iVec2 u(-2,-1);
		iVec2 v = u + 2;
		TEST_CHECK(v.x == 0);
		TEST_CHECK(v.y == 1);
	}
	{
		Vec2 u(0.5f,-0.5f);
		Vec2 v(1.f, -1.f);
		Vec2 w = u - v;
		TEST_CHECK(TestEqual(w.x,-0.5f));
		TEST_CHECK(TestEqual(w.y, 0.5f));
	}
	{
		iVec2 u(3, 6);
		iVec2 v(4, 8);
		iVec2 w = u - v;
		TEST_CHECK(w.x == -1);
		TEST_CHECK(w.y == -2);
	}
	{
		Vec2 u(0.5f,-0.5f);
		Vec2 v = u - 0.25f;
		TEST_CHECK(TestEqual(v.x, 0.25f));
		TEST_CHECK(TestEqual(v.y,-0.75f));
	}
	{
		iVec2 u(-2,-1);
		iVec2 v = u - 2;
		TEST_CHECK(v.x == -4);
		TEST_CHECK(v.y == -3);
	}

// inline tVec2<T> &operator+=(const tVec2<T> &b) {
// inline tVec2<T> &operator+=(const T v) {
// inline tVec2<T> &operator-=(const tVec2<T> &b) {
// inline tVec2<T> &operator-=(const T v) {
// inline tVec2<T> &operator*=(const tVec2<T> &b) {
// inline tVec2<T> &operator*=(const T v) {
// inline tVec2<T> &operator/=(const tVec2<T> &b) {
// inline tVec2<T> &operator/=(const T v) {
// inline float operator[](size_t n) const { return (&x)[n]; }
// inline float &operator[](size_t n) { return (&x)[n]; }
// typedef tVec2<float> Vec2;
// typedef tVec2<int> iVec2;
// template <typename T> tVec2<T> tVec2<T>::Zero = tVec2<T>(0, 0);
// template <typename T> tVec2<T> tVec2<T>::One = tVec2<T>(1, 1);
// template <typename T> bool operator==(const tVec2<T> &a, const tVec2<T> &b) { return a.x == b.x && a.y == b.y; }
// template <typename T> bool operator!=(const tVec2<T> &a, const tVec2<T> &b) { return a.x != b.x || a.y != b.y; }
// template <typename T> bool AlmostEqual(const tVec2<T> &a, const tVec2<T> &b, float e) { return Abs(a.x - b.x) <= e && Abs(a.y - b.y) <= e; }


// template <typename T> tVec2<T> operator*(const tVec2<T> &a, const tVec2<T> &b) { return tVec2<T>(a.x * b.x, a.y * b.y); }
// template <typename T> tVec2<T> operator*(const tVec2<T> &v, const T k) { return tVec2<T>(v.x * k, v.y * k); }
// template <typename T> tVec2<T> operator*(const T k, const tVec2<T> &v) { return v * k; }
// template <typename T> tVec2<T> operator/(const tVec2<T> &a, const tVec2<T> &b) { return tVec2<T>(a.x / b.x, a.y / b.y); }
// template <typename T> tVec2<T> operator/(const tVec2<T> &v, const T k) { return tVec2<T>(v.x / k, v.y / k); }
// template <typename T> tVec2<T> operator*(const tVec2<T> &v, const Mat3 &m);
// template <typename T> tVec2<T> Min(const tVec2<T> &v, const tVec2<T> &m) { return tVec2<T>(v.x < m.x ? v.x : m.x, v.y < m.y ? v.y : m.y); }
// template <typename T> tVec2<T> Max(const tVec2<T> &v, const tVec2<T> &m) { return tVec2<T>(v.x > m.x ? v.x : m.x, v.y > m.y ? v.y : m.y); }
// template <typename T> T Len2(const tVec2<T> &v) { return v.x * v.x + v.y * v.y; }
// template <typename T> T Len(const tVec2<T> &v);
// template <typename T> T Dot(const tVec2<T> &a, const tVec2<T> &b) { return a.x * b.x + a.y * b.y; }
// template <typename T> tVec2<T> Normalize(const tVec2<T> &v) {
// template <typename T> tVec2<T> Reverse(const tVec2<T> &v) { return tVec2<T>(-v.x, -v.y); }
// template <typename T> T Dist2(const tVec2<T> &a, const tVec2<T> &b) { return (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y); }
// template <typename T> T Dist(const tVec2<T> &a, const tVec2<T> &b);
}

//
void test_vec3_zero() {
	Vec3 zero(0, 0, 0);
	TEST_CHECK(Len(zero) == 0);
}

void test_vec3_mul() {
	Vec3 a(2.f, 4.f, 6.f);
	Vec3 b(1.f, 2.f, 3.f);
	TEST_CHECK(b * 2.f == a);
	TEST_CHECK(2.f * b == a);
}

//
void test_vec4_mul() {
	Vec4 a(3.f, 6.f, 9.f, 12.f);
	Vec4 b(1.f, 2.f, 3.f, 4.f);
	TEST_CHECK(b * 3.f == a);
	TEST_CHECK(3.f * b == a);
}

//
void test_log() {
	log("Calling Log()");
	warn("Calling Warn()");
	error("Calling Error()");
}

//
TEST_LIST = {
	{"Math", test_math},
	{"Units", test_units},
	{"String", test_string},
	{"Path_tools", test_path_tools},
	{"Vec2", test_vec2},


	{"Clock.Update", test_clock_update},

	{"Vec3.Zero", test_vec3_zero},
	{"Vec3.Mul", test_vec3_mul},

	{"Vec4.Mul", test_vec4_mul},

//	{"Log", test_log},

	{NULL, NULL},
};
