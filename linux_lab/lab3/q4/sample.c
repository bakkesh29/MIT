#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    FILE *src, *dest;
    int ch;
    int count = 0;

    if (argc != 3) {
        printf("Usage: %s source_file dest_file\n", argv[0]);
        return 1;
    }

    src = fopen(argv[1], "r");
    if (src == NULL) {
        printf("Cannot open source file %s\n", argv[1]);
        return 1;
    }

    dest = fopen(argv[2], "w");
    if (dest == NULL) {
        printf("Cannot open destination file %s\n", argv[2]);
        fclose(src);
        return 1;
    }

    /* Character-by-character copy using getc and putc from stdio.h */
    while ((ch = getc(src)) != EOF) {
        putc(ch, dest);
        count++;
    }

    fclose(src);
    fclose(dest);

    printf("Copied %s to %s character by character (%d characters)\n",
           argv[1], argv[2], count);
    return 0;
}
