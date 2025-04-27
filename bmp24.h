#ifndef BMP24_H
#define BMP24_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


#define BITMAP_MAGIC 0x00
#define BITMAP_SIZE 0x02
#define BITMAP_OFFSET 0x0A
#define BITMAP_WIDTH 0x12
#define BITMAP_HEIGHT 0x16
#define BITMAP_DEPTH 0x1C
#define BITMAP_SIZE_RAW 0x22

#define BMP_TYPE 0x4D42


#define HEADER_SIZE 0x0E
#define INFO_SIZE 0x28


#define DEFAULT_DEPTH 0x18 // 24


typedef struct {
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
} t_bmp_header;
