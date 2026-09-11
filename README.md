# ESP32 webOS Remote

Un mando web local y extensible para televisores LG webOS, con Wake-on-LAN,
emparejamiento SSAP/WebSocket y actualización OTA. Está pensado como base para
automatizaciones maker: el mismo ESP32 puede añadir sensores, LEDs, relés y
otras rutas HTTP.

Mando web autónomo para la TV LG webOS. El ESP32 sirve la interfaz en
`http://televisor.local` (también mediante la IP configurada), controla la TV por SSAP/WebSocket y la enciende mediante
Wake-on-LAN.

## Hardware probado

- ESP32-C3 con 4 MB de flash (`esp32-c3-devkitm-1`).
- La TV y el ESP32 deben estar en la misma red local.
- Configura la IP y MAC de tu TV en `include/config.h`.

La placa probada usa USB Serial/JTAG nativo; cualquier placa ESP32-C3
compatible con Arduino/PlatformIO debería poder adaptarse cambiando `board`.

## Primera instalación

1. Instala PlatformIO y conecta el ESP32 por USB.
2. Copia `include/config.example.h` como `include/config.h` y adapta IP, MAC y
   broadcast de tu instalación. `include/config.h` está excluido del repositorio.
3. Ejecuta `pio run -e esp32c3 -t upload` desde esta carpeta.
3. Si el ESP32 no conserva credenciales Wi-Fi, conéctate al punto de acceso
   `webOS-Remote-Setup` y selecciona tu red.
5. Abre `http://televisor.local` o la IP configurada.
5. Enciende la TV y acepta la solicitud de emparejamiento. La clave se almacena
   en NVS y no queda incluida en el firmware ni en el repositorio.

## Actualizaciones OTA

Después de la primera carga USB, usa `pio run -e esp32c3_ota -t upload
--upload-port <IP_DEL_ESP32>`. OTA no está disponible hasta que este firmware
haya sido instalado al menos una vez por USB.

## Seguridad

La interfaz solo se publica en la red local y no tiene autenticación. No abras
el puerto 80 del ESP32 hacia Internet. El firmware solo expone botones normales;
no contiene comandos de calibración ni menús de servicio.

## Licencia

MIT. Consulta [LICENSE](LICENSE).
