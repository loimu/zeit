project(zeit)
cmake_minimum_required (VERSION 3.1.0)

option(BUILD_TESTS        "Build tests"     OFF)

set(QT_MIN_VERSION "5.2.0")
find_package (Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    Widgets
    Test
)
set(CMAKE_AUTOMOC ON)

include_directories(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
)

add_subdirectory(crontablib)
add_subdirectory(src)

if(BUILD_TESTS)
  add_subdirectory(tests)
endif()
