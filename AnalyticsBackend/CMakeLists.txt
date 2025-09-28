cmake_minimum_required(VERSION 3.10)

project(AnalyticsBackend LANGUAGES CXX CUDA)

set(CMAKE_CUDA_ARCHITECTURES 80)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(SOURCE_FILES
   src/BackendMain.cpp
)

set(HEADERS
   include/BackendMain.hpp
   include/BackendTypes.hpp
   tmp/input/system.hpp
)
set(OUTPUT_TEMP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tmp/output/")

include_directories(
   include
   tmp/input
)

add_executable(AnalyticsBackend
               src/main.cpp  ${SOURCE_FILES} ${HEADERS})

find_package(CUDA REQUIRED)
include_directories(${CUDA_INCLUDE_DIRS})


target_link_libraries(AnalyticsBackend PUBLIC ConnectionManager)

target_link_libraries(AnalyticsBackend PRIVATE ${CUDA_LIBRARIES})
target_compile_definitions(AnalyticsBackend PRIVATE
   OUTPUT_PATH="${OUTPUT_TEMP_PATH}")
