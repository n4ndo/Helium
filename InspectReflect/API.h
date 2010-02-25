#pragma once

#include "Common/Assert.h"

#ifdef INSPECTREFLECT_EXPORTS
#define INSPECTREFLECT_API __declspec(dllexport)
#else
#define INSPECTREFLECT_API __declspec(dllimport)
#endif

//#define INSPECTREFLECT_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined(INSPECTREFLECT_PROFILE)
# define INSPECTREFLECT_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define INSPECTREFLECT_SCOPE_TIMER(__Str)
#endif
