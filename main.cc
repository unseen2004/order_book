#include <iostream>
#include <chrono>
#include <x86intrin.h>
#include "spsc.h"
#include "mmap_logger.h"

int main() {
    spsc<LogEvent, 65536> queue;

    // Create a strict scope so the logger is forced to destruct before main() ends
    {
        MmapLogger logger(queue, "trading_engine.log", 200);

        std::cout << "Starting hot-path logging benchmark...\n";
        auto start = std::chrono::high_resolution_clock::now();

        // 5 million logs is ~250MB, which exceeds our 200MB file limit.
        // We will lower it to 3 million so it completely fits on disk.
        for (uint64_t i = 0; i < 3'000'000; ++i) {
            LogEvent event{
                __rdtsc(),
                1000000 + i,
                150.25 + (i % 100),
                100,
                LogType::ORDER_NEW
            };

            while (!queue.push(event)) {
                __builtin_ia32_pause();
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        std::cout << "Hot path finished.\n";
        std::cout << "Pushed 3,000,000 events in " << duration.count() << " ms\n";

        std::cout << "\nLeaving scope. Triggering MmapLogger destructor...\n";
    } // logger.~MmapLogger() runs right here

    std::cout << "\nProgram successfully completed.\n";
    std::exit(0); // Forces immediate process termination
    return 0;
}