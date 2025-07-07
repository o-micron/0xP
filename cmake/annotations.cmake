function(JOIN VALUES GLUE OUTPUT)
  string (REGEX REPLACE "([^\\]|^);" "\\1${GLUE}" _TMP_STR "${VALUES}")
  string (REGEX REPLACE "[\\](.)" "\\1" _TMP_STR "${_TMP_STR}") #fixes escaping
  set (${OUTPUT} "${_TMP_STR}" PARENT_SCOPE)
endfunction()

function(GENERATE_ANNOTATIONS_DEFINITIONS VALUES CLANG_DEFINITIONS EMPTY_DEFINITIONS)
    foreach(VALUE ${VALUES})
        string(APPEND CLANG_DEFINITIONS_CODE "\t#define ${VALUE} __attribute__((optnone)) __attribute__((used)) __attribute__((annotate(\"${VALUE}\")))\n")
        string(APPEND EMPTY_DEFINITIONS_CODE "\t#define ${VALUE}\n")
    endforeach()
    set(${CLANG_DEFINITIONS} "${CLANG_DEFINITIONS_CODE}" PARENT_SCOPE)
    set(${EMPTY_DEFINITIONS} "${EMPTY_DEFINITIONS_CODE}" PARENT_SCOPE)
endfunction()

function(GENERATE_ANNOTATIONS_ENTRY_EXIT_FUNCTIONS INTERFACE_NAMES INTERFACE_RECEIVERS INTERFACE_FUNCTIONS_DECLARATIONS INTERFACE_FUNCTIONS_DEFINITIONS INTERFACE_RECEIVERS_INCLUDES)
    list(LENGTH INTERFACE_NAMES INTERFACE_LENGTH_1)
    math(EXPR INTERFACE_LENGTH "${INTERFACE_LENGTH_1} - 1")
    foreach(INTERFACE_INDEX RANGE ${INTERFACE_LENGTH})
        list(GET INTERFACE_NAMES ${INTERFACE_INDEX} INTERFACE_NAME)
        list(GET INTERFACE_RECEIVERS ${INTERFACE_INDEX} INTERFACE_RECEIVER)
        string(APPEND INTERFACE_FUNCTIONS_DECLARATIONS_CODE
        "extern \"C\" void ${INTERFACE_NAME}_entry(const char *file, const char *function, int line);
        extern \"C\" void ${INTERFACE_NAME}_exit(const char *file, const char *function, int line);")
        string(APPEND INTERFACE_FUNCTIONS_DEFINITIONS_CODE
        "extern \"C\" void ${INTERFACE_NAME}_entry(const char *file, const char *function, int line)
        {                                                                                  
            ${INTERFACE_RECEIVER}::instance().entry(file, function, line);
        }                                                                                  
        extern \"C\" void ${INTERFACE_NAME}_exit(const char *file, const char *function, int line)      
        {                                                                                  
            ${INTERFACE_RECEIVER}::instance().exit(file, function, line);
        }")
        string(APPEND INTERFACE_RECEIVERS_INCLUDES_CODE
        "#include <Utilities/${INTERFACE_RECEIVER}.h>\n"
        )
    endforeach()
    set(${INTERFACE_FUNCTIONS_DECLARATIONS} "${INTERFACE_FUNCTIONS_DECLARATIONS_CODE}" PARENT_SCOPE)
    set(${INTERFACE_FUNCTIONS_DEFINITIONS} "${INTERFACE_FUNCTIONS_DEFINITIONS_CODE}" PARENT_SCOPE)
    set(${INTERFACE_RECEIVERS_INCLUDES} "${INTERFACE_RECEIVERS_INCLUDES_CODE}" PARENT_SCOPE)
endfunction()


# function interfaces
set(XP_FUNCTION_INTERFACES XPProfilable XPRecordable XPLockable XPDiscardable)
JOIN("${XP_FUNCTION_INTERFACES}" "," XP_FUNCTION_INTERFACES_JOINED)
set(XP_FUNCTION_INTERFACES_RECEIVERS XPProfiler XPRecorder XPLocker XPDiscarder)
JOIN("${XP_FUNCTION_INTERFACES_RECEIVERS}" "," XP_FUNCTION_INTERFACES_RECEIVERS_JOINED)

# function flows
set(XP_FUNCTION_FLOWS XPFlowable)
JOIN("${XP_FUNCTION_FLOWS}" "," XP_FUNCTION_FLOWS_JOINED)

GENERATE_ANNOTATIONS_DEFINITIONS("${XP_FUNCTION_INTERFACES}" XP_FUNCTION_INTERFACES_CLANG_DEFINITIONS XP_FUNCTION_INTERFACES_EMPTY_DEFINITIONS)
GENERATE_ANNOTATIONS_ENTRY_EXIT_FUNCTIONS("${XP_FUNCTION_INTERFACES}" "${XP_FUNCTION_INTERFACES_RECEIVERS}" XP_FUNCTIONS_INTERFACES_FUNCTIONS_DECLARATIONS XP_FUNCTIONS_INTERFACES_FUNCTIONS_DEFINITIONS XP_FUNCTIONS_INTERFACES_FUNCTIONS_RECEIVERS_INCLUDES)

GENERATE_ANNOTATIONS_DEFINITIONS("${XP_FUNCTION_FLOWS}" XP_FUNCTION_FLOWS_CLANG_DEFINITIONS XP_FUNCTION_FLOWS_EMPTY_DEFINITIONS)

configure_file(${CMAKE_SOURCE_DIR}/src/Templates/XPAnnotations.h.in ${CMAKE_SOURCE_DIR}/src/Utilities/XPAnnotations.h)
configure_file(${CMAKE_SOURCE_DIR}/src/Templates/XPAnnotations.cpp.in ${CMAKE_SOURCE_DIR}/src/Utilities/XPAnnotations.cpp)
execute_process(COMMAND /Users/$ENV{USER}/Library/Developer/Toolchains/LLVM19.1.6.xctoolchain/usr/bin/clang-format -style=file -i ${CMAKE_SOURCE_DIR}/src/Utilities/XPAnnotations.h)
execute_process(COMMAND /Users/$ENV{USER}/Library/Developer/Toolchains/LLVM19.1.6.xctoolchain/usr/bin/clang-format -style=file -i ${CMAKE_SOURCE_DIR}/src/Utilities/XPAnnotations.cpp)

if(CMAKE_C_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mllvm -enable-function-interface=1 -mllvm -debug-function-interface=0 -mllvm -requested-function-interfaces=${XP_FUNCTION_INTERFACES_JOINED}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mllvm -enable-function-interface=1 -mllvm -debug-function-interface=0 -mllvm -requested-function-interfaces=${XP_FUNCTION_INTERFACES_JOINED}")
endif()