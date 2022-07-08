// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#pragma once

#include "foundation/time.h"

#include <string>
#include <vector>

namespace hg {

struct ProfilerFrame {
	uint32_t frame;

	struct Section {
		Section() : start(0), end(0) {}
#if __cpluplus >= 201103L
		Section(Section &&s) : thread_id(s.thread_id), start(s.start), end(s.end), details(std::move(s.details)) {}
#endif

		time_ns start, end;
		std::string details;
	};

	struct Task {
		std::string name;
		time_ns duration;
		std::vector<size_t> section_indexes;
	};

	std::vector<Section> sections;
	std::vector<Task> tasks;

	time_ns start, end;
};

//
typedef size_t ProfilerSectionIndex;

/// Begin a named profiler section. Call EndProfilerSection to end the section.
ProfilerSectionIndex BeginProfilerSection(const std::string &name, const std::string &section_details = std::string());
void EndProfilerSection(ProfilerSectionIndex section_index);

/// capture and end the current profiler frame
ProfilerFrame EndProfilerFrame();
/// capture current profiler frame without ending it
ProfilerFrame CaptureProfilerFrame();

void PrintProfilerFrame(const ProfilerFrame &frame);

//
class ProfilerPerfSection {
public:
	ProfilerPerfSection(const std::string &task_name, const std::string &section_details = std::string());
	~ProfilerPerfSection();

private:
	ProfilerSectionIndex section_index;
};

} // namespace hg
