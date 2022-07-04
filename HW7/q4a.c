#include<stdio.h>
int arr[10] = {0,0,0,0,0,0,0,0,0,0};
int *i = &arr;

void add(int a, int b) {
    *(i+b) += a;
}

int main() {
    int a = 1;
    int b = 2;
    int c = 3;
    int d = 4; 
    add(c,d);
    c = a;
    d = b;
    add(c,d);
    for(int i =0; i<10;i++)
    printf("%d ", arr[i]);
    return 0;
}

