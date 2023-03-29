#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "big_num.c"

#define FIB_DEV "/dev/fibonacci"

int main()
{
    big_num_t a;
    big_num_t b;
    big_num_t c;
    big_num_init(&a, 23567342);
    big_num_init(&b, 7651);
    big_num_init(&c, 1);

    printf("The value of a is %s\n", a.num_string);
    big_num_add(&a, &b, &c);
    printf("The value of c after add is %s\n", c.num_string);
    reverse_bn(&c);

    printf("The value of c is %s\n", c.num_string);

    char buf[1];
    char write_buf[] = "testing writing";
    // int offset = 100; /* TODO: try test something bigger than the limit */
    // FILE *data_set = fopen("data.dat", "w");

    // int fd = open(FIB_DEV, O_RDWR);
    // if (fd < 0) {
    //     perror("Failed to open character device");
    //     exit(1);
    // }

    // for (int i = 0; i <= offset; i++) {
    //     kt = write(fd, write_buf, 0);
    //     printf("Writing to " FIB_DEV ", returned the sequence %lld\n", kt);
    // }

    // for (int i = 0; i <= offset; i++) {
    //     long long sz, kt;
    //     lseek(fd, i, SEEK_SET);
    //     sz = read(fd, buf, 1);
    //     printf("Reading from " FIB_DEV
    //            " at offset %d, returned the sequence "
    //            "%lld.\n",
    //            i, sz);
    //     kt = write(fd, write_buf, sizeof(write_buf));
    //     fprintf(data_set, "%lld %d\n", kt, i);
    //     printf("The time span is %lld\n", kt);
    // }

    // for (int i = offset; i >= 0; i--) {
    //     lseek(fd, i, SEEK_SET);
    //     sz = read(fd, buf, 1);
    //     printf("Reading from " FIB_DEV
    //            " at offset %d, returned the sequence "
    //            "%lld.\n",
    //            i, sz);
    // }
    // fclose(data_set);
    // close(fd);
    return 0;
}
