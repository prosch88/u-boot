// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2014 NVIDIA Corporation
 */

#include <common.h>
#include <backlight.h>
#include <dm.h>
#include <panel.h>
#include <log.h>
#include <misc.h>
#include <mipi_display.h>
#include <mipi_dsi.h>
#include <asm/gpio.h>
#include <linux/delay.h>
#include <linux/err.h>

struct sharp_panel_priv {
	struct udevice *backlight;
};

static struct display_timing default_timing = {
	.pixelclock.typ		= 278000000,
	.hactive.typ		= 2560,
	.hfront_porch.typ	= 128,
	.hback_porch.typ	= 64,
	.hsync_len.typ		= 64,
	.vactive.typ		= 1600,
	.vfront_porch.typ	= 4,
	.vback_porch.typ	= 8,
	.vsync_len.typ		= 32,
};

static int sharp_panel_write(struct udevice *dev, u16 offset, u8 value)
{
	u8 payload[3] = { offset >> 8, offset & 0xff, value };
	struct mipi_dsi_panel_plat *plat = dev_get_plat(dev);
	struct mipi_dsi_device *dsi = plat->device;
	ssize_t ret;

	ret = mipi_dsi_generic_write(dsi, payload, sizeof(payload));
	if (ret < 0) {
		printf("%s: failed to write %02x to %04x: %zd\n",
			__func__, value, offset, ret);
		return ret;
	}

	ret = mipi_dsi_dcs_nop(dsi);
	if (ret < 0) {
		printf("%s: failed to send DCS nop: %zd\n", __func__, ret);
		return ret;
	}

	udelay(15);

	return 0;
}

static int sharp_panel_enable_backlight(struct udevice *dev)
{
	/* needs power supply enable here */

	mdelay(150);

	return 0;
}

static int sharp_panel_set_backlight(struct udevice *dev, int percent)
{
	struct sharp_panel_priv *priv = dev_get_priv(dev);
	struct mipi_dsi_panel_plat *plat = dev_get_plat(dev);
	struct mipi_dsi_device *dsi = plat->device;
	u8 format = MIPI_DCS_PIXEL_FMT_24BIT;
	int ret;

	ret = mipi_dsi_dcs_exit_sleep_mode(dsi);
	if (ret < 0) {
		printf("%s: failed to exit sleep mode: %d\n", __func__, ret);
		return ret;
	}

	/* set left-right mode */
	ret = sharp_panel_write(dev, 0x1000, 0x2a);
	if (ret < 0) {
		printf("%s: failed to set left-right mode: %d\n", __func__, ret);
		return ret;
	}

	/* enable command mode */
	ret = sharp_panel_write(dev, 0x1001, 0x01);
	if (ret < 0) {
		printf("%s: failed to enable command mode: %d\n", __func__, ret);
		return ret;
	}

	ret = mipi_dsi_dcs_set_pixel_format(dsi, format);
	if (ret < 0) {
		printf("%s: failed to set pixel format: %d\n", __func__, ret);
		return ret;
	}

	/* setup symmetrical split */
	ret = mipi_dsi_dcs_set_column_address(dsi, 0, default_timing.hactive.typ / 2 - 1);
	if (ret < 0) {
		printf("%s: failed to set column address: %d\n", __func__, ret);
		return ret;
	}

	ret = mipi_dsi_dcs_set_page_address(dsi, 0, default_timing.vactive.typ - 1);
	if (ret < 0) {
		printf("%s: failed to set page address: %d\n", __func__, ret);
		return ret;
	}

	ret = mipi_dsi_dcs_set_display_on(dsi);
	if (ret < 0) {
		printf("%s: failed to set display on: %d\n", __func__, ret);
		return ret;
	}

	mdelay(120);

	ret = backlight_enable(priv->backlight);
	if (ret)
		return ret;

	ret = backlight_set_brightness(priv->backlight, percent);
	if (ret)
		return ret;

	return 0;
}

static int sharp_panel_timings(struct udevice *dev,
				  struct display_timing *timing)
{
	memcpy(timing, &default_timing, sizeof(*timing));
	return 0;
}

static int sharp_panel_of_to_plat(struct udevice *dev)
{
	struct sharp_panel_priv *priv = dev_get_priv(dev);
	int ret;

	ret = uclass_get_device_by_phandle(UCLASS_PANEL_BACKLIGHT, dev,
					   "backlight", &priv->backlight);
	if (ret) {
		printf("%s: Cannot get backlight: ret = %d\n", __func__, ret);
		return ret;
	}

	return 0;
}

static int sharp_panel_probe(struct udevice *dev)
{
	struct mipi_dsi_panel_plat *plat = dev_get_plat(dev);

	/* fill characteristics of DSI data link */
	plat->lanes = 4;
	plat->format = MIPI_DSI_FMT_RGB888;
	plat->mode_flags = MIPI_DSI_MODE_VIDEO;

	return 0;
}

static const struct panel_ops sharp_panel_ops = {
	.enable_backlight	= sharp_panel_enable_backlight,
	.set_backlight		= sharp_panel_set_backlight,
	.get_display_timing	= sharp_panel_timings,
};

static const struct udevice_id sharp_panel_ids[] = {
	{ .compatible = "sharp,lq101r1sx01" },
	{ }
};

U_BOOT_DRIVER(sharp_panel) = {
	.name		= "sharp_lq101r1sx01",
	.id		= UCLASS_PANEL,
	.of_match	= sharp_panel_ids,
	.ops		= &sharp_panel_ops,
	.of_to_plat	= sharp_panel_of_to_plat,
	.probe		= sharp_panel_probe,
	.plat_auto	= sizeof(struct mipi_dsi_panel_plat),
	.priv_auto	= sizeof(struct sharp_panel_priv),
};
