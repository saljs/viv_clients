#include <VivariumMonitor.h>
#include <ESP8266WiFi.h>

VivariumMonitor monitor;

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
    .ntp_zone = "CST6CDT,M3.2.0,M11.1.0",
    .ntp_server = "pool.ntp.org",
    .stats_url = stats_url,
    .stats_interval = 600,
  };
 
  // Set hostname
  WiFi.hostname(F("viv-monitor-lite")); 

  // Initialize hardware
  monitor.init(config);
}

void loop() {
  monitor.handle_events();
}
