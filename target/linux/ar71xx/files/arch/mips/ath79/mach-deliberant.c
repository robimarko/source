/*
 *  DELIBERANT board support
 *
 *  Copyright (C) 2010-2012 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2011-2012 Anan Huang <axishero@foxmail.com>
 *  Copyright (C) 2015 Jernej Kos <jernej@kos.mx>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */
 
#include <linux/pci.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/ath9k_platform.h>
#include <linux/ar8216_platform.h>

#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "dev-ap9x-pci.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-spi.h"
#include "dev-wmac.h"
#include "machtypes.h"

#define DELIBERANT_GPIO_BTN_RESET	17
#define DELIBERANT_GPIO_LED_LAN0	1
#define DELIBERANT_GPIO_LED_L1		2
#define DELIBERANT_GPIO_LED_L2		3
#define DELIBERANT_GPIO_LED_L3		4
#define DELIBERANT_GPIO_LED_L4		13

#define DELIBERANT_GPIO_LED_PWR		0

#define DELIBERANT_KEYS_POLL_INTERVAL		20	/* msecs */
#define DELIBERANT_KEYS_DEBOUNCE_INTERVAL	(3 * DELIBERANT_KEYS_POLL_INTERVAL)

#define DELIBERANT_PCIE_CALDATA_OFFSET	0x5000

static struct gpio_led deliberant_leds_gpio[] __initdata = {
	{
		.name		= "deliberant:green:system",
		.gpio		= DELIBERANT_GPIO_LED_PWR,
		.active_low	= 1,
	}, {
		.name		= "deliberant:green:lan0",
		.gpio		= DELIBERANT_GPIO_LED_LAN0,
		.active_low	= 1,
	}, {
		.name		= "deliberant:orange:link1",
		.gpio		= DELIBERANT_GPIO_LED_L1,
		.active_low	= 1,
	}, {
		.name		= "deliberant:orange:link2",
		.gpio		= DELIBERANT_GPIO_LED_L2,
		.active_low	= 1,
	}, {
		.name		= "deliberant:orange:link3",
		.gpio		= DELIBERANT_GPIO_LED_L3,
		.active_low	= 1,
	}, {
		.name		= "deliberant:orange:link4",
		.gpio		= DELIBERANT_GPIO_LED_L4,
		.active_low	= 1,
	},
};

static struct gpio_keys_button deliberant_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = DELIBERANT_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= DELIBERANT_GPIO_BTN_RESET,
		.active_low	= 1,
	}
};

static void __init deliberant_setup(void)
{
	u8 *wifi_mac = (u8 *) KSEG1ADDR(0x1fff1002);
	u8 *eth_mac = (u8 *) KSEG1ADDR(0x1fff0000);
	u8 *ee = (u8 *) KSEG1ADDR(0x1fff1000);

	ath79_register_m25p80(NULL);

	ath79_register_leds_gpio(-1, ARRAY_SIZE(deliberant_leds_gpio),
				 deliberant_leds_gpio);
	ath79_register_gpio_keys_polled(-1, DELIBERANT_KEYS_POLL_INTERVAL,
					 ARRAY_SIZE(deliberant_gpio_keys),
					 deliberant_gpio_keys);
	ath79_register_wmac(ee, wifi_mac);

	ath79_setup_ar934x_eth_cfg(AR934X_ETH_CFG_MII_GMAC0 |
				   AR934X_ETH_CFG_MII_GMAC0_SLAVE);

	ath79_register_mdio(1, 0x0);
	ath79_register_mdio(0, 0x0);

	ath79_init_mac(ath79_eth0_data.mac_addr, eth_mac, 1);

	/* GMAC0 is connected to an AR8327 switch */
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ath79_eth0_data.phy_mask = BIT(0);
	ath79_eth0_data.mii_bus_dev = &ath79_mdio0_device.dev;
	ath79_register_eth(0);
}

MIPS_MACHINE(ATH79_MACH_DELIBERANT, "DELIBERANT",
	     "Deliberant Generic", deliberant_setup);
