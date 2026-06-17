int foo(int *x) {
    return *x;
}

int main() {
    int a = 5;
    return foo(a); // Passing non-pointer where pointer expected
}
