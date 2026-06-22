int main() {
    int a = 10;
    int *p = &a;
    int lue = *p;    /* lue == 10 */

    *p = 42;         /* a == 42 */

    int **pp = &p;
    int via_pp = **pp;  /* via_pp == 42 */

    return via_pp;
}
