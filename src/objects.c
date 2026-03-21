#include "/home/droy/Projects/Test/mini_git/include/objects.h"
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <zlib.h>

char *hash_object(const char *filepath) {
  FILE *file = fopen(filepath, "rb");
  if (!file) {
    perror("Failed to open file!");
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

  char header[64];
  int header_len = snprintf(header, sizeof(header), "blob %ld", file_size);

  long store_size = header_len + 1 + file_size;
  unsigned char *store_data = malloc(store_size);

  memcpy(store_data, header, header_len);
  store_data[header_len] = '\0';
  memcpy(store_data + header_len + 1, file_content, file_size);
  free(file_content);

  unsigned char hash[SHA_DIGEST_LENGTH];
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
      perror("Failed to create object directory!");
      free(store_data);
      free(hex_hash);
      return NULL;
    }
  }

  // 5. Compress the data with zlib and save it
  char file_path[512];
  snprintf(file_path, sizeof(file_path), ".minigit/objects/%.2s/%s", hex_hash,
           hex_hash + 2);

  // Calculate maximum compressed size and allocate buffer
  uLongf compressed_size = compressBound(store_size);
  unsigned char *compressed_data = malloc(compressed_size);

  if (compress(compressed_data, &compressed_size, store_data, store_size) !=
      Z_OK) {
    fprintf(stderr, "Failed to compress object data\n");
    free(store_data);
    free(compressed_data);
    free(hex_hash);
    return NULL;
  }

  // Write the compressed data to the file
  FILE *out_file = fopen(file_path, "wb");
  if (out_file) {
    fwrite(compressed_data, 1, compressed_size, out_file);
    fclose(out_file);
  } else {
    perror("Failed to write object file");
  }

  free(store_data);
  free(compressed_data);

  return hex_hash;
}
