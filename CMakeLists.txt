cmake_minimum_required(VERSION 3.13)
project(SDIOAnalyzer)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include(ExternalAnalyzerSDK)

set(SOURCES
    src/SDIOAnalyzer.cpp
    src/SDIOAnalyzer.h
    src/SDIOAnalyzerResults.cpp
    src/SDIOAnalyzerResults.h
    src/SDIOAnalyzerSettings.cpp
    src/SDIOAnalyzerSettings.h
    src/SDIOSimulationDataGenerator.cpp
    src/SDIOSimulationDataGenerator.h)

add_analyzer_plugin(${PROJECT_NAME} SOURCES ${SOURCES})
