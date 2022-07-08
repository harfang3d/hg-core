// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "foundation/profiler.h"
#include "foundation/log.h"
#include "foundation/math.h"

#include <algorithm>
#include <fmt/format.h>
#include <xxhash.h>

namespace hg {

static const size_t task_bucket_count = 256;
static std::vector<size_t> task_buckets[task_bucket_count];

struct Task {
	std::string name;
	std::vector<size_t> section_indexes;
};

static std::vector<Task> tasks;

struct Section {
	Section() : start(0), end(0) {}

	time_ns start, end;
	std::string details;
};

static std::vector<Section> sections;

static uint32_t frame = 0;
static ProfilerFrame last_frame_profile;

//
static size_t GetTaskBucket(const std::string &name) { return XXH32(name.data(), name.length(), 0) & 255; }

static size_t GetTaskInBucket(size_t bucket_index, const std::string &name) {
	std::vector<size_t> &bucket = task_buckets[bucket_index];

	// look into the bucket for the period
	for (std::vector<size_t>::const_iterator i = bucket.begin(); i != bucket.end(); ++i)
		if (tasks[*i].name == name)
			return *i;

	// create missing event
	const size_t idx = tasks.size();

	Task task;
	task.name = name;
	tasks.push_back(task);

	bucket.push_back(idx);
	return idx;
}

//
static bool _compare_tree_entry(const ProfilerFrame::Task &a, const ProfilerFrame::Task &b) {
	const size_t task_a_name_length = a.name.length(), task_b_name_length = b.name.length();
	const size_t shortest_length = Min(task_a_name_length, task_b_name_length);

	for (size_t i = 0; i < shortest_length; ++i) {
		const char dt = b.name[i] - a.name[i];
		if (dt)
			return dt > 0;
	}

	return task_a_name_length < task_b_name_length;
}

ProfilerFrame CaptureProfilerFrame() {
	ProfilerFrame f;
	f.frame = frame;

	const time_ns t_now = time_now();

	f.tasks.clear();
	f.sections.clear();

	const size_t task_count = tasks.size();

	if (!task_count) {
		f.start = f.end = 0;
	} else {
		f.start = f.end = sections[0].start;

		f.sections.resize(sections.size());
		for (size_t i = 0; i < sections.size(); ++i) {
			const Section &section = sections[i];
			ProfilerFrame::Section &frame_section = f.sections[i];

			frame_section.start = section.start;
			if (section.end == 0)
				frame_section.end = t_now; // fix-up pending section
			else
				frame_section.end = section.end;
			frame_section.details = section.details;

			if (frame_section.start < f.start)
				f.start = frame_section.start;
			if (frame_section.end > f.end)
				f.end = frame_section.end;
		}

		f.tasks.clear();
		f.tasks.resize(task_count);

		for (size_t i = 0; i < task_count; ++i) {
			const Task &task = tasks[i];
			ProfilerFrame::Task &frame_task = f.tasks[i];

			frame_task.name = task.name;
			frame_task.duration = 0;

			for (std::vector<size_t>::const_iterator i = task.section_indexes.begin(); i != task.section_indexes.end(); ++i) {
				const ProfilerFrame::Section &section = f.sections[*i];
				frame_task.duration += section.end - section.start;
			}

			frame_task.section_indexes = task.section_indexes;
		}

		std::sort(f.tasks.begin(), f.tasks.end(), &_compare_tree_entry);
	}
	return f;
}

//
ProfilerFrame EndProfilerFrame() {
	ProfilerFrame profiler_frame = CaptureProfilerFrame();

	for (size_t i = 0; i < task_bucket_count; ++i)
		task_buckets[i].clear();

	tasks.clear();
	sections.clear();

	++frame;

	return profiler_frame;
}

//
struct TaskReport {
	std::string name;
	size_t section_count;
	time_ns total, avg;
};

static bool _compare_task_report(const TaskReport &a, const TaskReport &b) { return a.name < b.name; }

void PrintProfilerFrame(const ProfilerFrame &frame) {
	// compile task reports
	std::vector<TaskReport> task_reports;

	for (std::vector<ProfilerFrame::Task>::const_iterator i = frame.tasks.begin(); i != frame.tasks.end(); ++i) {
		if (i->section_indexes.empty())
			continue;

		// compute section statistics
		time_ns total = 0;

		for (std::vector<size_t>::const_iterator j = i->section_indexes.begin(); j != i->section_indexes.end(); ++j) {
			const ProfilerFrame::Section &section = frame.sections[*j];
			total += section.end - section.start;
		}

		const time_ns avg = total / i->section_indexes.size();

		//
		TaskReport report;
		report.name = i->name;
		report.section_count = i->section_indexes.size();
		report.total = total;
		report.avg = avg;

		task_reports.push_back(report);
	}

	// output report
	std::sort(task_reports.begin(), task_reports.end(), &_compare_task_report);

	log(fmt::format("Profile for Frame {} (duration: {} ms)", frame.frame, time_to_ms(frame.end - frame.start)));
	for (std::vector<TaskReport>::const_iterator i = task_reports.begin(); i != task_reports.end(); ++i)
		log(fmt::format("    Task {} ({} section): total {} ms, average {} ms", i->name, i->section_count, time_to_ms(i->total), time_to_ms(i->avg)));
}

//
ProfilerSectionIndex BeginProfilerSection(const std::string &name, const std::string &section_details) {
	const size_t bucket_idx = GetTaskBucket(name);
	const size_t task_idx = GetTaskInBucket(bucket_idx, name);
	Task &task = tasks[task_idx];

	const size_t section_idx = sections.size();

	Section section;
	section.details = section_details;
	section.start = time_now();
	sections.push_back(section);

	task.section_indexes.push_back(section_idx);
	return section_idx;
}

void EndProfilerSection(ProfilerSectionIndex section_index) {
	if (section_index < sections.size() && sections[section_index].start != 0)
		sections[section_index].end = time_now();
}

//
ProfilerPerfSection::ProfilerPerfSection(const std::string &task_name, const std::string &section_details)
	: section_index(BeginProfilerSection(task_name, section_details)) {}
ProfilerPerfSection::~ProfilerPerfSection() { EndProfilerSection(section_index); }

} // namespace hg
