# Tetris

Tetris made with C++, SDL2 and OpenGL.

## Building

The build must first be set up with the command

```
cmake -S . -B build
```

By default the `debug` build type will be built, you can define the build type
to build by passing `-DCMAKE_BUILD_TYPE={Release|Debug}`to the previous cmake
command.

The game can then be build using the command

```
cmake --build build
```

## Running the Game

The game can be run with

```
build/game/tetris
```

## Running the Tests

The unit tests can be run with

```
cmake --build build --target run_unit_tests
```

To run the gameplay tests, use

```
cmake --build build --target run_gameplay_tests
```
