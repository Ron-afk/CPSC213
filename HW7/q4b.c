#include <stdio.h>
int x[8] = {1, 2, 3, -1, -2, 0, 184, 340057058};
int y[8] = {0, 0, 0, 0, 0, 0, 0, 0};

int f(int a) {
    int i = 0;
    while(a) {
        if((a & 0x80000000) != 0) {
            i++;
        }
    a <<= 1;
    }
    return i;
}

int main() {
    int i = 8;
    while (i != 0) {
        i--;
        y[i] = f(x[i]);
    } 

    for(int i = 0; i < 8; i++) {
        printf("%d ",x[i]);
    }
    printf("\n");
    for(int i = 0; i < 8; i++) {
        printf("%d ",y[i]);
    }
    return 0;

}