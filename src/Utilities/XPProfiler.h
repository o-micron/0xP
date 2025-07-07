/// --------------------------------------------------------------------------------------
/// Copyright 2025 Omar Sherif Fathy
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
/// --------------------------------------------------------------------------------------

#pragma once

#include <Utilities/XPMacros.h>
#include <Utilities/XPPlatforms.h>

#include <array>
#include <chrono>
#include <stack>
#include <thread>
#include <unordered_map>
#include <vector>

#define XP_PROFILER_TIMELINE_WIDTH 64

struct XPProfilerTrace
{
    XPProfilerTrace()
      : file("")
      , function("")
    {
    }

    XPProfilerTrace(const char* file, const char* function)
      : file(file)
      , function(function)
    {
    }

    bool operator==(const XPProfilerTrace& other) const { return file == other.file && function == other.function; }

    const char* file;
    const char* function;
};

// boost hash_combine ...
template<class T>
inline void
hash_combine(std::size_t& s, const T& v)
{
    std::hash<T> h;
    s ^= h(v) + 0x9e3779b9 + (s << 6) + (s >> 2);
}

// since c++17, string_view ..
static size_t
hash_cstr(const char* s)
{
    return std::hash<std::string_view>()(std::string_view(s, std::strlen(s)));
}

namespace std {
template<>
struct hash<XPProfilerTrace>
{
    size_t operator()(const XPProfilerTrace& trace) const
    {
        size_t result       = 0;
        size_t fileHash     = hash_cstr(trace.file);
        size_t functionHash = hash_cstr(trace.function);
        hash_combine(result, fileHash);
        hash_combine(result, functionHash);
        return result;
    }
};
}

struct XPProfilerTimeline
{
    XPProfilerTimeline();
    XPProfilerTimeline(const char* file, const char* function);
    void push(uint32_t index);
    void pop(uint32_t index);

    typedef std::chrono::time_point<std::chrono::high_resolution_clock> HRTIME;

    const char*                                                file;
    const char*                                                function;
    std::array<std::stack<HRTIME>, XP_PROFILER_TIMELINE_WIDTH> tmpValues;
    std::array<uint32_t, XP_PROFILER_TIMELINE_WIDTH>           values;
};

class XPProfiler
{
  public:
    static XPProfiler& instance();

    // delete copy and move constructors and assign operators
    XPProfiler(XPProfiler const&)            = delete; // Copy construct
    XPProfiler(XPProfiler&&)                 = delete; // Move construct
    XPProfiler& operator=(XPProfiler const&) = delete; // Copy assign
    XPProfiler& operator=(XPProfiler&&)      = delete; // Move assign

    void next();
    void entry(const char* file, const char* function, int line);
    void exit(const char* file, const char* function, int line);

    const std::unordered_map<XPProfilerTrace, XPProfilerTimeline>& getTimelines() const;
    const std::array<uint32_t, XP_PROFILER_TIMELINE_WIDTH>&        getXAxis() const;

  protected:
    XPProfiler();
    ~XPProfiler();

  private:
    std::unordered_map<XPProfilerTrace, XPProfilerTimeline> _timelines;
    std::array<uint32_t, XP_PROFILER_TIMELINE_WIDTH>        _xAxis;
    uint32_t                                                _index;
};
