# ESP32 webOS Remote

A local, extensible web remote for LG webOS TVs, with Wake-on-LAN,
SSAP/WebSocket pairing, and OTA updates. It is designed as a foundation for
maker automation: the same ESP32 can add sensors, LEDs, relays, and other HTTP
routes.

The ESP32 serves the remote at `http://televisor.local` (or its configured IP),
controls the TV over SSAP/WebSocket, and powers it on through Wake-on-LAN.

## Tested hardware

- ESP32-C3 with 4 MB flash (`esp32-c3-devkitm-1`).
- The TV and ESP32 must be on the same local network.
- Set your TV IP address and MAC address in `include/config.h`.

The tested board uses native USB Serial/JTAG. Other ESP32-C3 boards supported by
Arduino/PlatformIO can be adapted by changing `board`.

## First installation

1. Install PlatformIO and connect the ESP32 over USB.
2. Copy `include/config.example.h` to `include/config.h` and set your IP, MAC,
   and broadcast address. `include/config.h` is ignored by this repository.
3. Run `pio run -e esp32c3 -t upload` from this directory.
4. If the ESP32 has no saved Wi-Fi credentials, connect to the
   `webOS-Remote-Setup` access point and select your network.
5. Open `http://televisor.local` or the configured IP address.
6. Turn on the TV and accept the pairing request. The key is stored in NVS and
   is never included in the firmware source or repository.

## OTA updates

After the first USB upload, run `pio run -e esp32c3_ota -t upload
--upload-port <ESP32_IP>`. OTA is not available until this firmware has been
installed at least once over USB.

## Security

The interface is exposed only on the local network and has no authentication.
Do not expose the ESP32 port 80 to the Internet. The firmware exposes normal
remote buttons only; it contains no calibration or service-menu commands.

## License

MIT. See [LICENSE](LICENSE).
