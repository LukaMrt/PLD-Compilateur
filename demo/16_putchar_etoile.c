#include <stdio.h>

int my_abs(int x) {
    if (x < 0) {
        x = -x;
    }
    return x;
}

// Appartenance au triangle pointe-en-haut : apex ligne 0, base ligne 2n.
int in_up(int r, int col, int n, int C) {
    int res;
    res = 0;
    if (r <= 2 * n) {
        if (my_abs(col - C) <= r) {
            res = 1;
        }
    }
    return res;
}

// Appartenance au triangle pointe-en-bas : apex ligne 3n, base ligne n.
int in_down(int r, int col, int n, int C) {
    int res;
    res = 0;
    if (r >= n) {
        if (my_abs(col - C) <= (3 * n - r)) {
            res = 1;
        }
    }
    return res;
}

// Union des deux triangles (pas de || : deux if successifs).
int inside(int r, int col, int n, int C) {
    int res;
    res = 0;
    if (in_up(r, col, n, C)) {
        res = 1;
    }
    if (in_down(r, col, n, C)) {
        res = 1;
    }
    return res;
}

// Cellule au bord = dans l'union mais avec au moins un voisin hors union.
int is_edge(int r, int col, int n, int C) {
    int res;
    res = 0;
    if (inside(r, col, n, C)) {
        if (inside(r - 1, col, n, C) == 0) {
            res = 1;
        }
        if (inside(r + 1, col, n, C) == 0) {
            res = 1;
        }
        if (inside(r, col - 1, n, C) == 0) {
            res = 1;
        }
        if (inside(r, col + 1, n, C) == 0) {
            res = 1;
        }
    }
    return res;
}

int main() {
    int n;
    int C;
    int R;
    int W;
    int r;
    int col;

    n = 8;         // taille de l'étoile
    C = 3 * n;     // colonne centrale
    R = 3 * n;     // derniere ligne
    W = 6 * n + 1; // largeur

    r = 0;
    while (r <= R) {
        col = 0;
        while (col < W) {
            if (is_edge(r, col, n, C)) {
                putchar('*');
            } else {
                putchar(' ');
            }
            col = col + 1;
        }
        putchar('\n');
        r = r + 1;
    }
    return 0;
}
