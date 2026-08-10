#ifndef MMAP_LOGGER_H
#define MMAP_LOGGER_H

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <thread>
#include <atomic>
#include <cstdio>
#include <cstdint>
#include <stdexcept>
#include <iostream>
#include "spsc.h"

enum class LogType : uint8_t { ORDER_NEW, ORDER_FILL, ORDER_CANCEL };

struct LogEvent {
    uint64_t timestamp;
    uint64_t order_id;
    double price;
    uint32_t qty;
    LogType type;
};

class MmapLogger {
private:
    spsc<LogEvent, 65536>& queue_;
    int fd_;
    std::size_t file_size_;
    char* buffer_;
    std::size_t offset_;

    std::atomic<bool> running_;
    std::thread worker_;

    void write_event(const LogEvent& event) {
        if (offset_ >= file_size_ - 128) return; // Prevent overflow

        int written = std::snprintf(
            buffer_ + offset_,
            file_size_ - offset_,
            "[%lu] TYPE:%d ID:%lu PX:%.2f QTY:%u\n",
            event.timestamp,
            static_cast<int>(event.type),
            event.order_id,
            event.price,
            event.qty
        );

        if (written > 0) {
            offset_ += written;
        }
    }

    void background_loop() {
        LogEvent event;

        // 1. Normal operation: spin and wait for events
        while (running_.load(std::memory_order_acquire)) {
            if (queue_.pop(event)) {
                write_event(event);
            } else {
                __builtin_ia32_pause();
            }
        }

        // 2. Shutting down: drain whatever is left in the queue
        while (queue_.pop(event)) {
            write_event(event);
        }
    }

public:
    MmapLogger(spsc<LogEvent, 65536>& q, const char* filepath, std::size_t size_mb)
        : queue_(q), offset_(0), running_(true)
    {
        file_size_ = size_mb * 1024 * 1024;

        fd_ = open(filepath, O_RDWR | O_CREAT | O_TRUNC, 0666);
        if (fd_ == -1) throw std::runtime_error("Failed to open log file");

        if (ftruncate(fd_, file_size_) == -1) {
            close(fd_);
            throw std::runtime_error("Failed to truncate file");
        }

        buffer_ = static_cast<char*>(mmap(
            nullptr, file_size_, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0
        ));

        if (buffer_ == MAP_FAILED) {
            close(fd_);
            throw std::runtime_error("mmap failed");
        }

        worker_ = std::thread(&MmapLogger::background_loop, this);
    }

    ~MmapLogger() {
        std::cout << "[Destructor] Signalling thread to stop...\n";
        running_.store(false, std::memory_order_release);

        if (worker_.joinable()) {
            worker_.join();
            std::cout << "[Destructor] Thread joined. Queue drained.\n";
        }

        std::cout << "[Destructor] Unmapping RAM (this triggers the massive OS disk flush)...\n";
        munmap(buffer_, file_size_);

        std::cout << "[Destructor] Shrinking file from 200MB to exact bytes written...\n";
        ftruncate(fd_, offset_);

        close(fd_);
        std::cout << "[Destructor] Logger fully closed.\n";
    }
};

#endif // MMAP_LOGGER_H