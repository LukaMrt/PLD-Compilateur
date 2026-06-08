int sum8(int a, int b, int c, int d, int e, int f, int g, int h) {
    return a + b + c + d + e + f + g + h;
}

int sum7(int a, int b, int c, int d, int e, int f, int g) {
    return a + b + c + d + e + f + g;
}

int main() {
    int s8 = sum8(1, 2, 3, 4, 5, 6, 7, 8);
    int s7 = sum7(10, 20, 30, 40, 50, 60, 70);
    return s8 + s7;
}
