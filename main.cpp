/*
** TECHNICAL TEST, 2025
** telemetry
** File description:
** main
*/

#include <iostream>
#include "include/TelemetryLogger.hpp"

int main() {
    TelemetryLogger logger("telemetry_logs");

    logger.declareSeries("battery_level", "V", TelemetryType::DOUBLE);
    logger.declareSeries("status", "", TelemetryType::STRING);

    logger.log("battery_level", 29.5);
    logger.log("battery_level", 28.9);
    // logger.log("status", "OK");
    // logger.log("status", "WARNING");

    logger.saveToFile("telemetry.rrd");

}
