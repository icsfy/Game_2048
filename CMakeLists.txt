cmake_minimum_required(VERSION 3.11)
project(Game_2048 C)

set(CMAKE_C_STANDARD 99)

add_executable(game_2048 2048.c)

target_link_libraries(game_2048 ncurses)
