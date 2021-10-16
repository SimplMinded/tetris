add_executable(unit_test test.cpp)

add_custom_target(run_unit_tests unit_test)
add_dependencies(run_unit_tests unit_test)
