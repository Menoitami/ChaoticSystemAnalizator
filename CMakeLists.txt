cmake_minimum_required(VERSION 3.16)

project(SystemsAnalytic VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(PYTHON_SCHEME_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/SystemChecker/run_scheme.py)
# set(QWT_ROOT = C:/Qwt-6.3.0)



set(PYTHON_SCHEME_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/SystemChecker/run_scheme.py)

configure_file(
    ${SOURCE_DIR}/config.h.in
    ${CMAKE_BINARY_DIR}/config.h
)


find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Svg)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Svg)
#find_package(nlohmann_json REQUIRED)


#set(CMAKE_PREFIX_PATH "$ENV{VCPKG_ROOT}/installed/x64-windows")
#set(VTK_DIR "$ENV{VCPKG_ROOT}/installed/x64-windows/share/vtk")

#find_package(VTK REQUIRED)


file(GLOB_RECURSE UI
    ${SOURCE_DIR}/*.ui
)

file(GLOB_RECURSE SOURCES
    ${SOURCE_DIR}/*.cpp
)

file(GLOB_RECURSE HEADERS
    ${SOURCE_DIR}/*.hpp
    ${SOURCE_DIR}/*.h
)


qt6_add_resources(RESOURCES_RCC "assets/others.qrc" "assets/pics.qrc" "assets/qmls.qrc")

qt6_wrap_cpp(MOC_SOURCES ${HEADERS})


add_subdirectory(AnalyticsBackend)
add_subdirectory(ConnectionManager)


find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(SystemsAnalytic
    ${SOURCE_DIR}/main.cpp  ${SOURCES} ${HEADERS} ${MOC_SOURCES}  ${RESOURCES_RCC} ${UI}
)

target_include_directories(SystemsAnalytic PRIVATE
    ${SOURCE_DIR}/
    ${SOURCE_DIR}/field_window_widgets
    ${SOURCE_DIR}/managers
    ${SOURCE_DIR}/field_window_widgets/SystemSettings
    ${SOURCE_DIR}/field_window_widgets/PhasePortrait
    ${SOURCE_DIR}/major_widget
    ${SOURCE_DIR}/major_widget/menu
    ${SOURCE_DIR}/major_widget/field
    ${SOURCE_DIR}/resources
    ${SOURCE_DIR}/tools
    ${SOURCE_DIR}/qml
    ${SOURCE_DIR}/ui_tools
    ${SOURCE_DIR}/ui_tools/btns
    ${SOURCE_DIR}/ui_tools/stylesheet_processor
    ${SOURCE_DIR}/ui_tools/dialog_widget
    ${CMAKE_CURRENT_SOURCE_DIR}/ConnectionManager/include
    # ${QWT_ROOT}/include
)
target_include_directories(SystemsAnalytic PRIVATE ${CMAKE_BINARY_DIR})

#target_link_libraries(SystemsAnalytic PRIVATE ${VTK_LIBRARIES})
target_link_libraries(SystemsAnalytic PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Svg
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets)
target_link_libraries(SystemsAnalytic PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}/ConnectionManager/ConnectionManager.lib)


include(GNUInstallDirs)
install(TARGETS SystemsAnalytic
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Для получения хэша
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    # Формируем строку
    set(VERSION_STRING "${GIT_BRANCH}-${GIT_HASH}")

    # Создаём файл version
#    file(WRITE "${CMAKE_BINARY_DIR}/version" "${VERSION_STRING}\n") Art: Этот файл ломает мне сборку
endif()

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(SystemsAnalytic)
endif()
