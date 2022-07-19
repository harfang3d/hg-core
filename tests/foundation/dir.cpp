// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.
#include <math.h>

#define TEST_NO_MAIN
#include "acutest.h"

#include "foundation/cext.h"

#include "foundation/dir.h"

#include "foundation/path_tools.h"
#include "foundation/file.h"

#include "../utils.h"

using namespace hg;

static void test_entries(DirEntry *expected, int count, std::vector<DirEntry> &entries) {
	for(size_t j=0; j<entries.size(); j++) {
		for(int i=0; i<count; i++) {
			if((entries[j].name == expected[i].name) && (entries[j].type == expected[i].type)) {
				expected[i].size++;
			}
		}
	}
	for(int i=0; i<count; i++) {
		TEST_CHECK(expected[i].size == 1);
	}
}

void test_dir() {
	std::string base = hg::test::GetTempDirectoryName();

	TEST_CHECK(IsDir(base) == true);

	{
		std::string dirname = PathJoin(base, "hg_core_ut__00");
		TEST_CHECK(RmDir(dirname, false) == false);
		TEST_CHECK(IsDir(dirname) == false);
	}
	{
		std::string dirname = PathJoin(base, "hg_core_ut__00");

		TEST_CHECK(Exists(dirname) == false);
		TEST_CHECK(MkDir(dirname, 01755, true) == true);
		TEST_CHECK(IsDir(dirname) == true);
		TEST_CHECK(Exists(dirname) == true);
		TEST_CHECK(RmDir(dirname, true) == true);
		TEST_CHECK(RmDir(dirname, true) == false);
	}
	{
		std::vector<std::string> path(4);
		path[0] = base;
		path[1] = "hg_core_ut__00";
		path[2] = "0000";
		path[3] = "0010";

		std::string dirname = PathJoin(path);
		TEST_CHECK(MkDir(dirname, 01755, true) == false);
		TEST_CHECK(MkTree(dirname) == true);

		std::string subdirname = PathJoin(path[0], path[1]);
		TEST_CHECK(RmDir(subdirname) == false);
		TEST_CHECK(RmTree(subdirname) == true);

		TEST_CHECK(Exists(subdirname) == false);
		TEST_CHECK(Exists(dirname) == false);
	}
	{
		std::string root = PathJoin(base, "hg_core_ut__00");

		std::string path, lorem_filepath;

		path = PathJoin(root, "00", "a");
		lorem_filepath = PathJoin(path, "lorem.txt");
		TEST_CHECK(MkTree(path, 01755, true) == true);
		TEST_CHECK(StringToFile(lorem_filepath, hg::test::LoremIpsum) == true);
		TEST_CHECK(IsDir(lorem_filepath) == false);

		TEST_CHECK(MkTree(PathJoin(root, "00", "b"), 01755, true) == true);

		TEST_CHECK(MkTree(PathJoin(root, "01", "c"), 01755, true) == true);
		TEST_CHECK(MkTree(PathJoin(root, "01", "d"), 01755, true) == true);
		TEST_CHECK(CopyFile(lorem_filepath, PathJoin(root, "01", "lorem2.txt")) == true);

		TEST_CHECK(MkTree(PathJoin(root, "02", "e"), 01755, true) == true);
		TEST_CHECK(MkTree(PathJoin(root, "02", "f"), 01755, true) == true);
		TEST_CHECK(MkTree(PathJoin(root, "02", "g"), 01755, true) == true);
		TEST_CHECK(MkTree(PathJoin(root, "02", "h"), 01755, true) == true);

		TEST_CHECK(CopyDirRecursive(PathJoin(root, "03"), PathJoin(root, "02", "h")) == false);
		TEST_CHECK(CopyDirRecursive(PathJoin(root, "00"), PathJoin(root, "02", "h")) == true);

		TEST_CHECK(IsFile(lorem_filepath) == true);

		path = PathJoin(root, "02", "h");
		path = PathJoin(path, "a", "lorem.txt");
		TEST_CHECK(IsFile(path) == true);

		std::vector<DirEntry> entries = ListDir(root, DE_All);
		if (TEST_CHECK(entries.size() == 3)) {
			DirEntry expected[3] = {
				{ DE_Dir, "00", 0, 0},
				{ DE_Dir, "01", 0, 0},
				{ DE_Dir, "02", 0, 0}
			};
			test_entries(expected, 3, entries);
		}

		entries = ListDirRecursive(root, DE_File);
		if (TEST_CHECK(entries.size() == 3)) {
			DirEntry expected[3] = {
				{ DE_File, "00/a/lorem.txt", 0, 0},
				{ DE_File, "01/lorem2.txt", 0, 0},
				{ DE_File, "02/h/a/lorem.txt", 0, 0}
			};
			test_entries(expected, 3, entries);
		}
		TEST_CHECK(GetDirSize(root) == (3 * hg::test::LoremIpsum.size()));

		path = PathJoin(root, "00");
		TEST_CHECK(RmTree(path) == true);
		TEST_CHECK(Exists(path) == false);

		TEST_CHECK(GetDirSize(root) == (2 * hg::test::LoremIpsum.size()));

		entries = ListDir(path);
		TEST_CHECK(entries.empty() == true);

		TEST_CHECK(RmTree(root) == true);

		entries = ListDirRecursive(root);
		TEST_CHECK(entries.empty() == true);
	}
	{ 
		std::string root = PathJoin(base, "hg_core_ut__01"); 
		std::string src = PathJoin(root, "01");
		std::string dst = PathJoin(root, "02");
		TEST_CHECK(MkDir(PathJoin(src, "c")) == false);
		TEST_CHECK(MkTree(src) == true);
		TEST_CHECK(StringToFile(PathJoin(src, "lorem.txt"), hg::test::LoremIpsum) == true);

		TEST_CHECK(CopyDir(src, dst) == false);

		TEST_CHECK(MkDir(dst) == true);
		TEST_CHECK(CopyDir(PathJoin(root, "03"), dst) == false);
		TEST_CHECK(CopyDir(src, dst) == true);
				
		std::vector<DirEntry> entries = ListDir(dst, DE_File);
		if (TEST_CHECK(entries.size() == 1)) {
			TEST_CHECK(entries[0].name == "lorem.txt");
			TEST_CHECK(entries[0].type == DE_File);
			TEST_CHECK(entries[0].size = hg::test::LoremIpsum.size());
		}

		TEST_CHECK(RmTree(root) == true);
	}
#if !_WIN32
	{
		std::string root = PathJoin(base, "hg_core_ut__02");
		TEST_CHECK(MkTree(root, 01755) == true);

		std::string path_00 = PathJoin(root, "00");
		TEST_CHECK(MkDir(path_00, 00000) == true);
		TEST_CHECK(MkDir(PathJoin(path_00, "01"), 00755) == false);
		TEST_CHECK(MkTree(PathJoin(path_00, "02", "03"), 01755) == false);
		
		std::string path_01 = PathJoin(root, "01");
		TEST_CHECK(MkDir(path_01, 00755) == true);

		std::string lorem_filepath = PathJoin(path_01, "lorem.txt");
		TEST_CHECK(StringToFile(lorem_filepath, hg::test::LoremIpsum) == true);

		std::string path_02 = PathJoin(root, "02");
		TEST_CHECK(MkDir(path_02, 00755) == true);
		TEST_CHECK(MkDir(PathJoin(path_02, "03"), 00755) == true);

		std::string path_0103 = PathJoin(path_01, "03");
		TEST_CHECK(MkDir(path_0103, 00000) == true);

		TEST_CHECK(CopyDir(path_01, path_00) == false);
		TEST_CHECK(CopyDirRecursive(path_01, path_00) == false);
		TEST_CHECK(CopyDirRecursive(path_01, path_02) == false);

		TEST_CHECK(RmTree(root) == false);

		TEST_CHECK(chmod(path_00.c_str(), 0755) == 0);
		TEST_CHECK(chmod(path_0103.c_str(), 0755) == 0);
		TEST_CHECK(RmTree(root) == true);
	}
#endif
}
