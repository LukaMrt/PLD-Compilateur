int helper() {
    return 42;
}

int main() {
    int x = helper; // Missing function call parentheses
    return x;
}
