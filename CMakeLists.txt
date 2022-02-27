cmake_minimum_required(VERSION 3.16)

set(ANIMO_VERSION "0.0.0")
project(Animo VERSION ${ANIMO_VERSION})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_SUPPRESS_REGENERATION true) # Don't add ZERO_CHECK target
set(VS_STARTUP_PROJECT ${PROJECT_NAME})

find_package(imgui CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(Vulkan REQUIRED)

# Add Animo sources
# add_subdirectory(lib)

set(SOURCES 
    src/main.cpp
    src/Animo.cpp
    src/Animo.h
)
add_executable(${PROJECT_NAME} ${APPLICATION_TYPE} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} PUBLIC imgui::imgui glfw Vulkan::Vulkan)

# Add unit tests
# add_subdirectory(tests EXCLUDE_FROM_ALL)