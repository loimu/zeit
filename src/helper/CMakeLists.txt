
add_executable(zeit_helper zeithelper.cpp)
target_link_libraries(zeit_helper KF5::AuthCore)
target_include_directories(zeit_helper PRIVATE Qt5::Core KF5::Auth)

set(SYSCONF_INSTALL_DIR "/etc")
set(DBUS_SYSTEM_SERVICES_INSTALL_DIR "share/dbus-1/system-services")
install(TARGETS zeit_helper DESTINATION ${KAUTH_HELPER_INSTALL_DIR})
kauth_install_helper_files(zeit_helper local.zeit.crontab root)
kauth_install_actions(local.zeit.crontab local.zeit.crontab.actions)
