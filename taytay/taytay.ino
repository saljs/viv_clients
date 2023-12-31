#include <VivariumMonitor.h>
#include <PIDController.h>
#include <ESP8266WiFi.h>

VivariumMonitor monitor;
PIDController heat_controller(27.0, 11.9, 8.5, 3.2, 0.6);

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
    .num_therm_sensors = 1,
    .ntp_zone = "CST6CDT,M3.2.0,M11.1.0",
    .ntp_server = "pool.ntp.org",
    .stats_url = stats_url,
    .stats_interval = 600,
  };
 
  // Set hostname
  WiFi.hostname(F("viv-monitor-taytay")); 
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
 * Handler for LEDs:
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
 * Handler for mister:
 *   Turn on for 16 seconds at 9am and 9pm
 */
byte digital_2_handler(SensorData reading)
{
  struct tm* timeinfo;
  timeinfo = localtime(&reading.timestamp);
  if ( (timeinfo->tm_hour == 9 || timeinfo->tm_hour == 21)
      && timeinfo->tm_min == 0 && timeinfo->tm_sec < 16
  ) {
    return 1;    
  }
  return 0;
} 

/* 
 *  Hander for Heat pad:
 *    Apply PID controller based on high temp
 */
byte analog_handler(SensorData reading)
{
  return heat_controller.add_reading(reading.high_temp);
}
