/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright(C) 2024 Lynx, Inc.
*/

#ifndef __LYNX_CLK_P100_H
#define __LYNX_CLK_P100_H

#include <linux/module.h>

#include "clk-lynx.h"

static struct __lynx_clock lynx_init_clocks_p100[] = {

};

static const struct lynx_clk_desc lynx_clk_p100 = {
	.clks = lynx_init_clocks_p100,
	.num_clks = ARRAY_SIZE(lynx_init_clocks_p100),
};

#endif