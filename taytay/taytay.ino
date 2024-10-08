#include <VivariumMonitor.h>
#include <PIDController.h>
#include <ESP8266WiFi.h>

// Common handler functions
#include <common_functions.h>

VivariumMonitor monitor;
PIDController heat_controller(27.0, 8.5, 0.5, 0, 0.6);

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
    .sample_interval = 10,
    .ntp_zone = "CST6CDT,M3.2.0,M11.1.0",
    .ntp_server = "pool.ntp.org",
    .stats_url = stats_url,
    .stats_interval = 300,
  };
 
  // Set hostname
  WiFi.hostname(F("viv-monitor-taytay")); 
  // Set output handlers
  monitor.setDigitalOneHandler(twelve_hour_on);
  monitor.setDigitalTwoHandler(digital_2_handler);
  monitor.setAnalogHandler(analog_handler);

  // Initialize hardware
  monitor.init(config);
}

void loop() {
  monitor.handle_events();
}

/*
 * Handler for mister:
 *   Turn on for 4 seconds at 9am and 9pm
 */
byte digital_2_handler(SensorData reading, time_t now)
{
  struct tm* timeinfo;
  timeinfo = localtime(&now);
  if ( (timeinfo->tm_hour == 9 || timeinfo->tm_hour == 21)
      && timeinfo->tm_min == 0 && timeinfo->tm_sec < 4
  ) {
    return 1;    
  }
  return 0;
} 

/* 
 *  Hander for Heat pad:
 *    Apply PID controller based on high temp
 */
byte analog_handler(SensorData reading, time_t now)
{
  return heat_controller.add_reading(reading.high_temp, reading.timestamp);
}
