int f() {
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
    *r = *q;               // b = via_q = 10, mais q pointait sur a qui vaut 7 maintenant
    int apres_copie = b;   // apres_copie == 7

    return via_q + lue + apres_ecriture + double_deref + apres_double + apres_copie;
    // 10 + 10 + 42 + 42 + 7 + 7 = 118
}
