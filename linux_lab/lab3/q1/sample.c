#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    FILE *fp;
    char line[1024];
    int found = 0;

    if (argc != 3) {
        printf("Usage: %s word filename\n", argv[0]);
        return 1;
    }

    fp = fopen(argv[2], "r");
    if (fp == NULL) {
        printf("Cannot open file %s\n", argv[2]);
        return 1;
    }

    printf("Lines in %s containing \"%s\":\n", argv[2], argv[1]);

    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strstr(line, argv[1]) != NULL) {
            printf("%s", line);
            found = 1;
        }
    }

    if (!found)
        printf("No matching lines found.\n");

    fclose(fp);
    return 0;
}
