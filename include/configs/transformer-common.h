/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (c) 2022, Svyatoslav Ryhel <clamor95@gmail.com>.
 */

#ifndef __TRANSFORMER_COMMON_H
#define __TRANSFORMER_COMMON_H

/* High-level configuration options */
#define CONFIG_TEGRA_BOARD_STRING	"ASUS Transformer"

/*
 * SOS and LNX offset is relative to
 * mmcblk0 start on both t20 and t30
 */

#define TRANSFORMER_T20_EMMC_LAYOUT \
	"ebt_offset_r=0x1C00\0" \
	"ebt_size=0x2000\0" \
	"sos_offset_r=0x1C00\0" \
	"sos_size=0x2800\0" \
	"lnx_offset_r=0x4400\0" \
	"lnx_size=0x4000\0"

#define TRANSFORMER_T30_EMMC_LAYOUT \
	"ebt_offset_r=0x1C00\0" \
	"ebt_size=0x4000\0" \
	"sos_offset_r=0x3C00\0" \
	"sos_size=0x4000\0" \
	"lnx_offset_r=0x7C00\0" \
	"lnx_size=0x4000\0"

#define TRANSFORMER_BOOTZ \
	"bootkernel=bootz ${kernel_addr_r} - ${fdt_addr_r}\0" \
	"bootrdkernel=bootz ${kernel_addr_r} ${ramdisk_addr_r} ${fdt_addr_r}\0"

#define TRANSFORMER_BOOT_SCRIPT \
	"boot_script=echo Loading boot script;" \
		"if load mmc 0:5 ${scriptaddr} uboot-transformer.bcs;" \
		"then env import -t -r ${scriptaddr} ${filesize};" \
		"else echo Boot Configuration NOT FOUND!; fi;" \
		"echo Loading DTB;" \
		"load ${dev_type} ${mmcdev}:${mmcpart} ${fdt_addr_r} ${dtb_file};" \
		"echo Loading Kernel;" \
		"load ${dev_type} ${mmcdev}:${mmcpart} ${kernel_addr_r} ${kernel_file};" \
		"echo Loading Initramfs;" \
		"if load ${dev_type} ${mmcdev}:${mmcpart} ${ramdisk_addr_r} ${ramdisk_file};" \
		"then echo Booting Kernel;" \
			"run bootrdkernel;" \
		"else echo Booting Kernel;" \
			"run bootkernel; fi\0"

#define TRANSFORMER_BOOT_SOS \
	"boot_sos=echo Reading SOS partition;" \
		"mmc dev;" \
		"if mmc read ${kernel_addr_r} ${sos_offset_r} ${sos_size};" \
		"then echo Booting Kernel;" \
			"bootm ${kernel_addr_r};" \
		"else echo Reading SOS failed; fi\0"

#define TRANSFORMER_BOOT_LNX \
	"boot_lnx=echo Reading LNX partition;" \
		"mmc dev;" \
		"if mmc read ${kernel_addr_r} ${lnx_offset_r} ${lnx_size};" \
		"then echo Booting Kernel;" \
			"bootm ${kernel_addr_r};" \
		"else echo Reading LNX failed; fi\0"

#define TRANSFORMER_FLASH_UBOOT \
	"flash_uboot=echo Reading U-Boot binary;" \
		"if load mmc 1:1 ${kernel_addr_r} ${bootloader_file};" \
		"then echo Writing U-Boot into EBT;" \
			"mmc dev 0 1;" \
			"mmc write ${kernel_addr_r} ${ebt_offset_r} ${ebt_size};" \
		"else echo Reading U-Boot failed; fi\0"

#define TRANSFORMER_FLASH_SPI \
	"update_spi=sf probe 0:1;" \
		"echo Dumping current SPI flash content ...;" \
		"sf read ${kernel_addr_r} 0x0 0x400000;" \
		"if fatwrite mmc 1:1 ${kernel_addr_r} spi-flash-backup.bin 0x400000;" \
		"then echo SPI flash content was successfully written into spi-flash-backup.bin;" \
			"echo Reading bootloader binary;" \
			"if load mmc 1:1 ${kernel_addr_r} bootloader-update.bin;" \
			"then echo Writing bootloader into SPI flash;" \
  				"sf probe 0:1;" \
  				"sf update ${kernel_addr_r} 0x0 0x400000;" \
				"poweroff;" \
			"else echo Reading bootloader failed;" \
				"poweroff; fi;" \
		"else echo SPI flash backup FAILED! Aborting ...;" \
			"poweroff; fi\0"

#define TRANSFORMER_FASTBOOT_ALIAS \
	"fastboot_raw_partition_boot=${lnx_offset_r} ${lnx_size} mmcpart 0\0" \
	"fastboot_raw_partition_recovery=${sos_offset_r} ${sos_size} mmcpart 0\0" \
	"fastboot_partition_alias_system=APP\0" \
	"fastboot_partition_alias_cache=CAC\0" \
	"fastboot_partition_alias_misc=MSC\0" \
	"fastboot_partition_alias_staging=USP\0" \
	"fastboot_partition_alias_vendor=VDR\0" \
	"fastboot_partition_alias_userdata=UDA\0"

#define TRANSFORMER_BOOTMENU \
	TRANSFORMER_BOOT_SCRIPT \
	TRANSFORMER_BOOT_SOS \
	TRANSFORMER_BOOT_LNX \
	TRANSFORMER_FLASH_UBOOT \
	TRANSFORMER_FLASH_SPI \
	TRANSFORMER_FASTBOOT_ALIAS \
	"bootmenu_0=boot with script=run boot_script\0" \
	"bootmenu_1=boot LNX=run boot_lnx\0" \
	"bootmenu_2=boot SOS=run boot_sos\0" \
	"bootmenu_3=fastboot=echo Starting Fastboot protocol ...; fastboot usb 0\0" \
	"bootmenu_4=reboot=reset\0" \
	"bootmenu_5=power off=poweroff\0" \
	"bootmenu_delay=-1\0"

#define TRANSFORMER_BUTTON_CHECK \
	"check_button=gpio input ${gpio_vol_down}; test $? -eq 0;\0"

#define TRANSFORMER_DEFAULT_FILESET \
	"kernel_file=vmlinuz\0" \
	"ramdisk_file=uInitrd\0" \
	"bootloader_file=u-boot-dtb-tegra.bin\0"

#define TRANSFORMER_LOAD_KERNEL \
	"echo Loading Kernel;" \
	"if load mmc ${bootdev}:1 ${kernel_addr_r} ${kernel_file};" \
	"then echo Loading DTB;" \
		"load mmc ${bootdev}:1 ${fdt_addr_r} ${fdtfile};" \
		"setenv bootargs console=ttyS0,115200n8 root=/dev/mmcblk${bootdev}p${rootpart} rw gpt;" \
		"echo Loading Initramfs;" \
		"if load mmc ${bootdev}:1 ${ramdisk_addr_r} ${ramdisk_file};" \
		"then echo Booting Kernel;" \
			"run bootrdkernel;" \
		"else echo Booting Kernel;" \
			"run bootkernel; fi;"

#endif /* __CONFIG_H */
