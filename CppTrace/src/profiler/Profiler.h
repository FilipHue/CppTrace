#pragma once

#include "ProfilingTimer.h"

namespace cpptrace_noheader {

#define PROFILE_BEGIN_SESSION(session_name, output) ProfilerManager::GetInstance().BeginSession(session_name, output)
#define PROFILE_END_SESSION() ProfilerManager::GetInstance().EndSession()

#define PROFILE_SCOPE(timer_name) ProfilingTimer timer##__LINE__(timer_name)
#define PROFILE_FUNCTION() PROFILE_SCOPE(__FUNCSIG__)
} // cpptrace_noheader
