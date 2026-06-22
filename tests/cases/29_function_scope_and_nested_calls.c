int add(int a, int b) {
    return a + b;
}

int sub(int a, int b) {
    return a - b;
}

int compute(int x, int y, int z) {
    int a = add(x, y);
    int b = sub(a, z);
    return add(b, x);
}

int main() {
    int a = 5;
    int b = 3;
    int c = 2;
    int result = compute(a, b, c);
    return result + a;
}
