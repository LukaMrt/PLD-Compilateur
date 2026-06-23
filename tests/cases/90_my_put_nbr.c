// Fonction stub pour afficher un caractère (simulation)
// Dans un vrai programme, ce serait write(1, &c, 1) ou putchar(c)
int my_putchar(char c) {
    // Retourne le code ASCII du caractère (pour vérification en test)
    return c;
}

// Fonction recérsive pour afficher les chiffres
int my_put_nbr_recur(int nb) {
    if (nb > 9) {
        my_put_nbr_recur(nb / 10);
    }
    my_putchar(nb % 10 + 48);  // 48 = code ASCII de '0'
    return 0;
}

// Fonction pour afficher un nombre entier avec gestion du signe
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
    // Afficher plusieurs nombres
    my_put_nbr(42);      // Affiche "42"
    my_put_nbr(-7);      // Affiche "-7"
    my_put_nbr(128);     // Affiche "128"
    my_put_nbr(0);       // Affiche "0"
    my_put_nbr(-999);    // Affiche "-999"
    
    // Retourner 0 si succès
    return 0;
}
