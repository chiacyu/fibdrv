#include "big_num.h"
#include <linux/slab.h>
#include <linux/types.h>
#define XOR_SWAP(a, b, type) \
    do {                     \
        type *__c = (a);     \
        type *__d = (b);     \
        *__c ^= *__d;        \
        *__d ^= *__c;        \
        *__c ^= *__d;        \
    } while (0)


#define Max_len 128

// static void big_num_add(big_num_t *a, big_num_t *b, big_num_t *c)
// {
//     int a_size = a->num_size;
//     int b_size = b->num_size;
//     int max_size = (a_size > b_size) ? a_size : b_size;

//     int carry = 0;
//     int a_num;
//     int b_num;

//     for (int i = 0; i < max_size; i++) {
//         int total_sum;
//         if (a->num_string[i]) {
//             a_num = a->num_string[i] - '0';
//         } else {
//             a_num = 0;
//         }
//         if (b->num_string[i]) {
//             b_num = b->num_string[i] - '0';
//         } else {
//             b_num = 0;
//         }
//         total_sum = a_num + b_num + carry;
//         c->num_string[i] = (total_sum % 10) + '0';
//         c->num_size += 1;
//         carry = total_sum / 10;
//     }
//     if (carry) {
//         c->num_string[max_size] = carry + '0';
//         max_size += 1;
//     }
//     c->num_size = max_size;
//     return;
// };

static void big_num_fix_add(big_num_fix_t *ina,
                            big_num_fix_t *inb,
                            big_num_fix_t *out)
{
    size_t size_a = strlen(ina->num);
    size_t size_b = strlen(inb->num);
    int i, sum, carry = 0;
    if (size_a >= size_b) {
        for (i = 0; i < size_b; i++) {
            sum = (ina->num[i] - '0') + (inb->num[i] - '0') + carry;
            out->num[i] = '0' + sum % 10;
            carry = sum / 10;
        }

        for (i = size_b; i < size_a; i++) {
            sum = (ina->num[i] - '0') + carry;
            out->num[i] = '0' + sum % 10;
            carry = sum / 10;
        }
    } else {
        for (i = 0; i < size_a; i++) {
            sum = (ina->num[i] - '0') + (inb->num[i] - '0') + carry;
            out->num[i] = '0' + sum % 10;
            carry = sum / 10;
        }

        for (i = size_a; i < size_b; i++) {
            sum = (inb->num[i] - '0') + carry;
            out->num[i] = '0' + sum % 10;
            carry = sum / 10;
        }
    }

    if (carry)
        out->num[i++] = '0' + carry;
    out->num[i] = '\0';
}

static void big_num_fix_mul(big_num_fix_t *ina,
                            big_num_fix_t *inb,
                            big_num_fix_t *out)
{
    int carry = 0, sum, base;
    bool carry_flag = false;
    size_t size_a = strlen(ina->num);
    size_t size_b = strlen(inb->num);
    int bottom_index = size_a + size_b;
    for (int i = 0; i < size_b; i++) {
        for (int j = 0; j < size_a; j++) {
            base = out->num[j + i] - '0';
            if (i == 0)
                base = 0;
            sum = ((inb->num[i] - '0') * (ina->num[j] - '0')) + carry + base;
            carry = sum / 10;
            out->num[j + i] = (sum % 10) + '0';
        }
        if (carry) {
            out->num[i + size_a] = ('0' + carry);
            carry = 0;
            carry_flag = true;
        }
    }
    if (!carry_flag)
        bottom_index--;
    out->num[bottom_index] = '\0';
    return;
}

static void big_num_fix_sub(big_num_fix_t *ina,
                            big_num_fix_t *inb,
                            big_num_fix_t *out)
{
    size_t sizes = strlen(ina->num) > strlen(inb->num) ? strlen(ina->num)
                                                       : strlen(inb->num);
    for (int i = 0; i < sizes; i++) {
        if (ina->num[i] < inb->num[i]) {
            ina->num[i + 1]--;
            ina->num[i] += 10;
        }
        out->num[i] = (ina->num[i] - inb->num[i]) + '0';
    }
    out->num[--sizes] = '\0';
    return;
}

// static void big_num_init(big_num_t *input, long long num)
// {
//     if (num == 0) {
//         input->num_size = 1;
//         input->num_string = kmalloc(Max_len * sizeof(char), GFP_KERNEL);
//         input->num_string[0] = '0';
//         input->num_string[1] = '\0';
//         return;
//     }
//     input->num_size = 1;
//     input->num_string = kmalloc(Max_len * sizeof(char), GFP_KERNEL);
//     input->num_string[0] = '1';
//     input->num_string[1] = '\0';
//     return;
// }

static void __swap(void *a, void *b, size_t size)
{
    if (a == b)
        return;

    switch (size) {
    case 1:
        XOR_SWAP(a, b, char);
        break;
    case 2:
        XOR_SWAP(a, b, short);
        break;
    case 4:
        XOR_SWAP(a, b, unsigned int);
        break;
    case 8:
        XOR_SWAP(a, b, unsigned long);
        break;
    default:
        /* Do nothing */
        break;
    }
}

static void reverse_str(char *str, size_t n)
{
    for (int i = 0; i < (n >> 1); i++)
        __swap(&str[i], &str[n - i - 1], sizeof(char));
}

static void add_str(char *a, char *b, char *out)
{
    size_t size_a = strlen(a), size_b = strlen(b);
    int i, sum, carry = 0;
    if (size_a >= size_b) {
        for (i = 0; i < size_b; i++) {
            sum = (a[i] - '0') + (b[i] - '0') + carry;
            out[i] = '0' + sum % 10;
            carry = sum / 10;
        }

        for (i = size_b; i < size_a; i++) {
            sum = (a[i] - '0') + carry;
            out[i] = '0' + sum % 10;
            carry = sum / 10;
        }
    } else {
        for (i = 0; i < size_a; i++) {
            sum = (a[i] - '0') + (b[i] - '0') + carry;
            out[i] = '0' + sum % 10;
            carry = sum / 10;
        }

        for (i = size_a; i < size_b; i++) {
            sum = (b[i] - '0') + carry;
            out[i] = '0' + sum % 10;
            carry = sum / 10;
        }
    }

    if (carry)
        out[i++] = '0' + carry;
    out[i] = '\0';
}