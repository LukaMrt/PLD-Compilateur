int main() {
    int arr[4] = {2, 3, 2, 5};
    int product = 1;
    int i = 0;
    while (i < 4) {
        product = product * arr[i];
        i = i + 1;
    }
    return product;
}
