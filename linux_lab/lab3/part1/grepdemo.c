
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    FILE *fp;
    char line[200];

    if(argc != 3)
    {
        printf("Usage: %s word filename\n", argv[0]);
        return 1;
    }

    fp = fopen(argv[2], "r");

    if(fp == NULL)
    {
        printf("Cannot open file\n");
        return 1;
    }

    while(fgets(line, sizeof(line), fp) != NULL)
    {
        if(strstr(line, argv[1]) != NULL)
        {
            printf("%s", line);
        }
    }

    fclose(fp);

    return 0;
}
