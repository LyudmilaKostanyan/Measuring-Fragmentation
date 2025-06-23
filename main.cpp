#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <cstring>

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
            return rss; // kB
        }
    }
    return 0;
}
#endif

int main(int argc, char** argv) {
    size_t iterations = 10000;
    size_t max_object_size = 32 * 1024;

    std::vector<void*> allocations;
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<size_t> size_dist(1024, max_object_size);
    std::uniform_real_distribution<double> action_dist(0.0, 1.0);

    std::ofstream log_file("fragmentation_log.csv");
    log_file << "iteration,memory_usage_kb,allocated_objects\n";

    for (size_t i = 0; i < iterations; ++i) {
        if (!allocations.empty() && action_dist(gen) < 0.5) {
            size_t idx = gen() % allocations.size();
            free(allocations[idx]);
            allocations.erase(allocations.begin() + idx);
        } else {
            size_t alloc_size = size_dist(gen);
            void* ptr = malloc(alloc_size);
            if (ptr != nullptr) {
                memset(ptr, 0xAA, alloc_size);
                allocations.push_back(ptr);
            }
        }

        if (i % 100 == 0) {
            size_t mem_usage = getCurrentRSS();
            log_file << i << "," << mem_usage << "," << allocations.size() << "\n";
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    for (auto& alloc : allocations)
        free(alloc);

    log_file.close();
    std::cout << "Fragmentation log saved to fragmentation_log.csv\n";
}
