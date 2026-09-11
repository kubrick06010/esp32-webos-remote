#include <Arduino.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include <Preferences.h>
#include <WebServer.h>
#include <WebSocketsClient.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <WiFiUdp.h>

#include "config.h"

namespace {

WebServer server(80);
WebSocketsClient controlWs;
WebSocketsClient pointerWs;
Preferences preferences;

bool controlConnected = false;
bool registered = false;
bool pointerConnected = false;
String clientKey;
String lastAction = "Ninguna todavía";
uint32_t requestId = 1;

const char PAGE[] PROGMEM = R"HTML(<!doctype html><html lang="es"><head>
<meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<meta name="theme-color" content="#101318"><title>Mando webOS ESP32</title><style>
:root{color-scheme:dark;--bg:#101318;--remote:#292a2d;--btn:#202124;--line:#41434a;--txt:#f6f7f8;--muted:#9da2ab}
*{box-sizing:border-box}body{margin:0;min-height:100vh;background:var(--bg);color:var(--txt);font-family:-apple-system,BlinkMacSystemFont,"Segoe UI",sans-serif}.app{padding:20px 12px 28px}.top{display:flex;justify-content:center;align-items:center;gap:12px;margin:0 auto 16px;font-size:13px;color:var(--muted)}.top b{font-size:19px;color:var(--txt)}.dot{width:10px;height:10px;border-radius:50%;background:#777}.ok .dot{background:#4fd06a;box-shadow:0 0 0 4px #4fd06a20}.bad .dot{background:#ed3b3b}.remote{width:304px;margin:auto;padding:20px 18px 24px;border:1px solid #3b3c40;border-radius:38px;background:var(--remote);box-shadow:0 20px 50px #0008}.grid{display:grid;grid-template-columns:repeat(3,1fr);gap:10px 16px}.b{height:48px;border:1px solid #0d0e10;border-radius:50%;background:var(--btn);color:var(--txt);font:650 15px inherit;box-shadow:0 3px 5px #0007;cursor:pointer;-webkit-tap-highlight-color:transparent}.b:active{transform:translateY(2px) scale(.96)}.power{background:#d43235;font-size:22px}.nums{margin-top:12px}.nums .b{font-size:18px}.controls{display:grid;grid-template-columns:64px 1fr 64px;gap:12px;align-items:center;margin:18px 0 14px}.rock{height:104px;display:grid;grid-template-rows:1fr auto 1fr;align-items:center;border:1px solid #0d0e10;border-radius:28px;background:var(--btn);overflow:hidden;text-align:center}.rock button{height:100%;border:0;background:none;color:var(--txt);font-size:23px}.rock span{font-size:11px}.mute{width:48px;justify-self:center}.pad{position:relative;width:194px;height:194px;margin:0 auto 16px;border:1px solid #0d0e10;border-radius:50%;background:var(--btn);box-shadow:0 4px 7px #0008}.pad button{position:absolute;border:0;background:none;color:var(--txt);font-size:24px}.up{left:63px;top:0;width:68px;height:57px}.down{left:63px;bottom:0;width:68px;height:57px}.left{left:0;top:63px;width:57px;height:68px}.right{right:0;top:63px;width:57px;height:68px}.pad .enter{left:57px;top:57px;width:78px;height:78px;border:1px solid #101114;border-radius:50%;background:#24262a;font-size:16px;font-weight:700}.nav{margin-bottom:15px}.nav .b{font-size:12px}.colors{display:grid;grid-template-columns:repeat(4,1fr);gap:18px;padding:0 8px;margin-bottom:15px}.color{width:36px;height:36px;justify-self:center;color:transparent}.r{background:#ed3838}.g{background:#19ae70}.y{background:#ffc62d}.bl{background:#1587e8}.apps{display:grid;grid-template-columns:1fr 1fr;gap:12px;margin-top:15px}.appb{height:42px;border-radius:9px;font-size:13px}.netflix{background:#f5f5f4;color:#d51520;font-weight:850}.prime{background:#1689db}.activity{max-width:550px;min-height:44px;margin:18px auto 0;padding:11px 16px;border:1px solid var(--line);border-radius:12px;font-size:13px;color:var(--muted)}.activity b{color:var(--txt)}
</style></head><body><main class="app"><header class="top"><b>▣ Mando webOS</b><span>ESP32 · televisor.local</span><span id="s"><i class="dot"></i> Conectando…</span></header><section class="remote">
<div class="grid"><button class="b power" data-c="POWER">⏻</button><button class="b" data-c="INPUT_HUB">INPUT</button><button class="b" data-c="MENU">⚙</button></div>
<div class="grid nums"><button class="b" data-c="1">1</button><button class="b" data-c="2">2</button><button class="b" data-c="3">3</button><button class="b" data-c="4">4</button><button class="b" data-c="5">5</button><button class="b" data-c="6">6</button><button class="b" data-c="7">7</button><button class="b" data-c="8">8</button><button class="b" data-c="9">9</button><i></i><button class="b" data-c="0">0</button></div>
<div class="controls"><div class="rock"><button data-c="VOLUMEUP">＋</button><span>VOL</span><button data-c="VOLUMEDOWN">−</button></div><button class="b mute" data-c="MUTE">⌁</button><div class="rock"><button data-c="CHANNELUP">⌃</button><span>CH</span><button data-c="CHANNELDOWN">⌄</button></div></div>
<div class="pad"><button class="up" data-c="UP">⌃</button><button class="down" data-c="DOWN">⌄</button><button class="left" data-c="LEFT">‹</button><button class="right" data-c="RIGHT">›</button><button class="enter" data-c="ENTER">OK</button></div>
<div class="grid nav"><button class="b" data-c="BACK">Atrás</button><button class="b" data-c="HOME">⌂</button><button class="b" data-c="EXIT">Salir</button></div>
<div class="colors"><button class="b color r" data-c="RED">R</button><button class="b color g" data-c="GREEN">G</button><button class="b color y" data-c="YELLOW">Y</button><button class="b color bl" data-c="BLUE">B</button></div>
<div class="grid"><button class="b" data-c="REWIND">◀◀</button><button class="b" data-c="PLAY">▶</button><button class="b" data-c="FASTFORWARD">▶▶</button></div>
<div class="apps"><button class="b appb netflix" data-c="NETFLIX">NETFLIX</button><button class="b appb prime" data-c="AMAZON">prime video</button></div></section><div class="activity"><b>Última acción:</b> <span id="a">Ninguna todavía</span></div></main>
<script>const s=document.querySelector('#s'),a=document.querySelector('#a');async function status(){try{let d=await(await fetch('/api/status')).json();s.className=d.registered?'ok':'bad';s.innerHTML='<i class="dot"></i> '+(d.registered?'Conectado':d.control?'Emparejando…':'Sin conexión');a.textContent=d.last||a.textContent}catch(e){s.className='bad';s.innerHTML='<i class="dot"></i> Sin conexión'}}async function send(c){a.textContent='Enviando '+c+'…';try{let r=await fetch('/api/command',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({command:c})}),d=await r.json();a.textContent=d.message||d.error;if(c==='POWER')setTimeout(status,5000)}catch(e){a.textContent='Error: '+e.message}status()}document.querySelectorAll('[data-c]').forEach(b=>b.onclick=()=>send(b.dataset.c));status();setInterval(status,5000)</script></body></html>)HTML";

const char *buttonCommands[] = {"INPUT_HUB", "MENU", "MUTE", "UP", "DOWN", "LEFT", "RIGHT", "ENTER", "BACK", "HOME", "EXIT", "RED", "GREEN", "YELLOW", "BLUE", "REWIND", "PLAY", "PAUSE", "STOP", "FASTFORWARD", "NETFLIX", "AMAZON", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};

bool isButtonCommand(const String &command) {
  for (const char *candidate : buttonCommands) if (command == candidate) return true;
  return false;
}

bool tvReachable() {
  WiFiClient probe;
  bool reachable = probe.connect(TV_IP, 3001, 700);
  probe.stop();
  return reachable;
}

void sendWakeOnLan() {
  uint8_t mac[6];
  unsigned int values[6];
  if (sscanf(TV_MAC, "%x:%x:%x:%x:%x:%x", &values[0], &values[1], &values[2], &values[3], &values[4], &values[5]) != 6) return;
  for (int i = 0; i < 6; ++i) mac[i] = values[i];
  uint8_t packet[102];
  memset(packet, 0xff, 6);
  for (int i = 1; i <= 16; ++i) memcpy(packet + i * 6, mac, 6);
  IPAddress broadcast; broadcast.fromString(WOL_BROADCAST);
  WiFiUDP udp; udp.begin(9);
  for (int i = 0; i < 6; ++i) {
    udp.beginPacket(broadcast, 9); udp.write(packet, sizeof(packet)); udp.endPacket(); delay(80);
  }
  udp.stop();
}

void sendRequest(const String &uri, const JsonDocument *payload = nullptr, const String &fixedId = "") {
  JsonDocument message;
  message["id"] = fixedId.length() ? fixedId : String("req_") + requestId++;
  message["type"] = "request";
  message["uri"] = uri;
  if (payload) message["payload"] = *payload;
  String output; serializeJson(message, output); controlWs.sendTXT(output);
}

void requestPointerSocket() { sendRequest("ssap://com.webos.service.networkinput/getPointerInputSocket", nullptr, "pointer_socket"); }

void sendRegistration() {
  JsonDocument root;
  root["id"] = "register_0"; root["type"] = "register";
  JsonObject payload = root["payload"].to<JsonObject>();
  if (clientKey.length()) payload["client-key"] = clientKey;
  payload["forcePairing"] = false; payload["pairingType"] = "PROMPT";
  JsonObject manifest = payload["manifest"].to<JsonObject>();
  manifest["manifestVersion"] = 1; manifest["appVersion"] = "1.0"; manifest["deviceName"] = "ESP32 webOS Remote";
  JsonArray permissions = manifest["permissions"].to<JsonArray>();
  const char *perms[] = {"LAUNCH", "CONTROL_AUDIO", "CONTROL_INPUT_JOYSTICK", "CONTROL_INPUT_MEDIA_PLAYBACK", "CONTROL_INPUT_TV", "CONTROL_POWER", "READ_POWER_STATE", "CONTROL_MOUSE_AND_KEYBOARD"};
  for (const char *permission : perms) permissions.add(permission);
  String output; serializeJson(root, output); controlWs.sendTXT(output);
}

void startPointer(const String &socketPath) {
  int pathStart = socketPath.indexOf('/', socketPath.indexOf("//") + 2);
  String path = pathStart >= 0 ? socketPath.substring(pathStart) : "/resources";
  pointerWs.disconnect(); pointerWs.beginSSL(TV_IP, 3001, path); pointerWs.setReconnectInterval(3000);
}

void controlEvent(WStype_t type, uint8_t *payload, size_t length) {
  if (type == WStype_CONNECTED) { controlConnected = true; registered = false; sendRegistration(); return; }
  if (type == WStype_DISCONNECTED) { controlConnected = registered = pointerConnected = false; return; }
  if (type != WStype_TEXT) return;
  JsonDocument doc;
  if (deserializeJson(doc, payload, length)) return;
  String messageType = doc["type"] | ""; String id = doc["id"] | "";
  if (messageType == "registered") {
    registered = true; clientKey = doc["payload"]["client-key"] | clientKey;
    if (clientKey.length()) { preferences.putString("clientKey", clientKey); }
    requestPointerSocket();
  } else if (id == "pointer_socket") {
    String socketPath = doc["payload"]["socketPath"] | "";
    if (socketPath.length()) startPointer(socketPath);
  }
}

void pointerEvent(WStype_t type, uint8_t *, size_t) {
  pointerConnected = type == WStype_CONNECTED ? true : type == WStype_DISCONNECTED ? false : pointerConnected;
}

bool dispatchCommand(const String &command, String &message) {
  bool reachable = tvReachable();
  if (command == "POWER" && !reachable) {
    controlWs.disconnect(); pointerWs.disconnect();
    controlConnected = registered = pointerConnected = false;
    sendWakeOnLan(); message = "Señal de encendido enviada"; return true;
  }
  if (!reachable) {
    controlWs.disconnect(); pointerWs.disconnect();
    controlConnected = registered = pointerConnected = false;
    message = "TV apagada o inaccesible; pulsa encendido"; return false;
  }
  if (!registered) { message = "TV accesible; esperando conexión o emparejamiento"; return false; }
  if (command == "POWER") sendRequest("ssap://system/turnOff");
  else if (command == "VOLUMEUP") sendRequest("ssap://audio/volumeUp");
  else if (command == "VOLUMEDOWN") sendRequest("ssap://audio/volumeDown");
  else if (command == "CHANNELUP") sendRequest("ssap://tv/channelUp");
  else if (command == "CHANNELDOWN") sendRequest("ssap://tv/channelDown");
  else if (isButtonCommand(command)) {
    if (!pointerConnected) { requestPointerSocket(); message = "Preparando entrada; vuelve a pulsar en un segundo"; return false; }
    pointerWs.sendTXT(String("type:button\nname:") + command + "\n\n");
  } else { message = "Comando no permitido"; return false; }
  lastAction = command + " enviado"; message = lastAction; return true;
}

void setupHttp() {
  server.on("/", HTTP_GET, [] { server.send_P(200, "text/html; charset=utf-8", PAGE); });
  server.on("/api/status", HTTP_GET, [] {
    JsonDocument doc; doc["wifi"] = WiFi.isConnected(); doc["control"] = controlConnected; doc["registered"] = registered; doc["pointer"] = pointerConnected; doc["last"] = lastAction;
    String body; serializeJson(doc, body); server.send(200, "application/json", body);
  });
  server.on("/api/command", HTTP_POST, [] {
    JsonDocument input; JsonDocument output; String message;
    if (deserializeJson(input, server.arg("plain"))) { output["ok"] = false; output["error"] = "JSON no válido"; }
    else { String command = input["command"] | ""; bool ok = dispatchCommand(command, message); output["ok"] = ok; if (ok) output["message"] = message; else output["error"] = message; }
    String body; serializeJson(output, body); server.send(output["ok"] ? 200 : 503, "application/json", body);
  });
  server.onNotFound([] { server.send(404, "application/json", "{\"error\":\"No encontrado\"}"); });
  server.begin();
}

}  // namespace

void setup() {
  Serial.begin(115200);
  preferences.begin("webos", false); clientKey = preferences.getString("clientKey", "");
  WiFi.mode(WIFI_STA);
  IPAddress local(REMOTE_IP), gateway(REMOTE_GATEWAY), subnet(REMOTE_SUBNET);
  WiFi.config(local, gateway, subnet);
  WiFi.setHostname(REMOTE_HOSTNAME);
  WiFiManager manager;
  manager.setConfigPortalTimeout(180);
  if (!manager.autoConnect("webOS-Remote-Setup")) ESP.restart();
  if (MDNS.begin(REMOTE_HOSTNAME)) {
    MDNS.addService("http", "tcp", 80);
    MDNS.addServiceTxt("http", "tcp", "path", "/");
  }
  ArduinoOTA.setHostname(REMOTE_HOSTNAME); ArduinoOTA.begin();
  controlWs.beginSSL(TV_IP, 3001, "/"); controlWs.onEvent(controlEvent); controlWs.setReconnectInterval(3000);
  controlWs.enableHeartbeat(15000, 3000, 2);
  pointerWs.enableHeartbeat(15000, 3000, 2);
  pointerWs.onEvent(pointerEvent);
  setupHttp();
  Serial.printf("Mando: http://%s/\n", WiFi.localIP().toString().c_str());
}

void loop() {
  controlWs.loop(); pointerWs.loop(); server.handleClient(); ArduinoOTA.handle(); delay(2);
}
