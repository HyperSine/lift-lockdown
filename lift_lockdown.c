#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/security.h>

MODULE_AUTHOR("Aiyu Xiao <hypersine.git@outlook.com>");
MODULE_DESCRIPTION("A kernel module to lift lockdown at runtime.");
MODULE_LICENSE("GPL");

typedef unsigned long (*kallsyms_lookup_name_fn_t)(const char* name);

static int __init mod_init(void) {
    int err = 0;
    kallsyms_lookup_name_fn_t kallsyms_lookup_name = 0;
    enum lockdown_reason* kernel_locked_down = 0;

    {
        struct kprobe kln_prober =
            { .symbol_name = "kallsyms_lookup_name" };

        err = register_kprobe(&kln_prober);
        if (err < 0) {
            printk(KERN_DEBUG "lift_lockdown: register_kprobe failed, err = %d\n", err);
            return err;
        } else {
            kallsyms_lookup_name = (kallsyms_lookup_name_fn_t)kln_prober.addr;
            unregister_kprobe(&kln_prober);
        }
    }

    printk(KERN_INFO "lift_lockdown: kallsyms_lookup_name = 0x%pK\n", kallsyms_lookup_name);
    if (kallsyms_lookup_name == 0) {
        return -EFAULT;
    }

    kernel_locked_down =
        (enum lockdown_reason*)kallsyms_lookup_name("kernel_locked_down");

    printk(KERN_INFO "lift_lockdown: kernel_locked_down = 0x%pK\n", kernel_locked_down);
    if (kernel_locked_down == 0) {
        return -EFAULT;
    }

    *kernel_locked_down = LOCKDOWN_NONE;
    printk(KERN_INFO "lift_lockdown: lockdown is lifted.\n");

    return 0;
}

static void __exit mod_exit(void) {
    printk(KERN_INFO "lift_lockdown: exited.\n");
}

module_init(mod_init)
module_exit(mod_exit)
