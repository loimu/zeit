project(zeit)

#### setup
cmake_minimum_required (VERSION 2.8.12 FATAL_ERROR)
set (QT_MIN_VERSION "5.2.0")
find_package (ECM 1.3.0 REQUIRED NO_MODULE)
set (CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
find_package (Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    Widgets
    Test
)
set(CMAKE_AUTOMOC ON)

### sources
set(zeit_crontablib_SRCS
   crontablib/cthost.cpp
   crontablib/ctcron.cpp
   crontablib/ctmonth.cpp
   crontablib/ctminute.cpp
   crontablib/cthour.cpp
   crontablib/ctdom.cpp
   crontablib/ctdow.cpp
   crontablib/cttask.cpp
   crontablib/ctunit.cpp
   crontablib/ctvariable.cpp
   crontablib/ctGlobalCron.cpp
   crontablib/ctSystemCron.cpp
   crontablib/ctInitializationError.cpp
   crontablib/ctSaveStatus.cpp
   crontablib/ctHelper.cpp
)
set(zeit_SRCS
   ${zeit_crontablib_SRCS}
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
#add translations
qt5_add_translation(QM ${TRANSLATION})

set(qt_LIBS Qt5::Core Qt5::Widgets)
#tests
set(tests_SRCS ${zeit_crontablib_SRCS} src/tests.cpp)
set(qtTest_LIBS ${qt_LIBS} Qt5::Test)

include_directories(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/crontablib
    ${CMAKE_CURRENT_BINARY_DIR}
)
qt5_wrap_ui(zeit_FORMS_HEADERS ${zeit_FRMS})
add_executable(zeit ${zeit_SRCS} ${zeit_HEADERS_MOC} ${zeit_FORMS_HEADERS} ${QM})
add_executable(tests ${tests_SRCS})
target_link_libraries(zeit ${qt_LIBS})
target_link_libraries(tests ${qtTest_LIBS})

install(TARGETS zeit RUNTIME DESTINATION bin)
