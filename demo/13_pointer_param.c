int write_through(int *ptr) {
    *ptr = 99;
    return *ptr;
}

int main() {
    int w = 5;
    int result = write_through(&w);
    return result;  /* 99 */
}
