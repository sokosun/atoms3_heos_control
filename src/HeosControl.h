// HeosControl class communicates with and controls HEOS CLI supported devices.
// Please find HEOS CLI Protocol Specification for details.
//
// Usage:
// 1. Create an Instance
//   HeosControl hc;
// 2. Connect to the HEOS device
//   const IPAddress heosdevice(192,168,1,40);
//   hc.Connect(heosdevice);
// 3. Call APIs
//   hc.SetVolume(20);
//   hc.VolumeUp();
//   hc.ToggleMute();
//   ...
//
// Notes:
// * Connect() and HEOS Commands get stuck if HEOS device doesn't respond.
// * Connect() and HEOS Commands spend much time because of blocking communication

#include <WiFi.h>
#include <functional>
#include <ArduinoJson.h>

class HeosControl {
public:
  enum class COMMAND {
    GetPlayers,
    SetVolume,
    VolumeUp,
    VolumeDown,
    SetMute,
    ToggleMute,
    PlayInputSource,
    Invalid
  };

  enum class INPUT_SOURCE {
    ANALOG_IN_1,
    ANALOG_IN_2,
    USBDAC,
    OPTICAL_IN_1,
    OPTICAL_IN_2,
    COAX_IN_1,
    COAX_IN_2,
    Invalid
  };

  HeosControl();
  ~HeosControl();

  /// @return false if failed.
  bool Connect(const IPAddress heosdevice);

//----- HEOS Commands -----//
  // Any HEOS commands return true if succeeded, false if not.

  /// @param response_callback is a callback called with response 
  bool GetPlayers(std::function<bool(DynamicJsonDocument)> response_callback);

  /// @param level of volume. (0 to 100)
  bool SetVolume(unsigned int level);

  /// @param step level of volume. (1 to 10)
  bool VolumeUp(unsigned int step = 5);

  /// @param step level of volume. (1 to 10)
  bool VolumeDown(unsigned int step = 5);

  /// @param state to be set. Unmute if false, mute if true.
  bool SetMute(bool state = true);

  bool ToggleMute();

  bool PlayInputSource(INPUT_SOURCE input);

private:
  /// CommandRoutine sends 'uri' to HEOS device and wait for its response. It spends much time because of blocking communication.
  /// @param cmd COMMAND type to be sent (e.g. COMMAND::SetVolume)
  /// @param uri whole string to be sent (e.g. "heos://player/set_volume?pid=12345678&level=20\r\n")
  /// @param response_callback is a callback called with response. Please return true if succeeded in the callback. 
  /// @return true if succeeded. false if not.
  bool CommandRoutine(COMMAND cmd, String uri, std::function<bool(DynamicJsonDocument)> response_callback = nullptr);

  /// WaitJsonResponse could get stuck because it does not check timeout.
  /// @return JSON formatted string if succeeded. Return empty String if failed.
  String WaitJsonResponse();

  const int heosport = 1255;
  WiFiClient m_self;
  long m_pid = 0;
};

