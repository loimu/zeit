include_directories(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
)

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

add_library(crontablib SHARED ${crontablib_SRCS})

target_link_libraries(crontablib
    Qt5::Core
    Qt5::Widgets
)

install(FILES ${crontablib_HDRS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/crontablib/)
install(TARGETS crontablib LIBRARY DESTINATION lib)
