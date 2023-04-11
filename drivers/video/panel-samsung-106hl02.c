// SPDX-License-Identifier: GPL-2.0-only

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
#include <linux/delay.h>

struct samsung_panel_priv {
	struct udevice *backlight;
	//struct gpio_desc reset_gpio;
};

static struct display_timing default_timing = {
	.pixelclock.typ	= (1920 + 32 + 32 + 64) * (1080 + 6 + 3 + 22) * 60,
	.hactive.typ	 	= 1920,
	.hfront_porch.typ	= 32,
	.hback_porch.typ	= 32,
	.hsync_len.typ		= 64,
	.vactive.typ 		= 1080,
	.vfront_porch.typ	= 6,
	.vback_porch.typ	= 3,
	.vsync_len.typ		= 22,
};


static int samsung_panel_timings(struct udevice *dev,
				  struct display_timing *timing)
{
	memcpy(timing, &default_timing, sizeof(*timing));
	return 0;
}

static int samsung_panel_enable_backlight(struct udevice *dev)
{
	/* needs power supply enable here */

	mdelay(150);

	return 0;
}

static int samsung_panel_set_backlight(struct udevice *dev, int percent)
{
	struct samsung_panel_priv *priv = dev_get_priv(dev);
	struct mipi_dsi_panel_plat *plat = dev_get_plat(dev);
	struct mipi_dsi_device *dsi = plat->device;

	int ret;

	ret = backlight_enable(priv->backlight);
	if (ret)
		return ret;

	ret = backlight_set_brightness(priv->backlight, percent);
	if (ret)
		return ret;

	//mdelay(1000);

// Panel start
/*
	ret = gpio_request_by_name(dev, "reset-gpios", 0,
				   &priv->reset_gpio, GPIOD_IS_OUT);
	if (ret) {
		printf("missing reset GPIO\n");
		return ret;
	}

	// RESET PANEL
	dm_gpio_set_value(&priv->reset_gpio, 0);
	udelay(35000);
	dm_gpio_set_value(&priv->reset_gpio, 1);
	udelay(70000);

	printf("Panel was reset :)\n");
*/
	/* TURN PANEL ON*/

	ret = mipi_dsi_dcs_exit_sleep_mode(dsi);
	if (ret < 0) {
		printf("Failed to exit sleep mode: %d\n", ret);
		return ret;
	}
	mdelay(200);

	ret = mipi_dsi_dcs_set_display_on(dsi);
	if (ret < 0) {
		printf("Failed to exit sleep mode: %d\n", ret);
		return ret;
	}
	mdelay(200);

// Panel end
	return 0;
}

static int sharp_panel_of_to_plat(struct udevice *dev)
{
	struct samsung_panel_priv *priv = dev_get_priv(dev);
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
	plat->mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_LPM;

	return 0;
}

static const struct panel_ops samsung_panel_ops = {
	.enable_backlight	= samsung_panel_enable_backlight,
	.set_backlight		= samsung_panel_set_backlight,
	.get_display_timing	= samsung_panel_timings,
};

static const struct udevice_id samsung_panel_ids[] = {
	{ .compatible = "samsung,106hl02" },
	{ }
};

U_BOOT_DRIVER(samsung_panel) = {
	.name		= "samsung_106hl02",
	.id		= UCLASS_PANEL,
	.of_match	= samsung_panel_ids,
	.ops		= &samsung_panel_ops,
	.of_to_plat	= sharp_panel_of_to_plat,
	.probe		= sharp_panel_probe,
	.plat_auto	= sizeof(struct mipi_dsi_panel_plat),
	.priv_auto	= sizeof(struct samsung_panel_priv),
};
