// cas 6 : paramètre pointeur — écriture et lecture au travers du paramètre,
// et passage d'une adresse (&w) en argument
int write_through(int *ptr) {
    *ptr = 99;             // écrit via le paramètre pointeur
    return *ptr;           // relit via le paramètre pointeur -> 99
}

int main() {
    int a = 10;

    // cas 0 : déclaration sans initialisation puis affectation
    int *q;
    q = &a;
    int via_q = *q;        // via_q == 10

    // cas 1 : référencement simple (&a) et lecture via déréférencement (*p)
    int *p = &a;
    int lue = *p;          // lue == 10

    // cas 2 : écriture via déréférencement (*p = ...)
    *p = 42;               // a == 42 maintenant
    int apres_ecriture = a; // apres_ecriture == 42

    // cas 3 : pointeur de pointeur (**pp)
    int **pp = &p;
    int double_deref = **pp; // double_deref == 42

    // cas 4 : écriture via double déréférencement
    **pp = 7;              // a == 7 maintenant
    int apres_double = a;  // apres_double == 7

    // cas 5 : déréférencement des deux côtés (*p = *q)
    int b = 3;
    int *r = &b;
    *r = *q;               // *q relit a, qui vaut 7 maintenant -> b == 7
    int apres_copie = b;   // apres_copie == 7

    // cas 6 : appel d'une fonction avec un paramètre pointeur
    int w = 5;
    int renvoye = write_through(&w); // renvoye == 99, et w vaut 99 ensuite

    return via_q + lue + apres_ecriture + double_deref + apres_double + apres_copie
         + renvoye + w;
    // 10 + 10 + 42 + 42 + 7 + 7 + 99 + 99 = 316  (code de sortie 316 % 256 = 60)
}
