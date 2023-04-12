
#include <unordered_map>
#include "HeosControl.h"

namespace {
  const std::unordered_map<HeosControl::COMMAND, String> COMMAND_LIST = {
    { HeosControl::COMMAND::GetPlayers,        String("player/get_players") },
    { HeosControl::COMMAND::SetVolume,         String("player/set_volume")  },
    { HeosControl::COMMAND::VolumeUp,          String("player/volume_up")   },
    { HeosControl::COMMAND::VolumeDown,        String("player/volume_down") },
    { HeosControl::COMMAND::SetMute,           String("player/set_mute")    },
    { HeosControl::COMMAND::ToggleMute,        String("player/toggle_mute") },
    { HeosControl::COMMAND::PlayInputSource,   String("player/play_input")  }
  };

  const std::unordered_map<HeosControl::INPUT_SOURCE, String> INPUT_SOURCE_LIST = {
    { HeosControl::INPUT_SOURCE::ANALOG_IN_1,  String("inputs/analog_in_1")  },
    { HeosControl::INPUT_SOURCE::ANALOG_IN_2,  String("inputs/analog_in_2")  },
    { HeosControl::INPUT_SOURCE::USBDAC,       String("inputs/usbdac")       },
    { HeosControl::INPUT_SOURCE::OPTICAL_IN_1, String("inputs/optical_in_1") },
    { HeosControl::INPUT_SOURCE::OPTICAL_IN_2, String("inputs/optical_in_2") },
    { HeosControl::INPUT_SOURCE::COAX_IN_1,    String("inputs/coax_in_1")    },
    { HeosControl::INPUT_SOURCE::COAX_IN_2,    String("inputs/coax_in_2")    }
  };

  String GetCommandName(HeosControl::COMMAND cmd){
    return COMMAND_LIST.count(cmd) > 0 ? COMMAND_LIST.at(cmd) : String();
  }

  String GetInputSourceName(HeosControl::INPUT_SOURCE input){
    return INPUT_SOURCE_LIST.count(input) > 0 ? INPUT_SOURCE_LIST.at(input) : String();
  }
}

HeosControl::HeosControl(){
}

HeosControl::~HeosControl(){
}


bool HeosControl::Connect(const IPAddress heosdevice){
  if(m_self.connected()){
    m_self.stop();
  }

// FYI: WiFiClient::connect sometimes fail. Please wait 30 sec. and retry.
  m_self.connect(heosdevice, heosport);
  delay(100);
  if(!m_self.connected()){
    USBSerial.print("Cannot connect to HEOS device\r\n");
    return false;
  }

  auto response_callback = [this](DynamicJsonDocument doc){
    m_pid = doc["payload"][0]["pid"];
    return true;
  };

  if(!GetPlayers(response_callback)){
    USBSerial.print("Cannot get player_id\r\n");
    return false;
  }

  return true;
}


String HeosControl::WaitJsonResponse(){
  String response;

// FYI: Delimiter of HEOS CLI protocol is "\r\n" 
  while(1){
    if (m_self.available()) {
      char c = m_self.read();
      response += c;

      if(c == '\n'){
        return response;
      }
    }
  }
}


bool HeosControl::CommandRoutine(
  HeosControl::COMMAND cmd,
  String uri,
  std::function<bool(DynamicJsonDocument)> response_callback
) {
  if(!m_self.connected()){
    USBSerial.print("Missing HEOS device\r\n");
    return false;
  }

  USBSerial.print("Commmand: ");
  USBSerial.print(uri);

  m_self.print(uri);

  auto response = WaitJsonResponse();
  if(response.isEmpty()){
    USBSerial.print("Missing response\r\n");
    return false;
  }

  USBSerial.print("Response: ");
  USBSerial.print(response);
  
  DynamicJsonDocument doc(512);
  DeserializationError error = deserializeJson(doc, response.c_str());
  if(error){
    USBSerial.print("Invalid response: ");
    USBSerial.println(error.c_str());
    return false;
  }

  String response_heos_command = doc["heos"]["command"];
  String response_heos_result  = doc["heos"]["result"];
  
  if(GetCommandName(cmd) != response_heos_command){
    USBSerial.print("Command mismatch\r\n");
    return false;
  }

  if(response_heos_result != "success"){
    USBSerial.print("Command failure\r\n");
    return false;
  }

  if(response_callback){
    return response_callback(doc);
  }

  return true;
}

//----- HEOS Commands -----//

bool HeosControl::GetPlayers(std::function<bool(DynamicJsonDocument)> response_callback){
  const auto uri = String("heos://") + GetCommandName(COMMAND::GetPlayers) + String("\r\n");
  return CommandRoutine(COMMAND::GetPlayers, uri, response_callback);
}

bool HeosControl::SetVolume(unsigned int level){
  if(level > 100){
    return false;
  }
  const auto uri = String("heos://") + GetCommandName(COMMAND::SetVolume) + String("?pid=") + String(m_pid) + String("&level=") + String(level) + String("\r\n");
  return CommandRoutine(COMMAND::SetVolume, uri);
}

bool HeosControl::VolumeUp(unsigned int step){
  if(step == 0 || step > 10){
    return false;
  }
  const auto uri = String("heos://") + GetCommandName(COMMAND::VolumeUp) + String("?pid=") + String(m_pid) + String("&step=") + String(step) + String("\r\n");
  return CommandRoutine(COMMAND::VolumeUp, uri);
}

bool HeosControl::VolumeDown(unsigned int step){
  if(step == 0 || step > 10){
    return false;
  }
  const auto uri = String("heos://") + GetCommandName(COMMAND::VolumeDown) + String("?pid=") + String(m_pid) + String("&step=") + String(step) + String("\r\n");
  return CommandRoutine(COMMAND::VolumeDown, uri);
}

bool HeosControl::SetMute(bool state){
  const auto uri = String("heos://") + GetCommandName(COMMAND::SetMute) + String("?pid=") + String(m_pid) + (state ? String("&state=on\r\n") : String("&state=off\r\n"));
  return CommandRoutine(COMMAND::SetMute, uri);
}

bool HeosControl::ToggleMute(){
  const auto uri = String("heos://") + GetCommandName(COMMAND::ToggleMute) + String("?pid=") + String(m_pid) + String("\r\n");
  return CommandRoutine(COMMAND::ToggleMute, uri);
}

bool HeosControl::PlayInputSource(INPUT_SOURCE input){
  const auto uri = String("heos://") + GetCommandName(COMMAND::PlayInputSource) + String("?pid=") + String(m_pid) + String("&input=") + GetInputSourceName(input) + String("\r\n");
  return CommandRoutine(COMMAND::PlayInputSource, uri);
}


