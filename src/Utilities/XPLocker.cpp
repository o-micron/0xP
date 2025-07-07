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

#include <Utilities/XPLocker.h>

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

XPLocker::XPLocker() {}

XPLocker::~XPLocker() {}

XPLocker&
XPLocker::instance()
{
    static XPLocker x;
    return x;
}

void
XPLocker::entry(const char* file, const char* function, int line)
{
    puts(fmt::format("[LOCKER::ENTRY] {0}:{1} {2}", file, line, function).c_str());
}

void
XPLocker::exit(const char* file, const char* function, int line)
{
    puts(fmt::format("[LOCKER::EXIT] {0}:{1} {2}", file, line, function).c_str());
}
