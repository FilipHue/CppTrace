#include <iostream>
#include <algorithm>

#include "ProfilerManager.h"

namespace cpptrace_noheader {

	ProfilerManager::~ProfilerManager()
	{
		EndSession();
	}

	void ProfilerManager::BeginSession(const std::string& session_name, const std::string& output)
	{
		if (m_active_session) {
			EndSession();
		}

		m_console_output = output == CONSOLE_OUTPUT;
		m_session_name = session_name;
		m_active_session = true;

		if (m_console_output) {
			WriteHeaderConsole();
		}
		else {
			m_output_stream.open(output);
			WriteHeader();
		}
	}

	void ProfilerManager::EndSession()
	{
		if (!m_active_session) {
			return;
		}

		if (m_console_output) {
			WriteFooterConsole();
		}
		else {
			WriteFooter();
			m_output_stream.close();
		}

		m_active_session = false;
		m_timers_count = 0;
	}

	void ProfilerManager::WriteHeader()
	{
		m_output_stream << "{\"otherData\": {},\"traceEvents\": [";
	}

	void ProfilerManager::WriteHeaderConsole()
	{
		std::cout << "Starting session: " << m_session_name << std::endl;
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

	void ProfilerManager::WriteProfileConsole(const ProfilingResult& result)
	{
		std::lock_guard<std::mutex> lock(m_lock);

		std::string timer_name = result.m_timer_name;
		std::replace(timer_name.begin(), timer_name.end(), '"', '\"');

		std::cout << "Timer: " << timer_name << " - Duration: " << (result.m_end - result.m_start) << "ms" << std::endl;
	}

	void ProfilerManager::WriteFooter()
	{
		m_output_stream << "]}";
	}

	void ProfilerManager::WriteFooterConsole()
	{
		std::cout << "Ending session: " << m_session_name << std::endl;
	}

	ProfilerManager::ProfilerManager()
	{
	}

	bool ProfilerManager::IsConsoleOutput() const
	{
		return m_console_output;
	}
}
