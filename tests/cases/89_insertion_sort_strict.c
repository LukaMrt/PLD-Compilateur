// Tri par insertion avec validation stricte
int insertion_sort_validate(int *arr, int size) {
    int i, j, key;
    
    i = 1;
    while (i < size) {
        key = arr[i];
        j = i - 1;
        
        while (j >= 0 & arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
        i = i + 1;
    }
    return 0;
}

int main() {
    // Tableau initial : {50, 30, 20, 40, 10}
    int arr[5] = {50, 30, 20, 40, 10};
    int i;
    int is_sorted;
    
    // Trier le tableau
    insertion_sort_validate(arr, 5);
    
    // Vérifier que le tableau est bien trié (10, 20, 30, 40, 50)
    is_sorted = 1;
    i = 0;
    while (i < 4) {
        if (arr[i] > arr[i + 1]) {
            is_sorted = 0;
        }
        i = i + 1;
    }
    
    // Retourner 0 si bien trié
    if (is_sorted == 1) {
        // Valider aussi les valeurs exactes
        if (arr[0] == 10 & arr[1] == 20 & arr[2] == 30 & arr[3] == 40 & arr[4] == 50) {
            return 0;
        } else {
            return 2;
        }
    } else {
        return 1;
    }
}
