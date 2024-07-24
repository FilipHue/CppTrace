#pragma once

#include <string>
#include <fstream>
#include <mutex>

namespace cpptrace_noheader {

	constexpr auto CONSOLE_OUTPUT = "console";

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
		~ProfilerManager();
		void operator=(const ProfilerManager& other) = delete;

		void BeginSession(const std::string& session_name, const std::string& output_filepath = "result.json");
		void EndSession();

		void WriteHeader();
		void WriteHeaderConsole();
		void WriteProfile(const ProfilingResult& result);
		void WriteProfileConsole(const ProfilingResult& result);
		void WriteFooterConsole();
		void WriteFooter();

		bool IsConsoleOutput() const;

		static ProfilerManager& GetInstance()
		{
			static ProfilerManager instance;

			return instance;
		}

	protected:
		ProfilerManager();

	private:
		std::ofstream m_output_stream;
		std::string m_session_name = "None";
		bool m_active_session = false;
		bool m_console_output = false;
		int m_timers_count = 0;
		std::mutex m_lock;
	};
} // cpptrace_noheader
