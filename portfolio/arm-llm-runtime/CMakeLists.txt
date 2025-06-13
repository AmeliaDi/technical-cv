cmake_minimum_required(VERSION 3.16)
project(ARM_LLM_Runtime VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Options
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)
option(BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
option(ENABLE_NEON "Enable ARM NEON optimizations" ON)
option(ENABLE_PROFILING "Enable profiling support" OFF)

# ARM-specific settings
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm|aarch64)")
    set(ARM_TARGET ON)
    if(ENABLE_NEON)
        add_definitions(-D__ARM_NEON)
        if(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv8-a")
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpu=neon -march=armv7-a")
        endif()
    endif()
else()
    set(ARM_TARGET OFF)
    message(WARNING "This project is optimized for ARM architectures. Performance may be suboptimal on other architectures.")
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -flto")

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math -funroll-loops")
    
    if(ARM_TARGET)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=native")
    endif()
endif()

# Find dependencies
find_package(PkgConfig REQUIRED)

# CURL for HTTP requests
find_package(CURL REQUIRED)

# JSON-C for JSON parsing
pkg_check_modules(JSON_C REQUIRED json-c)

# OpenMP for parallelization
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(OpenMP::OpenMP_CXX)
endif()

# Threads
find_package(Threads REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CURL_INCLUDE_DIRS}
    ${JSON_C_INCLUDE_DIRS}
)

# Source files
set(SOURCES
    src/arm_llm_runtime.cpp
    src/tensor.cpp
    src/memory_manager.cpp
    src/huggingface_client.cpp
    src/quantization_engine.cpp
    src/inference_engine.cpp
    src/model.cpp
    src/tokenizer.cpp
    src/utils.cpp
)

set(HEADERS
    src/arm_llm_runtime.h
)

# Main library
add_library(arm_llm_runtime ${SOURCES} ${HEADERS})

target_link_libraries(arm_llm_runtime
    ${CURL_LIBRARIES}
    ${JSON_C_LIBRARIES}
    Threads::Threads
)

if(OpenMP_CXX_FOUND)
    target_link_libraries(arm_llm_runtime OpenMP::OpenMP_CXX)
endif()

# Set library properties
set_target_properties(arm_llm_runtime PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "src/arm_llm_runtime.h"
)

# Command line tool
add_executable(arm-llm
    tools/main.cpp
)

target_link_libraries(arm-llm arm_llm_runtime)

# Model conversion tool
add_executable(convert_model
    tools/convert_model.cpp
)

target_link_libraries(convert_model arm_llm_runtime)

# Quantization tool
add_executable(quantize_model
    tools/quantize_model.cpp
)

target_link_libraries(quantize_model arm_llm_runtime)

# Examples
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Python bindings
if(BUILD_PYTHON_BINDINGS)
    find_package(pybind11 QUIET)
    if(pybind11_FOUND)
        pybind11_add_module(arm_llm_runtime_python
            bindings/python_bindings.cpp
        )
        target_link_libraries(arm_llm_runtime_python PRIVATE arm_llm_runtime)
        
        # Set properties for Python module
        set_target_properties(arm_llm_runtime_python PROPERTIES
            OUTPUT_NAME arm_llm_runtime
            SUFFIX ${PYTHON_MODULE_EXTENSION}
        )
    else()
        message(WARNING "pybind11 not found. Python bindings will not be built.")
    endif()
endif()

# Installation rules
include(GNUInstallDirs)

install(TARGETS arm_llm_runtime arm-llm convert_model quantize_model
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/arm_llm
)

# Install additional headers
install(DIRECTORY src/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/arm_llm
    FILES_MATCHING PATTERN "*.h"
)

# Install configuration files
install(FILES
    config/default_config.yaml
    DESTINATION ${CMAKE_INSTALL_DATADIR}/arm_llm_runtime
)

# pkg-config file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/arm_llm_runtime.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/arm_llm_runtime.pc
    @ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/arm_llm_runtime.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

# Uninstall target
if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY
    )

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
    )
endif()

# Packaging
set(CPACK_PACKAGE_NAME "arm-llm-runtime")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Fast LLM inference engine for ARM architectures")
set(CPACK_PACKAGE_VENDOR "ARM LLM Runtime Team")
set(CPACK_PACKAGE_CONTACT "support@arm-llm-runtime.com")

# DEB package
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libcurl4, libjson-c-dev")
set(CPACK_DEBIAN_PACKAGE_SECTION "science")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")

# RPM package
set(CPACK_RPM_PACKAGE_REQUIRES "libcurl-devel, json-c-devel")
set(CPACK_RPM_PACKAGE_GROUP "Development/Libraries")

include(CPack)

# Print configuration summary
message(STATUS "")
message(STATUS "ARM LLM Runtime Configuration Summary:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Architecture: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "  ARM target: ${ARM_TARGET}")
message(STATUS "  NEON enabled: ${ENABLE_NEON}")
message(STATUS "  OpenMP found: ${OpenMP_CXX_FOUND}")
message(STATUS "  Build shared libs: ${BUILD_SHARED_LIBS}")
message(STATUS "  Build tests: ${BUILD_TESTS}")
message(STATUS "  Build examples: ${BUILD_EXAMPLES}")
message(STATUS "  Build Python bindings: ${BUILD_PYTHON_BINDINGS}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "") 