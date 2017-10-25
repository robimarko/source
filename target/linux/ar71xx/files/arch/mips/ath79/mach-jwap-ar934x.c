/*
 *  JJPlus JWAP603 board support
 *
 *  Copyright (C) 2010-2012 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2011-2012 Anan Huang <axishero@foxmail.com>
 *  Copyright (C) 2017 Robert Marko <robimarko@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */
 
#include <linux/pci.h>
#include <linux/phy.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/ath9k_platform.h>
#include <linux/ar8216_platform.h>

#include <asm/mach-ath79/ar71xx_regs.h>
#include <asm/mach-ath79/ath79.h>

#include "common.h"
#include "dev-ap9x-pci.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-nfc.h"
#include "dev-spi.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"

#define JJ_AR934X_GPIO_11		11
#define JJ_AR934X_GPIO_12 	        12
#define JJ_AR934X_GPIO_13 	        13
#define JJ_AR934X_GPIO_14		14
#define JJ_AR934X_GPIO_15		15
#define JJ_AR934X_GPIO_16		16

#define JJPLUS_KEYS_POLL_INTERVAL	20	/* msecs */
#define JJPLUS_KEYS_DEBOUNCE_INTERVAL	(3 * JJPLUS_KEYS_POLL_INTERVAL)

#define JJPLUS_MAC0_OFFSET		0x0
#define JJPLUS_MAC1_OFFSET		0x6
#define JJPLUS_WMAC_CALDATA_OFFSET	0x1000
#define JJPLUS_PCIE_CALDATA_OFFSET	0x5000

static struct gpio_led jj934x_leds_gpio[] __initdata = {
	{
		.name		= "jj934x:green:led1",
		.gpio		= JJ_AR934X_GPIO_11,
		.active_low	= 1,
	},
	{
		.name		= "jj934x:green:led2",
		.gpio		= JJ_AR934X_GPIO_13,
		.active_low	= 1,
	},
	{
		.name		= "jj934x:green:led3",
		.gpio		= JJ_AR934X_GPIO_14,
		.active_low	= 1,
	}
};

static struct gpio_keys_button jj934x_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = JJPLUS_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= JJ_AR934X_GPIO_15,
		.active_low	= 1,
	},
};

static struct ar8327_pad_cfg jwap604_ar8327_pad0_cfg = {
	.mode = AR8327_PAD_MAC_RGMII,
	.txclk_delay_en = true,
	.rxclk_delay_en = true,
	.txclk_delay_sel = AR8327_CLK_DELAY_SEL1,
	.rxclk_delay_sel = AR8327_CLK_DELAY_SEL2,
};

static struct ar8327_platform_data jwap604_ar8327_data = {
	.pad0_cfg = &jwap604_ar8327_pad0_cfg,
	.port0_cfg = {
		.force_link = 1,
		.speed = AR8327_PORT_SPEED_1000,
		.duplex = 1,
		.txpause = 1,
		.rxpause = 1,
	},
};

static struct mdio_board_info jwap604_mdio0_info[] = {
	{
		.bus_id = "ag71xx-mdio.0",
		.phy_addr = 0,
		.platform_data = &jwap604_ar8327_data,
	}
};


static void jj934x_common_setup(void)
{
    u8 *art = (u8 *) KSEG1ADDR(0x1fff0000);

    ath79_register_m25p80(NULL);

    ath79_register_leds_gpio(-1, ARRAY_SIZE(jj934x_leds_gpio),
                 jj934x_leds_gpio);
    ath79_register_gpio_keys_polled(-1, JJPLUS_KEYS_POLL_INTERVAL,
                    ARRAY_SIZE(jj934x_gpio_keys),
                    jj934x_gpio_keys);
    ath79_register_usb();
    ath79_register_wmac(art + JJPLUS_WMAC_CALDATA_OFFSET, NULL);

	ath79_register_mdio(0, 0x0);
	ath79_init_mac(ath79_eth0_data.mac_addr, art + JJPLUS_MAC0_OFFSET, 0);
}

static void __init jwap604_setup(void)
{
    jj934x_common_setup();

	ap91_pci_init_simple();

	ath79_setup_ar934x_eth_cfg(AR934X_ETH_CFG_RGMII_GMAC0 |
				   AR934X_ETH_CFG_SW_ONLY_MODE);

	mdiobus_register_board_info(jwap604_mdio0_info,
				    ARRAY_SIZE(jwap604_mdio0_info));

	/* GMAC0 is connected to an AR8327 switch */
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ath79_eth0_data.phy_mask = BIT(0);
	ath79_eth0_data.mii_bus_dev = &ath79_mdio0_device.dev;
	ath79_eth0_pll_data.pll_1000 = 0x06000000;
	ath79_register_eth(0);
}

MIPS_MACHINE(ATH79_MACH_JWAP604, "JWAP604", "jjPlus JWAP604 board",
	     jwap604_setup);


#define AR934X_GMAC_REG_MAC_CFG1    0x0
#define AR934X_GMAC_REG_MAC_CFG2    0x4
#define AR934X_GMAC_REG_MII_CFG2    0x20
#define AR934X_GMAC_REG_ETH_FIFO    0x48
#define AR934X_GMAC_REG_ETH_CFG1    0x4c
#define AR934X_GMAC_REG_ETH_CFG2    0x50
#define AR934X_GMAC_REG_ETH_CFG3    0x54
#define AR934X_GMAC_REG_ETH_CFG4    0x58
#define AR934X_GMAC_REG_ETH_CFG5    0x5c

#define AR934X_GE0_BASE     0x19000000
#define AR934X_GE0_SIZE     0x10000

static void __init ar8035_gphy_setup(void)
{
    void __iomem *base;
    u32 t;

    base = ioremap(AR934X_GE0_BASE, AR934X_GE0_SIZE);

    t = __raw_readl(base + AR934X_GMAC_REG_MAC_CFG1);
    t |= 0x30;
    __raw_writel(t, base + AR934X_GMAC_REG_MAC_CFG1);

    t = 0x7215;
    __raw_writel(t, base + AR934X_GMAC_REG_MAC_CFG2);

    t = 0x80000006;
    __raw_writel(t, base + AR934X_GMAC_REG_MII_CFG2);
    t = 0x6;
    __raw_writel(t, base + AR934X_GMAC_REG_MII_CFG2);

    t = 0x015500aa;
    __raw_writel(t, base + AR934X_GMAC_REG_ETH_CFG2);

    t = 0x66bc2;
    __raw_writel(t, base + AR934X_GMAC_REG_ETH_CFG5);

    iounmap(base);
}

static void __init jwap603_setup(void)
{
    jj934x_common_setup();

    ap91_pci_init_simple();

    ath79_setup_ar934x_eth_cfg(AR934X_ETH_CFG_RGMII_GMAC0 | 0x14000);
    ar8035_gphy_setup();

    /* GMAC0 is connected to an AR8035 PHY */
    ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
    ath79_eth0_data.phy_mask = BIT(0);
    ath79_eth0_data.speed = SPEED_1000;
    ath79_eth0_data.duplex = DUPLEX_FULL;
    if (ath79_soc != ATH79_SOC_AR9344)
        ath79_eth0_pll_data.pll_1000 = 0x06000000;
    else
        ath79_eth0_pll_data.pll_1000 = 0x0e000000;

    ath79_register_eth(0);
}

MIPS_MACHINE(ATH79_MACH_JWAP603, "JWAP603", "jjPlus JWAP603/606 board",
         jwap603_setup);


static struct gpio_led jwap702_leds_gpio[] __initdata = {
    {
        .name       = "jwap702:green:led1",
        .gpio       = JJ_AR934X_GPIO_11,
        .active_low = 1,
    },
    {
        .name       = "jwap702:green:reserve1",
        .gpio       = JJ_AR934X_GPIO_12,
        .active_low = 1,
    },
    {
        .name       = "jwap702:green:led2",
        .gpio       = JJ_AR934X_GPIO_13,
        .active_low = 1,
    },
    {
        .name       = "jwap702:green:led3",
        .gpio       = JJ_AR934X_GPIO_14,
        .active_low = 1,
    },
    {
        .name       = "jwap702:green:reserve2",
        .gpio       = JJ_AR934X_GPIO_16,
        .active_low = 1,
    }
};

static struct gpio_keys_button jwap702_gpio_keys[] __initdata = {
    {
        .desc       = "reset",
        .type       = EV_KEY,
        .code       = KEY_RESTART,
        .debounce_interval = JJPLUS_KEYS_DEBOUNCE_INTERVAL,
        .gpio       = JJ_AR934X_GPIO_15,
        .active_low = 1,
    },
};

static void __init jwap702_setup(void)
{
    u8 *art = (u8 *) KSEG1ADDR(0x1fff0000);

    ath79_register_m25p80(NULL);

    ath79_register_leds_gpio(-1, ARRAY_SIZE(jwap702_leds_gpio),
		                 jwap702_leds_gpio);
    ath79_register_gpio_keys_polled(-1, JJPLUS_KEYS_POLL_INTERVAL,
		                    ARRAY_SIZE(jwap702_gpio_keys),
						                    jwap702_gpio_keys);
    ath79_register_usb();
    ath79_register_wmac(art + JJPLUS_WMAC_CALDATA_OFFSET, NULL);

    ath79_register_mdio(0, 0x0);
    ath79_init_mac(ath79_eth0_data.mac_addr, art + JJPLUS_MAC0_OFFSET, 0);

    ath79_setup_ar934x_eth_cfg(AR934X_ETH_CFG_RGMII_GMAC0 | 0x14000);
    ar8035_gphy_setup();

    /* GMAC0 is connected to an AR8035 PHY */
    ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
    ath79_eth0_data.phy_mask = BIT(0);
    ath79_eth0_data.speed = SPEED_1000;
    ath79_eth0_data.duplex = DUPLEX_FULL;
    if (ath79_soc != ATH79_SOC_AR9344)
        ath79_eth0_pll_data.pll_1000 = 0x06000000;
    else
        ath79_eth0_pll_data.pll_1000 = 0x0e000000;

    ath79_register_eth(0);
}

MIPS_MACHINE(ATH79_MACH_JWAP702, "JWAP702", "jjPlus JWAP702/212 board",
         jwap702_setup);
