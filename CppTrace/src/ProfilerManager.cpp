#include <algorithm>

#include "ProfilerManager.h"

namespace cpptrace {

	ProfilerManager* ProfilerManager::m_instance = nullptr;

	ProfilerManager::~ProfilerManager()
	{
		EndSession();
	}

	void ProfilerManager::BeginSession(const std::string& session_name, const std::string& output_filepath)
	{
		if (m_active_session) {
			EndSession();
		}

		m_output_stream.open(output_filepath);
		m_session_name = session_name;
		m_active_session = true;
		WriteHeader();
	}

	void ProfilerManager::EndSession()
	{
		if (!m_active_session) {
			return;
		}

		WriteFooter();
		m_output_stream.close();
		m_active_session = false;
		m_timers_count = 0;
	}

	void ProfilerManager::WriteHeader()
	{
		m_output_stream << "{\"otherData\": {},\"traceEvents\": [";
	}

	void ProfilerManager::WriteProfile(const ProfilingResult& result)
	{
		std::lock_guard<std::mutex> lock(m_lock);

		if (m_timers_count++ > 0) {
			m_output_stream << ",";
		}

		std::string timer_name = result.m_timer_name;
		std::replace(timer_name.begin(), timer_name.end(), '"', '\"');

		m_output_stream << "{";
		m_output_stream << "\"cat\":\"function\",";
		m_output_stream << "\"dur\":" << (result.m_end - result.m_start) << ',';
		m_output_stream << "\"name\":\"" << timer_name << "\",";
		m_output_stream << "\"ph\":\"X\",";
		m_output_stream << "\"pid\":0,";
		m_output_stream << "\"tid\":" << result.m_thread_id << ",";
		m_output_stream << "\"ts\":" << result.m_start;
		m_output_stream << "}";
	}

	void ProfilerManager::WriteFooter()
	{
		m_output_stream << "]}";
	}

	ProfilerManager* ProfilerManager::GetInstance()
	{
		if (m_instance == nullptr) {
			m_instance = new ProfilerManager();
		}

		return m_instance;
	}

	ProfilerManager::ProfilerManager()
	{
	}
}
