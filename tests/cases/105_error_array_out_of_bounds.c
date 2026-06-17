int main() {
    int arr[3] = {1, 2, 3};
    arr[5] = 10; // Out of bounds (compiler may not catch at compile time)
    return arr[5];
}
