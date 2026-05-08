/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
#ifndef __UAPI_LINUX_RVD_H
#define __UAPI_LINUX_RVD_H

#include <linux/ioctl.h>
#include <linux/types.h>

/*
 * /dev/rvd — userspace ABI for CSR-related requests proxied to firmware (OpenSBI).
 * All ioctl commands require CAP_SYS_ADMIN.
 */
#define RVD_IOCTL_MAGIC		0xA7

struct rvd_csr_req {
	__u32 csrno;
	__u32 reserved;
	__u64 value;
};

#define RVD_IOCTL_CSR_READ	_IOWR(RVD_IOCTL_MAGIC, 0x01, struct rvd_csr_req)

#endif /* __UAPI_LINUX_RVD_H */
