int main() {
    int arr[4] = {10, 20, 30, 40};
    arr[0] = arr[3];
    arr[2] = arr[0] + arr[1];
    return arr[2];
}
