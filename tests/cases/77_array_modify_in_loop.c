int main() {
    int arr[5] = {1, 1, 1, 1, 1};
    int i = 0;
    while (i < 5) {
        arr[i] = arr[i] * 2;
        i = i + 1;
    }
    return arr[4];
}
