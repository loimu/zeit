set(test_LIBS Qt5::Core Qt5::Gui Qt5::Test)
set(tests_SRCS tests.cpp)
add_executable(tests ${tests_SRCS})
target_include_directories(tests PRIVATE ${Qt5Core_INCLUDE_DIRS}
                           ${Qt5Gui_INCLUDE_DIRS}
                           ${CMAKE_CURRENT_SOURCE_DIR}/../crontablib
                           ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(tests crontab ${test_LIBS})
target_compile_features(tests PRIVATE cxx_nullptr)
