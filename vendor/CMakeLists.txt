message("Configuring SDL2 vendor library")
add_subdirectory(SDL EXCLUDE_FROM_ALL)

message("Configuring Catch2 vendor library")
add_subdirectory(catch EXCLUDE_FROM_ALL)

message("Configuring GLAD vendor library")
add_subdirectory(glad EXCLUDE_FROM_ALL)
