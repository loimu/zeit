project(zeit)

#### setup
cmake_minimum_required (VERSION 2.8.12 FATAL_ERROR)
set (QT_MIN_VERSION "5.2.0")
find_package (ECM 1.3.0 REQUIRED NO_MODULE)
set (CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
find_package (Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    Widgets
)
find_package(KF5 REQUIRED COMPONENTS
    I18n
    #IconThemes
)
#add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
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

### build
set(qt_LIBS Qt5::Core Qt5::Widgets)
include_directories(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/crontablib
    ${CMAKE_CURRENT_BINARY_DIR}
)
qt5_wrap_ui(zeit_FORMS_HEADERS ${zeit_FRMS})
add_executable(zeit ${zeit_SRCS} ${zeit_HEADERS_MOC} ${zeit_FORMS_HEADERS})
target_link_libraries(zeit
    ${qt_LIBS}
    KF5::I18n
    #KF5::IconThemes
)

install(TARGETS zeit RUNTIME DESTINATION bin)
