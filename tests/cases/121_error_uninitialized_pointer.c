int main() {
    int x = 5;
    int *ptr;
    ptr[0] = 10; // Dereferencing uninitialized pointer
    return *ptr;
}
