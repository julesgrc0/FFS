#pragma once

#include "stdafx.h"

#define SEPARATOR ';'
#define START_SEPARATOR '<'
#define END_SEPARATOR '>'
#define DEFAULT_BUFFER_SIZE 1024

inline void ReadDirectory(const std::string& path, std::function<bool(const WIN32_FIND_DATAA&)> handleFunc) 
{
    WIN32_FIND_DATAA findData;
    HANDLE findHandle = FindFirstFileA((path + "\\*").c_str(), &findData);

    if (findHandle == INVALID_HANDLE_VALUE) {
        FindClose(findHandle);
        return;
    }

    do {
        if (!handleFunc(findData))
        {
            break;
        }
    } while (FindNextFileA(findHandle, &findData));

    FindClose(findHandle);
}

inline bool IsValidDirectory(const WIN32_FIND_DATAA& findData)
{
    return (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0;
}


class Timer {
public:

    static Timer* get()
    {
        if (m_instance == nullptr)
        {
            m_instance = new Timer();
        }

        return m_instance;
    }

    void start(std::string id)
    {
        this->m_timers[id] = std::chrono::high_resolution_clock::now();
    }


    void end(std::string id)
    {
        auto elapsed = std::chrono::high_resolution_clock::now() - this->m_timers[id];
        long long mili = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
        std::cout << "[" << id << "]: " << mili << " ms" << std::endl;

        this->m_timers.erase(id);
    }
private:
    static Timer* m_instance;

    Timer()  {}

    std::unordered_map<std::string, std::chrono::steady_clock::time_point> m_timers = {};
};

Timer* Timer::m_instance = nullptr;