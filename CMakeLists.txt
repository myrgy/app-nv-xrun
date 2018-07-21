cmake_minimum_required(VERSION 3.10)

find_package(KernelHeaders REQUIRED)
add_executable(gpu-ctl src/main.c)
