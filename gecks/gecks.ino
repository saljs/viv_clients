#include <VivariumMonitor.h>
#include <PIDController.h>
#include <ESP8266WiFi.h>

// Common handler functions
#include <common_functions.h>

VivariumMonitor monitor;
PIDController heat_controller(35, 8.6, 0.5, 2.2, 0.8);

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
  WiFi.hostname(F("viv-monitor-gecks"));
  // Set output handlers
  monitor.setDigitalTwoHandler(twelve_hour_on);
  monitor.setAnalogHandler(analog_handler);

  // Initialize hardware
  monitor.init(config);
}

void loop() {
  monitor.handle_events();
}

/*
 * Handler for deep heat projector
 */
byte analog_handler(SensorData reading, time_t now)
{
  return heat_controller.add_reading(reading.high_temp, reading.timestamp);
}
