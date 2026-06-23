int main() {
    int size = 5;
    
    int i, j;
    char key;
    int arr[5] = {64, 34, 25, 12, 22};
    
    i = 1; // Affiche la taille du tableau pour vérification
    while (i < size) {
        key = arr[i];
        j = i - 1;
        
        // Comparer les codes ASCII
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
        i = i + 1;
    }
    return arr[0];
}
