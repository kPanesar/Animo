cmake_minimum_required(VERSION 3.16)

set(ANIMO_VERSION "0.0.0")
project(Animo VERSION ${ANIMO_VERSION})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_SUPPRESS_REGENERATION TRUE) # Don't add ZERO_CHECK target
set(VS_STARTUP_PROJECT ${PROJECT_NAME})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Detect current OS / System
if (WIN32)
    add_compile_definitions(OS_WINDOWS)
elseif (APPLE)
    add_compile_definitions(OS_MACOS)
elseif (UNIX AND NOT APPLE)
    add_compile_definitions(OS_LINUX)
else ()
    message(FATAL_ERROR "Unknown / unsupported system!")
endif()

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

# Add Animo sources
add_subdirectory(lib)

set(SOURCES 
    src/main.cpp
    src/Animo.cpp
    src/Animo.h
)

add_executable(${PROJECT_NAME} ${APPLICATION_TYPE} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE src)

if (WIN32)
    target_link_options(${PROJECT_NAME} PRIVATE "LINKER:/SUBSYSTEM:WINDOWS")
    target_link_libraries(${PROJECT_NAME} PUBLIC libanimo)
else ()
    target_link_libraries(${PROJECT_NAME} PUBLIC libanimo pthread)
endif ()

# Add unit tests
# add_subdirectory(tests EXCLUDE_FROM_ALL)