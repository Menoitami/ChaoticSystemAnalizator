cmake_minimum_required(VERSION 3.10)
project(ConnectionManager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCE_FILES
   src/BackendBase.cpp
   src/FrontendBase.cpp
)

# Header files
set(HEADERS
   include/BackendBase.hpp
   include/FrontendBase.hpp
)

# Create the library
add_library(ConnectionManager STATIC
    ${SOURCE_FILES}
    ${HEADERS}
)

# Windows-specific socket library linking
if(WIN32)
    target_link_libraries(ConnectionManager PRIVATE
        ws2_32  # Windows Socket 2 library
        wsock32 # Additional Windows socket library
    )
endif()

# Set include directories
target_include_directories(ConnectionManager PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
install(TARGETS ConnectionManager DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)
