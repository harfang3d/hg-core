// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "foundation/assert.h"

#include <cassert>
#include <fmt/ostream.h>
#include <iostream>

namespace hg {

static void generic_trigger_assert(const std::string &, int, const std::string &, const std::string &condition, const std::string &message) {
	fmt::print(std::cerr, "Assertion failed: {}\n", condition);
	if (!message.empty())
		fmt::print(std::cerr, "    Reason: {}\n", message);
	assert(false);
}

void (*trigger_assert)(
	const std::string &source, int line, const std::string &function, const std::string &condition, const std::string &message) = &generic_trigger_assert;

} // namespace hg
