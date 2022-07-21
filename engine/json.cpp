// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "foundation/file_rw_interface.h"
#include "foundation/rw_interface.h"
#include "foundation/file.h"

#include "engine/assets_rw_interface.h"

#include <rapidjson/document.h>
#include <rapidjson/writer.h>

namespace hg {

bool LoadJson(const Reader &ir, const Handle &h, rapidjson::Document &doc) {
	if (!ir.is_valid(h))
		return false;

	std::string str;
	if (!Read(ir, h, str))
		return false;

	doc.Parse(str);
	return true;
}

bool LoadJsonFromFile(const std::string &path, rapidjson::Document &doc) {
	return LoadJson(g_file_reader, ScopedReadHandle(g_file_read_provider, path, true), doc);
}

bool LoadJsonFromAssets(const std::string &name, rapidjson::Document &doc) {
	return LoadJson(g_assets_reader, ScopedReadHandle(g_assets_read_provider, name, true), doc);
}

bool SaveJsonToFile(const rapidjson::Value &js, const std::string &path) {
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

	js.Accept(writer);
	const std::string js_ = buffer.GetString();

	return StringToFile(path, js_);
}

} // namespace hg
