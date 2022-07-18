// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.
#include <math.h>
#include <stdlib.h>

#define TEST_NO_MAIN
#include "acutest.h"

#include "foundation/cext.h"

#include "foundation/file.h"

#include "foundation/path_tools.h"
#include "foundation/data.h"

using namespace hg;

static std::string CreateTempFilepath() {
	std::string out;
#if WIN32
	DWORD temp_path_len;
	TCHAR temp_path[MAX_PATH];
	
	DWORD temp_filename_len;
	TCHAR temp_filename[MAX_PATH];

	temp_path_len = GetTempPath(MAX_PATH, temp_path);
	if ((temp_path_len == 0) || (temp_path_len >= MAX_PATH)) {
		temp_path[0] = '\0';
		temp_path_len = 0;
	}

	temp_filename_len = GetTempFileName(temp_path, TEXT("hg_core"), 0, temp_filename);
	if (temp_filename_len == 0) {
		return "dummy.txt";
	}

#ifdef UNICODE
	size_t len;
	wcstombs_s(&len, NULL, 0, &temp_filename[0], temp_filename_len);
	out.resize(len);
	wcstombs_s(&len, out.data(), len, &temp_filename[0], temp_filename_len);
#else
	out = temp_filename;
#endif

#else
	out = tmpnam(NULL);
#endif
	return out;
}

static const char g_dummy_data[37] = "abcdefghijklmnopqrstuvwxyz0123456789";

 void CreateDummyFile(std::string &filename, size_t &size) { 
	
	filename = CreateTempFilepath();
	FILE *f = fopen(filename.c_str(), "wb");
	if (f) {
		size = fwrite(g_dummy_data, 1, 37, f);
		fclose(f);
	}
}

void test_file() {
	{
		File f = Open("invalid.txt");
		TEST_CHECK(f.ref == invalid_gen_ref);
		TEST_CHECK(Close(f) == false);
		TEST_CHECK(IsValid(f) == false);
		TEST_CHECK(GetSize(f) == 0);

		FileInfo info = GetFileInfo("invalid.txt");
		TEST_CHECK(info.is_file == false);
		TEST_CHECK(info.size == 0);
		TEST_CHECK(info.created == 0);
		TEST_CHECK(info.modified == 0);
	}
	{
		File f = Open("invalid.txt", true);
		TEST_CHECK(f.ref == invalid_gen_ref);
		TEST_CHECK(Close(f) == false);
		TEST_CHECK(IsValid(f) == false);
	}
	{
		std::string filename;
		size_t size;
		CreateDummyFile(filename, size);

		FileInfo info = GetFileInfo(filename);
		TEST_CHECK(info.is_file == true);
		TEST_CHECK(info.size == size);

		TEST_CHECK(IsFile(filename) == true);
		TEST_CHECK(IsFile(GetFilePath(filename)) == false);

		File f = Open(filename, true);
		uint8_t dummy = 'A';

		TEST_CHECK(GetSize(f) == size);

		TEST_CHECK(Write(f, &dummy, 1) == 0);

		std::vector<char> buffer(size);

		TEST_CHECK(IsEOF(f) == false);
		TEST_CHECK(Tell(f) == 0);

		TEST_CHECK(Read(f, buffer.data(), size) == size);
		TEST_CHECK(memcmp(buffer.data(), g_dummy_data, size) == 0);

		memset(buffer.data(), 0, size);

		TEST_CHECK(Tell(f) == size);

		uint8_t byte = 0xff;
		TEST_CHECK(Read(f, &byte, 1) == 0);
		TEST_CHECK(IsEOF(f) == true);

		TEST_CHECK(Seek(f, 4, SM_Start));
		TEST_CHECK(Tell(f) == 4);
		TEST_CHECK(Seek(f, 4, SM_Current));
		TEST_CHECK(Tell(f) == 8);
		TEST_CHECK(Seek(f, -1, SM_Current));
		TEST_CHECK(Tell(f) == 7);
		TEST_CHECK(Seek(f, -4, SM_End));
		TEST_CHECK(Tell(f) == (size - 4));

		Rewind(f);
		TEST_CHECK(Tell(f) == 0);

		TEST_CHECK(IsValid(f));
		TEST_CHECK(Close(f));

		f = OpenText(filename);
		TEST_CHECK(IsValid(f));
		TEST_CHECK(Read(f, buffer.data(), size) == size);
		TEST_CHECK(memcmp(buffer.data(), g_dummy_data, size) == 0);
		TEST_CHECK(Close(f));

		TEST_CHECK(Unlink(filename) == true);
		TEST_CHECK(IsFile(filename) == false);
	}

	{
		std::string filename = CreateTempFilepath();
		
		File f = OpenWriteText(filename);
		TEST_CHECK(Write(f, g_dummy_data, 36) == 36);
		Close(f);

		f = OpenAppendText(filename);
		TEST_CHECK(WriteStringAsText(f, g_dummy_data) == true);
		Close(f);

		FileInfo info;
		info = GetFileInfo(filename);
		TEST_CHECK(info.is_file == true);
		TEST_CHECK(info.size == 72);

		{
			std::string str = FileToString(filename);
			TEST_CHECK(memcmp(&str[0], g_dummy_data, 36) == 0);
			TEST_CHECK(memcmp(&str[36], g_dummy_data, 36) == 0);
		}

		{
			Data in;
			TEST_CHECK(FileToData("invalid.txt", in) == false);
		
			TEST_CHECK(FileToData(filename, in) == true);
			in.SetCursor(0);
			TEST_CHECK(memcmp(in.GetCursorPtr(), g_dummy_data, 36) == 0);
			in.SetCursor(36);
			TEST_CHECK(memcmp(in.GetCursorPtr(), g_dummy_data, 36) == 0);
		}

		uint64_t data = 0xcafe0000cdcd;
		f = OpenWrite(filename);
		TEST_CHECK(Write(f, data) == true);
		Close(f);

		info = GetFileInfo(filename);
		TEST_CHECK(info.is_file == true);
		TEST_CHECK(info.size == sizeof(uint64_t));

		f = Open(filename);
		TEST_CHECK(Read<uint64_t>(f) == data);
		Close(f);

		TEST_CHECK(IsFile(filename) == true);
		TEST_CHECK(Unlink(filename) == true);
	}

	{
		std::string filename = CreateTempFilepath();

		std::string input(g_dummy_data);

		File f = OpenWrite(filename);
		TEST_CHECK(WriteString(f, input) == true);
		Close(f);

		f = Open(filename);
		TEST_CHECK(ReadString(f) == input);
		Close(f);

		Unlink(filename);
	}

	{
		std::string filename_0 = CreateTempFilepath();
		std::string filename_1 = CreateTempFilepath();

		TEST_CHECK(filename_0 != filename_1);

		std::string input(g_dummy_data);
		TEST_CHECK(StringToFile(filename_0, input) == true);
		TEST_CHECK(CopyFile(filename_0, filename_1) == true);
		TEST_CHECK(FileToString(filename_1) == input);

		Unlink(filename_0);
		Unlink(filename_1);
	}
}
/*

bool CopyFile(const std::string &src, const std::string &dst);
std::string FileToString(const std::string &path, bool silent = false);
bool StringToFile(const std::string &path, const std::string &str);
Data FileToData(const std::string &path, bool silent = false);
struct ScopedFile {
	ScopedFile(File file) : f(file) {}
	~ScopedFile() { Close(f); }

	operator const File &() const { return f; }
	operator bool() const { return IsValid(f); }

	File f;
};
*/