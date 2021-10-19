message(STATUS "Configuring SDL2 vendor library")
add_subdirectory(SDL EXCLUDE_FROM_ALL)

message(STATUS "Configuring Catch2 vendor library")
add_subdirectory(catch EXCLUDE_FROM_ALL)

message(STATUS "Configuring GLAD vendor library")
add_subdirectory(glad EXCLUDE_FROM_ALL)
