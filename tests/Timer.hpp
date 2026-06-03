#pragma once

#include <chrono>
#include <iostream>


class Timer {
    std::chrono::high_resolution_clock::time_point m_start;
    std::string m_message;

  public:
    Timer() : m_start(std::chrono::high_resolution_clock::now()) {}
    Timer(std::string const & message) : m_start(std::chrono::high_resolution_clock::now()), m_message(message) {}
    ~Timer() {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - m_start;
       
        std::cout << std::format("{:<40}{:>9.5f} ms\n", m_message, duration.count());
    }

    Timer(const Timer&) = delete;
    Timer(Timer&&) = delete;

    Timer& operator=(const Timer&) = delete;
    Timer& operator=(Timer&&) = delete;
};