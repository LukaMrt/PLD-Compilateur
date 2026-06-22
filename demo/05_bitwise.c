int main() {
    int a = 12;   /* 1100 */
    int b = 10;   /* 1010 */
    int r_and = a & b;   /* 1000 = 8  */
    int r_or  = a | b;   /* 1110 = 14 */
    int r_xor = a ^ b;   /* 0110 = 6  */
    return r_and;
}
