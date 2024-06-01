#pragma once

#include <chrono>

#include "ProfilerManager.h"

namespace cpptrace {

	class ProfilingTimer
	{
	public:
		ProfilingTimer(const std::string& timer_name);
		~ProfilingTimer();

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_start_point_time;
		ProfilerManager::ProfilingResult m_result;

		bool m_stopped = false;
	};
} // cpptrace
