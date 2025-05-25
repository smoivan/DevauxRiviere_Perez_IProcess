cmake_minimum_required(VERSION 3.16)
project(projetC C)

set(CMAKE_C_STANDARD 11)

add_executable(main
        main.c
        bmp8.c
        bmp8equalize.c
        bmp24equalize.c
        bmp24.c
)