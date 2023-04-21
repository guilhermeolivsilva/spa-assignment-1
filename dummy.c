int sum(int x, int y) {
    if (x == 0) {
        return y;
    }
    return x+y;
}

int main() {
    int x = 1;
    int y = 2;
    int z = sum(x,y);
    return 0;
}
