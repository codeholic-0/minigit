#include "/home/droy/Projects/Test/mini_git/include/init.h"
#include "/home/droy/Projects/Test/mini_git/include/objects.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void print_usage() {
  printf("usage: minigit <command> [<args>]\n\n");
  printf("Common Commands: \n init\n add\n commit\n status\n log\n");
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    print_usage();
    return EXIT_FAILURE;
  }

  const char *command = argv[1];

  if (strcmp(command, "init") == 0) {
    if (init_repo() != 0)
      return EXIT_FAILURE;
  }

  else if (strcmp(command, "add") == 0) {
    if (argc < 3) {
      fprintf(stderr, "Fatal! Nothing specified! Nothing added\n");
      fprintf(stderr, "usage: minigit add <file>\n");
      return EXIT_FAILURE;
    }
  }

  else if (strcmp(command, "commit") == 0) {
    if (argc < 3 || strcmp(argv[2], "-m") != 0) {
      fprintf(stderr, "usage: minigit commit -m '<messege>' \n");
      return EXIT_FAILURE;
    }
    printf("Routing to: create_commit(\"%s\")...\n", argv[3]);
  }
  // Temporary!! Test for Objects!
  else if (strcmp(command, "hash_object") == 0) {
    if (argc < 3) {
      fprintf(stderr, "usage: minigit hash_object\n");
      return EXIT_FAILURE;
    }
    char *hash = hash_object(argv[2]);
    if (hash) {
      printf("%s\n", hash);
      free(hash);
    } else {
      return EXIT_FAILURE;
    }
  }

  else {
    fprintf(stderr, "minigit: '%s' is not a valid command!", argv[3]);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
