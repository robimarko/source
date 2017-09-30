/*
 * TP-LINK CPE210-V2 board support
 *
 *  Copyright (C) 2017 Robert Marko <robimarko@otvorenamreza.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/ar8216_platform.h>
#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-wmac.h"
#include "machtypes.h"


#define CPE210_V2_GPIO_LED_LAN0		11
#define CPE210_V2_GPIO_LED_L1		13
#define CPE210_V2_GPIO_LED_L2		14
#define CPE210_V2_GPIO_LED_L3		15
#define CPE210_V2_GPIO_LED_L4		16

#define CPE210_V2_GPIO_EXTERNAL_LNA0	18
#define CPE210_V2_GPIO_EXTERNAL_LNA1	19

#define CPE210_V2_GPIO_BTN_RESET	17

#define CPE210_V2_KEYS_POLL_INTERVAL	20	/* msecs */
#define CPE210_V2_KEYS_DEBOUNCE_INTERVAL	(3 * CPE210_V2_KEYS_POLL_INTERVAL)

static const char *cpe210_v2_part_probes[] = {
	"tp-link",
	NULL,
};

static struct flash_platform_data cpe210_v2_flash_data = {
	.part_probes	= cpe210_v2_part_probes,
};

static struct gpio_led cpe210_v2_leds_gpio[] __initdata = {
	{
		.name		= "tp-link:green:system",
		.gpio		= CPE210_V2_GPIO_LED_SYSTEM,
		.active_low	= 1,
	}, {
		.name		= "tp-link:green:link1",
		.gpio		= CPE210_V2_GPIO_LED_L1,
		.active_low	= 1,
	}, {
		.name		= "tp-link:green:link2",
		.gpio		= CPE210_V2_GPIO_LED_L2,
		.active_low	= 1,
	}, {
		.name		= "tp-link:green:link3",
		.gpio		= CPE210_V2_GPIO_LED_L3,
		.active_low	= 1,
	}, {
		.name		= "tp-link:green:link4",
		.gpio		= CPE210_V2_GPIO_LED_L4,
		.active_low	= 1,
	},
};

static struct gpio_keys_button cpe210_v2_gpio_keys[] __initdata = {
	{
		.desc		= "Reset button",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = CPE210_V2_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= CPE210_V2_GPIO_BTN_RESET,
		.active_low	= 1,
	},
};

static void __init cpe210_v2_setup(void)
{
	u8 *mac = (u8 *) KSEG1ADDR(0x1f01fc00);
	u8 *ee = (u8 *) KSEG1ADDR(0x1fff1000);

	/* disable PHY_SWAP and PHY_ADDR_SWAP bits */
	ath79_setup_ar933x_phy4_switch(false, false);

	ath79_register_m25p80(&cpe210_v2_flash_data);
	ath79_register_leds_gpio(-1, ARRAY_SIZE(cpe210_v2_leds_gpio),
				 cpe210_v2_leds_gpio);
	ath79_register_gpio_keys_polled(-1, CPE210_V2_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(cpe210_v2_gpio_keys),
					cpe210_v2_gpio_keys);
	
	ath79_register_mdio(0, 0x0);
	ath79_init_mac(ath79_eth0_data.mac_addr, mac, 0);
	ath79_eth0_data.duplex = DUPLEX_FULL;
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ath79_eth0_data.speed = SPEED_100;
	ath79_eth0_data.phy_mask = BIT(4);
	ath79_register_eth(0);
	ath79_register_wmac(ee, mac);
}

MIPS_MACHINE(ATH79_MACH_TL_CPE210_V2, "CPE210-v2", "TP-LINK CPE210 v2",
	     cpe210_v2_setup);
