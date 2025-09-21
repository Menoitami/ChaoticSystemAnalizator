cmake_minimum_required(VERSION 3.10)

project(AnalyticsBackend VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_FILES
   src/BackendMain.cpp
)

set(HEADERS
   include/BackendMain.hpp
)

include_directories(include)

add_executable(AnalyticsBackend
               src/main.cpp  ${SOURCE_FILES} ${HEADERS})

