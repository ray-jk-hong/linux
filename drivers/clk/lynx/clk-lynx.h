/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright(C) 2024 Lynx, Inc.
*/
#ifndef __LYNX_CLK_H
#define __LYNX_CLK_H

#include <linux/types.h>

struct __lynx_clock {
    const char *name;
    const char *parent_name;
    unsigned long rate;
};

struct lynx_clk_desc {
    struct __lynx_clock *clks;
    size_t num_clks;
};

#endif