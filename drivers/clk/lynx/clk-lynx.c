/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright(C) 2024 Lynx, Inc.
*/
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/clk-provider.h>

#include "clk-lynx-p100.h"
#include "clk-lynx.h"

static int lynx_clk_probe(struct platform_device *pdev)
{
	return 0;
}

static const struct of_device_id lynx_clk_of_match[] = {
	{.compatible = "lynx,clk-p100", .data = &lynx_clk_p100},
	{}
};
MODULE_DEVICE_TABLE(of, lynx_clk_of_match);

static struct platform_driver lynx_clk_driver = {
	.driver = {
		.name = "lynx-clk",
		.of_match_table = lynx_clk_of_match,
	},
	.probe = lynx_clk_probe,
};
module_platform_driver(lynx_clk_driver);

MODULE_AUTHOR("Ryan JK Hong");
MODULE_DESCRIPTION("Lynx P100 clock driver");
MODULE_LICENSE("GPL");
