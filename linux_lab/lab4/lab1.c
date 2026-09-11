
#include <stdio.h>
#include <sys/stat.h>

int main() {
    char filename[256];
    struct stat file_info;

    // Prompt user to enter the file name
    printf("Enter the filename: ");
    if (scanf("%255s", filename) != 1) {
        fprintf(stderr, "Error reading input\n");
        return 1;
    }

    // Call stat() to retrieve file metadata
    if (stat(filename, &file_info) != 0) {
        perror("stat failed");
        return 1;
    }

    // Print the inode number stored in st_ino
    printf("Inode number of '%s': %lu\n", filename, (unsigned long)file_info.st_ino);

    return 0;
}
