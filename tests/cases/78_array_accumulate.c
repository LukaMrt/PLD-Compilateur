int main() {
    int arr[6] = {5, 10, 15, 20, 25, 30};
    int result = 0;
    int i = 0;
    while (i < 6) {
        result = result + arr[i];
        i = i + 1;
    }
    return result / 15;
}
