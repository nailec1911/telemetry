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
    logger.saveToFile("toto.log");

    std::cout << "Current series:" << std::endl;
    auto names = logger.getSeriesList();
    for (auto &name : names) {
        std::cout << name << std::endl;
    }

    logger.logStatic("battery_level", 0.);
    logger.log("battery_level", 29.5);
    logger.log("battery_level", 28.9);
    logger.log("status", "OK");
    logger.log("status", "WARNING");

    logger.saveToFile("telemetry.rrd");
}
