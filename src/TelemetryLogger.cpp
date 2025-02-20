/*
** TECHNICAL TEST, 2025
** telemetry
** File description:
** TelemetryLogger
*/

#include "TelemetryLogger.hpp"

#include <cstdint>
#include <iostream>

TelemetryLogger::TelemetryLogger(std::string recordName)
    : m_recordName(std::move(recordName)),
      m_startTime(std::chrono::steady_clock::now()),
      m_readableStdout(false),
      m_toStdout(false),
      m_toFile(false)
{
}

TelemetryLogger::~TelemetryLogger() {}

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
    // TODO : save the serie declaration
}

void TelemetryLogger::saveToFile(const std::string &fileName)
{
    // print the buffer into the file when it's called
    // after, print info directly into the file
}

void TelemetryLogger::stopSaveToFile()
{
    m_toFile = false;
    //! TODO : close ofstream
}

void TelemetryLogger::saveToStdout(bool readable)
{
    m_toStdout = true;
    m_readableStdout = readable;
    // print the buffer to stdout
    //  after, print info directly into the file

    for (const auto &value : m_buffer) {
        if (m_readableStdout) {
            std::cout << value.serieName << " [" << value.timestamp << "]: ";
            switch (m_series[value.serieName].type) {
                case TelemetryType::DOUBLE:
                    std::cout << std::get<double>(value.value);
                    break;
                case TelemetryType::STRING:
                    std::cout << std::get<std::string>(value.value);
                    break;
            }
            std::cout << std::endl;
        }
    }
}

void TelemetryLogger::StopSaveToStdout()
{
    m_toStdout = false;
    m_readableStdout = false;
}

void TelemetryLogger::log(
    const std::string &serieName,
    const std::variant<double, std::string> &value)
{
    auto now = std::chrono::steady_clock::now();
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(now - m_startTime).count();
    writeBuffer({serieName, timestamp, value});
}

void TelemetryLogger::logStatic(
    const std::string &serieName,
    const std::variant<double, std::string> &value)
{
    writeBuffer({serieName, 0, value});
}

bool TelemetryLogger::checkValueType(
    const TelemetrySeries &serie, const TelemetryData &value)
{
    if ((serie.type == TelemetryType::DOUBLE &&
         !std::holds_alternative<double>(value.value)) ||
        (serie.type == TelemetryType::STRING &&
         !std::holds_alternative<std::string>(value.value))) {
        std::cerr << "Error: Series '" << value.serieName
                  << "' has an incompatible type for the logged value!\n";
        return false;
    }
    return true;
}

void TelemetryLogger::writeBuffer(TelemetryData value)
{
    auto it = m_series.find(value.serieName);
    if (it == m_series.end()) {
        std::cerr << "Error: Series '" << value.serieName
                  << "' not declared!\n";
        return;
    }
    if (!checkValueType(it->second, value))
        return;

    m_buffer.push_back(value);

    if (m_toStdout) {
        saveToStdout(m_readableStdout);
    }
    if (m_toFile) {
        saveToFile(m_recordName + ".bin");
    }
    if (m_toStdout || m_toFile)
        m_buffer.clear();
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
