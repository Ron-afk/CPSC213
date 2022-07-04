#include <unistd.h>

struct S {
    int a;
    char *str;
};

char _str1[] = "Welcome! Please enter a name:\n ";
char _str2[] = "Good luck, ";
char _str3[] = "The secret phrase is \"squeamish ossifrage\"";

struct S str1={.a = 30, .str = &_str1};
struct S str2={.a = 11, .str = &_str2};
struct S str3={.a = 43, .str = &_str3};


void print(struct S s) {
    write(1,s.str,s.a);
}

void proof() {
    print(str3);
}

int main() {
    char buf[128];
    print(str1);
    unsigned int size = (unsigned int)read(0,buf,128);
    print(str2);
    write(1,buf,size);
}