#include <stdio.h>
#include "int_element.h"
#include "refcount.h"


/* TODO: Implement all public int_element functions, including element interface functions.

You may add your own private functions here too. */
struct int_element_class {
    void(*print)(void*);
    int(*compare)(void*);
    int(*int_element_get_value)(void*);
    int(*is_int_element)(void*);
};

struct int_element {
    struct int_element_class *class;
    int value;
}; 

int int_element_get_value(struct int_element *e) {
    return e -> value;
}

int is_int_element(struct element *e) {
    
}

void int_print(struct element *e) {
    struct int_element *this = e;
    printf("%i", this->value);
}

int int_compare(struct element *e1, struct element *e2) {

}

struct int_element_class int_element_class = {int_print, int_compare, int_element_get_value, is_int_element};

struct int_element *int_element_new(int value) {
    struct int_element *e = rc_malloc(sizeof(struct int_element), 0);
    e -> class = &int_element_class;
    e -> value = value;
    return e;
};

