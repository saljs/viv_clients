/*
 * Common functions shared between multiple comtrollers
 */

#include "common_functions.h"


/*
 * Turns on an output for twelve hours between 8 am and 8 pm.
 */
byte twelve_hour_on(SensorData reading, time_t now)
{
  struct tm* timeinfo;
  timeinfo = localtime(&now);
  if (timeinfo->tm_hour > 7 && timeinfo->tm_hour < 20) {
    return 1;
  }
  return 0;
}

/*
 * Handler for downstairs misting system
 *    Turn on for 10 seconds three times a day:
 *      10pm, 6am, and 2pm
 */
byte activate_misting_system(SensorData reading, time_t now)
{
  struct tm* timeinfo;
  timeinfo = localtime(&now);
  if ( (timeinfo->tm_hour == 6 || timeinfo->tm_hour == 14 || timeinfo->tm_hour == 22)
      && timeinfo->tm_min == 0 && timeinfo->tm_sec < 10
  ) {
    DEBUG_MSG("MISTING SYSTEM ACTIVE: %d:%d\n", timeinfo->tm_hour, timeinfo->tm_sec);
    return 1;
  }
  return 0;
}
