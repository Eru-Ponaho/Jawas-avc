cmake_minimum_required(VERSION 3.5)
project(Jawas_avc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    Software/camera_reading.c
    Software/Derivative_Ideas.c
    Software/infrared_motor_op.c
    Software/motor_go_straight.c
    Software/motor_turn_test.c
    Software/skeleton_code.c
    lt_PiCode.cpp
    Master_Code.c)

add_executable(Jawas_avc ${SOURCE_FILES})