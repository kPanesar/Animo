cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

set(JSON_MultipleHeaders ON)

set(LIBANIMO_SOURCES
    src/ampch.cpp
    src/ampch.h
    src/animo.h
    src/api/animo_api.cpp
    src/api/animo_api.h
    src/core/timer.h
    src/core/timestep.h
    src/core/uuid.cpp
    src/core/uuid.h
)

add_subdirectory(external/nativefiledialog)

add_library(libanimo ${LIBANIMO_SOURCES})
set_target_properties(libanimo PROPERTIES POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS})

if (WIN32)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-export-all-symbols")
    set_target_properties(libanimo nfd PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

target_include_directories(libanimo PUBLIC src external)
target_link_libraries(libanimo PUBLIC Vulkan::Vulkan imgui::imgui fmt::fmt nlohmann_json::nlohmann_json nfd)