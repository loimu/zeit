cmake_minimum_required (VERSION 3.5)
project(zeit)

option(BUILD_TESTS        "Build tests"            OFF)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 6)
set(VERSION_PATCH 95)
set(ZEIT_V    "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
set(CRONTAB_V "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

file(READ rev ZEIT_REVISION LIMIT 9)

if(WITH_QT6)
    find_package (Qt6 6.2.0 CONFIG REQUIRED COMPONENTS Core Gui Widgets Test)
    # root actions support KF6
    find_package(KF6Auth)
    if(KF6Auth_FOUND)
      find_package(KF6CoreAddons REQUIRED)
      if(KF6CoreAddons_FOUND)
        set(BUILD_HELPER TRUE)
      endif()
    endif()
else()
    find_package (Qt5 5.14.0 CONFIG REQUIRED COMPONENTS Core Gui Widgets Test)
    # root actions support KF5
    find_package(KF5Auth)
    if(KF5Auth_FOUND)
      find_package(KF5CoreAddons REQUIRED)
      if(KF5CoreAddons_FOUND)
        set(BUILD_HELPER TRUE)
      endif()
    endif()
endif()

set(CMAKE_AUTOMOC ON)

add_subdirectory(crontablib)
add_subdirectory(src)
add_subdirectory(misc)

if(BUILD_TESTS)
  add_subdirectory(tests)
endif()
