#!/bin/ash

if [[ \( 1 -gt $# \) -o \( 1 -ne $# && 3 -ne $# \) ]]
then
    echo "USAGE (Open AP): $0 [SSID]"
    echo "USAGE (Secured AP): $0 [SSID] [Encryption] [Passphrase]"
    exit
elif [ 1 -eq $# ]
then
    SSID=$1
elif [ 3 -eq $# ]
then
    SSID=$1
    ENCRYPTION=$2
    PASS=$3
fi

# Create an independent interface for wireless in network config
if [ $(grep wwan /etc/config/network -c) -eq 0 ]; then echo -e "config interface 'wwan'\n\toption proto 'dhcp'\n" >> /etc/config/network; fi

# Disable Ethernet bridge between Ethernet and WiFi
uci delete network.lan.type

# Setup static Ethernet IP off of common subnet
uci set network.lan.ipaddr='192.168.2.1'

# Enable wireless radio
uci delete wireless.radio0.disabled

# Connect radio to new interface
uci set wireless.@wifi-iface[0].network='wwan'

# Set radio to station mode
uci set wireless.@wifi-iface[0].mode='sta'

# Set SSID
uci set wireless.@wifi-iface[0].ssid=$SSID

# Setup encryption type if present. Open if not
if [ 1 -lt $# ]
then
    uci set wireless.@wifi-iface[0].encryption=$ENCRYPTION
    uci set wireless.@wifi-iface[0].key=$PASS
else
    uci set wireless.@wifi-iface[0].encryption='none'
    uci delete wireless.@wifi-iface[0].key
fi

# Commit changes to /etc/config/{network,wireless}
uci commit network
uci commit wireless

# Reload interfaces
/etc/init.d/network restart

