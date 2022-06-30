// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#pragma once

#include <string>

namespace hg {

enum LogLevel { LL_Normal = 0x1, LL_Warning = 0x2, LL_Error = 0x4, LL_Debug = 0x8, LL_All = 0xff };

void log(const std::string &msg);
void log(const std::string &msg, const std::string &details);
void warn(const std::string &msg);
void warn(const std::string &msg, const std::string &details);
void error(const std::string &msg);
void error(const std::string &msg, const std::string &details);
void debug(const std::string &msg);
void debug(const std::string &msg, const std::string &details);

void set_log_hook(void (*on_log)(const std::string &msg, int mask, const std::string &details, void *user), void *user);

/// Get log mask.
int get_log_level();
/// Set log mask.
void set_log_level(int log_level);

/// Is log output detailed.
bool get_log_detailed();
/// Enable detailed log output.
void set_log_detailed(bool is_detailed);

} // namespace hg
