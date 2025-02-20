/*
** TECHNICAL TEST, 2025
** telemetry
** File description:
** TelemetryLogger
*/

#include "TelemetryLogger.hpp"

#include <iostream>

TelemetryLogger::TelemetryLogger(std::string recordName)
    : m_recordName(std::move(recordName)),
      m_startTime(std::chrono::steady_clock::now())
{
}

TelemetryLogger::~TelemetryLogger() {}

uint64_t getCurrentTimestamp()
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
               std::chrono::steady_clock::now().time_since_epoch())
        .count();
}

void TelemetryLogger::declareSeries(
    const std::string &name, const std::string &unit, TelemetryType type)
{
    if (m_series.find(name) != m_series.end()) {
        std::cerr << "Error: Series '" << name << "' already declared!\n";
        return;
    }
    uint64_t seriesStartTime =
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now() - m_startTime)
            .count();

    m_series[name] = {unit, type, seriesStartTime};
}

void TelemetryLogger::saveToFile(const std::string &fileName)
{
    // print the buffer into the file when it's called
    // after, print info directly into the file
}

void TelemetryLogger::to_stdout()
{
    // print the buffer to stdout
    //  after, print info directly into the file
}

void TelemetryLogger::log(const std::string &serieName, double value)
{
    // log a value into the serie
    // error if the serie doesn't exist or type is incorrect
}

void TelemetryLogger::logStatic(const std::string &serieName, double value)
{
    // same as log but no timestamp
}

void TelemetryLogger::clear()
{
    m_series.clear();
}

std::vector<std::string> TelemetryLogger::getSeriesList()
{
    std::vector<std::string> names;
    names.reserve(m_series.size());

    for (const auto &entry : m_series) {
        names.push_back(entry.first);
    }
    return names;
}
