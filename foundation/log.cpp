// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "foundation/cext.h"
#include "foundation/log.h"
#include "foundation/time.h"

#include <cstring>
#include <fmt/format.h>
#include <iostream>
#include <sstream>

namespace hg {

static int log_level = ~0;
static bool log_is_detailed = false;

void set_log_level(int level) { log_level = level; }
void set_log_detailed(bool is_detailed) { log_is_detailed = is_detailed; }
int get_log_level() { return log_level; }
bool get_log_detailed() { return log_is_detailed; }

//
static void default_log_hook(const std::string &msg, int mask, const std::string &details, void *user) {
	if (!(mask & log_level))
		return; // skip masked entries

	const time_ns now = time_now();
	const std::string timestamp = fmt::format("{}:{:3d}:{:3d}:{:3d}", now / 1000000000, (now / 1000000) % 1000, (now / 1000) % 1000, now % 1000);

	std::ostringstream m;

	m << "(" << timestamp << ") ";
	if (mask & LL_Error)
		m << "ERROR: ";
	else if (mask & LL_Warning)
		m << "WARNING: ";
	else if (mask & LL_Debug)
		m << "DEBUG: ";
	m << msg;

	if (log_is_detailed)
		if (!details.empty())
			m << "\n  Details:\n" << details;
	m << std::endl;

	std::cout << m.str();
}

//
static void (*on_log_hook)(const std::string &msg, int mask, const std::string &details, void *user) = &default_log_hook;
static void *on_log_hook_user = nullptr;

void set_log_hook(void (*on_log)(const std::string &msg, int mask, const std::string &details, void *user), void *user) {
	on_log_hook = on_log ? on_log : &default_log_hook;
	on_log_hook_user = user;
}

//
void log(const std::string &msg) { on_log_hook(msg, LL_Normal, "", on_log_hook_user); }
void log(const std::string &msg, const std::string &details) { on_log_hook(msg, LL_Normal, details, on_log_hook_user); }
void warn(const std::string &msg) { on_log_hook(msg, LL_Warning, "", on_log_hook_user); }
void warn(const std::string &msg, const std::string &details) { on_log_hook(msg, LL_Warning, details, on_log_hook_user); }
void error(const std::string &msg) { on_log_hook(msg, LL_Error, "", on_log_hook_user); }
void error(const std::string &msg, const std::string &details) { on_log_hook(msg, LL_Error, details, on_log_hook_user); }
void debug(const std::string &msg) { on_log_hook(msg, LL_Debug, "", on_log_hook_user); }
void debug(const std::string &msg, const std::string &details) { on_log_hook(msg, LL_Debug, details, on_log_hook_user); }

} // namespace hg
