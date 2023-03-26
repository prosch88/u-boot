// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2010-2013, NVIDIA CORPORATION.  All rights reserved.
 *
 * Copyright (c) 2023, Svyatoslav Ryhel <clamor95@gmail.com>
 */

/* T114 Transformers derive from Macallan board */

#include <common.h>
#include <dm.h>
#include <log.h>
#include <i2c.h>
#include <asm/arch/pinmux.h>
#include <asm/arch/gp_padctrl.h>
#include <linux/delay.h>

#include "pinmux-config-transformer.h"

#define PMU_I2C_ADDRESS			0x58	/* TPS65913 PMU */

/* page 1 */
#define TPS65913_SMPS9_CTRL		0x38
#define TPS65913_SMPS9_VOLTAGE		0x3B
#define TPS65913_LDO2_CTRL		0x52
#define TPS65913_LDO2_VOLTAGE		0x53
#define TPS65913_LDO9_CTRL		0x60
#define TPS65913_LDO9_VOLTAGE		0x61
#define TPS65913_LDOUSB_CTRL		0x64
#define TPS65913_LDOUSB_VOLTAGE		0x65

/* page 2 */
#define TPS65913_GPIO_DATA_DIR		0x81
#define TPS65913_GPIO_DATA_OUT		0x82
#define TPS65913_GPIO_4_MASK		BIT(4)

#define TPS65913_DEV_CTRL		0xA0
#define TPS65913_INT3_MASK		0x1B
#define TPS65913_INT3_MASK_VBUS		BIT(7)

#ifdef CONFIG_CMD_POWEROFF
int do_poweroff(struct cmd_tbl *cmdtp, int flag,
		int argc, char *const argv[])
{
	struct udevice *dev;
	int ret;

	/* Mask INT3 on second page first */
	ret = i2c_get_chip_for_busnum(0, PMU_I2C_ADDRESS + 1, 1, &dev);
	if (ret) {
		printf("%s: Cannot find PMIC I2C chip\n", __func__);
		return 1;
	}

	ret = dm_i2c_reg_read(dev, TPS65913_INT3_MASK);
	if (ret < 0)
		return ret;

	ret = dm_i2c_reg_write(dev, TPS65913_INT3_MASK,
			       ret | TPS65913_INT3_MASK_VBUS);
	if (ret)
		return ret;

	ret = i2c_get_chip_for_busnum(0, PMU_I2C_ADDRESS, 1, &dev);
	if (ret) {
		printf("%s: Cannot find PMIC I2C chip\n", __func__);
		return 1;
	}

	/* TPS65913: DEV_CTRL > OFF */
	ret = dm_i2c_reg_write(dev, TPS65913_DEV_CTRL, 0);
	if (ret)
		printf("%s: PMU i2c_write DEV_CTRL < OFF returned %d\n",
			__func__, ret);

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
	pinmux_config_pingrp_table(tegra114_pinmux_common,
		ARRAY_SIZE(tegra114_pinmux_common));

	/* Initialize any non-default pad configs (APB_MISC_GP regs) */
	pinmux_config_drvgrp_table(transformer_t114_padctrl,
		ARRAY_SIZE(transformer_t114_padctrl));
}

#if defined(CONFIG_MMC_SDHCI_TEGRA)
/*
 * Do I2C/PMU writes to bring up SD card bus power
 *
 */
void board_sdmmc_voltage_init(void)
{
	struct udevice *dev;
	int ret;

	ret = i2c_get_chip_for_busnum(0, PMU_I2C_ADDRESS, 1, &dev);
	if (ret) {
		printf("%s: Cannot find PMIC I2C chip\n", __func__);
		return;
	}

	/* TPS65913: SMPS9_VOLTAGE = 2.9V */
	ret = dm_i2c_reg_write(dev, TPS65913_SMPS9_VOLTAGE, 0xE5);
	if (ret)
		printf("%s: PMU i2c_write SMPS9 < 2.9v returned %d\n",
			__func__, ret);

	/* TPS65913: SMPS9_CTRL = Active */
	ret = dm_i2c_reg_write(dev, TPS65913_SMPS9_CTRL, 0x01);
	if (ret)
		printf("%s: SMPS9 enable returned %d\n", __func__, ret);

	/* TPS65913: LDO2_VOLTAGE = 1.2V */
	ret = dm_i2c_reg_write(dev, TPS65913_LDO2_VOLTAGE, 0x07);
	if (ret)
		printf("%s: PMU i2c_write LDO2 < 1.8v returned %d\n",
			__func__, ret);

	/* TPS65913: LDO2_CTRL = Active */
	ret = dm_i2c_reg_write(dev, TPS65913_LDO2_CTRL, 0x01);
	if (ret)
		printf("%s: LDO2 enable returned %d\n", __func__, ret);

	/* TPS65913: LDO9_VOLTAGE = 1.8V */
	ret = dm_i2c_reg_write(dev, TPS65913_LDO9_VOLTAGE, 0x13);
	if (ret)
		printf("%s: PMU i2c_write LDO9 < 1.8v returned %d\n",
			__func__, ret);

	/* TPS65913: LDO9_CTRL = Active */
	ret = dm_i2c_reg_write(dev, TPS65913_LDO9_CTRL, 0x01);
	if (ret)
		printf("%s: LDO9 enable returned %d\n", __func__, ret);

	/* TPS65913: LDOUSB_VOLTAGE = 3.3V */
	ret = dm_i2c_reg_write(dev, TPS65913_LDOUSB_VOLTAGE, 0x31);
	if (ret)
		printf("%s: PMU i2c_write LDOUSB < 3.3v returned %d\n",
			__func__, ret);

	/* TPS65913: LDOUSB_CTRL = Active */
	ret = dm_i2c_reg_write(dev, TPS65913_LDOUSB_CTRL, 0x01);
	if (ret)
		printf("%s: LDOUSB enable returned %d\n", __func__, ret);

	/* TPS65913: configure GPIO 4 */
	ret = i2c_get_chip_for_busnum(0, PMU_I2C_ADDRESS + 1, 1, &dev);
	if (ret) {
		printf("%s: Cannot find PMIC I2C chip\n", __func__);
		return;
	}

	ret = dm_i2c_reg_read(dev, TPS65913_GPIO_DATA_DIR);
	if (ret)
		printf("%s: GPIO direction read fail %d\n", __func__, ret);

	ret = dm_i2c_reg_write(dev, TPS65913_GPIO_DATA_DIR,
			       ret | TPS65913_GPIO_4_MASK);
	if (ret)
		printf("%s: GPIO 4 dir output set fail %d\n", __func__, ret);

	ret = dm_i2c_reg_read(dev, TPS65913_GPIO_DATA_OUT);
	if (ret)
		printf("%s: GPIO output read fail %d\n", __func__, ret);

	ret = dm_i2c_reg_write(dev, TPS65913_GPIO_DATA_OUT,
			       ret | TPS65913_GPIO_4_MASK);
	if (ret)
		printf("%s: GPIO 4 output on set fail %d\n", __func__, ret);
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

	/* Bring up the SDIO3 power rail */
	board_sdmmc_voltage_init();
}
#endif /* MMC */
