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

#include <Utilities/XPPlatforms.h>

enum XPLoggerSeverity
{
    XPLoggerSeverityInfo,
    XPLoggerSeverityTrace,
    XPLoggerSeverityWarning,
    XPLoggerSeverityError,
    XPLoggerSeverityFatal,
    XPLoggerSeverityBreakPoint
};

#define XP_LOG(Severity, Text)                                                                                         \
    switch (Severity) {                                                                                                \
        case XPLoggerSeverityInfo: printf("[INFO] %s\n", Text); break;                                                 \
        case XPLoggerSeverityTrace: printf("[TRACE] %s\n", Text); break;                                               \
        case XPLoggerSeverityWarning: printf("[WARNING] %s\n", Text); break;                                           \
        case XPLoggerSeverityError: printf("[ERROR] %s\n", Text); break;                                               \
        case XPLoggerSeverityFatal: {                                                                                  \
            printf("[FATAL] %s\n", Text);                                                                              \
            exit(-1);                                                                                                  \
        } break;                                                                                                       \
        case XPLoggerSeverityBreakPoint: {                                                                             \
            printf("[BREAKPOINT] %s\n", Text);                                                                         \
            XPDebugBreak();                                                                                            \
        } break;                                                                                                       \
    }

#define XP_LOGV(Severity, Text, ...)                                                                                   \
    switch (Severity) {                                                                                                \
        case XPLoggerSeverityInfo:                                                                                     \
            printf("[INFO] ");                                                                                         \
            printf(Text "\n", ##__VA_ARGS__);                                                                          \
            break;                                                                                                     \
        case XPLoggerSeverityTrace:                                                                                    \
            printf("[TRACE] ");                                                                                        \
            printf(Text "\n", ##__VA_ARGS__);                                                                          \
            break;                                                                                                     \
        case XPLoggerSeverityWarning:                                                                                  \
            printf("[WARNING] ");                                                                                      \
            printf(Text "\n", ##__VA_ARGS__);                                                                          \
            break;                                                                                                     \
        case XPLoggerSeverityError:                                                                                    \
            printf("[ERROR] ");                                                                                        \
            printf(Text "\n", ##__VA_ARGS__);                                                                          \
            break;                                                                                                     \
        case XPLoggerSeverityFatal: {                                                                                  \
            printf("[FATAL] ");                                                                                        \
            printf(Text "\n", ##__VA_ARGS__);                                                                          \
            exit(-1);                                                                                                  \
        } break;                                                                                                       \
    }