configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/config.h)

set(crontablib_SRCS
   cthost.cpp
   ctcron.cpp
   ctmonth.cpp
   ctminute.cpp
   cthour.cpp
   ctdom.cpp
   ctdow.cpp
   cttask.cpp
   ctunit.cpp
   ctvariable.cpp
   ctGlobalCron.cpp
   ctSystemCron.cpp
   ctInitializationError.cpp
   ctSaveStatus.cpp
   ctHelper.cpp
)

set(crontablib_HDRS
   cthost.h
   ctcron.h
   ctmonth.h
   ctminute.h
   cthour.h
   ctdom.h
   ctdow.h
   cttask.h
   ctunit.h
   ctvariable.h
   ctGlobalCron.h
   ctSystemCron.h
   ctInitializationError.h
   ctSaveStatus.h
   ctHelper.h
)

add_library(crontab SHARED ${crontablib_SRCS})

target_link_libraries(crontab Qt5::Core Qt5::Gui KF5::AuthCore)
target_include_directories(crontab PRIVATE ${Qt5Core_INCLUDE_DIRS}
                                           ${Qt5Gui_INCLUDE_DIRS}
                                           "/usr/include/KF5/KAuth"
                                           "/usr/include/KF5/KCoreAddons"
                                           ${CMAKE_CURRENT_BINARY_DIR})
target_compile_features(crontab PRIVATE cxx_lambdas cxx_nullptr cxx_unicode_literals)
set_target_properties(crontab PROPERTIES VERSION ${CRONTAB_V} SOVERSION ${VERSION_MAJOR})

install(FILES ${crontablib_HDRS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/crontablib/)
install(TARGETS crontab LIBRARY DESTINATION lib)
