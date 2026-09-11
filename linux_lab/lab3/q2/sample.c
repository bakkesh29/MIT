#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    FILE *fp;
    char line[1024];
    int i, count;
    int ch;

    if (argc < 2) {
        printf("Usage: %s file1 [file2 ...]\n", argv[0]);
        return 1;
    }

    for (i = 1; i < argc; i++) {
        fp = fopen(argv[i], "r");
        if (fp == NULL) {
            printf("Cannot open file %s\n", argv[i]);
            continue;
        }

        printf(":::::::: %s ::::::::\n", argv[i]);
        count = 0;

        while (fgets(line, sizeof(line), fp) != NULL) {
            printf("%s", line);
            count++;

            if (count == 20) {
                printf("--More-- (press Enter to continue)");
                fflush(stdout);
                ch = getchar();
                (void)ch;
                printf("\n");
                count = 0;
            }
        }

        fclose(fp);
        printf("\n");
    }

    return 0;
}
