// Author: Mohimen Al Mahaini

#ifndef connect_to_wifi_h
#define connect_to_wifi_h

#include <Arduino.h>
#include <ESPAsyncWiFiManager.h>
#include <Ticker.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer serv(80);

Ticker ticker;
int BUILTIN_LED = 5;

const byte DNS_PORT = 53;
DNSServer dns;
IPAddress apIP(8, 8, 4, 4);
IPAddress netMsk(255, 255, 255, 0);

void tick()
{
    // toggle state
    int state = digitalRead(BUILTIN_LED); // get the current state of GPIO1 pin
    digitalWrite(BUILTIN_LED, !state);    // set pin to the opposite state
}

void configModeCallback(AsyncWiFiManager *myWiFiManager)
{
    Serial.println("Entered config mode . . .");
    Serial.println("Connected to the WiFi Network");
    Serial.println(WiFi.softAPIP());
    // entered config mode, make led toggle faster
    ticker.attach(0.2, tick);
}

void connectWiFi()
{
    Serial.print("Connecting to WIFI");
    WiFi.softAPConfig(apIP, apIP, netMsk);
    // Staring DNS Server
    // Apple
    dns.start(DNS_PORT, "captive.apple.com", apIP);
    // Android & Chrome
    dns.start(DNS_PORT, "connectivitycheck.gstatic.com", apIP);
    dns.start(DNS_PORT, "connectivitycheck.android.com", apIP);
    dns.start(DNS_PORT, "clients1.google.com", apIP);
    dns.start(DNS_PORT, "clients3.google.com", apIP);
    dns.start(DNS_PORT, "play.googleapis.com", apIP);
    dns.start(DNS_PORT, "android.clients.google.com", apIP);
    // Windows
    dns.start(DNS_PORT, "www.msftconnecttest.com", apIP);
    dns.start(DNS_PORT, "www.msftncsi.com", apIP);
    // Ubuntu / Gnome/linux
    dns.start(DNS_PORT, "start.ubuntu.com", apIP);
    dns.start(DNS_PORT, "connectivity-check.ubuntu.com", apIP);
    dns.start(DNS_PORT, "nmcheck.gnome.org", apIP);
    // FireFox
    dns.start(DNS_PORT, "detectportal.firefox.com", apIP);
    dns.start(DNS_PORT, "*", apIP);

    // WiFiManager
    // Local intialization. Once its business is done, there is no need to keep it around
    AsyncWiFiManager wifiManager(&serv, &dns);
    WiFi.disconnect(true);
    delay(200);
    // set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
    wifiManager.setAPCallback(configModeCallback);

    // fetches ssid and pass and tries to connect
    // if it does not connect it starts an access point with the specified name
    // and goes into a blocking loop awaiting configuration
    if (!wifiManager.autoConnect("BSCESP-32-AP", "Password"))
    {
        Serial.println("failed to connect and hit timeout");
    }

    // if you reach here you have connected to the WiFi
    Serial.println("Connected to the WiFi Network");
    Serial.println("MAC: " + WiFi.macAddress());
    Serial.println("IP: " + WiFi.localIP().toString());
    Serial.print("Received Signal Strength Indicator (RSSI) = " + WiFi.RSSI());
    Serial.println("connected...");

    WiFi.softAPdisconnect(true); // Turn off AP

    ticker.detach();

    // Turn LED off after detaching ticker
    digitalWrite(BUILTIN_LED, LOW);
}

void eraseAP()
{
    WiFi.disconnect(false, true);
    delay(3000);
}

#endif