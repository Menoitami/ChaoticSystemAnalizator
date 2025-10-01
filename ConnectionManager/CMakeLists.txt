cmake_minimum_required(VERSION 3.10)
project(ConnectionManager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network Concurrent)
# Sources and headers
set(SOURCE_FILES
    src/BackendBase.cpp
    src/FrontendBase.cpp
    src/ConnectionUnit.cpp

)

set(HEADER_FILES
    include/BackendBase.hpp
    include/FrontendBase.hpp
    include/ConnectionUnit.hpp
    include/ConnectionTypes.hpp
)

add_library(ConnectionManager STATIC
    ${SOURCE_FILES}
    ${HEADER_FILES}
)

target_include_directories(ConnectionManager
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PUBLIC ${CMAKE_BINARY_DIR}
)

target_link_libraries(ConnectionManager
    PUBLIC Qt6::Core Qt6::Concurrent Qt6::Network Qt6::Widgets
)
target_link_libraries(ConnectionManager
    PUBLIC AnalyticsBackend
)


install(TARGETS ConnectionManager DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)
