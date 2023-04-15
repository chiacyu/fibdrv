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
#define NUM_THREADS 10


void *thread_func(void *arg)
{
    char *read_buf = "";
    char *write_buf = "testing writing";
    int offset = 100;

    int fd = open(FIB_DEV, O_RDWR);
    if (fd < 0) {
        perror("Failed to open character device");
        exit(1);
    }

    for (int i = 0; i <= offset; i++) {
        long long kt, sz;
        lseek(fd, i, SEEK_SET);
        sz = read(fd, read_buf, 1);
        printf("Reading from " FIB_DEV
               " at offset %d, returned the sequence "
               "%s and size is %lld.\n",
               i, read_buf, sz);
        kt = write(fd, write_buf, sizeof(write_buf));
    }

    close(fd);
    return;
}

int main()
{
    pthread_t thr[NUM_THREADS];
    int i;

    for (i = 0; i < NUM_THREADS; i++) {
        int rc;
        if ((rc = pthread_create(&thr[i], NULL, thread_func, NULL))) {
            fprintf(stderr, "error:pthread_create, rc: %d\n", rc);
            return EXIT_FAILURE;
        }
    }

    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(thr[i], NULL);
    }

    return EXIT_SUCCESS;
}