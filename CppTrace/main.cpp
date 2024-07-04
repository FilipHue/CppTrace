#include <iostream>
#include <thread>
#include <vector>

#include "src/Profiler.h"

using namespace cpptrace;

int main() {
    PROFILE_BEGIN_SESSION("Test Session", "results.json");

    {
        PROFILE_FUNCTION();  // Profiles the main function scope
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate work
    }

    auto thread_func = []() {
        PROFILE_FUNCTION();  // Profiles this thread function scope
        std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Simulate work
        };

    {
        PROFILE_SCOPE("Single Threaded Section");
        std::this_thread::sleep_for(std::chrono::milliseconds(150)); // Simulate work
    }

    {
        PROFILE_SCOPE("Multi-threaded Section");
        std::vector<std::thread> threads;
        for (int i = 0; i < 5; ++i) {
            threads.emplace_back(thread_func);
        }
        for (auto& thread : threads) {
            thread.join();
        }
    }

    PROFILE_END_SESSION();
    return 0;
}