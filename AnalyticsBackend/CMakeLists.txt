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
   tmp/system.hpp

)

include_directories(
   include
   tmp
)

add_executable(AnalyticsBackend
               src/main.cpp  ${SOURCE_FILES} ${HEADERS})

find_package(CUDA REQUIRED)

target_link_libraries(AnalyticsBackend PRIVATE ${CUDA_LIBRARIES})
