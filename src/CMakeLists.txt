configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/config.h)

set(zeit_SRCS
    aboutdialog.cpp
    alarmdialog.cpp
    basedialog.cpp
    commanddialog.cpp
    commands.cpp
    main.cpp
    mainwindow.cpp
    taskdialog.cpp
    timerdialog.cpp
    variabledialog.cpp
    data/basedelegate.cpp
    data/commanddelegate.cpp
    data/taskdelegate.cpp
    data/variabledelegate.cpp
)
set(zeit_FRMS
    alarmdialog.ui
    commanddialog.ui
    mainwindow.ui
    taskdialog.ui
    timerdialog.ui
    variabledialog.ui
)
set(zeit_RSRCS ../zeit.qrc)

#translations
SET(TRANSLATION
    ../translations/zeit_be_BY.ts
    ../translations/zeit_ru_RU.ts
    ../translations/zeit_sv_SE.ts
    ../translations/crontablib_be_BY.ts
    ../translations/crontablib_ru_RU.ts
    ../translations/crontablib_sv_SE.ts
)

if(WITH_QT6)
    find_package(Qt6LinguistTools REQUIRED)
    qt_add_translation(QM_FILES ${TRANSLATION})

    set(qt_LIBS Qt6::Core Qt6::Gui Qt6::Widgets)

    qt_wrap_ui(zeit_FORMS_HEADERS ${zeit_FRMS})
    qt_add_resources(zeit_RESOURCES_RCC ${zeit_RSRCS})
    set(QT_INCLUDE_DIRECTORIES ${Qt6Core_INCLUDE_DIRS}
                               ${Qt6Widgets_INCLUDE_DIRS})
else()
    find_package(Qt5LinguistTools REQUIRED)
    qt5_add_translation(QM_FILES ${TRANSLATION})

    set(qt_LIBS Qt5::Core Qt5::Gui Qt5::Widgets)

    qt5_wrap_ui(zeit_FORMS_HEADERS ${zeit_FRMS})
    qt5_add_resources(zeit_RESOURCES_RCC ${zeit_RSRCS})
    set(QT_INCLUDE_DIRECTORIES ${Qt5Core_INCLUDE_DIRS}
                               ${Qt5Widgets_INCLUDE_DIRS})
endif()


add_executable(zeit ${zeit_SRCS} ${zeit_HEADERS_MOC} ${zeit_FORMS_HEADERS}
                    ${QM_FILES} ${zeit_RESOURCES_RCC})
target_include_directories(zeit PRIVATE ${QT_INCLUDE_DIRECTORIES}
                           ${CMAKE_CURRENT_SOURCE_DIR}/../crontablib
                           ${CMAKE_CURRENT_SOURCE_DIR}
                           ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(zeit crontab ${qt_LIBS})
target_compile_features(zeit PRIVATE cxx_lambdas cxx_nullptr cxx_unicode_literals)

install(TARGETS zeit RUNTIME DESTINATION bin)
install(FILES ${QM_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/zeit/translations)

if(BUILD_HELPER)
  add_subdirectory(helper)
endif()
