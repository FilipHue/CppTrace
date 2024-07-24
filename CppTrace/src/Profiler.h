#pragma once

#include <algorithm>
#include <string>
#include <fstream>
#include <mutex>

namespace cpptrace {

	class ProfilerManager
	{
	public:
		struct ProfilingResult {
			std::string m_timer_name;
			long long m_start, m_end;
			size_t m_thread_id;
		};

	public:
		ProfilerManager(ProfilerManager& other) = delete;
		~ProfilerManager()
		{
			EndSession();
		};
		void operator=(const ProfilerManager& other) = delete;

		void BeginSession(const std::string& session_name, const std::string& output_filepath = "result.json")
		{
			if (m_active_session) {
				EndSession();
			}

			m_output_stream.open(output_filepath);
			m_session_name = session_name;
			m_active_session = true;
			WriteHeader();
		}
		void EndSession()
		{
			if (!m_active_session) {
				return;
			}

			WriteFooter();
			m_output_stream.close();
			m_active_session = false;
			m_timers_count = 0;
		}

		void WriteHeader()
		{
			m_output_stream << "{\"otherData\": {},\"traceEvents\": [";
		}
		void WriteProfile(const ProfilingResult& result)
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
		void WriteFooter()
		{
			m_output_stream << "]}";
		}

		static ProfilerManager& GetInstance()
		{
			static ProfilerManager instance;

			return instance;
		}

	protected:
		ProfilerManager() = default;

	private:
		std::ofstream m_output_stream;
		std::string m_session_name = "None";
		bool m_active_session = false;
		int m_timers_count = 0;
		std::mutex m_lock;
	};

	class ProfilingTimer
	{
	public:
		ProfilingTimer(const std::string& timer_name)
			: m_result({ timer_name, 0, 0, 0 }), m_start_point_time(std::chrono::high_resolution_clock::now()) {};
		~ProfilingTimer()
		{
			if (!m_stopped) {
				auto end_point_time = std::chrono::high_resolution_clock::now();

				m_result.m_start = m_start_point_time.time_since_epoch().count();
				m_result.m_end = end_point_time.time_since_epoch().count();
				m_result.m_thread_id = std::hash<std::thread::id>{}(std::this_thread::get_id());

				ProfilerManager::GetInstance().WriteProfile(m_result);

				m_stopped = true;
			}
		};

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_start_point_time;
		ProfilerManager::ProfilingResult m_result;

		bool m_stopped = false;
	};

#define PROFILE_BEGIN_SESSION(session_name, output_filepath) ProfilerManager::GetInstance().BeginSession(session_name, output_filepath)
#define PROFILE_END_SESSION() ProfilerManager::GetInstance().EndSession()

#define PROFILE_SCOPE(timer_name) ProfilingTimer timer##__LINE__(timer_name)
#define PROFILE_FUNCTION() PROFILE_SCOPE(__FUNCSIG__)
} // cpptrace
