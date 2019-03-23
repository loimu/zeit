project(zeit)
cmake_minimum_required (VERSION 3.1.0)

option(BUILD_TESTS        "Build tests"            OFF)
option(BUILD_HELPER       "Build PolKit helper"     ON)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 4)
set(VERSION_PATCH 97)
set(ZEIT_V    "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
set(CRONTAB_V "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

set(QT_MIN_VERSION "5.7.1")
find_package (Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Test
)
if(BUILD_HELPER)
  find_package(KF5Auth REQUIRED)
endif()
set(CMAKE_AUTOMOC ON)

add_subdirectory(crontablib)
add_subdirectory(src)

if(BUILD_TESTS)
  add_subdirectory(tests)
endif()
