#include <VivariumMonitor.h>
#include <PIDController.h>
#include <ESP8266WiFi.h>

VivariumMonitor monitor;
PIDController heat_controller(35.0, 14.6, 9.5, 3.2, 0.8);

void setup() {
  DEBUG_MSG("Vivarium Monitor firmware " FIRMWARE_VERSION);
  Url stats_url = {
    {.host = "stats.salskare.ml"},      
    {.path = "/"},
    .port = 5000,
    .set = true,
  };

  VivariumMonitorConfig config = {
    .has_sht_sensor = true,
    .num_therm_sensors = 2,
    .sample_interval = 10,
    .ntp_zone = "CST6CDT,M3.2.0,M11.1.0",
    .ntp_server = "pool.ntp.org",
    .stats_url = stats_url,
    .stats_interval = 600,
  };

  // Set hostname
  WiFi.hostname(F("viv-monitor-rufus"));
  // Set output handlers
  monitor.setDigitalOneHandler(digital_1_handler);
  monitor.setDigitalTwoHandler(digital_2_handler);
  monitor.setAnalogHandler(analog_handler);

  // Initialize hardware
  monitor.init(config);
}

void loop() {
  monitor.handle_events();
}

/*
 * Handler for UVB lamp:
 *   Turn on between 8am and 8pm
 */
byte digital_1_handler(SensorData reading)
{
  struct tm* timeinfo;
  timeinfo = localtime(&reading.timestamp);
  if (timeinfo->tm_hour > 7 && timeinfo->tm_hour < 20) {
    return 1;
  }
  return 0;
} 

/*
 * Handler for downstairs misting system
 *    Turn on for 6 seconds three times a day:
 *      10pm, 6am, and 2pm
 */
byte digital_2_handler(SensorData reading)
{
  struct tm* timeinfo;
  timeinfo = localtime(&reading.timestamp);
  if ( (timeinfo->tm_hour == 6 || timeinfo->tm_hour == 14 || timeinfo->tm_hour == 22)
      && timeinfo->tm_min == 0 && timeinfo->tm_sec < 6
  ) {
    return 1;
  }
  return 0;
}

/*
 * Handler for halogen lamp
 *    Apply PID controller between 8am and 8pm
 */
byte analog_handler(SensorData reading)
{
  struct tm* timeinfo;
  timeinfo = localtime(&reading.timestamp);
  if (timeinfo->tm_hour > 7 && timeinfo->tm_hour < 20) {
    return max((byte)5, heat_controller.add_reading(reading.high_temp));
  }
  return 0;
}
