int test(int i) {
    i = i + 1;
    return i;
}

void main() {
    int i = 0;
    while (i < 10) {
        test(i);
    }
}