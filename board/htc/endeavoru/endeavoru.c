// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  (C) Copyright 2010-2013
 *  NVIDIA Corporation <www.nvidia.com>
 *
 *  (C) Copyright 2021
 *  Svyatoslav Ryhel <clamor95@gmail.com>
 */

#include <common.h>
#include <dm.h>
#include <i2c.h>
#include <log.h>
#include <asm/arch/pinmux.h>
#include <asm/arch/gp_padctrl.h>
#include <asm/arch/gpio.h>
#include <asm/gpio.h>
#include <linux/delay.h>
#include "pinmux-config-endeavoru.h"

#define TPS80032_CTL1_I2C_ADDR		0x48
#define TPS80032_PHOENIX_DEV_ON		0x25
#define   DEVOFF			BIT(0)
#define TPS80032_LDO1_CFG_STATE		0x9E
#define TPS80032_LDO1_CFG_VOLTAGE	0x9F

#define TPS65200_I2C_ADDR		0x6A
#define TPS65200_CONTROL		0x00
#define   SMON_EN			BIT(5)
#define   LDO_EN			BIT(3)
#define   CH_EN				0x02

#define TPS65200_CONFIG_A		0x01
#define   VICHRG_1050MA			0x05 << 3
#define   VITERM_100MA			0x01

#define TPS65200_CONFIG_B		0x02
#define   IIN_LIMIT_500MA		0x01 << 6
#define   VOREG				0x23 /* approx 4.3v */

#define TPS65200_CONFIG_C		0x03
#define   VSREG				0x03
#define   VS_REF			BIT(7)

#ifdef CONFIG_CMD_POWEROFF
int do_poweroff(struct cmd_tbl *cmdtp, int flag,
		int argc, char *const argv[])
{
	struct udevice *dev;
	int ret;

	ret = i2c_get_chip_for_busnum(0, TPS80032_CTL1_I2C_ADDR, 1, &dev);
	if (ret) {
		debug("%s: Cannot find PMIC I2C chip\n", __func__);
		return 0;
	}

	ret = dm_i2c_reg_write(dev, TPS80032_PHOENIX_DEV_ON, DEVOFF);
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
	pinmux_config_pingrp_table(endeavoru_pinmux_common,
		ARRAY_SIZE(endeavoru_pinmux_common));
}

#ifdef CONFIG_MMC_SDHCI_TEGRA
/*
 * Set up slow charging mode so that phone could charge from bootloader
 */
void tps65200_start_slow_charge(void)
{
	struct udevice *dev;
	int ret;

	ret = i2c_get_chip_for_busnum(0, TPS65200_I2C_ADDR, 1, &dev);
	if (ret)
		printf("%s: Cannot find charger I2C chip\n", __func__);

	ret = dm_i2c_reg_write(dev, TPS65200_CONFIG_A,
				VICHRG_1050MA | VITERM_100MA);
	if (ret)
		printf("config A set failed: %d\n", ret);

	ret = dm_i2c_reg_write(dev, TPS65200_CONTROL,
				SMON_EN | LDO_EN | CH_EN);
	if (ret)
		printf("control register set failed: %d\n", ret);

	ret = dm_i2c_reg_write(dev, TPS65200_CONFIG_C,
				VS_REF | VSREG);
	if (ret)
		printf("config C set failed: %d\n", ret);

	ret = dm_i2c_reg_write(dev, TPS65200_CONFIG_B,
				IIN_LIMIT_500MA | VOREG);
	if (ret)
		printf("config B set failed: %d\n", ret);
}

/*
 * Until correct driver is available lets set pmic regulators here
 */
void tps65200_primary_regulators_init(void)
{
	struct udevice *dev;
	int ret;

	ret = i2c_get_chip_for_busnum(0, TPS80032_CTL1_I2C_ADDR, 1, &dev);
	if (ret)
		printf("%s: Cannot find PMIC I2C chip\n", __func__);

	/* TPS80032: LDO1_REG = 1.2v to DSI */
	ret = dm_i2c_reg_write(dev, TPS80032_LDO1_CFG_VOLTAGE, 0x03);
	if (ret)
		printf("%s: avdd_dsi_csi voltage set failed: %d\n", __func__, ret);

	/* TPS80032: LDO1_REG enable */
	ret = dm_i2c_reg_write(dev, TPS80032_LDO1_CFG_STATE, 0x01);
	if (ret)
		printf("%s: avdd_dsi_csi enable failed: %d\n", __func__, ret);
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
	 * Here we can bring up eMMC power.
	 */

	/* Enable charging in slow mode */
	tps65200_start_slow_charge();

	/* Bring up DSI power */
	tps65200_primary_regulators_init();
}
#endif	/* MMC */
