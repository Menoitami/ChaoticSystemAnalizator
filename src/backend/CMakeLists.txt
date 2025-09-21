cmake_minimum_required(VERSION 3.10)

project(AnalyticsBackend VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCE_FILES
   include/BackendMain.hpp
   src/BackendMain.cpp
)

add_executable(AnalyticsBackend ${SOURCE_FILES})

find_package(Qt5 REQUIRED COMPONENTS Core Widgets)
target_link_libraries(MySubprogram Qt5::Core Qt5::Widgets)
