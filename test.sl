int main(int x) {
    int y = 5;
    {
        int z = x + y;
    }

    //int lambda(int z) {
    //    return y + z;
    //}

    //y = lambda(x);
    return y + 1;
}

int test(float x, float y, float z) {
    return 234;
}

float test2(float x, float y, float z) {
    int x_ = 0;
    int y_ = 0;
    int z_ = 0;
    return x + y + z;
}

//int x = main(5);