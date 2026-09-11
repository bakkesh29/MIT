
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

int main()
{
    char filename[256];
    struct stat fileInfo;

    printf("Enter the filename: ");
    scanf("%255s", filename);

    if (stat(filename, &fileInfo) == -1)
    {
        perror("stat");
        return 1;
    }

    printf("\nFile information\n");
    printf("------------------------------------\n");

    printf("File name         : %s\n", filename);
    printf("Device ID         : %llu\n", (unsigned long long)fileInfo.st_dev);
    printf("Inode number      : %llu\n", (unsigned long long)fileInfo.st_ino);
    printf("File mode         : %o\n", (unsigned int)fileInfo.st_mode);
    printf("Permissions       : %o\n", (unsigned int)(fileInfo.st_mode & 0777));
    printf("Number of hard links : %llu\n", (unsigned long long)fileInfo.st_nlink);
    printf("Owner user ID     : %u\n", (unsigned int)fileInfo.st_uid);
    printf("Owner group ID    : %u\n", (unsigned int)fileInfo.st_gid);
    printf("Special device ID : %llu\n", (unsigned long long)fileInfo.st_rdev);
    printf("File size         : %lld bytes\n", (long long)fileInfo.st_size);
    printf("Block size        : %lld bytes\n", (long long)fileInfo.st_blksize);
    printf("Number of blocks  : %lld\n", (long long)fileInfo.st_blocks);
    printf("Last access time  : %s", ctime(&fileInfo.st_atime));
    printf("Last modification : %s", ctime(&fileInfo.st_mtime));
    printf("Last status change: %s", ctime(&fileInfo.st_ctime));

    printf("File type         : ");
    if (S_ISREG(fileInfo.st_mode))
        printf("regular file\n");
    else if (S_ISDIR(fileInfo.st_mode))
        printf("Directory\n");
    else if (S_ISLNK(fileInfo.st_mode))
        printf("Symbolic link\n");
    else if (S_ISCHR(fileInfo.st_mode))
        printf("Character device\n");
    else if (S_ISBLK(fileInfo.st_mode))
        printf("Block device\n");
    else if (S_ISFIFO(fileInfo.st_mode))
        printf("FIFO or named pipe\n");
    else if (S_ISSOCK(fileInfo.st_mode))
        printf("Socket\n");
    else
        printf("Unknown file type\n");

    return 0;
}


