#include <stdio.h>

int main() {
    char c = 'A';
    putchar(c);       /* affiche 'A' */

    char input = getchar();
    putchar(input);   /* réaffiche ce qu'on a lu */

    return 0;
}
