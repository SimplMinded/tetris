add_executable(unit_test test_list_view.cpp)
target_link_libraries(unit_test catch)
#TODO: Remove this once the code has been split up into modules.
target_include_directories(unit_test PRIVATE ../../game/src)

add_custom_target(run_unit_tests unit_test)
add_dependencies(run_unit_tests unit_test)
