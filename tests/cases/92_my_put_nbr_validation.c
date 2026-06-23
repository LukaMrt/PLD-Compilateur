// Simple test: call my_put_nbr functions
int my_putchar(char c) {
    return c;
}

int my_put_nbr_recur(int nb) {
    if (nb > 9) {
        my_put_nbr_recur(nb / 10);
    }
    my_putchar(nb % 10 + 48);
    return 0;
}

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
    // Test the functions with simple values
    my_put_nbr(5);
    my_put_nbr(42);
    my_put_nbr(-7);
    
    // Return 0 to indicate success
    return 0;
}
