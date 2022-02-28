cmake_minimum_required(VERSION 3.16)
project(libanimo)

set(CMAKE_CXX_STANDARD 20)
set(BUILD_SHARED_LIBS OFF)

find_package(imgui CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
set(JSON_MultipleHeaders ON)

if (WIN32)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-export-all-symbols")
endif()

set(CMAKE_SHARED_LIBRARY_PREFIX "")

set(LIBANIMO_SOURCES
    src/animo.hpp
    src/api/animo_api.cpp
    src/api/animo_api.hpp
)

add_compile_definitions(ANIMO_PROJECT_NAME="${PROJECT_NAME}")

add_library(libanimo SHARED ${LIBANIMO_SOURCES})
set_target_properties(libanimo PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(libanimo PUBLIC src external)
target_link_libraries(libanimo PUBLIC imgui::imgui glfw fmt::fmt nlohmann_json nlohmann_json::nlohmann_json)