/*
** TECHNICAL TEST, 2025
** telemetry
** File description:
** TelemetryLogger
*/

#include "TelemetryLogger.hpp"

#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

TelemetryLogger::TelemetryLogger(std::string recordName)
    : m_recordName(std::move(recordName)),
      m_startTime(std::chrono::steady_clock::now()),
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
        std::cout << valLog << std::endl;
    }
    if (m_toFile) {
        if (m_readableFile)
            m_fileStream << valLog << std::endl;
        else {
            auto packed = getSerieAsBytes(name, m_series[name]);
            m_fileStream.write(
                reinterpret_cast<const char *>(packed.data()),   // NOLINT
                static_cast<long>(packed.size()));
        }
    }
}

std::string TelemetryLogger::getReadableSerieInfos(
    const std::string &name, const TelemetrySeries &serie)
{
    return "Series '" + name + "' (ID: " + std::to_string(serie.id) +
           ", Unit: " + serie.unit +
           ", Start time: " + std::to_string(serie.startTime) + ")";
}

std::vector<uint8_t> TelemetryLogger::getSerieAsBytes(
    const std::string &name, const TelemetrySeries &serie)
{
    std::vector<uint8_t> packedData;
    appendToVector(packedData, 0); // magic number to identify this is a series declaration
    appendToVector(packedData, static_cast<uint16_t>(name.size()));
    for (char c : name) {
        packedData.push_back(static_cast<uint8_t>(c));
    }

    appendToVector(packedData, serie.id);
    appendToVector(packedData, serie.startTime);

    std::string unit = serie.unit.substr(0, 16);
    for (size_t i = 0; i < 16; ++i) {
        if (i < unit.size()) {
            packedData.push_back(unit[i]);
        } else {
            packedData.push_back(0);
        }
    }
    return packedData;
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
        m_fileStream << getReadableSerieInfos(serie.first, serie.second)
                     << std::endl;
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

void TelemetryLogger::saveToStdout()
{
    m_toStdout = true;

    for (const auto &serie : m_series) {
        std::cout << getReadableSerieInfos(serie.first, serie.second)
                  << std::endl;
    }
    for (const auto &value : m_buffer) {
        writeInStdout(value);
    }
    m_buffer.clear();
}

void TelemetryLogger::StopSaveToStdout()
{
    m_toStdout = false;
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
    std::cout << getReadableValue(value) << std::endl;
}

void TelemetryLogger::writeInFile(const TelemetryData &value)
{
    if (m_readableFile) {
        m_fileStream << getReadableValue(value) << std::endl;
    } else {
        auto packed = getValueAsBytes(value);
        m_fileStream.write(
            reinterpret_cast<const char *>(packed.data()),   // NOLINT
            static_cast<long>(packed.size()));
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

std::vector<uint8_t> TelemetryLogger::getValueAsBytes(
    const TelemetryData &value)
{
    std::vector<uint8_t> packedData;

    auto it = m_series.find(value.serieName);

    appendToVector(packedData, it->second.id);
    appendToVector(packedData, value.timestamp);

    switch (it->second.type) {
        case TelemetryType::DOUBLE: {
            uint64_t doubleAsBits = 0;
            std::memcpy(
                &doubleAsBits, &std::get<double>(value.value), sizeof(double));
            appendToVector(packedData, doubleAsBits);
            break;
        }
        case TelemetryType::STRING: {
            std::string strValue = std::get<std::string>(value.value);
            uint32_t strLength = strValue.length();
            appendToVector(packedData, strLength);

            for (char c : strValue) {
                packedData.push_back(static_cast<uint8_t>(c));
            }
            break;
        }
    }

    return packedData;
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
