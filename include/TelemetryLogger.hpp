/*
** TECHNICAL TEST, 2025
** telemetry
** File description:
** TelemetryLogger
*/

#pragma once

#include <chrono>
#include <cstdint>
#include <fstream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

enum class TelemetryType
{
    DOUBLE,
    STRING
};

struct TelemetryData
{
    std::string serieName;
    uint64_t timestamp;
    std::variant<double, std::string> value;
};

struct TelemetrySeries
{
    uint16_t id;
    std::string unit;
    TelemetryType type;
    uint64_t startTime;
};

class TelemetryLogger
{
   public:
    TelemetryLogger() = delete;
    TelemetryLogger(const TelemetryLogger &) = delete;
    TelemetryLogger(TelemetryLogger &&) = delete;
    TelemetryLogger &operator=(const TelemetryLogger &) = delete;
    TelemetryLogger &operator=(TelemetryLogger &&) = delete;
    TelemetryLogger(std::string recordName = "logs.bin");
    ~TelemetryLogger();

    void declareSeries(
        const std::string &name, const std::string &unit, TelemetryType type);
    void saveToFile(const std::string &fileName, bool readable = false);
    void stopSaveToFile();
    void saveToStdout(bool readable = true);
    void StopSaveToStdout();

    void log(
        const std::string &serieName,
        const std::variant<double, std::string> &value);
    void logStatic(
        const std::string &serieName,
        const std::variant<double, std::string> &value);

    void clear();
    std::vector<std::string> getSeriesList();

   protected:
   private:
    uint16_t m_nextSeriesId = 0;
    std::string m_fileName;
    std::string m_recordName;
    bool m_readableStdout;
    bool m_readableFile;
    bool m_toStdout;
    bool m_toFile;
    std::unordered_map<std::string, TelemetrySeries> m_series;
    std::chrono::steady_clock::time_point m_startTime;
    std::vector<TelemetryData> m_buffer;
    std::ofstream m_fileStream;

    void writeInStdout(const TelemetryData &value);
    void writeInFile(const TelemetryData &value);
    void saveValue(TelemetryData value);
    bool checkValueType(
        const std::string &serieName,
        const std::variant<double, std::string> &value) const;
    std::string getReadableValue(const TelemetryData &value) const;
    static std::string getReadableSerieInfos(const std::string &name, const TelemetrySeries &serie);
};
