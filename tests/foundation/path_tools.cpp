// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#define TEST_NO_MAIN
#include "acutest.h"

#include "foundation/path_tools.h"

using namespace hg;

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
