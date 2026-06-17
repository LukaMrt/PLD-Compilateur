int test(int x) {
    int x = 10; // Redeclaration of parameter
    return x;
}

int main() {
    return test(5);
}
