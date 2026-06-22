int main() {
    int x = 5;
    int result = 0;
    while (x > 0) {
        if (x > 2) {
            result = result + 10;
        } else {
            result = result + 1;
        }
        x = x - 1;
    }
    return result;  /* 3*10 + 2*1 = 32 */
}
