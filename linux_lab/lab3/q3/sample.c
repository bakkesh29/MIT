#include <stdio.h>

int main(void)
{
    int n = 255;
    unsigned int u = 4000000000u;
    double d = 123.456;
    char c = 'A';
    char str[] = "Manipal";

    printf("Integer conversion specifiers\n");
    printf("  %%d  (decimal)           : %d\n", n);
    printf("  %%i  (integer)           : %i\n", n);
    printf("  %%o  (octal)             : %o\n", n);
    printf("  %%x  (hex lowercase)     : %x\n", n);
    printf("  %%X  (hex uppercase)     : %X\n", n);
    printf("  %%u  (unsigned)          : %u\n", u);

    printf("\nFloating-point conversion specifiers\n");
    printf("  %%f  (decimal float)     : %f\n", d);
    printf("  %%e  (scientific)        : %e\n", d);
    printf("  %%E  (scientific upper)  : %E\n", d);
    printf("  %%g  (shortest of f/e)   : %g\n", d);
    printf("  %%G  (shortest upper)    : %G\n", d);

    printf("\nCharacter and string conversion specifiers\n");
    printf("  %%c  (character)         : %c\n", c);
    printf("  %%s  (string)            : %s\n", str);

    printf("\nPointer conversion specifier\n");
    printf("  %%p  (address)           : %p\n", (void *)str);

    printf("\nWidth, precision and flags\n");
    printf("  %%10d  (width 10)        : [%10d]\n", n);
    printf("  %%-10d (left justify)    : [%-10d]\n", n);
    printf("  %%08d  (zero padded)     : [%08d]\n", n);
    printf("  %%+d   (show sign)       : [%+d]\n", n);
    printf("  %%.2f  (2 decimal places): %.2f\n", d);
    printf("  %%10.2f (width+precision): [%10.2f]\n", d);

    return 0;
}
