// Fonction pour afficher un caractère (retourne sa valeur)
int my_putchar(char c) {
    return c;
}

// Affiche les chiffres d'un nombre en appelant my_putchar
int my_put_nbr_recur(int nb) {
    if (nb > 9) {
        my_put_nbr_recur(nb / 10);
    }
    my_putchar(nb % 10 + 48);
    return 0;
}

// Affiche un nombre avec gestion du signe
int my_put_nbr(int nb) {
    int lg = nb;
    if (lg < 0) {
        my_putchar('-');
        lg = lg * -1;
    }
    my_put_nbr_recur(lg);
    return 0;
}

int main() {
    // Test 1 : afficher 123
    my_put_nbr(123);
    
    // Test 2 : afficher -45
    my_put_nbr(-45);
    
    // Test 3 : afficher 0
    my_put_nbr(0);
    
    // Test 4 : afficher un grand nombre
    my_put_nbr(9876);
    
    // Retourner 0 si tous les appels ont réussi
    return 0;
}
