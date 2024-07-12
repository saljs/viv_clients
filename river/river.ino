#include <VivariumMonitor.h>
#include <PIDController.h>
#include <ESP8266WiFi.h>

VivariumMonitor monitor;
PIDController heat_controller(33.0, 8.6, 3.5, 2.2, 0.8);

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
    .stats_interval = 300,
  };

  // Set hostname
  WiFi.hostname(F("viv-monitor-river"));
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
byte digital_1_handler(SensorData reading, time_t now)
{
  struct tm* timeinfo;
  timeinfo = localtime(&now);
  if (timeinfo->tm_hour > 7 && timeinfo->tm_hour < 20) {
    return 1;
  }
  return 0;
} 

/*
 * Handler for halogen lamp:
 *   Turn on between 8:30am and 7:30pm
 */
byte digital_2_handler(SensorData reading, time_t now)
{
  struct tm* timeinfo;
  timeinfo = localtime(&now);
  if ( (timeinfo->tm_hour > 8 || (timeinfo->tm_hour  == 8 && timeinfo->tm_min >= 30)) 
      && (timeinfo->tm_hour < 19 || (timeinfo->tm_hour == 19 && timeinfo->tm_min < 30))
  ) {
    return 1;    
  }
  return 0;
} 

/* 
 *  Hander for CHE:
 *    Apply PID controller based on high temp
 */
byte analog_handler(SensorData reading, time_t now)
{
  return heat_controller.add_reading(reading.high_temp, reading.timestamp);
}
