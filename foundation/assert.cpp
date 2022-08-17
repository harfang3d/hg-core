// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "foundation/assert.h"

#include <fmt/ostream.h>
#include <iostream>

#include "extern/debug-trap.h"

namespace hg {

static void generic_trigger_assert(const std::string &source, int line, const std::string &function, const std::string &condition, const std::string &message) {
	fmt::print(std::cerr, "Assertion failed in {} at {}:{}: {}\n", function, source, line, condition);

	if (!message.empty()) {
		fmt::print(std::cerr, "    Reason: {}\n", message);
	}

	psnip_trap();
}

void (*trigger_assert)(
	const std::string &source, int line, const std::string &function, const std::string &condition, const std::string &message) = &generic_trigger_assert;

} // namespace hg
