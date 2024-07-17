#ifndef _SZ_LPAQ
#define _SZ_LPAQ
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void lpaq_compress(int* content, size_t length, int level, unsigned char** output, size_t *output_size) {
    printf("lpaq compress, length: %d, level: %d\n", length, level);
    unsigned char* data = (unsigned char*)malloc(length * 4);

	unsigned char buffer[4];
    size_t i = 0;
    for(i = 0; i < length;i++) {
      intToBytes_bigEndian(buffer, content[i]);
      // printf("%d ", content[i]);
      memcpy(data + i * 4, buffer, 4);
    }
    // printf("\n"); 
    char tmp_input[] = "./sz_lpaq_tmp_input_com";
    char tmp_output[] = "./sz_lpaq_tmp_output_com";
    int tmp_fd = mkstemp(tmp_input);
    FILE* tmp_input_file = fdopen(tmp_fd, "w");
    fwrite(data, sizeof(char), 4 * length, tmp_input_file);
    fclose(tmp_input_file);

    char command[100];
    char* temp_filename = NULL;
    sprintf(command, "lpaq9m %d %s %s", level, tmp_input, tmp_output);
    system(command);


    FILE* tmp_output_file = fopen(tmp_output, "rb");
    fseek(tmp_output_file, 0, SEEK_END);
    *output_size = ftell(tmp_output_file);
    fseek(tmp_output_file, 0, SEEK_SET);
    *output = (unsigned char*)malloc(4 + *output_size);
    intToBytes_bigEndian(buffer, *output_size);
    memcpy(*output, buffer, 4);

    fread(*output+4, 1, *output_size, tmp_output_file);
    fclose(tmp_output_file);

    remove(tmp_input);
    remove(tmp_output);
    free(data);
    *output_size = *output_size + 4;
}


void lpaq_decompress(unsigned char* content, size_t length, int* out) {
    printf("lpaq decompress, length:%d\n", length);
    unsigned char buffer[4];
    memcpy(buffer, content, 4);
    int tmp_size = bytesToInt_bigEndian(buffer);
    char tmp_input[] = "./sz_lpaq_tmp_input_dec";
    char tmp_output[] = "./sz_lpaq_tmp_output_dec";

    int tmp_fd = mkstemp(tmp_input);
    FILE* tmp_input_file = fdopen(tmp_fd, "wb");
    fwrite(content+4, 1, tmp_size, tmp_input_file);
    fclose(tmp_input_file);
    char command[100];
    sprintf(command, "lpaq9m d %s %s", tmp_input, tmp_output);
    system(command);
    printf("%s\n", command);

    FILE* tmp_output_file = fopen(tmp_output, "rb");

    for (size_t i = 0; i < length; i++) {
        fread(buffer, 1, 4, tmp_output_file);
        out[i] = bytesToInt_bigEndian(buffer);
        // printf("%d ", out[i]);
    }
    // printf("\n");
    fclose(tmp_output_file);
    remove(tmp_input);
    remove(tmp_output);
}

#endif
