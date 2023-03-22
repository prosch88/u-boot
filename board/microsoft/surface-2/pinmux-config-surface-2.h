/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2010-2013, NVIDIA CORPORATION.  All rights reserved.
 */

#ifndef _PINMUX_CONFIG_SURFACE_2_H_
#define _PINMUX_CONFIG_SURFACE_2_H_

#define DEFAULT_PINMUX(_pingrp, _mux, _pull, _tri, _io)		\
	{							\
		.pingrp		= PMUX_PINGRP_##_pingrp,	\
		.func		= PMUX_FUNC_##_mux,		\
		.pull		= PMUX_PULL_##_pull,		\
		.tristate	= PMUX_TRI_##_tri,		\
		.io		= PMUX_PIN_##_io,		\
		.lock		= PMUX_PIN_LOCK_DEFAULT,	\
		.od		= PMUX_PIN_OD_DEFAULT,		\
		.ioreset	= PMUX_PIN_IO_RESET_DEFAULT,	\
	}

#define I2C_PINMUX(_pingrp, _mux, _pull, _tri, _io, _lock, _od)	\
	{							\
		.pingrp		= PMUX_PINGRP_##_pingrp,	\
		.func		= PMUX_FUNC_##_mux,		\
		.pull		= PMUX_PULL_##_pull,		\
		.tristate	= PMUX_TRI_##_tri,		\
		.io		= PMUX_PIN_##_io,		\
		.lock		= PMUX_PIN_LOCK_##_lock,	\
		.od		= PMUX_PIN_OD_##_od,		\
		.ioreset	= PMUX_PIN_IO_RESET_DEFAULT,	\
	}

#define DDC_PINMUX(_pingrp, _mux, _pull, _tri, _io, _lock, _rcv_sel) \
	{							\
		.pingrp		= PMUX_PINGRP_##_pingrp,	\
		.func		= PMUX_FUNC_##_mux,		\
		.pull		= PMUX_PULL_##_pull,		\
		.tristate	= PMUX_TRI_##_tri,		\
		.io		= PMUX_PIN_##_io,		\
		.lock		= PMUX_PIN_LOCK_##_lock,	\
		.rcv_sel	= PMUX_PIN_RCV_SEL_##_rcv_sel,	\
		.ioreset	= PMUX_PIN_IO_RESET_DEFAULT,	\
	}

#define VI_PINMUX(_pingrp, _mux, _pull, _tri, _io, _lock, _ioreset) \
	{							\
		.pingrp		= PMUX_PINGRP_##_pingrp,	\
		.func		= PMUX_FUNC_##_mux,		\
		.pull		= PMUX_PULL_##_pull,		\
		.tristate	= PMUX_TRI_##_tri,		\
		.io		= PMUX_PIN_##_io,		\
		.lock		= PMUX_PIN_LOCK_##_lock,	\
		.od		= PMUX_PIN_OD_DEFAULT,		\
		.ioreset	= PMUX_PIN_IO_RESET_##_ioreset	\
	}

#define CEC_PINMUX(_pingrp, _mux, _pull, _tri, _io, _lock, _od)		\
	{								\
		.pingrp     = PMUX_PINGRP_##_pingrp,			\
		.func       = PMUX_FUNC_##_mux,				\
		.pull       = PMUX_PULL_##_pull,			\
		.tristate   = PMUX_TRI_##_tri,				\
		.io         = PMUX_PIN_##_io,				\
		.lock       = PMUX_PIN_LOCK_##_lock,			\
		.od         = PMUX_PIN_OD_##_od,			\
		.ioreset    = PMUX_PIN_IO_RESET_DEFAULT,		\
	}

#define USB_PINMUX CEC_PINMUX

#define DEFAULT_PADCFG(_drvgrp, _slwf, _slwr, _drvup, _drvdn, _lpmd, _schmt, _hsm) \
	{						\
		.drvgrp = PMUX_DRVGRP_##_drvgrp,	\
		.slwf   = _slwf,			\
		.slwr   = _slwr,			\
		.drvup  = _drvup,			\
		.drvdn  = _drvdn,			\
		.lpmd   = PMUX_LPMD_##_lpmd,		\
		.schmt  = PMUX_SCHMT_##_schmt,		\
		.hsm    = PMUX_HSM_##_hsm,		\
	}

static struct pmux_pingrp_config tegra114_pinmux_common[] = {
	// For UART only
	DEFAULT_PINMUX(SDMMC3_CMD_PA7,    UARTA,      DOWN,        TRISTATE,   INPUT),
	DEFAULT_PINMUX(SDMMC3_DAT1_PB6,   UARTA,      NORMAL,        NORMAL,   OUTPUT),
	
	/* SDMMC3 pinmux */
	/*
	DEFAULT_PINMUX(SDMMC3_CLK_PA6,    SDMMC3,      NORMAL,    NORMAL,   INPUT),
	DEFAULT_PINMUX(SDMMC3_CMD_PA7,    SDMMC3,      UP,        NORMAL,   INPUT),
	DEFAULT_PINMUX(SDMMC3_DAT0_PB7,   SDMMC3,      UP,        NORMAL,   INPUT),
	DEFAULT_PINMUX(SDMMC3_DAT1_PB6,   SDMMC3,      UP,        NORMAL,   INPUT),
	DEFAULT_PINMUX(SDMMC3_DAT2_PB5,   SDMMC3,      UP,        NORMAL,   INPUT),
	DEFAULT_PINMUX(SDMMC3_DAT3_PB4,   SDMMC3,      UP,        NORMAL,   INPUT),
	DEFAULT_PINMUX(SDMMC3_CLK_LB_IN_PEE5,  SDMMC3,  UP,        TRISTATE, INPUT),
	DEFAULT_PINMUX(SDMMC3_CLK_LB_OUT_PEE4, SDMMC3,  DOWN,      NORMAL,   INPUT),
	*/
};

static struct pmux_pingrp_config unused_pins_lowpower[] = {
};

/* Initially setting all used GPIO's to non-TRISTATE */
static struct pmux_pingrp_config tegra114_pinmux_set_nontristate[] = {
	
};

static struct pmux_drvgrp_config surface_2_padctrl[] = {
	/* (_drvgrp, _slwf, _slwr, _drvup, _drvdn, _lpmd, _schmt, _hsm) */
	DEFAULT_PADCFG(SDIO3, SDIOCFG_DRVUP_SLWF, SDIOCFG_DRVDN_SLWR, \
		SDIOCFG_DRVUP, SDIOCFG_DRVDN, NONE, NONE, NONE),
};
#endif /* PINMUX_CONFIG_COMMON_H */
