#include <stdio.h>

int main() {
    int size = 5;

    int i, j;
    int key;
    int arr[5] = {64, 34, 25, 12, 22};
    int placed;

    i = 1; // Affiche la taille du tableau pour vérification
    while (i < size) {
        putchar(i);
        key = arr[i];
        j = i - 1;

        placed = 0;
        while (j >= 0) {
            if (placed == 0) {
                if (arr[j] > key) {
                    arr[j + 1] = arr[j];
                    j = j - 1;
                } else {
                    arr[j + 1] = key;
                    placed = 1;
                    j = -1;
                }
            }
        }
        if (placed == 0) {
            arr[0] = key;
        }
        i = i + 1;
    }
    return arr[0];
}
