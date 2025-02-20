/*
** TECHNICAL TEST, 2025
** telemetry
** File description:
** TelemetryLogger
*/

#include "TelemetryLogger.hpp"

#include <cstdint>
#include <iostream>
#include <string>

TelemetryLogger::TelemetryLogger(std::string recordName)
    : m_recordName(std::move(recordName)),
      m_startTime(std::chrono::steady_clock::now()),
      m_readableStdout(false),
      m_readableFile(false),
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

    uint16_t id = m_nextSeriesId++;

    m_series[name] = {id, unit, type, seriesStartTime};

    std::string valLog = getReadableSerieInfos(name, m_series[name]);

    if (m_toStdout) {
        if (m_readableStdout)
            std::cout << valLog << std::endl;
    }
    if (m_toFile) {
        if (m_readableFile)
            m_fileStream << valLog << std::endl;
    }
}

std::string TelemetryLogger::getReadableSerieInfos(const std::string &name, const TelemetrySeries &serie)
{
    return "Series '" + name + "' (ID: " + std::to_string(serie.id) + ", Unit: " + serie.unit  + ", Start time: " + std::to_string(serie.startTime) + ")";
}

void TelemetryLogger::saveToFile(const std::string &fileName, bool readable)
{
    if (m_toFile) {
        stopSaveToFile();
    }
    m_readableFile = readable;
    m_toFile = true;
    m_fileName = fileName;

    if (m_readableFile)
        m_fileStream.open(m_fileName, std::ios::binary | std::ios::trunc);
    else {
        m_fileStream.open(m_fileName, std::ios::trunc);
    }
    if (!m_fileStream) {
        std::cerr << "Error: Could not open file " << m_fileName
                  << " for writing!\n";
        m_toFile = false;
        return;
    }
    for (const auto &serie : m_series) {
        m_fileStream << getReadableSerieInfos(serie.first, serie.second) << std::endl;
    }
    for (const auto &value : m_buffer) {
        writeInFile(value);
    }
    m_buffer.clear();
}

void TelemetryLogger::stopSaveToFile()
{
    m_toFile = false;
    m_fileStream.close();
    m_fileName.clear();
}

void TelemetryLogger::saveToStdout(bool readable)
{
    m_toStdout = true;
    m_readableStdout = readable;

    for (const auto &serie : m_series) {
        std::cout << getReadableSerieInfos(serie.first, serie.second) << std::endl;
    }
    for (const auto &value : m_buffer) {
        writeInStdout(value);
    }
    m_buffer.clear();
}

void TelemetryLogger::StopSaveToStdout()
{
    m_toStdout = false;
    m_readableStdout = false;
}

bool TelemetryLogger::checkValueType(
    const std::string &serieName,
    const std::variant<double, std::string> &value) const
{
    auto it = m_series.find(serieName);

    if (it == m_series.end()) {
        std::cerr << "Error: Series '" << serieName << "' not declared!\n";
        return false;
    }
    if ((it->second.type == TelemetryType::DOUBLE &&
         !std::holds_alternative<double>(value)) ||
        (it->second.type == TelemetryType::STRING &&
         !std::holds_alternative<std::string>(value))) {
        std::cerr << "Error: Series '" << serieName
                  << "' has an incompatible type for the logged value!\n";
        return false;
    }
    return true;
}

void TelemetryLogger::log(
    const std::string &serieName,
    const std::variant<double, std::string> &value)
{
    if (!checkValueType(serieName, value))
        return;

    auto now = std::chrono::steady_clock::now();
    uint64_t timestamp =
        std::chrono::duration_cast<std::chrono::nanoseconds>(now - m_startTime)
            .count();
    saveValue({serieName, timestamp, value});
}

void TelemetryLogger::logStatic(
    const std::string &serieName,
    const std::variant<double, std::string> &value)
{
    if (!checkValueType(serieName, value))
        return;

    saveValue({serieName, 0, value});
}

void TelemetryLogger::saveValue(TelemetryData value)
{
    if (m_toStdout) {
        writeInStdout(value);
    }
    if (m_toFile) {
        writeInFile(value);
    }
    if (!m_toStdout && !m_toFile)
        m_buffer.push_back(value);
}

void TelemetryLogger::writeInStdout(const TelemetryData &value)
{
    if (m_readableStdout) {
        std::cout << getReadableValue(value) << std::endl;
    }
}

void TelemetryLogger::writeInFile(const TelemetryData &value)
{
    if (m_readableFile) {
        m_fileStream << getReadableValue(value) << std::endl;
    }
}

std::string TelemetryLogger::getReadableValue(const TelemetryData &value) const
{
    std::ostringstream oss;
    oss << value.serieName << " [" << value.timestamp << "]: ";

    switch (m_series.at(value.serieName).type) {
        case TelemetryType::DOUBLE:
            oss << std::to_string(std::get<double>(value.value));
            break;
        case TelemetryType::STRING:
            oss << std::get<std::string>(value.value);
            break;
        default:
            oss << "Unknown Type";
            break;
    }

    return oss.str();
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
