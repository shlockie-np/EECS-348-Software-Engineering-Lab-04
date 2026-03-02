cmake_minimum_required(VERSION 3.20)
project(nfl_score C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(nfl_score nfl_scores.c)
add_executable(temp_convert temp_conv.c)