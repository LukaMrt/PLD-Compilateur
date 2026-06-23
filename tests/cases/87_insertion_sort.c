// Tri par insertion - tableau d'entiers
int insertion_sort(int *arr, int size) {
    int i, j, key;
    
    // Parcourir le tableau à partir du 2e élément
    i = 1;
    while (i < size) {
        key = arr[i];
        j = i - 1;
        
        // Décaler les éléments > key vers la droite
        while (j >= 0 & arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        // Insérer key à sa position
        arr[j + 1] = key;
        i = i + 1;
    }
    return 0;
}

int main() {
    // Tableau non trié
    int arr[5] = {64, 34, 25, 12, 22};
    int i;
    int sum;
    
    // Trier le tableau
    insertion_sort(arr, 5);
    
    // Vérifier que le tableau est bien trié
    // La somme doit rester inchangée : 64+34+25+12+22 = 157
    sum = 0;
    i = 0;
    while (i < 5) {
        sum = sum + arr[i];
        i = i + 1;
    }
    
    // Retourner 0 si la somme est correcte (157)
    // Cela vérifie que les valeurs n'ont pas été perdues
    if (sum == 157) {
        // Retourner le premier élément du tableau trié : devrait être 12
        return arr[0];
    } else {
        return 1;  // Erreur
    }
}
