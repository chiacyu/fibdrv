#include <linux/device.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <stdio.h>

typedef struct bignum {
    char *num_digit;
    uint8_t num_count;
} bignum_t;

/* add two numbers*/
void add_num(bignum_t *a, bignum_t *b);

/* substract two numbers*/
void subtract_num(bignum_t *a, bignum_t *b);
