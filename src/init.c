#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<string.h>
#include<unistd.h>
#include "init.h"


int init_repo(void){
  struct stat st = {0};
  if(stat(".minigit", &st) != -1){
    printf("minigit already exists in this directory!\n");
    return 0;
  }

  if(mkdir(".minigit", 0755) != 0){
    perror("Failed to create .minigit directory!");
    return -1;
  }

  if(mkdir(".minigit/objects", 0755) != 0){
    perror("Error! Failed to create objects in .minigit directory!\n");
    return -1;
  }
  if(mkdir(".minigit/refs", 0755) != 0){
    perror("Error! Failed to create refs in .minigit directory!\n");
    return -1;
  }

  if(mkdir(".minigit/refs/heads", 0755) != 0){
    perror("Error! Failed to create refs/heads in .minigit directory!\n");
    return -1;
  }

  FILE *head_file = fopen(".minigit/HEAD", "w");
  if(head_file == NULL){
    perror("Failed to create HEAD file");
    return -1;
  }

  fprintf(head_file, "ref: refs/heads/master\n");
  fclose(head_file);

  printf("Initialized empty minigit repo in .minigit/\n");
  return 0;

}
