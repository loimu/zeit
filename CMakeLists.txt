project(zeit)

#### setup
cmake_minimum_required (VERSION 2.8.12 FATAL_ERROR)
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
   src/listitem.cpp
   src/main.cpp
   src/mainwindow.cpp
   src/taskdialog.cpp
   src/timerdialog.cpp
)
set(zeit_FRMS
   src/aboutdialog.ui
   src/alarmdialog.ui
   src/mainwindow.ui
   src/taskdialog.ui
   src/timerdialog.ui)

#translations
SET(TRANSLATION
    translations/ru_RU.ts
)
find_package(Qt5LinguistTools REQUIRED)
qt5_add_translation(QM_FILES ${TRANSLATION})

set(qt_LIBS Qt5::Core Qt5::Widgets)
#tests
set(test_LIBS Qt5::Core Qt5::Test)
set(tests_SRCS src/tests.cpp)

#build
include_directories(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/crontablib
    ${CMAKE_CURRENT_BINARY_DIR}
)
qt5_wrap_ui(zeit_FORMS_HEADERS ${zeit_FRMS})
add_executable(zeit ${zeit_SRCS} ${zeit_HEADERS_MOC} ${zeit_FORMS_HEADERS} ${QM_FILES})
add_executable(tests ${tests_SRCS})
target_link_libraries(zeit crontablib ${qt_LIBS})
target_link_libraries(tests crontablib ${test_LIBS})

install(TARGETS zeit RUNTIME DESTINATION bin)
install(FILES ${QM_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/zeit/translations)
