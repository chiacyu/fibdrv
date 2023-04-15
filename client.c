#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "big_num.h"

#define FIB_DEV "/dev/fibonacci"


static long long utime_tons()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec * 1e9 + ts.tv_nsec;
}

int main()
{
    char read_buf[] = "";
    char write_buf[] = "testing writing";
    int offset = 100; /* TODO: try test something bigger than the limit */
    FILE *data_set_k = fopen("data_k.dat", "w");
    FILE *data_set_u = fopen("data_u.dat", "w");
    FILE *data_set_d = fopen("data_d.dat", "w");

    int fd = open(FIB_DEV, O_RDWR);
    if (fd < 0) {
        perror("Failed to open character device");
        exit(1);
    }

    for (int i = 0; i <= offset; i++) {
        long long kt, ut, sz;
        ut = utime_tons();
        lseek(fd, i, SEEK_SET);
        sz = read(fd, read_buf, 1);
        printf("Reading from " FIB_DEV
               " at offset %d, returned the sequence "
               "%s and size is %lld.\n",
               i, read_buf, sz);
        kt = write(fd, write_buf, sizeof(write_buf));
        ut = utime_tons() - ut;
        // fprintf(data_set_k, "%lld %d\n", i, kt);
        // fprintf(data_set_u, "%lld %d\n", i, ut);
        // fprintf(data_set_d, "%lld %d\n", i, ut - kt);
        // printf("The time is %lld\n", kt);
    }

    fclose(data_set_k);
    fclose(data_set_u);
    fclose(data_set_d);
    close(fd);
    return 0;
}