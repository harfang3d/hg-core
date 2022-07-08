// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#pragma once

#include <string>

namespace hg {

enum ScriptParamType { SPT_Null, SPT_Int, SPT_Float, SPT_String, SPT_Bool, SPT_Enum };

struct ScriptParam {
	ScriptParam() : type(SPT_Null), iv(0) {}

	ScriptParamType type;

	union {
		bool bv;
		int iv;
		float fv;
	};

	std::string sv;
};

} // namespace hg
