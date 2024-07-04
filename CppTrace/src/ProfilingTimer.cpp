#include <thread>

#include "ProfilingTimer.h"

namespace cpptrace {

	ProfilingTimer::ProfilingTimer(const std::string& timer_name)
		: m_result({ timer_name, 0, 0, 0 }), m_start_point_time(std::chrono::high_resolution_clock::now())
	{

	}

	ProfilingTimer::~ProfilingTimer()
	{
		if (!m_stopped) {
			auto end_point_time = std::chrono::high_resolution_clock::now();

			m_result.m_start = m_start_point_time.time_since_epoch().count();
			m_result.m_end = end_point_time.time_since_epoch().count();
			m_result.m_thread_id = std::hash<std::thread::id>{}(std::this_thread::get_id());

			ProfilerManager::GetInstance().WriteProfile(m_result);

			m_stopped = true;
		}
	}
}
