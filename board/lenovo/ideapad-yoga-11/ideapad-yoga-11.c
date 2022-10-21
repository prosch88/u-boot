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
#include "pinmux-config-ideapad-yoga-11.h"
#include <i2c.h>

#define PMU_I2C_ADDRESS		0x2D
#define MAX_I2C_RETRY		3

void setup_bridge(void);

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
	uchar reg, data_buffer[1];
	int ret;
	int i;

	gpio_request(TEGRA_GPIO(U, 4), "Touchpad enable");
	gpio_direction_output(TEGRA_GPIO(U, 4), 0);
	udelay(5);
	gpio_set_value(TEGRA_GPIO(U, 4), 1);
	printf("Touchpad U4\n");
	
	gpio_request(TEGRA_GPIO(U, 3), "Touchscreen Enable");
	gpio_direction_output(TEGRA_GPIO(U, 3), 0);
	udelay(5);
	gpio_set_value(TEGRA_GPIO(U, 3), 1);
	printf("Touchscreen U3\n");

// Panel
	gpio_request(TEGRA_GPIO(L, 4), "vdd-panel");

	
	mdelay(200);
// Panel

// Bridge
	gpio_request(TEGRA_GPIO(P, 0), "bridge power");
	gpio_request(TEGRA_GPIO(C, 1), "bridge shutdown");
	gpio_request(TEGRA_GPIO(Z, 3), "bridge reset");
	
	gpio_direction_output(TEGRA_GPIO(P, 0), 0);
	gpio_direction_output(TEGRA_GPIO(C, 1), 0);
	gpio_direction_output(TEGRA_GPIO(Z, 3), 0);
	
	udelay(5);

	gpio_set_value(TEGRA_GPIO(P, 0), 1);
	udelay(50);
	gpio_set_value(TEGRA_GPIO(C, 1), 1);
	udelay(50);
	gpio_set_value(TEGRA_GPIO(Z, 3), 1);
	mdelay(50);
// Bridge



gpio_direction_output(TEGRA_GPIO(L, 4), 1);

mdelay(10);

// Backlight
	gpio_request(TEGRA_GPIO(H, 0), "backlight enable");
	gpio_request(TEGRA_GPIO(H, 2), "backlight pwm");
	
	gpio_direction_output(TEGRA_GPIO(H, 2), 1); // pwm
	mdelay(1);
	gpio_direction_output(TEGRA_GPIO(H, 0), 1); // enable
	
// Backlight
mdelay(10);

	setup_bridge();




	ret = i2c_get_chip_for_busnum(4, PMU_I2C_ADDRESS, 1, &dev);
	if (ret) {
		printf("%s: Cannot find PMIC I2C chip: %d\n", __func__, ret);
		return;
	}

	/* TPS659110: LDO3_REG = 3.3v, ACTIVE to SDMMC1 */
	data_buffer[0] = 0x65;
	reg = 0x37;

	for (i = 0; i < MAX_I2C_RETRY; ++i) {
		if (dm_i2c_write(dev, reg, data_buffer, 1))
			udelay(100);
	}
	
	/* TPS659110: GPIO8_REG = Output High */
	data_buffer[0] = 0x07;
	reg = 0x68;

	for (i = 0; i < MAX_I2C_RETRY; ++i) {
		if (dm_i2c_write(dev, reg, data_buffer, 1))
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

int bridge_write_reg(struct udevice *dev, uchar reg, uchar data_buffer) {
	return  dm_i2c_reg_write(dev, reg, data_buffer);
}

int bridge_read_reg(struct udevice *dev, uchar reg) {
	return dm_i2c_reg_read(dev, reg);
}

int bridge_xfer(struct udevice *dev, struct i2c_msg *msg, int nmsgs) {
	return dm_i2c_xfer(dev, msg, nmsgs);
}


void setup_bridge(void)  {
	printf("Start Bridge Setup\n");
	struct udevice *dev;
	int ret;

	ret = i2c_get_chip_for_busnum(1, 0x8, 1, &dev);
	if (ret) {
		printf("%s: Cannot find Bridge I2C chip: %d\n", __func__, ret);
		return;
	}
	
	bridge_write_reg(dev, 0x0A, 0x30);
	udelay(310);
	bridge_write_reg(dev, 0x0A, 0x0C);
	udelay(110);
	bridge_write_reg(dev, 0x8F, 0x02);
	
	
	// check for lowest bit
	u8 in_buf = 0x8D, out_buf = 0;
	struct i2c_msg msgs[2]= {
		{ 0x08, 0,		1, &in_buf },
		{ 0x08, I2C_M_RD,	1, &out_buf }
	};

	int counter = 0;

//gpio_set_value(TEGRA_GPIO(L, 4), 1);

	do {
		udelay(1110);
		bridge_xfer(dev, msgs, 2);
		printf("Checking 0x8D: %02X\n", out_buf);
		out_buf = 0;
		
		counter++;
		if (counter > 100) goto fail;
	} while(!(out_buf & 0x04));
	
	printf("Bridge came up!\n");

fail:
	printf("setup failed\n\n");
}


