cmake_minimum_required(VERSION 3.10)
project(traitement_image C)

set(CMAKE_C_STANDARD 99)

add_executable(traitement_image
        main.c
        bmp8.c
        bmp24.c
        bmp8equalize.c
        bmp24equalize.c
)

target_link_libraries(traitement_image m)