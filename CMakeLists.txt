project("Saleae SDIO Analyzer")
cmake_minimum_required(VERSION 3.0)

message(WARNING "CMake support is still experimental!")

# Find Analyzer include dir
find_path(
    ANALYZER_SDK_INCLUDE_DIR
    NAMES
    Analyzer.h
    AnalyzerChannelData.h
    AnalyzerHelpers.h
    AnalyzerResults.h
    AnalyzerSettings.h
    AnalyzerTypes.h
    SimulationChannelDescriptor.h
    PATHS
    ../include/
    ../AnalyzerSDK/include
    DOC
    "Include directory of the analyzer SDK."
)

if(NOT ANALYZER_SDK_INCLUDE_DIR)
    message(SEND_ERROR "Analyzer SDK include directory not found")
else()
    message(STATUS 
        "Analyzer SDK include directory found at ${ANALYZER_SDK_INCLUDE_DIR}")
endif()

# needed to differ between 32 and 64 bit library
set(ANALYZER_BITNESS)
set(ANALYZER_LIB_NAME "")

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(STATUS "32 Bit detected")
    set(ANALYZER_BITNESS 32)
    set(ANALYZER_LIB_NAME "Analyzer")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(STATUS "64 Bit detected")
    set(ANALYZER_BITNESS 64)
    set(ANALYZER_LIB_NAME "Analyzer64")
else()
    message(FATAL_ERROR "Environment not supported")
endif()

if(NOT (WIN32 OR UNIX))
    # I have no idea what to do under MacOS
    message(WARNING "Environment may not be supported")
endif()

# find library
find_library(
    ANALYZER_SDK_LIBRARY
    NAMES
    ${ANALYZER_LIB_NAME}
    PATHS
    ../lib/
    ../AnalyzerSDK/lib/
    DOC
    "Analyzer SDK library. \
If you set it yourself, choose the correct architecture"
)

if(NOT ANALYZER_SDK_LIBRARY)
    message(SEND_ERROR "Analyzer SDK library not found")
else()
    message(STATUS "Analyzer SDK library found at ${ANALYZER_SDK_LIBRARY}")
endif()


add_library(SDIOAnalyzer SHARED
    source/SDIOAnalyzer.cpp
    source/SDIOAnalyzerResults.cpp
    source/SDIOAnalyzerSettings.cpp
    source/SDIOSimulationDataGenerator.cpp
)

target_include_directories(SDIOAnalyzer PUBLIC
    source
    ${ANALYZER_SDK_INCLUDE_DIR}
)

target_link_libraries(SDIOAnalyzer
    PUBLIC
    ${ANALYZER_SDK_LIBRARY}
)

target_compile_features(SDIOAnalyzer
    PRIVATE
    cxx_nullptr
)
