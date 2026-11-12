#pragma once

// Copy this file to include/config.h and adapt the values to your network.

// Optional preferred Wi-Fi (2.4 GHz only). Leave WIFI_SSID empty to rely on the
// webOS-Remote-Setup captive portal; it is also used as a fallback.
#define WIFI_SSID ""
#define WIFI_PASSWORD ""

#define REMOTE_HOSTNAME "televisor"
#define REMOTE_IP 192, 168, 1, 247
#define REMOTE_GATEWAY 192, 168, 1, 1
#define REMOTE_SUBNET 255, 255, 255, 0

// LG TV to control. TV_MAC is what identifies it: TV_IP is only the initial
// guess, the remote finds the TV by MAC (ARP) if DHCP gives it a new address.
#define TV_IP "192.168.1.100"
#define TV_MAC "AA:BB:CC:DD:EE:FF"
#define WOL_BROADCAST "192.168.1.255"
