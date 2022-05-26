#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("hello_world");
MODULE_DESCRIPTION("A Simple Character Device driver module");

static struct cdev cdev;
static dev_t devno;

static char tmp[128] = "This is a message from kernel character driver!";

static int
hello_open(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "open\n");
    return 0;
}

static int
hello_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "release\n");
    return 0;
}

static ssize_t
hello_read(struct file *filep, char __user *buf, size_t count, loff_t *offset)
{
    //
    size_t avail;

    printk(KERN_INFO "read\n");

    avail = sizeof(tmp) - *offset;

    if (count <= avail)
    {
        if (copy_to_user(buf, tmp + *offset, count) != 0)
        {
            return -EFAULT;
        }

        *offset += count;
        return count;
    }
    else
    {
        if (copy_to_user(buf, tmp + *offset, avail) != 0)
        {
            return -EFAULT;
        }

        *offset += avail;
        return avail;
    }
}

static ssize_t
hello_write(struct file *filep, const char __user *buf, size_t count,
            loff_t *offset)
{
    size_t avail;

    printk(KERN_INFO "write\n");

    avail = sizeof(tmp) - *offset;

    memset(tmp + *offset, 0, avail);

    if (count > avail)
    {
        if (copy_from_user(tmp + *offset, buf, avail) != 0)
        {
            return -EFAULT;
        }
        *offset += avail;
        return avail;
    }
    else
    {
        if (copy_from_user(tmp + *offset, buf, count) != 0)
        {
            return -EFAULT;
        }
        *offset += count;
        return count;
    }
}

static loff_t
hello_llseek(struct file *filep, loff_t off, int whence)
{
    printk(KERN_INFO "llseek\n");
    loff_t newpos;

    switch (whence)
    {
    case 0: /* SEEK_SET */
        newpos = off;
        break;
    case 1: /* SEEK_CUR */
        newpos = filep->f_pos + off;
        break;
    case 2: /* SEEK_END */
        newpos = sizeof(tmp) + off;
        break;
    default:
        return -EINVAL;
    }

    if (newpos < 0)
    {
        return -EINVAL;
    }

    filep->f_pos = newpos;
    return newpos;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = hello_open,
    .release = hello_release,
    .read = hello_read,
    .llseek = hello_llseek,
    .write = hello_write,
};

static int __init hello_init(void)
{
    int ret;

    printk(KERN_INFO "Load hello\n");

    // devno = MKDEV(111, 0);
    //向系统申请设备号
    // if ((ret = register_chrdev_region(devno, 1, "hello")) < 0)
    if((ret = alloc_chrdev_region(&devno, 0, 1, "hello")) < 0)
    {
        return ret;
    }
    //初始化cdev
    cdev_init(&cdev, &fops);
    cdev.owner = THIS_MODULE;
    cdev.dev = devno;
    //调用cdev_add将cdev加入系统
    cdev_add(&cdev, devno, 1);
    return 0;
}

static void __exit hello_cleanup(void)
{
    printk(KERN_INFO "cleanup hello\n");
    unregister_chrdev_region(devno, 1);
    cdev_del(&cdev);
}

module_init(hello_init);
module_exit(hello_cleanup);

MODULE_LICENSE("GPL");