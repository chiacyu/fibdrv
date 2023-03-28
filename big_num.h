#include <stdio.h>

typedef struct big_num {
    char *num_string;
    size_t num_size;
} big_num_t;

/* Add two number*/
void big_num_add(big_num_t *a, big_num_t *b);

/* string number initialization*/
void big_num_init(big_num_t *input, int num);

/* Reverse string number*/
void reverse_num_string(big_num_t *a);

/* Return the size of string num*/
size_t string_num_size(big_num_t *a);

/* How many digits in decimal of num*/
int num_digits(int num);