#include <M5AtomS3.h>
#include <WiFi.h>
#include "HeosControl.h"

#define GPIO_BUTTON 41

// Please modify 'ssid', 'password', 'heosdevice' and 'g_cmd'
const char* ssid     = "SSID";
const char* password = "12345678";
const IPAddress heosdevice(192,168,1,40);
auto g_cmd = HeosControl::COMMAND::PlayInputSource;

HeosControl hc;

volatile bool g_irq0 = false;
void setIRQ0(){
  g_irq0 = true;
}

void setup() {
  M5.begin(false, true, false, true); // Enable Serial and RGB LED
  M5.dis.clear();
  M5.dis.show();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  USBSerial.print("WiFi connected\r\n");

  if(!hc.Connect(heosdevice)){
    USBSerial.print("HEOS Connection failed\r\n");
  }

  pinMode(GPIO_BUTTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(GPIO_BUTTON), setIRQ0, FALLING);
}

void loop() {
  if(!g_irq0){
    return;
  }
  
  M5.dis.drawpix(CRGB::Aqua);
  M5.dis.show();

  switch(g_cmd){
    case HeosControl::COMMAND::SetVolume:       hc.SetVolume(20); break;
    case HeosControl::COMMAND::VolumeUp:        hc.VolumeUp(5); break;
    case HeosControl::COMMAND::VolumeDown:      hc.VolumeDown(5); break;
    case HeosControl::COMMAND::SetMute:         hc.SetMute(true); break;
    case HeosControl::COMMAND::ToggleMute:      hc.ToggleMute(); break;
    case HeosControl::COMMAND::PlayInputSource: hc.PlayInputSource(HeosControl::INPUT_SOURCE::OPTICAL_IN_1); break;
    default: break;
  }

  g_irq0 = false;
  M5.dis.clear();
  M5.dis.show();
}
