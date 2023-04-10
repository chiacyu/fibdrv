
typedef struct big_num {
    char *num_string;
    int num_size;
} big_num_t;

typedef struct big_num_fix {
    char num[128];
} big_num_fix_t;

/* Add two number*/
static void big_num_add(big_num_t *a, big_num_t *b, big_num_t *c);

/* Add two big_num_fix_t(ina, inb) and store result in out*/
static void big_num_fix_add(big_num_fix_t *ina,
                            big_num_fix_t *inb,
                            big_num_fix_t *out);

/* Multiply two big_num_fix_t(ina, inb) and store result in out*/
static void big_num_fix_mul(big_num_fix_t *ina,
                            big_num_fix_t *inb,
                            big_num_fix_t *out);

/* Subtract two big_num_fix_t(ina, inb) and store result in out */
static void big_num_fix_sub(big_num_fix_t *ina,
                            big_num_fix_t *inb,
                            big_num_fix_t *out);

/* string number initialization*/
static void big_num_init(big_num_t *input, long long num);
