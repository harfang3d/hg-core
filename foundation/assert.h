// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#pragma once

#include <string>

namespace hg {

/// Trigger a system assert error (may be swapped by a platform specialized version).
extern void (*trigger_assert)(const std::string &source, int line, const std::string &function, const std::string &condition, const std::string &message);

} // namespace hg

//#define ALWAYS_ENABLE_ASSERT

#if defined(ALWAYS_ENABLE_ASSERT) || defined(_DEBUG) || defined(MIXED_RELEASE)

#define HG_ASSERT(_EXP_)                                                                                                                                    \
	do {                                                                                                                                                    \
		if (!(_EXP_)) {                                                                                                                                     \
			hg::trigger_assert(__FILE__, __LINE__, __FUNCTION__, #_EXP_, "");																				\
		}																																					\
	} while (0)

#define HG_ASSERT_IF(_CND_, _EXP_)																															\
	do {																																					\
		if (_CND_)                                                                                                                                          \
			if (!(_EXP_)) {                                                                                                                                 \
				hg::trigger_assert(__FILE__, __LINE__, __FUNCTION__, #_EXP_, "");																			\
			}																																				\
	} while (0)

#define HG_ASSERT_MSG(_EXP_, _MSG_)																															\
	do {                                                                                                                                                    \
		if (!(_EXP_)) {                                                                                                                                     \
			hg::trigger_assert(__FILE__, __LINE__, __FUNCTION__, #_EXP_, _MSG_);                                                                            \
		}																																					\
	} while (0)

#define HG_ASSERT_ALWAYS(_MSG_) hg::trigger_assert(__FILE__, __LINE__, __FUNCTION__, "ALWAYS", _MSG_)

#else

#define HG_ASSERT(_EXP_) false
#define HG_ASSERT_IF(_CND_, _EXP_) false
#define HG_ASSERT_MSG(_EXP_, _MSG_) false
#define HG_ASSERT_ALWAYS(_MSG_) false

#endif

#define HG_RASSERT(_EXP_)                                                                                                                                     \
	do {                                                                                                                                                       \
		if (!(_EXP_))                                                                                                                                          \
			hg::trigger_assert(__FILE__, __LINE__, __FUNCTION__, #_EXP_, nullptr);                                                                             \
	} while (0)

#define HG_RASSERT_MSG(_EXP_, _MSG_)                                                                                                                          \
	do {                                                                                                                                                       \
		if (!(_EXP_))                                                                                                                                          \
			hg::trigger_assert(__FILE__, __LINE__, __FUNCTION__, #_EXP_, _MSG_);                                                                               \
	} while (0)
