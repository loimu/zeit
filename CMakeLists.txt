project(zeit)
cmake_minimum_required (VERSION 3.1.0)

option(BUILD_TESTS        "Build tests"            OFF)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 6)
set(VERSION_PATCH 95)
set(ZEIT_V    "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
set(CRONTAB_V "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

file(READ rev ZEIT_REVISION LIMIT 9)

set(QT_MIN_VERSION "5.7.1")
find_package (Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Test
)

# root actions support
find_package(KF5Auth)
if(KF5Auth_FOUND)
  find_package(KF5CoreAddons REQUIRED)
  if(KF5CoreAddons_FOUND)
    set(BUILD_HELPER TRUE)
  endif()
endif()

set(CMAKE_AUTOMOC ON)

add_subdirectory(crontablib)
add_subdirectory(src)
add_subdirectory(misc)

if(BUILD_TESTS)
  add_subdirectory(tests)
endif()
