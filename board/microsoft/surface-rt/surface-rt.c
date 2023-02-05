// SPDX-License-Identifier: GPL-2.0+
/*
 *  (C) Copyright 2010-2013
 *  NVIDIA Corporation <www.nvidia.com>
 */

#include <common.h>
#include <dm.h>
#include <log.h>
#include <asm/arch/pinmux.h>
#include <asm/arch/gp_padctrl.h>
#include <asm/arch/gpio.h>
#include <asm/gpio.h>
#include <linux/delay.h>
#include "pinmux-config-surface-rt.h"
#include <i2c.h>

#define PMU_I2C_ADDRESS		0x2D
#define MAX_I2C_RETRY		3

#define TPS65911_LDO1		0x30
#define TPS65911_LDO2		0x31
#define TPS65911_LDO5		0x32
#define TPS65911_LDO3		0x37

#define TPS65911_GPIO0		0x60
#define TPS65911_GPIO1		0x61
#define TPS65911_GPIO2		0x62
#define TPS65911_GPIO3		0x63
#define TPS65911_GPIO4		0x64
#define TPS65911_GPIO5		0x65
#define TPS65911_GPIO6		0x66
#define TPS65911_GPIO7		0x67
#define TPS65911_GPIO8		0x68

#define TPS65911_DEVCTRL	0x3F
#define DEVCTRL_PWR_OFF_MASK	0x80
#define DEVCTRL_DEV_OFF_MASK	0x01
#define DEVCTRL_DEV_ON_MASK	0x04

#ifdef CONFIG_CMD_POWEROFF
int do_poweroff(struct cmd_tbl *cmdtp, int flag,
		int argc, char *const argv[])
{
	struct udevice *dev;
	uchar data_buffer[1];
	int ret;

	ret = i2c_get_chip_for_busnum(0, PMU_I2C_ADDRESS, 1, &dev);
	if (ret) {
		debug("%s: Cannot find PMIC I2C chip\n", __func__);
		return 0;
	}

	ret = dm_i2c_read(dev, TPS65911_DEVCTRL, data_buffer, 1);
	if (ret)
		return ret;

	data_buffer[0] |= DEVCTRL_PWR_OFF_MASK;

	ret = dm_i2c_write(dev, TPS65911_DEVCTRL, data_buffer, 1);
	if (ret)
		return ret;

	data_buffer[0] |= DEVCTRL_DEV_OFF_MASK;
	data_buffer[0] &= ~DEVCTRL_DEV_ON_MASK;

	ret = dm_i2c_write(dev, TPS65911_DEVCTRL, data_buffer, 1);
	if (ret)
		return ret;

	// wait some time and then print error
	mdelay(5000);
	printf("Failed to power off!!!\n");
	return 1;
}
#endif

/*
 * Routine: pinmux_init
 * Description: Do individual peripheral pinmux configs
 */
void pinmux_init(void)
{
	pinmux_config_pingrp_table(tegra3_pinmux_common,
		ARRAY_SIZE(tegra3_pinmux_common));

	pinmux_config_pingrp_table(unused_pins_lowpower,
		ARRAY_SIZE(unused_pins_lowpower));

	/* Initialize any non-default pad configs (APB_MISC_GP regs) */
	pinmux_config_drvgrp_table(cardhu_padctrl, ARRAY_SIZE(cardhu_padctrl));
}

#if defined(CONFIG_MMC_SDHCI_TEGRA)
/*
 * Do I2C/PMU writes to bring up SD card bus power
 *
 */
void board_sdmmc_voltage_init(void)
{
	struct udevice *dev;
	uchar data_buffer[1];
	int ret;
	int i;

	// Enable Wifi power
	gpio_request(TEGRA_GPIO(D, 4), "MRVL-SD8797_Wifi_Enable");
	gpio_direction_output(TEGRA_GPIO(D, 4), 0);
	udelay(5);
	gpio_set_value(TEGRA_GPIO(D, 4), 1);
	printf("Enabled MRVL-SD8797 WiFi GPIO D4\n");

	// Disable USB VBus
	gpio_request(TEGRA_GPIO(DD, 6), "usb1_regulator_enable");
	gpio_direction_output(TEGRA_GPIO(DD, 6), 0);
	udelay(5);
	// disable USB VBus to prevent shorting Surface RT VBus to Host PC
	// USB module can enable it if the controller is in host mode.
	gpio_set_value(TEGRA_GPIO(DD, 6), 0);

	ret = i2c_get_chip_for_busnum(0, PMU_I2C_ADDRESS, 1, &dev);
	if (ret) {
		debug("%s: Cannot find PMIC I2C chip\n", __func__);
		return;
	}

	/* TPS659110: LDO5_REG = 3.3v, ACTIVE to SDMMC1 */
	data_buffer[0] = 0x65;
	for (i = 0; i < MAX_I2C_RETRY; ++i) {
		if (dm_i2c_write(dev, TPS65911_LDO5, data_buffer, 1))
			udelay(100);
	}

	/* TPS659110: LDO1_REG = 3.3v, ACTIVE to SDMMC4 */ // really realted to coverkeyboard
	data_buffer[0] = 0xc9;
	for (i = 0; i < MAX_I2C_RETRY; ++i) {
		if (dm_i2c_write(dev, TPS65911_LDO1, data_buffer, 1))
			udelay(100);
	}

	/* TPS659110: LDO2_REG = 3.1v, ACTIVE to SDMMC1 */
	data_buffer[0] = 0x15; //was = data_buffer[0] = 0xb9; 15
	for (i = 0; i < MAX_I2C_RETRY; ++i) {
		if (dm_i2c_write(dev, TPS65911_LDO2, data_buffer, 1))
			udelay(100);
	}

	/* TPS659110: LDO3_REG = 3.1v, ACTIVE to SDMMC1 VIO */// - enables touchkeyboard.
	data_buffer[0] = 0x5d;
	for (i = 0; i < MAX_I2C_RETRY; ++i) {
		if (dm_i2c_write(dev, TPS65911_LDO3, data_buffer, 1))
			udelay(100);
	}

		/* TPS659110: GPIO0_REG output high to VDD_5V0_SBY */
	data_buffer[0] = 0x07;
	for (i = 0; i < MAX_I2C_RETRY; ++i) {
		if (dm_i2c_write(dev, TPS65911_GPIO0, data_buffer, 1))
			udelay(100);
	}

	/* TPS659110: GPIO6_REG output high to VDD_3V3_SYS */
	data_buffer[0] = 0x07;
	for (i = 0; i < MAX_I2C_RETRY; ++i) {
		if (dm_i2c_write(dev, TPS65911_GPIO6, data_buffer, 1))
			udelay(100);
	}

	/* TPS659110: GPIO7_REG output high to VDD_1V5_DDR */
	data_buffer[0] = 0x09; // was = data_buffer[0] = 0x07;
	for (i = 0; i < MAX_I2C_RETRY; ++i) {
		if (dm_i2c_write(dev, TPS65911_GPIO7, data_buffer, 1))
			udelay(100);
	}

	/* TPS659110: GPIO8_REG pull_down output high to VDD_5V0_SYS */
	data_buffer[0] = 0x0f;
	for (i = 0; i < MAX_I2C_RETRY; ++i) {
		if (dm_i2c_write(dev, TPS65911_GPIO8, data_buffer, 1))
			udelay(100);
	}
}

/*
 * Routine: pin_mux_mmc
 * Description: setup the MMC muxes, power rails, etc.
 */
void pin_mux_mmc(void)
{
	/*
	 * NOTE: We don't do mmc-specific pin muxes here.
	 * They were done globally in pinmux_init().
	 */

	/* Bring up the SDIO1 power rail */
	board_sdmmc_voltage_init();
}
#endif	/* MMC */
