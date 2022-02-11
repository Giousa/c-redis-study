cmake_minimum_required(VERSION 3.19.2)
project(c_redis_study C)

set(CMAKE_C_STANDARD 99)

add_executable(c_redis_study main.c src/zmalloc.c src/zmalloc.h src/sds.c src/sds.h)
