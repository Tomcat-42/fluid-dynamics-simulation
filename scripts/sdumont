#!/bin/sh

VPNC_PATH=/etc/vpnc/sdumont.conf
MODE=${2:-ssh}

if ! [ -f "$VPNC_PATH" ]; then
	echo "no config found at $VPNC_PATH"
	exit 1
fi

if [ "$#" -lt 1 ]; then
	echo "no username"
	exit 1
fi

sudo vpnc /etc/vpnc/sdumont.conf
$MODE "$1"@login.sdumont.lncc.br
sudo vpnc-disconnect
