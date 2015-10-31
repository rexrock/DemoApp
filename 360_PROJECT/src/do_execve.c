#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/ptrace.h>
#include <linux/namei.h>
#include <linux/slab.h>
#include <linux/fs.h>

void final_putname(struct filename *name)
{
    if (name->separate) {
        __putname(name->name);
        kfree(name);
    } else {
        __putname(name);
    }
}

void printk_address(unsigned long address, int reliable)
{
    pr_cont(" [<%p>] %s%pB\n",
        (void *)address, reliable ? "" : "? ", (void *)address);
}

void __show_regs(struct pt_regs *regs, int all)
{
    unsigned long cr0 = 0L, cr2 = 0L, cr3 = 0L, cr4 = 0L, fs, gs, shadowgs;
    unsigned long d0, d1, d2, d3, d6, d7;
    unsigned int fsindex, gsindex;
    unsigned int ds, cs, es;

    printk(KERN_DEFAULT "RIP: %04lx:[<%016lx>] ", regs->cs & 0xffff, regs->ip);
    printk_address(regs->ip, 1);
    printk(KERN_DEFAULT "RSP: %04lx:%016lx  EFLAGS: %08lx\n", regs->ss,
            regs->sp, regs->flags);
    printk(KERN_DEFAULT "RAX: %016lx RBX: %016lx RCX: %016lx\n",
           regs->ax, regs->bx, regs->cx);
    printk(KERN_DEFAULT "RDX: %016lx RSI: %016lx RDI: %016lx\n",
           regs->dx, regs->si, regs->di);
    printk(KERN_DEFAULT "RBP: %016lx R08: %016lx R09: %016lx\n",
           regs->bp, regs->r8, regs->r9);
    printk(KERN_DEFAULT "R10: %016lx R11: %016lx R12: %016lx\n",
           regs->r10, regs->r11, regs->r12);
    printk(KERN_DEFAULT "R13: %016lx R14: %016lx R15: %016lx\n",
           regs->r13, regs->r14, regs->r15);

    asm("movl %%ds,%0" : "=r" (ds));
    asm("movl %%cs,%0" : "=r" (cs));
    asm("movl %%es,%0" : "=r" (es));
    asm("movl %%fs,%0" : "=r" (fsindex));
    asm("movl %%gs,%0" : "=r" (gsindex));

    rdmsrl(MSR_FS_BASE, fs);
    rdmsrl(MSR_GS_BASE, gs);
    rdmsrl(MSR_KERNEL_GS_BASE, shadowgs);

    if (!all)
        return;

    cr0 = read_cr0();
    cr2 = read_cr2();
    cr3 = read_cr3();
    cr4 = read_cr4();

    printk(KERN_DEFAULT "FS:  %016lx(%04x) GS:%016lx(%04x) knlGS:%016lx\n",
           fs, fsindex, gs, gsindex, shadowgs);
    printk(KERN_DEFAULT "CS:  %04x DS: %04x ES: %04x CR0: %016lx\n", cs, ds,
            es, cr0);
    printk(KERN_DEFAULT "CR2: %016lx CR3: %016lx CR4: %016lx\n", cr2, cr3,
            cr4);

    get_debugreg(d0, 0);
    get_debugreg(d1, 1);
    get_debugreg(d2, 2);
    printk(KERN_DEFAULT "DR0: %016lx DR1: %016lx DR2: %016lx\n", d0, d1, d2);
    get_debugreg(d3, 3);
    get_debugreg(d6, 6);
    get_debugreg(d7, 7);
    printk(KERN_DEFAULT "DR3: %016lx DR6: %016lx DR7: %016lx\n", d3, d6, d7);
}

static struct kprobe kp = 
{
  .symbol_name = "sys_execve",
};

static int handler_pre(struct kprobe *p, struct pt_regs *regs)
{
    int error;
    struct filename *fname;

    fname = getname( (char __user *) regs->bx );
    error = PTR_ERR(fname);
    if (!IS_ERR(fname))
    {
        printk("file to execve: %s\n", fname->name);
        final_putname(fname);
    }
    
    /*
    struct thread_info *thread = current_thread_info();
    printk(KERN_INFO "pre-handler thread info: flags = %x, status = %d, cpu = %d, task->pid = %d\n",
        thread->flags, thread->status, thread->cpu, thread->task->pid);
    */
    return 0;
}

static void handler_post(struct kprobe *p, struct pt_regs *regs, unsigned long flags)
{  
    /*
    struct thread_info *thread = current_thread_info();
    printk(KERN_INFO "post-handler thread info: flags = %x, status = %d, cpu = %d, task->pid = %d\n",
        thread->flags, thread->status, thread->cpu, thread->task->pid);
    */
}

static int handler_fault(struct kprobe *p, struct pt_regs *regs, int trapnr)
{
    /*
    printk(KERN_INFO "fault_handler: p->addr = 0x%p, trap #%dn",
        p->addr, trapnr);
    */

    return 0;
}

static int __init kprobe_init(void)
{
    int ret;
    kp.pre_handler = handler_pre;
    kp.post_handler = handler_post;
    kp.fault_handler = handler_fault;

    ret = register_kprobe(&kp);
    if (ret < 0) 
    {
        printk(KERN_INFO "register_kprobe failed, returned %d\n", ret);
        return ret;
    }
    printk(KERN_INFO "Planted kprobe at %p\n", kp.addr);
    return 0;
}

static void __exit kprobe_exit(void)
{
    unregister_kprobe(&kp);
    printk(KERN_INFO "kprobe at %p unregistered\n", kp.addr);
}

module_init(kprobe_init)
module_exit(kprobe_exit)
MODULE_LICENSE("GPL");
