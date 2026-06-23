#include <stdio.h>

// Tri par insertion - tableau de caractères (codes ASCII)
int char_insertion_sort(char *arr, int size) {
    int i, j;
    char key;
    int placed;

    i = 1;
    putchar(size);  // Affiche la taille du tableau pour vérification
    while (i < size) {
        key = arr[i];
        j = i - 1;

        // Décalage par insertion sans operateur && :
        // un if imbrique remplace la condition "arr[j] > key".
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
    return 0;
}

int main() {
    // Tableau de caractères : 'd', 'b', 'a', 'e', 'c'
    char arr[5] = {'d', 'b', 'a', 'e', 'c'};
    int i;
    int sum;

    // Trier le tableau
    char_insertion_sort(arr, 5);

    // Vérifier la somme des codes ASCII
    // 'd'(100) + 'b'(98) + 'a'(97) + 'e'(101) + 'c'(99) = 495
    sum = 0;
    i = 0;
    while (i < 5) {
        sum = sum + arr[i];
        i = i + 1;
    }

    // Retourner le premier élément du tableau trié : devrait être 'a' (97)
    if (sum == 495) {
        return arr[0];  // 'a' = 97
    } else {
        return 1;  // Erreur
    }
}
