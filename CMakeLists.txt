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

add_subdirectory(crontablib)

### sources
set(zeit_SRCS
   src/aboutdialog.cpp
   src/alarmdialog.cpp
   src/main.cpp
   src/mainwindow.cpp
   src/taskdialog.cpp
   src/timerdialog.cpp
   src/variabledialog.cpp
)
set(zeit_FRMS
   src/aboutdialog.ui
   src/alarmdialog.ui
   src/mainwindow.ui
   src/taskdialog.ui
   src/timerdialog.ui
   src/variabledialog.ui
)
set(zeit_RSRCS zeit.qrc)

#translations
SET(TRANSLATION
    translations/ru_RU.ts
)
find_package(Qt5LinguistTools REQUIRED)
qt5_add_translation(QM_FILES ${TRANSLATION})

set(qt_LIBS Qt5::Core Qt5::Widgets)
include_directories(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/crontablib
    ${CMAKE_CURRENT_BINARY_DIR}
)
qt5_wrap_ui(zeit_FORMS_HEADERS ${zeit_FRMS})
qt5_add_resources(zeit_RESOURCES_RCC ${zeit_RSRCS})
add_executable(zeit ${zeit_SRCS} ${zeit_HEADERS_MOC} ${zeit_FORMS_HEADERS}
                    ${QM_FILES} ${zeit_RESOURCES_RCC})
target_link_libraries(zeit crontab ${qt_LIBS})
target_compile_features(zeit PRIVATE cxx_nullptr)

install(TARGETS zeit RUNTIME DESTINATION bin)
install(FILES ${QM_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/zeit/translations)

if(BUILD_TESTS)
  add_subdirectory(tests)
endif()
