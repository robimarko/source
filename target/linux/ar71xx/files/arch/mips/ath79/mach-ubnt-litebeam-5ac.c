/*
 *  Ubiquiti Litebeam 5AC board support
 *
 *  Copyright (C) 2017 Robert Marko <robimarko@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/pci.h>
#include <linux/platform_device.h>
#include <linux/ath9k_platform.h>
#include <linux/etherdevice.h>
#include <linux/ar8216_platform.h>

#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/irq.h>
#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "pci.h"
#include "dev-ap9x-pci.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-wmac.h"
#include "machtypes.h"


#define LITEBEAM_5AC_KEYS_POLL_INTERVAL		20
#define LITEBEAM_5AC_KEYS_DEBOUNCE_INTERVAL	(3 * LITEBEAM_5AC_KEYS_POLL_INTERVAL)

#define LITEBEAM_5AC_GPIO_BTN_RESET		12

static struct gpio_keys_button ubnt_litebeam_5ac_23_gpio_keys[] __initdata = {
	{
		.desc			= "reset",
		.type			= EV_KEY,
		.code			= KEY_RESTART,
		.debounce_interval	= LITEBEAM_5AC_KEYS_DEBOUNCE_INTERVAL,
		.gpio			= LITEBEAM_5AC_GPIO_BTN_RESET,
		.active_low		= 1,
	}
};

static void __init ubnt_litebeam_5ac_23_setup(void)
{
	u8 *mac = (u8 *) KSEG1ADDR(0x1fff0000);

	/* Disable JTAG, enabling GPIOs 0-3 */
	/* Configure OBS4 line, for GPIO 4*/
	ath79_gpio_function_setup(AR934X_GPIO_FUNC_JTAG_DISABLE,
				  AR934X_GPIO_FUNC_CLK_OBS4_EN);

	ath79_register_m25p80(NULL);
	ath79_register_mdio(0, 0x0);

	ath79_register_gpio_keys_polled(-1, LITEBEAM_5AC_KEYS_POLL_INTERVAL,
	                                ARRAY_SIZE(ubnt_litebeam_5ac_23_gpio_keys),
	                                ubnt_litebeam_5ac_23_gpio_keys);

	ath79_setup_ar934x_eth_cfg(AR934X_ETH_CFG_RGMII_GMAC0);

	/* AR8035 connected to PHY4 */
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ath79_eth0_data.duplex = DUPLEX_FULL;
	ath79_eth0_data.speed = SPEED_1000;
	ath79_eth0_data.phy_mask = BIT(4);
	ath79_init_mac(ath79_eth0_data.mac_addr, mac, 0);
	ath79_register_eth(0);
	ath79_register_pci();
}

MIPS_MACHINE(ATH79_MACH_UBNT_LITEBEAM_5AC_23, "UBNT-LITEBEAM-5AC-23", "Ubiquiti LiteBeam 5AC-23", ubnt_litebeam_5ac_23_setup);
