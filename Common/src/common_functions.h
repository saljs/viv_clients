/*
 * Common functions shared between multiple controllers
 */

#include <VivariumMonitor.h>

/*
 * Turns on an output for twelve hours between 8 am and 8 pm.
 */
byte twelve_hour_on(SensorData reading, time_t now);

/*
 * Handler for downstairs misting system
 *    Turn on for 10 seconds three times a day:
 *      10pm, 6am, and 2pm
 */
byte activate_misting_system(SensorData reading, time_t now);
