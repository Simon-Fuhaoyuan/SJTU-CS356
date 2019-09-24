/**
 * File Name: seconds.c
 * Name: Haoyuan Fu
 * Student ID: 517021910753
 **/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/jiffies.h>
#include <asm/param.h>

#define BUFFER_SIZE 128

#define PROC_NAME "seconds"

ssize_t proc_read(struct file *file, char *buf, size_t count, loff_t *pos);

unsigned long start = 0;

static struct file_operations proc_ops = {
        .owner = THIS_MODULE,
        .read = proc_read,
};


int proc_init(void)
{
        proc_create(PROC_NAME, 0, NULL, &proc_ops);

        printk(KERN_INFO "/proc/%s created\n", PROC_NAME);

	start = jiffies;

	return 0;
}


void proc_exit(void)
{
        remove_proc_entry(PROC_NAME, NULL);

        printk( KERN_INFO "/proc/%s removed\n", PROC_NAME);
}


ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
        int rv = 0;
        char buffer[BUFFER_SIZE];
        static int completed = 0;

        if (completed) {
                completed = 0;
                return 0;
        }

        completed = 1;

	int second = 0;
	second = (jiffies-start)/HZ;
	rv = sprintf(buffer, "%d", second);
	buffer[rv++] = '\n';

        raw_copy_to_user(usr_buf, buffer, rv);

        return rv;
}


module_init( proc_init );
module_exit( proc_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Seconds Module");
MODULE_AUTHOR("SGG");

