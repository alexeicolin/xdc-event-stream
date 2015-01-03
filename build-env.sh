#!/bin/bash

# Paths to build dependencies
#
# Here most point to the bundle in TI-RTOS, but most are also available as
# independent distributions (see README.md for links)

XDCTOOLS_PATH="/opt/ti/xdctools/xdctools_3_30_04_52_core"

# Append only if it's not there already
if ! echo $PATH | grep -q $XDCTOOLS_PATH
then
    export PATH=$PATH:$XDCTOOLS_PATH
fi

unset XDCPATH

# gnu.targets.*
XDCPATH="$XDCPATH;/opt/ti/tirtos/tirtos_tivac_2_00_01_23/products/bios_6_40_01_15/packages"

# gnu.targets.rts86U rebuilt using XDCTools v3.30.04.52
# Note: this is a manually created repository (see README.md)
XDCPATH="$XDCPATH;/opt/ti/rts/packages"

# ti.drivers.uart
XDCPATH="$XDCPATH;/opt/ti/tirtos/tirtos_tivac_2_00_01_23/packages"


# The following two are necessary only for LoggerIdle only

# ti.sysbios.knl (for LoggerIdle)
XDCPATH="$XDCPATH;/opt/ti/tirtos/tirtos_tivac_2_00_01_23/products/bios_6_40_01_15/packages"

# UIA (for LoggerIdle)
XDCPATH="$XDCPATH;/opt/ti/tirtos/tirtos_tivac_2_00_01_23/products/uia_2_00_00_28/packages"

export XDCPATH

# TivaWare for building eventgenapp for Tiva C  (M4F)
TIVAWARE_INSTALLATION_DIR="/opt/ti/tirtos/tirtos_tivac_2_00_01_23/products/TivaWare_C_Series-2.1.0.12573c"
export TIVAWARE_INSTALLATION_DIR
