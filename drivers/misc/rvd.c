// SPDX-License-Identifier: GPL-2.0
/*
 * RVD — misc character device to accept userspace CSR requests for OpenSBI proxying.
 *
 * Exposes /dev/rvd. Actual SBI forwarding and CSR allowlisting are TODO.
 */

#include <linux/capability.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <uapi/linux/rvd.h>

static long rvd_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	void __user *argp = (void __user *)arg;
	struct rvd_csr_req req;

	(void)filp;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	switch (cmd) {
	case RVD_IOCTL_CSR_READ:
		if (copy_from_user(&req, argp, sizeof(req)))
			return -EFAULT;

		/*
		 * TODO: validate csrno; call firmware, e.g. sbi_ecall() with the
		 * vendor extension OpenSBI implements for CSR access.
		 */
		req.value = 0;
		req.reserved = 0;

		if (copy_to_user(argp, &req, sizeof(req)))
			return -EFAULT;
		return 0;
	default:
		return -ENOTTY;
	}
}

static const struct file_operations rvd_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = rvd_ioctl,
	.compat_ioctl = compat_ptr_ioctl,
};

static struct miscdevice rvd_misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "rvd",
	.fops = &rvd_fops,
};

static int __init rvd_init(void)
{
	return misc_register(&rvd_misc);
}

static void __exit rvd_exit(void)
{
	misc_deregister(&rvd_misc);
}

module_init(rvd_init);
module_exit(rvd_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("RISC-V rvd misc device (CSR / OpenSBI proxy stub)");
