#include "bmp24.h"
#include <string.h>


 */
void file_rawRead(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file) {
    fseek(file, position, SEEK_SET);
    fread(buffer, size, n, file);
}


 */
void file_rawWrite(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file) {
    fseek(file, position, SEEK_SET);
    fwrite(buffer, size, n, file);
}
