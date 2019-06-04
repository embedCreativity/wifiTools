#!/bin/ash

#if [[ \( 1 -gt $# \) -o \( 1 -ne $# && 3 -ne $# \) ]]
if [ 1 -ne $#  ]
then
    echo "USAGE: $0 [SSID]"
    #echo "USAGE (Secured AP): $0 [SSID] [Encryption] [Passphrase]"
    exit
elif [ 1 -eq $# ]
then
    SSID=$1
#elif [ 3 -eq $# ]
#then
#    SSID=$1
#    ENCRYPTION=$2
#    PASS=$3
fi

# Bridge between Ethernet and WiFi
uci set network.lan.type='bridge'

# Set static Ethernet IP
uci set network.lan.ipaddr='192.168.1.1'

# Enable wireless radio
uci delete wireless.radio0.disabled

# Connect radio to Ethernet bridge
uci set wireless.@wifi-iface[0].network='lan'

# Set radio to WAP mode
uci set wireless.@wifi-iface[0].mode='ap'

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

