#include "big_num.h"
#include <stdio.h>

#define Max_len 128

void big_num_add(big_num_t *a, big_num_t *b, big_num_t *c)
{
    int a_size = a->num_size;
    int b_size = b->num_size;
    int max_size = (a_size > b_size) ? a_size : b_size;

    int carry = 0;
    int a_num;
    int b_num;

    for (int i = 0; i < max_size; i++) {
        int total_sum;
        if (a->num_string[i]) {
            a_num = a->num_string[i] - '0';
        } else {
            a_num = 0;
        }
        if (b->num_string[i]) {
            b_num = b->num_string[i] - '0';
        } else {
            b_num = 0;
        }
        total_sum = a_num + b_num + carry;
        c->num_string[i] = (total_sum % 10) + '0';
        c->num_size += 1;
        carry = total_sum / 10;
    }
    if (carry) {
        c->num_string[max_size] = carry + '0';
        max_size += 1;
    }
    c->num_size = max_size;
    return;
};

void big_num_init(big_num_t *input, long long num)
{
    int bits = num_digits(num);
    input->num_size = bits;
    input->num_string = malloc(Max_len * sizeof(char));
    int i = 0;
    while (bits--) {
        int num_c = num % 10;
        input->num_string[i++] = (num_c + '0');
        num /= 10;
    }
    return;
}

void reverse_bn(big_num_t *a)
{
    char *buf = malloc(Max_len * sizeof(char));
    strncpy(buf, a->num_string, (Max_len * sizeof(char)));
    int bit_num = a->num_size;
    int j = 0;
    for (int i = bit_num - 1; i >= 0; i--) {
        a->num_string[j++] = buf[i];
    }
}

int num_digits(long long num)
{
    int result = 0;
    while (num) {
        result += 1;
        num /= 10;
    }
    return result;
}

// void bn_swap(big_num_t *a, big_num_t *b)
// {
//     big_num_t tmp = *a;
//     *a = *b;
//     *b = tmp;
// }
