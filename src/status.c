#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "status.h"

int print_status(void) {
    printf("On branch master\n\n");

    FILE *index_file = fopen(".minigit/index", "r");
    if (!index_file) {
        printf("No commits yet\n\n");
        printf("nothing to commit (create/copy files and use \"minigit add\" to track)\n");
        return 0;
    }

    printf("Changes to be committed:\n");
    printf("  (use \"minigit commit\" to record your changes)\n\n");

    char line[512];
    int has_files = 0;
    
    // Parse the index file
    while (fgets(line, sizeof(line), index_file)) {
        char mode[16], hash[64], path[256];
        // Read the mode, hash, and file path from our index format
        if (sscanf(line, "%s %s %[^\n]", mode, hash, path) == 3) {
            // Standard git uses green text here, but we'll stick to basic formatting
            printf("\tstaged:   %s\n", path);
            has_files = 1;
        }
    }
    fclose(index_file);

    if (!has_files) {
        printf("nothing to commit, working tree clean\n");
    } else {
        printf("\n");
    }

    return 0;
}
