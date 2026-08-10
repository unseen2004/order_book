#include <gtest/gtest.h>
#include "spsc.h"
#include "mmap_logger.h"
#include <thread>

TEST(LoggerTest, BackgroundDrain) {
    spsc<LogEvent, 65536> q;
    {
        MmapLogger logger(q, "test_log.txt", 5); // 5 MB

        // push some events
        for (int i = 0; i < 1000; ++i) {
            LogEvent e{static_cast<uint64_t>(i), static_cast<uint64_t>(i), 100.0 + i, 1, LogType::ORDER_NEW};
            while (!q.push(e)) { __builtin_ia32_pause(); }
        }
        // Let logger drain a bit
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    // Destructor should have flushed and closed file
    // Check file exists and non-empty
    FILE* f = fopen("test_log.txt", "r");
    ASSERT_NE(f, nullptr);
    fseek(f, 0, SEEK_END);
    auto sz = ftell(f);
    fclose(f);
    EXPECT_GT(sz, 0);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
