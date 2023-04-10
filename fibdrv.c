#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>

#include "big_num.c"


MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("National Cheng Kung University, Taiwan");
MODULE_DESCRIPTION("Fibonacci engine driver");
MODULE_VERSION("0.1");

#define DEV_FIBONACCI_NAME "fibonacci"

/* MAX_LENGTH is set to 92 because
 * ssize_t can't fit the number > 92
 */
#define MAX_LENGTH 92

static dev_t fib_dev = 0;
static struct cdev *fib_cdev;
static struct class *fib_class;
static ktime_t kt;
static DEFINE_MUTEX(fib_mutex);

static long long fib_sequence(long long k)
{
    /* FIXME: C99 variable-length array (VLA) is not allowed in Linux kernel.
     */
    long long f[k + 2];

    f[0] = 0;
    f[1] = 1;

    for (int i = 2; i <= k; i++) {
        f[i] = f[i - 1] + f[i - 2];
    }

    return f[k];
}

static __uint128_t fib_sequence_fast_doubling(long long k)
{
    __uint128_t a = 0;
    __uint128_t b = 1;
    __uint128_t t1, t2;
    int len = 64 - __builtin_clzl(k);

    while (len > 0) {
        t1 = a * (2 * b - a);
        t2 = (b * b) + (a * a);
        a = t1;
        b = t2;
        if (((k >> (len - 1)) & 0x1)) {
            t1 = a + b;
            a = b;
            b = t1;
        }
        len -= 1;
    }
    return a;
}

static long long fib_sequence_big_num_fix(long long k, char *buf)
{
    /* FIXME: C99 variable-length array (VLA) is not allowed in Linux kernel.
     */
    // big_num_t f[k + 2];
    big_num_fix_t *f = kmalloc((k + 2) * sizeof(big_num_fix_t), GFP_KERNEL);

    f[0].num[0] = '0';
    f[0].num[1] = '1';

    for (int i = 2; i <= k; i++) {
        big_num_fix_add(&f[i - 1], &f[i - 2], &f[i]);
    }

    size_t ret = strlen(f[k].num);
    reverse_str(f[k].num, ret);
    copy_to_user(buf, f[k].num, ret);
    return ret;
}

static long long fib_sequence_big_num_fix_fd(long long k, char *buf)
{
    big_num_fix_t a;
    big_num_fix_t b;
    big_num_fix_t t1, t2;
    big_num_fix_t const2;
    a.num[0] = '0';
    a.num[1] = '\0';
    b.num[0] = '7';
    b.num[1] = '\0';
    const2.num[0] = '8';
    const2.num[1] = '\0';
    big_num_fix_t twobtmp;
    big_num_fix_t twobmatmp;
    big_num_fix_t bsquare;
    big_num_fix_t asquare;
    int len = 64 - __builtin_clzl(k);

    while (len > 0) {
        big_num_fix_mul(&const2, &b, &twobtmp);
        big_num_fix_sub(&twobtmp, &a, &twobmatmp);
        big_num_fix_mul(&a, &twobmatmp, &t1);

        big_num_fix_mul(&b, &b, &bsquare);
        big_num_fix_mul(&a, &a, &asquare);
        big_num_add(&asquare, &bsquare, &t2);

        a = t1;
        b = t2;
        if (((k >> (len - 1)) & 0x1)) {
            big_num_add(&a, &b, &t1);
            a = b;
            b = t1;
        }
        len -= 1;
    }

    size_t ret = strlen(twobmatmp.num);
    copy_to_user(buf, twobtmp.num, ret);
    return ret;
}

static long long fib_sequences(long long k, int flag, char *buf)
{
    switch (flag) {
    case 1:
        return fib_sequence_fast_doubling(k);

    case 2:
        return fib_sequence_big_num_fix(k, buf);

    case 3:
        return fib_sequence_big_num_fix_fd(k, buf);

    default:
        return fib_sequence(k);
    }
}

static long long fib_time_proxy(long long k, int flag, char *buf)
{
    kt = ktime_get();
    long long result = fib_sequences(k, flag, buf);
    kt = ktime_sub(ktime_get(), kt);

    return result;
}

static int fib_open(struct inode *inode, struct file *file)
{
    if (!mutex_trylock(&fib_mutex)) {
        printk(KERN_ALERT "fibdrv is in use");
        return -EBUSY;
    }
    return 0;
}

static int fib_release(struct inode *inode, struct file *file)
{
    mutex_unlock(&fib_mutex);
    return 0;
}

/* calculate the fibonacci number at given offset */
static ssize_t fib_read(struct file *file,
                        char *buf,
                        size_t size,
                        loff_t *offset)
{
    return (ssize_t) fib_time_proxy(*offset, 3, buf);
}

/* write operation is skipped */
static ssize_t fib_write(struct file *file,
                         const char *buf,
                         size_t size,
                         loff_t *offset)
{
    return ktime_to_ns(kt);
}

static loff_t fib_device_lseek(struct file *file, loff_t offset, int orig)
{
    loff_t new_pos = 0;
    switch (orig) {
    case 0: /* SEEK_SET: */
        new_pos = offset;
        break;
    case 1: /* SEEK_CUR: */
        new_pos = file->f_pos + offset;
        break;
    case 2: /* SEEK_END: */
        new_pos = MAX_LENGTH - offset;
        break;
    }

    if (new_pos > MAX_LENGTH)
        new_pos = MAX_LENGTH;  // max case
    if (new_pos < 0)
        new_pos = 0;        // min case
    file->f_pos = new_pos;  // This is what we'll use now
    return new_pos;
}

const struct file_operations fib_fops = {
    .owner = THIS_MODULE,
    .read = fib_read,
    .write = fib_write,
    .open = fib_open,
    .release = fib_release,
    .llseek = fib_device_lseek,
};

static int __init init_fib_dev(void)
{
    int rc = 0;

    mutex_init(&fib_mutex);

    // Let's register the device
    // This will dynamically allocate the major number
    rc = alloc_chrdev_region(&fib_dev, 0, 1, DEV_FIBONACCI_NAME);

    if (rc < 0) {
        printk(KERN_ALERT
               "Failed to register the fibonacci char device. rc = %i",
               rc);
        return rc;
    }

    fib_cdev = cdev_alloc();
    if (fib_cdev == NULL) {
        printk(KERN_ALERT "Failed to alloc cdev");
        rc = -1;
        goto failed_cdev;
    }
    fib_cdev->ops = &fib_fops;
    rc = cdev_add(fib_cdev, fib_dev, 1);

    if (rc < 0) {
        printk(KERN_ALERT "Failed to add cdev");
        rc = -2;
        goto failed_cdev;
    }

    fib_class = class_create(THIS_MODULE, DEV_FIBONACCI_NAME);

    if (!fib_class) {
        printk(KERN_ALERT "Failed to create device class");
        rc = -3;
        goto failed_class_create;
    }

    if (!device_create(fib_class, NULL, fib_dev, NULL, DEV_FIBONACCI_NAME)) {
        printk(KERN_ALERT "Failed to create device");
        rc = -4;
        goto failed_device_create;
    }
    return rc;
failed_device_create:
    class_destroy(fib_class);
failed_class_create:
    cdev_del(fib_cdev);
failed_cdev:
    unregister_chrdev_region(fib_dev, 1);
    return rc;
}

static void __exit exit_fib_dev(void)
{
    mutex_destroy(&fib_mutex);
    device_destroy(fib_class, fib_dev);
    class_destroy(fib_class);
    cdev_del(fib_cdev);
    unregister_chrdev_region(fib_dev, 1);
}

module_init(init_fib_dev);
module_exit(exit_fib_dev);
