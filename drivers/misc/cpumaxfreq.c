/*
 *
 * Copyright (C) 2024 Agustin Gutierrez <gutierrezaverruz0@hotmail.com>
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/cpufreq.h>
#include <linux/slab.h>

static struct proc_dir_entry *entry;
char *temp_buf;

static ssize_t cpumaxfreq_read(struct file *file, char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned long maxfreq, freq, len;
    int i;

    maxfreq = cpufreq_quick_get_max(0);
    for_each_possible_cpu(i) {
      freq = cpufreq_quick_get_max(i);
      if (freq > maxfreq)
        maxfreq = freq;
    }
    /* value is used for setting cpumaxfreq */
    maxfreq /= 10000;
  
    // Allocate memory for temp_buf dynamically
    temp_buf = (char*)kmalloc(20, GFP_KERNEL);
    if (!temp_buf)
        return -ENOMEM;

    len = sprintf(temp_buf, "%lu.%02lu", maxfreq/100, maxfreq%100);

    return simple_read_from_buffer(buffer, count, ppos, temp_buf, len);
}

static const struct file_operations cpumaxfreq_fops = {
    .owner = THIS_MODULE,
    .read = cpumaxfreq_read,
};

static int __init cpumaxfreq_init(void)
{
    entry = proc_create("cpumaxfreq", 0444, NULL, &cpumaxfreq_fops);
    if (!entry) {
        pr_err("Failed to create /proc/cpumaxfreq\n");
        return -ENOMEM;
    }
    return 0;
}

static void __exit cpumaxfreq_exit(void)
{
    kfree(temp_buf); // Free dynamically allocated memory
    
    if (entry)
        pr_info("error in entry /proc/cpumaxfreq\n");
        
    return;
}

module_init(cpumaxfreq_init);
module_exit(cpumaxfreq_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Get CPU Max Frequency");
MODULE_AUTHOR("Agustin Gutierrez");
