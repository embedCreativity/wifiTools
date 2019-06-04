This is targetted for an Openwrt installation.

# scanWifi
Simple program that drives the Linux iw command and scrapes the output to locate available wireless APs in the area. Returns available APs with information about their encryption status.

The format will look like this (brackets are included in output):
[SSID][CIPHER][AUTH_SUITE]

TODO: The output of Cipher and Auth Suite should be mapped to OpenWRT-type encryption definitions (e.g., 'psk2')

# connectWifi
This script modifies /etc/config/{network,wireless} configuration files via the 'uci' command and resets the interfaces with the new parameters. The Ethernet adapter is set to a static IP address of 192.168.2.1 to make sure that it is always available. If it were to stay bridged to the wireless adapter, the WiFi interface would either inject DHCP into the LAN of the connected AP thereby wreaking havoc, or it would potentially never be assigned an IP address if the AP was not running a DHCP service. In that event, the device would be unreachable through either interface. We set it static to avoid the drama.
    (Open AP): connectWifi [SSID]
    (Secured AP): connectWifi [SSID] [Encryption] [Passphrase]
        Encryption - must match OpenWRT naming conventions (e.g., psk2)
        
# createWAP
This script also modifies /etc/config/{network,wireless} config files using 'uci'. However, it bridges Ethernet and WiFi interfaces, thereby serving DHCP on both interfaces. The script creates an open access point using whatever SSID is provided. There is code in there to optionally make this a secured AP, but it hasn't been tested.
    createWAP [SSID]
