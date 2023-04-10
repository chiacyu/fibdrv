#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "big_num.h"

#define FIB_DEV "/dev/fibonacci"

// static long long utime_tons()
// {
//     struct timespec ts;
//     clock_gettime(CLOCK_REALTIME, &ts);
//     return ts.tv_sec * 1e9 + ts.tv_nsec;
// }

int main()
{
    long long sz;
    char read_buf[] = "";
    char write_buf[] = "testing writing";
    int offset = 92; /* TODO: try test something bigger than the limit */
    FILE *data_set = fopen("data1.dat", "w");

    int fd = open(FIB_DEV, O_RDWR);
    if (fd < 0) {
        perror("Failed to open character device");
        exit(1);
    }

    for (int i = 0; i <= offset; i++) {
        long long kt;
        lseek(fd, i, SEEK_SET);
        sz = read(fd, read_buf, 1);
        read_buf[sz] = '\0';
        printf("Reading from " FIB_DEV
               " at offset %d, returned the sequence "
               "%s and size is %lld.\n",
               i, read_buf, sz);
        kt = write(fd, write_buf, sizeof(write_buf));
        // fprintf(data_set, "%lld %d\n", i, kt);
        printf("The time is %lld\n", kt);
    }

    for (int i = offset; i >= 0; i--) {
        lseek(fd, i, SEEK_SET);
        sz = read(fd, read_buf, 1);
        read_buf[sz] = '\0';
        printf("Reading from " FIB_DEV
               " at offset %d, returned the sequence "
               "%s.\n",
               i, read_buf);
    }

    fclose(data_set);
    close(fd);
    return 0;
}
