// HARFANG(R) Copyright (C) 2021 Emmanuel Julien, NWNC HARFANG. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "foundation/cmd_line.h"
#include "foundation/log.h"
#include "foundation/string.h"

#include <fmt/format.h>

#include <algorithm>
#include <numeric>
#include <sstream>

namespace hg {

class CmdLineFindPredicate {
public:
	CmdLineFindPredicate(const std::string &s) : arg(s) {}
	
	bool operator()(const CmdLineEntry &e) {
		return e.name == arg;
	}

private:
	const std::string &arg;
};

bool ParseCmdLine(const std::vector<std::string> &args, const CmdLineFormat &fmt, CmdLineContent &content) {
	bool ok = true;
	for (int i = 0; ok && (i < args.size()); ++i) {
		// resolve potential alias
		std::map<std::string, std::string>::const_iterator i_alias = fmt.aliases.find(args[i]);
		const std::string &arg_ = (i_alias == fmt.aliases.end())? args[i] : i_alias->second;
		
		CmdLineFindPredicate pred(arg_);

		// flags
		std::vector<CmdLineEntry>::const_iterator flag_i = std::find_if(fmt.flags.begin(), fmt.flags.end(), pred);

		if (flag_i != fmt.flags.end()) {
			content.flags.insert(flag_i->name);
		} else {
			// singles
			std::vector<CmdLineEntry>::const_iterator single_i = std::find_if(fmt.singles.begin(), fmt.singles.end(), pred);
			if (single_i != fmt.singles.end()) {
				++i;
				if (i < args.size()) {
					content.singles[arg_] = args[i];
				} else {
					log(fmt::format("Missing argument to {}", arg_));
					ok = false;
				}
			} else {
				// positionals
				if (content.positionals.size() < fmt.positionals.size()) {
					content.positionals.push_back(arg_);
				} else {
					log("Too many positional arguments");
					ok = false;
				}
			}
		}
	}
	return ok;
}

static std::string GetNameAndAliases(const std::string &name, const std::map<std::string, std::string> &aliases) {
	std::vector<std::string> es;
	es.push_back(name);

	for (std::map<std::string, std::string>::const_iterator e = aliases.begin(); e != aliases.end(); ++e) {
		if (e->second == name) {
			es.push_back(e->first);
		}
	}

	return join(es.begin(), es.end(), "|");
}

static std::string FormatOptional(const std::string &str, bool optional) {
	static const std::string os("["), cs("]");
	return optional ? (os + str + cs) : str;
}

std::string FormatCmdLineArgs(const CmdLineFormat &fmt) {
	std::vector<std::string> args;

	for (std::vector<CmdLineEntry>::const_iterator p = fmt.singles.begin(); p != fmt.singles.end(); ++p) {
		args.push_back(FormatOptional(fmt::format("{} (val)", GetNameAndAliases(p->name, fmt.aliases)), p->optional));
	}
	for (std::vector<CmdLineEntry>::const_iterator p = fmt.flags.begin(); p != fmt.flags.end(); ++p) {
		args.push_back(fmt::format("[{}]", GetNameAndAliases(p->name, fmt.aliases)));
	}
	for (std::vector<CmdLineEntry>::const_iterator p = fmt.positionals.begin(); p != fmt.positionals.end(); ++p) {
		args.push_back(fmt::format("<{}>", GetNameAndAliases(p->name, fmt.aliases)));
	}
	return join(args.begin(), args.end(), " ");
}

static int ArgAccumulateOp(int v, const CmdLineEntry &e) { 
	return std::max(v, static_cast<int>(e.name.length())); 
}

std::string FormatCmdLineArgsDescription(const CmdLineFormat &fmt) {
	std::vector<CmdLineEntry> args;

	std::copy(fmt.singles.begin(), fmt.singles.end(), std::back_inserter(args));
	std::copy(fmt.flags.begin(), fmt.flags.end(), std::back_inserter(args));
	std::copy(fmt.positionals.begin(), fmt.positionals.end(), std::back_inserter(args));

	const int padding = std::accumulate(args.begin(), args.end(), 0, ArgAccumulateOp);

	std::string desc;
	for (std::vector<CmdLineEntry>::const_iterator arg = args.begin(); arg != args.end(); ++arg) {
		desc += fmt::format("{}: {}\n", pad_right(arg->name, padding), arg->desc);
	}
	return desc;
}

//
bool GetCmdLineFlagValue(const CmdLineContent &cmd_content, const std::string &name) { return cmd_content.flags.find(name) != cmd_content.flags.end(); }

bool CmdLineHasSingleValue(const CmdLineContent &cmd_content, const std::string &name) {
	bool ret = false;
	std::map<std::string, std::string>::const_iterator i = cmd_content.singles.find(name);
	if (i != cmd_content.singles.end()) {
		ret = !i->second.empty();
	}
	return ret;
}

std::string GetCmdLineSingleValue(const CmdLineContent &cmd_content, const std::string &name, const std::string &default_value) {
	std::map<std::string, std::string>::const_iterator i = cmd_content.singles.find(name);
	return (i == cmd_content.singles.end()) ? default_value : i->second;
}

int GetCmdLineSingleValue(const CmdLineContent &cmd_content, const std::string &name, int default_value) {
	std::map<std::string, std::string>::const_iterator i = cmd_content.singles.find(name);
	return i == (cmd_content.singles.end()) ? default_value : strtol(i->second.c_str(), nullptr, 10);
}

float GetCmdLineSingleValue(const CmdLineContent &cmd_content, const std::string &name, float default_value) {
	std::map<std::string, std::string>::const_iterator i = cmd_content.singles.find(name);
	return (i == cmd_content.singles.end()) ? default_value : strtof(i->second.c_str(), nullptr);
}

} // namespace hg
