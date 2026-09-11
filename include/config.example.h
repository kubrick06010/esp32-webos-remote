#pragma once

// Copia este archivo como include/config.h y adapta los valores a tu red.
#define REMOTE_HOSTNAME "televisor"
#define REMOTE_IP 192, 168, 1, 247
#define REMOTE_GATEWAY 192, 168, 1, 1
#define REMOTE_SUBNET 255, 255, 255, 0

// IP, MAC y broadcast de la TV LG que se quiere controlar.
#define TV_IP "192.168.1.100"
#define TV_MAC "AA:BB:CC:DD:EE:FF"
#define WOL_BROADCAST "192.168.1.255"
