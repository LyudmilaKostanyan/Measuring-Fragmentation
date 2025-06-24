#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <chrono>
#include <cstring>
#include <thread>
#include <jemalloc/jemalloc.h>

// === Platform-specific RSS measurement ===
#if defined(__APPLE__)
#include <mach/mach.h>
size_t getCurrentRSS() {
    mach_task_basic_info info;
    mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO,
                  reinterpret_cast<task_info_t>(&info), &infoCount) != KERN_SUCCESS)
        return 0;
    return info.resident_size / 1024;
}
#elif defined(_WIN32)
#include <windows.h>
#include <psapi.h>
size_t getCurrentRSS() {
    PROCESS_MEMORY_COUNTERS info;
    GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof(info));
    return info.WorkingSetSize / 1024;
}
#else // Linux
#include <fstream>
#include <sstream>
size_t getCurrentRSS() {
    std::ifstream status("/proc/self/status");
    std::string line;
    while (std::getline(status, line)) {
        if (line.find("VmRSS:") == 0) {
            std::istringstream iss(line);
            std::string tmp;
            size_t rss;
            iss >> tmp >> rss;
            return rss;
        }
    }
    return 0;
}
#endif

// === jemalloc stat accessor ===
size_t get_stat(const char* name) {
    size_t value;
    size_t sz = sizeof(value);
    mallctl(name, &value, &sz, nullptr, 0);
    return value;
}

int main() {
    const size_t iterations = 10000;
    const size_t max_object_size = 32 * 1024;

    // Store both pointer and size for manual tracking
    std::vector<std::pair<void*, size_t>> allocations;

    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<size_t> size_dist(1024, max_object_size);
    std::uniform_real_distribution<double> action_dist(0.0, 1.0);

    std::ofstream log("fragmentation_log.csv");
    log << "iteration,actual_allocated,mapped,rss_kb,fragmentation_percent,live_allocations\n";

    for (size_t i = 0; i < iterations; ++i) {
        if (!allocations.empty() && action_dist(gen) < 0.5) {
            size_t idx = gen() % allocations.size();
            free(allocations[idx].first);
            allocations.erase(allocations.begin() + idx);
        } else {
            size_t alloc_size = size_dist(gen);
            void* ptr = malloc(alloc_size);
            if (ptr)
                memset(ptr, 0xAA, alloc_size);
            allocations.emplace_back(ptr, alloc_size);
        }

        if (i % 100 == 0) {
            size_t actual_allocated = 0;
            for (const auto& [ptr, size] : allocations)
                actual_allocated += size;

            size_t mapped = get_stat("stats.mapped");
            size_t rss = getCurrentRSS();
            double frag = (mapped > 0) ? (mapped - actual_allocated) * 100.0 / mapped : 0.0;

            log << i << "," << actual_allocated << "," << mapped << "," << rss << "," << frag << "," << allocations.size() << "\n";

            std::cout << "Iteration " << i
                      << " | Actual Allocated: " << actual_allocated / 1024 << " KB"
                      << " | Mapped: " << mapped / 1024 << " KB"
                      << " | RSS: " << rss << " KB"
                      << " | Fragmentation: " << frag << " %"
                      << " | Live allocations: " << allocations.size()
                      << "\n";
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    for (auto& [ptr, _] : allocations)
        free(ptr);

    log.close();
    std::cout << "Log written to fragmentation_log.csv\n";
    return 0;
}
