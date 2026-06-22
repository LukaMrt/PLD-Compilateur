int add(int a, int b) {
    return a + b;
}

int compute(int x, int y, int z) {
    int a = add(x, y);
    return add(a, z);
}

int main() {
    return compute(3, 4, 5);  /* 12 */
}
