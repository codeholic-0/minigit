#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <openssl/sha.h>
#include <zlib.h>
#include "objects.h"

char *write_object(const char *type, const void *data, size_t size) {
    char header[128];
    int header_len = snprintf(header, sizeof(header), "%s %zu", type, size);
    
    size_t store_size = header_len + 1 + size;
    unsigned char *store_data = malloc(store_size);
    if (!store_data) return NULL;
    
    memcpy(store_data, header, header_len);
    store_data[header_len] = '\0';
    memcpy(store_data + header_len + 1, data, size);

    unsigned char hash[SHA_DIGEST_LENGTH]; // 20 bytes
    SHA1(store_data, store_size, hash);

    char *hex_hash = malloc(SHA_DIGEST_LENGTH * 2 + 1);
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        sprintf(&hex_hash[i * 2], "%02x", hash[i]);
    }
    hex_hash[SHA_DIGEST_LENGTH * 2] = '\0';

    char dir_path[256];
    snprintf(dir_path, sizeof(dir_path), ".minigit/objects/%.2s", hex_hash);
    
    struct stat st = {0};
    if (stat(dir_path, &st) == -1) {
        if (mkdir(dir_path, 0755) != 0) {
            perror("Failed to create object directory");
            free(store_data);
            free(hex_hash);
            return NULL;
        }
    }

    char file_path[512]; 
    snprintf(file_path, sizeof(file_path), ".minigit/objects/%.2s/%s", hex_hash, hex_hash + 2);

    uLongf compressed_size = compressBound(store_size);
    unsigned char *compressed_data = malloc(compressed_size);

    if (compress(compressed_data, &compressed_size, store_data, store_size) != Z_OK) {
        fprintf(stderr, "Failed to compress object data\n");
        free(store_data);
        free(compressed_data);
        free(hex_hash);
        return NULL;
    }

    FILE *out_file = fopen(file_path, "wb");
    if (out_file) {
        fwrite(compressed_data, 1, compressed_size, out_file);
        fclose(out_file);
    } else {
        perror("Failed to write object file");
    }

    //Cleanup
    free(store_data);
    free(compressed_data);

    return hex_hash;
}

char *hash_object(const char *filepath) {
    // Read the entire file into memory
    FILE *file = fopen(filepath, "rb");
    if (!file) {
        perror("Failed to open file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    unsigned char *file_content = malloc(file_size);
    if (fread(file_content, 1, file_size, file) != (size_t)file_size) {
        perror("Failed to read file");
        free(file_content);
        fclose(file);
        return NULL;
    }
    fclose(file);

    // Pass the file content to our new core engine, labeling it a "blob"
    char *hash = write_object("blob", file_content, file_size);
    
    free(file_content);
    return hash;
}
