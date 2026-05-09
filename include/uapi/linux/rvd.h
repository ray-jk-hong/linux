/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
#ifndef __UAPI_LINUX_RVD_H
#define __UAPI_LINUX_RVD_H

#include <linux/ioctl.h>
#include <linux/types.h>

/*
 * /dev/rvd — userspace ABI for CSR-related requests proxied to OpenSBI.
 * All ioctl commands require CAP_SYS_ADMIN.
 */
#define RVD_IOCTL_MAGIC		0xA7

#define RVD_CSR_NAME_MAX	16

/**
 * struct rvd_csr_req - request to read a CSR by symbolic name
 * @name:     in:  NUL-terminated CSR name, case-insensitive (e.g. "satp")
 * @csrno:    out: resolved CSR number (12-bit)
 * @reserved: must be zero
 * @value:    out: value returned by SBI
 */
struct rvd_csr_req {
	char  name[RVD_CSR_NAME_MAX];
	__u32 hartid;
	__u32 csrno;
	__u32 reserved;
	__u64 value;
};

#define RVD_IOCTL_CSR_READ	_IOWR(RVD_IOCTL_MAGIC, 0x01, struct rvd_csr_req)

#endif /* __UAPI_LINUX_RVD_H */
