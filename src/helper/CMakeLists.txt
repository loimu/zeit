
add_executable(zeit_helper zeithelper.cpp)
target_link_libraries(zeit_helper KF5::AuthCore)
target_include_directories(zeit_helper PRIVATE ${Qt5Core_INCLUDE_DIRS}
                                           "/usr/include/KF5/KAuth"
                                           "/usr/include/KF5/KCoreAddons")

set(SYSCONF_INSTALL_DIR "/etc")
set(DBUS_SYSTEM_SERVICES_INSTALL_DIR "share/dbus-1/system-services")
install(TARGETS zeit_helper DESTINATION ${KAUTH_HELPER_INSTALL_DIR})
kauth_install_helper_files(zeit_helper local.zeithelper root)
kauth_install_actions(local.zeithelper zeit_helper_actions)
