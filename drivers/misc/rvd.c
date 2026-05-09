// SPDX-License-Identifier: GPL-2.0
/*
 * RVD — misc character device that forwards CSR read requests to OpenSBI.
 *
 * Userspace passes a CSR name (e.g. "satp"); the driver resolves it to a
 * 12-bit CSR number and issues an SBI ecall to a vendor extension that
 * OpenSBI is expected to implement. The returned value is copied back.
 *
 * The SBI extension/function IDs below sit in the experimental range; adjust
 * them to match the OpenSBI plug-in you are pairing this driver with.
 */

#include <linux/capability.h>
#include <linux/ctype.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/uaccess.h>

#include <asm/csr.h>
#include <asm/sbi.h>

#include <uapi/linux/rvd.h>

#define RVD_SBI_EXT_ID			0x525644
#define RVD_SBI_FID_CSR_READ	0u

struct rvd_csr_entry {
	const char *name;
	u32 csrno;
};

/*
 * Whitelist of CSRs that userspace is permitted to read through SBI.
 */
static const struct rvd_csr_entry rvd_csr_table[] = {
	/* Unprivileged counters */
	{ "cycle",      CSR_CYCLE   },
	{ "time",       CSR_TIME    },
	{ "instret",    CSR_INSTRET },

	{ "mscratch",   CSR_MSCRATCH   },
	{ "mepc",       CSR_MEPC       },
	{ "mcause",     CSR_MCAUSE     },
	{ "mtval",      CSR_MTVAL      },
	{ "mip",        CSR_MIP        },
	{ "mie",        CSR_MIE        },
	{ "mideleg",    CSR_MIDELEG    },
	{ "menvcfg",    CSR_MENVCFG    },
	{ "misa",       CSR_MISA       },

	{ "sstatus",    CSR_SSTATUS    },
	{ "sie",        CSR_SIE        },
	{ "stvec",      CSR_STVEC      },
	{ "scounteren", CSR_SCOUNTEREN },
	{ "senvcfg",    CSR_SENVCFG    },
	{ "sscratch",   CSR_SSCRATCH   },
	{ "sepc",       CSR_SEPC       },
	{ "scause",     CSR_SCAUSE     },
	{ "stval",      CSR_STVAL      },
	{ "sip",        CSR_SIP        },
	{ "satp",       CSR_SATP       },
};

static int rvd_csr_resolve(const char *name, u32 *csrno)
{
	size_t i;

	for (i = 0; i < ARRAY_SIZE(rvd_csr_table); i++) {
		if (!strcasecmp(name, rvd_csr_table[i].name)) {
			*csrno = rvd_csr_table[i].csrno;
			return 0;
		}
	}
	return -ENOENT;
}

static int rvd_sbi_csr_read(int hartid, u32 csrno, u64 *value)
{
	struct sbiret ret;

	ret = sbi_ecall(RVD_SBI_EXT_ID, RVD_SBI_FID_CSR_READ,
			hartid, csrno, 0, 0, 0, 0);
	if (ret.error)
		return sbi_err_map_linux_errno(ret.error);

	*value = (u64)(unsigned long)ret.value;
	return 0;
}

static int rvd_do_csr_read(struct rvd_csr_req *req)
{
	int err;

	req->name[RVD_CSR_NAME_MAX - 1] = '\0';
	if (req->name[0] == '\0')
		return -EINVAL;

	err = rvd_csr_resolve(req->name, &req->csrno);
	if (err)
		return err;

	req->reserved = 0;
	req->value = 0;

	return rvd_sbi_csr_read(req->hartid, req->csrno, &req->value);
}

static long rvd_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	void __user *argp = (void __user *)arg;
	struct rvd_csr_req req;
	int err;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	switch (cmd) {
	case RVD_IOCTL_CSR_READ:
		if (copy_from_user(&req, argp, sizeof(req)))
			return -EFAULT;

		err = rvd_do_csr_read(&req);
		if (err)
			return err;

		if (copy_to_user(argp, &req, sizeof(req)))
			return -EFAULT;
		return 0;
	default:
		return -ENOTTY;
	}
}

static const struct file_operations rvd_fops = {
	.owner		= THIS_MODULE,
	.unlocked_ioctl	= rvd_ioctl,
	.compat_ioctl	= compat_ptr_ioctl,
};

static struct miscdevice rvd_misc = {
	.minor	= MISC_DYNAMIC_MINOR,
	.name	= "rvd",
	.fops	= &rvd_fops,
};

static int __init rvd_init(void)
{
	long ret;

	ret = sbi_probe_extension(RVD_SBI_EXT_ID);
	if (ret <= 0) {
		pr_warn("rvd: SBI extension 0x%x not provided by firmware\n",
			RVD_SBI_EXT_ID);
		return -ENODEV;
	}

	return misc_register(&rvd_misc);
}

static void __exit rvd_exit(void)
{
	misc_deregister(&rvd_misc);
}

module_init(rvd_init);
module_exit(rvd_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("RISC-V rvd misc device: CSR-by-name read via OpenSBI");
