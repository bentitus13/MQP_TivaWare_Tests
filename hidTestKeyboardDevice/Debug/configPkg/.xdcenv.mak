#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = /home/alex/ti/tirtos_tivac_2_16_00_08/packages;/home/alex/ti/tirtos_tivac_2_16_00_08/products/tidrivers_tivac_2_16_00_08/packages;/home/alex/ti/tirtos_tivac_2_16_00_08/products/bios_6_45_01_29/packages;/home/alex/ti/tirtos_tivac_2_16_00_08/products/ndk_2_25_00_09/packages;/home/alex/ti/tirtos_tivac_2_16_00_08/products/uia_2_00_05_50/packages;/home/alex/ti/tirtos_tivac_2_16_00_08/products/ns_1_11_00_10/packages;/home/alex/ti/ccsv7/ccs_base
override XDCROOT = /home/alex/ti/xdctools_3_32_02_25_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = /home/alex/ti/tirtos_tivac_2_16_00_08/packages;/home/alex/ti/tirtos_tivac_2_16_00_08/products/tidrivers_tivac_2_16_00_08/packages;/home/alex/ti/tirtos_tivac_2_16_00_08/products/bios_6_45_01_29/packages;/home/alex/ti/tirtos_tivac_2_16_00_08/products/ndk_2_25_00_09/packages;/home/alex/ti/tirtos_tivac_2_16_00_08/products/uia_2_00_05_50/packages;/home/alex/ti/tirtos_tivac_2_16_00_08/products/ns_1_11_00_10/packages;/home/alex/ti/ccsv7/ccs_base;/home/alex/ti/xdctools_3_32_02_25_core/packages;..
HOSTOS = Linux
endif
