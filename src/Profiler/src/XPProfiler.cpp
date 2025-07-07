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

#include <Profiler/XPProfiler.h>

#include <Utilities/XPLogger.h>

#include <sstream>
#include <stack>
#include <stdio.h>
#include <unordered_map>

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wall"
    #pragma clang diagnostic ignored "-Weverything"
#endif
#define TRACY_ENABLE
#define TRACY_FIBERS
#include <tracy/TracyClient.cpp>
#include <tracy/tracy/Tracy.hpp>
#include <tracy/tracy/TracyC.h>
#ifdef __clang__
    #pragma clang diagnostic pop
#endif

#define HASH_TRACE_STREAM(A, B, C)                                                                                     \
    std::stringstream ss;                                                                                              \
    ss << A << ':' << B << ':' << C;

struct Trace
{
    std::string str;
};

struct Context
{
    std::unordered_map<std::string, ___tracy_source_location_data> sourceLocations;
    std::stack<TracyCZoneCtx>                                      zones;
};

Context* context = nullptr;

XP_EXTERN XP_ENGINE_PROFILER_API void
XPProfilerInitialize()
{
    context = new Context();
    XP_LOG(XPLoggerSeverityInfo, "XPProfilerInitialize");
}

XP_EXTERN XP_ENGINE_PROFILER_API void
XPProfilerPush(const char* file, const char* function, int line)
{
    const char* name = function;
    HASH_TRACE_STREAM(file, function, line);
    if (!context->sourceLocations.contains(ss.str())) {
        context->sourceLocations[ss.str()] = { name, function, file, (uint32_t)line, 0 };
    }
    TracyCZoneCtx ctx = ___tracy_emit_zone_begin(&context->sourceLocations[ss.str()], 1);
    context->zones.push(ctx);
}

XP_EXTERN XP_ENGINE_PROFILER_API void
XPProfilerPop(const char* file, const char* function, int line)
{
    HASH_TRACE_STREAM(file, function, line);
    ___tracy_emit_zone_end(context->zones.top());
    context->zones.pop();
}

XP_EXTERN XP_ENGINE_PROFILER_API void
XPProfilerFinalize()
{
    XP_LOG(XPLoggerSeverityInfo, "XPProfilerFinalize");
    delete context;
}