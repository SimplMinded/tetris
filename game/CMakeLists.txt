add_executable(tetris
    src/matrix.cpp
    src/tetris.cpp
)
target_link_libraries(tetris
    glad
    SDL2-static
)
