#include <openssl/bio.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "img_to_base64.h" 



//will be used for saving images to database <<<<<<<<<<<<<<<<<<-------------(under construction)-------------------------->>>>>>>>>>>>>>>>>>>




//convert base64 format back to image
int convert_back_to_jpg(char* base64) {
    char* in_buf = (char*)malloc(strlen(base64) + 1);
    strcpy(in_buf, base64);

    // Create a memory buffer to hold the decoded data
    BIO* bio_mem = BIO_new_mem_buf(in_buf, -1);

    // Decode base64 to binary
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio_mem = BIO_push(b64, bio_mem);
    int out_len = BIO_read(bio_mem, in_buf, strlen(base64));

    // Open output file (decoded image)
    FILE* out_file = fopen("decoded_image3.jpg", "wb");
    if (!out_file) {
        perror("Failed to open output file");
        return 1;
    }

    // Write decoded data to output file
    fwrite(in_buf, 1, out_len, out_file);

    // Close output file
    fclose(out_file);
    ShellExecute(NULL, "open", "decoded_image3.jpg", NULL, NULL, SW_SHOWNORMAL);
    // Clean up
    BIO_free_all(bio_mem);
    free(in_buf);
    return 0;
}


//converts image to base_64 format
char* encode_to_base_64(char* uri, char* base64_data) {
    // Open input file
    FILE* in_file = fopen(uri, "rb");
    if (!in_file) {
        perror("Failed to open input file");
        return 1;
    }
    // Create base64 BIO
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO* bio_mem = BIO_new(BIO_s_mem());
    bio_mem = BIO_push(b64, bio_mem);
    BIO_set_flags(bio_mem, BIO_FLAGS_BASE64_NO_NL);

    // Encode image to base64
    char in_buf[512];
    int in_len;
    while ((in_len = fread(in_buf, 1, sizeof(in_buf), in_file)) > 0) {
        BIO_write(bio_mem, in_buf, in_len);
    }
    BIO_flush(bio_mem);

    // Close input file
    fclose(in_file);

    // Get the base64 encoded data from the BIO
    /*char* base64_data;*/
    size_t base64_data_len;
    BIO_get_mem_data(bio_mem, &base64_data);
    base64_data_len = BIO_get_mem_data(bio_mem, &base64_data);
    /*convert_back_to_jpg(base64_data);*/
    // Clean up
    BIO_free_all(bio_mem);
    return base64_data;
}