#include <stdio.h>

typedef struct big_num {
    char *num_string;
    size_t num_size;
} big_num_t;

/* Add two number*/
void big_num_add(big_num_t *a, big_num_t *b, big_num_t *c);

/* string number initialization*/
void big_num_init(big_num_t *input, long long num);

/* Reverse string number*/
void reverse_bn(big_num_t *a);

/* Return the size of string num*/
size_t bn_size(big_num_t *a);

/* How many digits in decimal of num*/
int num_digits(long long num);

/* swap two string num*/
void bn_swap(big_num_t *a, big_num_t *b);