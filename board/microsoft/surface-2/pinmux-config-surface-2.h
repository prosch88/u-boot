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

	DEFAULT_PINMUX(SDMMC3_CMD_PA7,    UARTA,      DOWN,        TRISTATE,   INPUT),

	DEFAULT_PINMUX(SDMMC3_DAT1_PB6,   UARTA,      NORMAL,        NORMAL,   OUTPUT),
	
};

static struct pmux_pingrp_config unused_pins_lowpower[] = {
	DEFAULT_PINMUX(CLK1_REQ_PEE2,     RSVD3,       DOWN, TRISTATE, OUTPUT),
	DEFAULT_PINMUX(USB_VBUS_EN1_PN5,  RSVD3,       DOWN, TRISTATE, OUTPUT),
};

/* Initially setting all used GPIO's to non-TRISTATE */
static struct pmux_pingrp_config tegra114_pinmux_set_nontristate[] = {
	DEFAULT_PINMUX(GPIO_X4_AUD_PX4,     RSVD1,  DOWN,    NORMAL,    OUTPUT),
	DEFAULT_PINMUX(GPIO_X5_AUD_PX5,     RSVD1,  UP,      NORMAL,    INPUT),
	DEFAULT_PINMUX(GPIO_X6_AUD_PX6,     RSVD3,  UP,      NORMAL,    INPUT),
	DEFAULT_PINMUX(GPIO_X7_AUD_PX7,     RSVD1,  DOWN,    NORMAL,    OUTPUT),
	DEFAULT_PINMUX(GPIO_W2_AUD_PW2,     RSVD1,  UP,      NORMAL,    INPUT),
	DEFAULT_PINMUX(GPIO_W3_AUD_PW3,     SPI6,   UP,      NORMAL,    INPUT),
	DEFAULT_PINMUX(GPIO_X1_AUD_PX1,     RSVD3,  DOWN,    NORMAL,    INPUT),
	DEFAULT_PINMUX(GPIO_X3_AUD_PX3,     RSVD3,  UP,      NORMAL,    INPUT),

	DEFAULT_PINMUX(DAP3_FS_PP0,         I2S2,   DOWN,    NORMAL,    OUTPUT),
	DEFAULT_PINMUX(DAP3_DIN_PP1,        I2S2,   DOWN,    NORMAL,    OUTPUT),
	DEFAULT_PINMUX(DAP3_DOUT_PP2,       I2S2,   DOWN,    NORMAL,    OUTPUT),
	DEFAULT_PINMUX(DAP3_SCLK_PP3,       I2S2,   DOWN,    NORMAL,    OUTPUT),
	DEFAULT_PINMUX(PV0,                 RSVD3,  NORMAL,  NORMAL,    INPUT),
	DEFAULT_PINMUX(PV1,                 RSVD1,  NORMAL,  NORMAL,    INPUT),

	DEFAULT_PINMUX(PBB3,                RSVD3,  DOWN,    NORMAL,    OUTPUT),
	DEFAULT_PINMUX(PBB5,                RSVD3,  DOWN,    NORMAL,    OUTPUT),
	DEFAULT_PINMUX(PBB6,                RSVD3,  DOWN,    NORMAL,    OUTPUT),
	DEFAULT_PINMUX(PBB7,                RSVD3,  DOWN,    NORMAL,    OUTPUT),
	DEFAULT_PINMUX(PCC1,                RSVD3,  DOWN,    NORMAL,    INPUT),
	DEFAULT_PINMUX(PCC2,                RSVD3,  DOWN,    NORMAL,    INPUT),

	DEFAULT_PINMUX(GMI_AD0_PG0,         GMI,    NORMAL,  NORMAL,    OUTPUT),
	DEFAULT_PINMUX(GMI_AD1_PG1,         GMI,    NORMAL,  NORMAL,    OUTPUT),
	DEFAULT_PINMUX(GMI_AD10_PH2,        GMI,    DOWN,    NORMAL,    OUTPUT),
	DEFAULT_PINMUX(GMI_AD11_PH3,        GMI,    DOWN,    NORMAL,    OUTPUT),
	DEFAULT_PINMUX(GMI_AD12_PH4,        GMI,    UP,      NORMAL,    INPUT),
	DEFAULT_PINMUX(GMI_AD13_PH5,        GMI,    DOWN,    NORMAL,    OUTPUT),
	DEFAULT_PINMUX(GMI_AD2_PG2,         GMI,    NORMAL,  NORMAL,    INPUT),
	DEFAULT_PINMUX(GMI_AD3_PG3,         GMI,    NORMAL,  NORMAL,    INPUT),
	DEFAULT_PINMUX(GMI_AD8_PH0,         GMI,    DOWN,    NORMAL,    OUTPUT),
	DEFAULT_PINMUX(GMI_ADV_N_PK0,       GMI,    UP,      NORMAL,    INPUT),
	DEFAULT_PINMUX(GMI_CLK_PK1,         GMI,    DOWN,    NORMAL,    OUTPUT),
	DEFAULT_PINMUX(GMI_CS0_N_PJ0,       GMI,    UP,      NORMAL,    INPUT),
	DEFAULT_PINMUX(GMI_CS2_N_PK3,       GMI,    UP,      NORMAL,    INPUT),
	DEFAULT_PINMUX(GMI_CS3_N_PK4,       GMI,    UP,      NORMAL,    OUTPUT),
	DEFAULT_PINMUX(GMI_CS4_N_PK2,       GMI,    UP,      NORMAL,    INPUT),
	DEFAULT_PINMUX(GMI_CS7_N_PI6,       GMI,    UP,      NORMAL,    INPUT),
	DEFAULT_PINMUX(GMI_DQS_P_PJ3,       GMI,    UP,      NORMAL,    INPUT),
	DEFAULT_PINMUX(GMI_IORDY_PI5,       GMI,    UP,      NORMAL,    INPUT),
	DEFAULT_PINMUX(GMI_WP_N_PC7,        GMI,    UP,      NORMAL,    INPUT),

	DEFAULT_PINMUX(SDMMC1_WP_N_PV3,     SPI4,   UP,      NORMAL,    OUTPUT),
	DEFAULT_PINMUX(CLK2_REQ_PCC5,       RSVD3,  NORMAL,  NORMAL,    OUTPUT),

	DEFAULT_PINMUX(KB_COL3_PQ3,         KBC,    UP,      NORMAL,    OUTPUT),
	DEFAULT_PINMUX(KB_COL4_PQ4,         SDMMC3, UP,      NORMAL,    INPUT),
	DEFAULT_PINMUX(KB_COL5_PQ5,         KBC,    UP,      NORMAL,    INPUT),
	DEFAULT_PINMUX(KB_COL6_PQ6,         KBC,    UP,      NORMAL,    OUTPUT),
	DEFAULT_PINMUX(KB_COL7_PQ7,         KBC,    UP,      NORMAL,    OUTPUT),
	DEFAULT_PINMUX(KB_ROW3_PR3,         KBC,    DOWN,    NORMAL,    INPUT),
	DEFAULT_PINMUX(KB_ROW4_PR4,         KBC,    DOWN,    NORMAL,    INPUT),
	DEFAULT_PINMUX(KB_ROW6_PR6,         KBC,    DOWN,    NORMAL,    INPUT),
	DEFAULT_PINMUX(KB_ROW8_PS0,         KBC,    UP,      NORMAL,    INPUT),

	DEFAULT_PINMUX(CLK3_REQ_PEE1,       RSVD3,  NORMAL,  NORMAL,    OUTPUT),
	DEFAULT_PINMUX(PU4,                 RSVD3,  NORMAL,  NORMAL,    OUTPUT),
	DEFAULT_PINMUX(PU5,                 RSVD3,  NORMAL,  NORMAL,    INPUT),
	DEFAULT_PINMUX(PU6,                 RSVD3,  NORMAL,  NORMAL,    INPUT),

	DEFAULT_PINMUX(HDMI_INT_PN7,        RSVD1,   DOWN,    NORMAL,   INPUT),

	DEFAULT_PINMUX(GMI_AD9_PH1,         PWM1,   NORMAL,   NORMAL,   OUTPUT),
	DEFAULT_PINMUX(SPDIF_IN_PK6,        USB,    NORMAL,   NORMAL,   INPUT),

	DEFAULT_PINMUX(SDMMC3_CD_N_PV2,     SDMMC3, UP,       NORMAL,   INPUT),
};

static struct pmux_drvgrp_config surface_2_padctrl[] = {
	/* (_drvgrp, _slwf, _slwr, _drvup, _drvdn, _lpmd, _schmt, _hsm) */
	DEFAULT_PADCFG(SDIO3, SDIOCFG_DRVUP_SLWF, SDIOCFG_DRVDN_SLWR, \
		SDIOCFG_DRVUP, SDIOCFG_DRVDN, NONE, NONE, NONE),
};
#endif /* PINMUX_CONFIG_COMMON_H */
