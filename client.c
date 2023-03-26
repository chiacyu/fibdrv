#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define FIB_DEV "/dev/fibonacci"

int main()
{
    char buf[1];
    char write_buf[] = "testing writing";
    int offset = 100; /* TODO: try test something bigger than the limit */
    FILE *data_set = fopen("data.dat", "w");

    int fd = open(FIB_DEV, O_RDWR);
    if (fd < 0) {
        perror("Failed to open character device");
        exit(1);
    }

    // for (int i = 0; i <= offset; i++) {
    //     kt = write(fd, write_buf, 0);
    //     printf("Writing to " FIB_DEV ", returned the sequence %lld\n", kt);
    // }

    for (int i = 0; i <= offset; i++) {
        long long sz, kt;
        lseek(fd, i, SEEK_SET);
        sz = read(fd, buf, 1);
        printf("Reading from " FIB_DEV
               " at offset %d, returned the sequence "
               "%lld.\n",
               i, sz);
        kt = write(fd, write_buf, sizeof(write_buf));
        fprintf(data_set, "%lld %d\n", kt, i);
        printf("The time span is %lld\n", kt);
    }

    // for (int i = offset; i >= 0; i--) {
    //     lseek(fd, i, SEEK_SET);
    //     sz = read(fd, buf, 1);
    //     printf("Reading from " FIB_DEV
    //            " at offset %d, returned the sequence "
    //            "%lld.\n",
    //            i, sz);
    // }
    fclose(data_set);
    close(fd);
    return 0;
}
