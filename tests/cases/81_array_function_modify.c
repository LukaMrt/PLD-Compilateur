int modify_array(int arr[3]) {
    arr[0] = 100;
    arr[1] = 200;
    arr[2] = 300;
    return arr[0] + arr[1] + arr[2];
}

int main() {
    int a[3] = {1, 2, 3};
    return modify_array(a);
}
