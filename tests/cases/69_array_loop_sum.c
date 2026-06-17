int main() {
    int arr[5] = {1, 2, 3, 4, 5};
    int sum = 0;
    int i = 0;
    while (i < 5) {
        sum = sum + arr[i];
        i = i + 1;
    }
    return sum;
}
