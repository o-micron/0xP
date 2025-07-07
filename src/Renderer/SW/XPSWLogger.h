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

#include <Renderer/SW/XPSWThirdParty.h>

#include <format>
#include <iostream>

// ----------------------------------------
// Logging Levels
// ----------------------------------------
// - 0, None
// - 1, Emergency
// - 2, Alert
// - 3, Critical
// - 4, Error
// - 5, Warning
// - 6, Notice
// - 7, Info
// - 8, Debug
// ----------------------------------------

#define XP_SW_WARNING_LEVEL_NONE      0
#define XP_SW_WARNING_LEVEL_EMERGENCY 1
#define XP_SW_WARNING_LEVEL_ALERT     2
#define XP_SW_WARNING_LEVEL_CRITICAL  3
#define XP_SW_WARNING_LEVEL_ERROR     4
#define XP_SW_WARNING_LEVEL_WARNING   5
#define XP_SW_WARNING_LEVEL_NOTICE    6
#define XP_SW_WARNING_LEVEL_INFO      7
#define XP_SW_WARNING_LEVEL_DEBUG     8

#define XP_SW_WARNING_LEVEL XP_SW_WARNING_LEVEL_ERROR

#define LOG(TAG, MSG) std::cout << TAG << MSG << std::endl;
#define LOGV(TAG, MSG, ...)                                                                                            \
    std::cout << TAG;                                                                                                  \
    fmt::print(MSG, __VA_ARGS__);                                                                                      \
    std::cout << std::endl;

#if XP_SW_WARNING_LEVEL >= XP_SW_WARNING_LEVEL_EMERGENCY
    #define LOG_EMERGENCY(MSG)       LOG("[EMERGENCY] ", MSG)
    #define LOGV_EMERGENCY(MSG, ...) LOGV("[EMERGENCY] ", MSG, __VA_ARGS__)
#else
    #define LOG_EMERGENCY(MSG)
    #define LOGV_EMERGENCY(MSG, ...)
#endif

#if XP_SW_WARNING_LEVEL >= XP_SW_WARNING_LEVEL_ALERT
    #define LOG_ALERT(MSG)       LOG("[ALERT] ", MSG)
    #define LOGV_ALERT(MSG, ...) LOGV("[ALERT] ", MSG, __VA_ARGS__)
#else
    #define LOG_ALERT(MSG)
    #define LOGV_ALERT(MSG, ...)
#endif

#if XP_SW_WARNING_LEVEL >= XP_SW_WARNING_LEVEL_CRITICAL
    #define LOG_CRITICAL(MSG)       LOG("[CRITICAL] ", MSG)
    #define LOGV_CRITICAL(MSG, ...) LOGV("[CRITICAL] ", MSG, __VA_ARGS__)
#else
    #define LOG_CRITICAL(MSG)
    #define LOGV_CRITICAL(MSG, ...)
#endif

#if XP_SW_WARNING_LEVEL >= XP_SW_WARNING_LEVEL_ERROR
    #define LOG_ERROR(MSG)       LOG("[ERROR] ", MSG)
    #define LOGV_ERROR(MSG, ...) LOGV("[ERROR] ", MSG, __VA_ARGS__)
#else
    #define LOG_ERROR(MSG)
    #define LOGV_ERROR(MSG, ...)
#endif

#if XP_SW_WARNING_LEVEL >= XP_SW_WARNING_LEVEL_WARNING
    #define LOG_WARNING(MSG)       LOG("[WARNING] ", MSG)
    #define LOGV_WARNING(MSG, ...) LOGV("[WARNING] ", MSG, __VA_ARGS__)
#else
    #define LOG_WARNING(MSG)
    #define LOGV_WARNING(MSG, ...)
#endif

#if XP_SW_WARNING_LEVEL >= XP_SW_WARNING_LEVEL_NOTICE
    #define LOG_NOTICE(MSG)       LOG("[NOTICE] ", MSG)
    #define LOGV_NOTICE(MSG, ...) LOGV("[NOTICE] ", MSG, __VA_ARGS__)
#else
    #define LOG_NOTICE(MSG)
    #define LOGV_NOTICE(MSG, ...)
#endif

#if XP_SW_WARNING_LEVEL >= XP_SW_WARNING_LEVEL_INFO
    #define LOG_INFO(MSG)       LOG("[INFO] ", MSG)
    #define LOGV_INFO(MSG, ...) LOGV("[INFO] ", MSG, __VA_ARGS__)
#else
    #define LOG_INFO(MSG)
    #define LOGV_INFO(MSG, ...)
#endif

#if XP_SW_WARNING_LEVEL >= XP_SW_WARNING_LEVEL_DEBUG
    #define LOG_DEBUG(MSG)       LOG("[DEBUG] ", MSG)
    #define LOGV_DEBUG(MSG, ...) LOGV("[DEBUG] ", MSG, __VA_ARGS__)
#else
    #define LOG_DEBUG(MSG)
    #define LOGV_DEBUG(MSG, ...)
#endif
