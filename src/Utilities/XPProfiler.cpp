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

#include <Utilities/XPProfiler.h>

#include <Profiler/XPProfiler.h>

#include <chrono>
#include <numeric>

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wall"
    #pragma clang diagnostic ignored "-Weverything"
#endif
#define FMT_HEADER_ONLY
#include <fmt/format.h>
#ifdef __clang__
    #pragma clang diagnostic pop
#endif

XPProfiler::XPProfiler()
  : _index(0)
{
    std::iota(_xAxis.begin(), _xAxis.end(), 0);
}

XPProfiler::~XPProfiler() {}

XPProfiler&
XPProfiler::instance()
{
    static XPProfiler x;
    return x;
}

void
XPProfiler::next()
{
    _index = (_index + 1) % XP_PROFILER_TIMELINE_WIDTH;
}

void
XPProfiler::entry(const char* file, const char* function, int line)
{
    XP_UNUSED(line)

    XPProfilerPush(file, function, line);

    XPProfilerTrace trace(file, function);
    _timelines[trace].push(_index);
}

void
XPProfiler::exit(const char* file, const char* function, int line)
{
    XP_UNUSED(line)

    XPProfilerTrace trace(file, function);
    _timelines[trace].pop(_index);

    XPProfilerPop(file, function, line);
}

const std::unordered_map<XPProfilerTrace, XPProfilerTimeline>&
XPProfiler::getTimelines() const
{
    return _timelines;
}

const std::array<uint32_t, XP_PROFILER_TIMELINE_WIDTH>&
XPProfiler::getXAxis() const
{
    return _xAxis;
}

XPProfilerTimeline::XPProfilerTimeline()
  : file("")
  , function("")
{
    tmpValues.fill(std::stack<HRTIME>());
    values.fill(0);
}

XPProfilerTimeline::XPProfilerTimeline(const char* file, const char* function)
  : file(file)
  , function(function)
{
    tmpValues.fill(std::stack<HRTIME>());
    values.fill(0);
}

void
XPProfilerTimeline::push(uint32_t index)
{
    auto now = std::chrono::high_resolution_clock::now();
    tmpValues[index].push(now);
}

void
XPProfilerTimeline::pop(uint32_t index)
{
    if (tmpValues[index].empty()) return;

    auto endTime   = std::chrono::high_resolution_clock::now();
    auto startTime = tmpValues[index].top();
    tmpValues[index].pop();
    values[index] =
      static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count());
}
