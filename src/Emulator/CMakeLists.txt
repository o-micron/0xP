cmake_minimum_required(VERSION 4.0)
project(Emulator VERSION 0.1.0 LANGUAGES C)

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# EMULATOR
# ---------------------------------------------------------------------------------------------------------------------------------------------------
set(XP_EMULATOR_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/XPEmulatorBus.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/XPEmulatorCommon.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/XPEmulatorDecoder.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/XPEmulatorElfLoader.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/XPEmulatorHostMappedMemory.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/XPEmulatorMemory.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/XPEmulatorProcessor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/XPEmulatorUART.c
)

set(XP_EMULATOR_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Emulator/XPEmulatorBus.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Emulator/XPEmulatorCommon.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Emulator/XPEmulatorConfig.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Emulator/XPEmulatorDecoder.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Emulator/XPEmulatorElfLoader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Emulator/XPEmulatorEnums.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Emulator/XPEmulatorHostMappedMemory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Emulator/XPEmulatorInstruction.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Emulator/XPEmulatorLogger.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Emulator/XPEmulatorMemory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Emulator/XPEmulatorProcessor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Emulator/XPEmulatorSyscalls.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Emulator/XPEmulatorUART.h
)

add_library(XPEmulator STATIC ${XP_EMULATOR_SOURCES} ${XP_EMULATOR_HEADERS})
target_include_directories(XPEmulator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_property(TARGET XPEmulator
    PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded"
)
# ---------------------------------------------------------------------------------------------------------------------------------------------------


# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Launcher
# ---------------------------------------------------------------------------------------------------------------------------------------------------
if (APPLE)
    add_executable(XPLauncher ${CMAKE_CURRENT_SOURCE_DIR}/launcher/src/launcher.c)
    target_include_directories(XPLauncher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_directories(XPLauncher PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
    target_link_libraries(XPLauncher PRIVATE XPEmulator)
    add_dependencies(XPLauncher XPEmulator)
endif()
# ---------------------------------------------------------------------------------------------------------------------------------------------------
