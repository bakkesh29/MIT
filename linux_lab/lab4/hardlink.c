

#include <stdio.h>
#include <unistd.h>

int main()
{
    char oldPath[256];
    char newPath[256];

    printf("Enter the existing file path: ");
    scanf("%255s", oldPath);

    printf("Enter the new hard-link path: ");
    scanf("%255s", newPath);

    if (link(oldPath, newPath) == -1)
    {
        perror("link");
        return 1;
    }

    printf("\nHard link created successfully.\n");
    printf("Old path : %s\n", oldPath);
    printf("New path : %s\n", newPath);

    printf("\nPress Enter to unlink the new path...");
    getchar(); /* Removes newline left by scanf */
    getchar(); /* Waits for Enter */

    if (unlink(newPath) == -1)
    {
        perror("unlink");
        return 1;
    }

    printf("Hard link removed successfully.\n");
    printf("Unlinked path: %s\n", newPath);

    return 0;
}

