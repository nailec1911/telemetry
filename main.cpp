/*
** TECHNICAL TEST, 2025
** telemetry
** File description:
** main
*/

#include <unistd.h>
#include <cmath>
#include "include/TelemetryLogger.hpp"

int main() {
    TelemetryLogger logger("random_values");

    logger.saveToFile("sine.log", false);
    logger.declareSeries("sine_wave", "rad", TelemetryType::DOUBLE);
    logger.declareSeries("battery_state", "V", TelemetryType::DOUBLE);
    logger.declareSeries("logs", "", TelemetryType::STRING);
    srand(time(0));

    logger.log("logs", "start logging");
    logger.log("battery_state", 28.5);

    const double frequency = 2 * M_PI / 30;
    for (int i = 0; i < 30; ++i) {
        double value = sin(i * frequency) * 10;
        usleep(rand() % 500);
        logger.log("sine_wave", value);
    }
    logger.log("battery_state", 28.0);
    logger.log("logs", "sine_wave done");

    logger.declareSeries("random_noise", "dB", TelemetryType::DOUBLE);
    for (int i = 0; i < 1000; ++i) {
        double noiseValue = (rand() % 100) / 10.0 - 5.0;
        logger.log("random_noise", noiseValue);
    }
    logger.log("logs", "random_noise done");
    logger.log("battery_state", 28.3);
}
