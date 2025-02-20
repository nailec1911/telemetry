/*
** TECHNICAL TEST, 2025
** telemetry
** File description:
** TelemetryLogger
*/

#pragma once

#include <chrono>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <variant>

enum class TelemetryType { DOUBLE, STRING };

struct TelemetryData {
    std::string serieName;
    uint64_t timestamp;
    std::variant<double, std::string> value;
};

struct TelemetrySeries {
    std::string unit;
    TelemetryType type;
    uint64_t startTime;
};

class TelemetryLogger {
public:
    TelemetryLogger() = delete;
    TelemetryLogger(const TelemetryLogger &) = delete;
    TelemetryLogger(TelemetryLogger &&) = delete;
    TelemetryLogger &operator=(const TelemetryLogger &) = delete;
    TelemetryLogger &operator=(TelemetryLogger &&) = delete;
    TelemetryLogger(std::string recordName = "logs.bin");
    ~TelemetryLogger();


    void declareSeries(const std::string& name, const std::string& unit, TelemetryType type);
    void saveToFile(const std::string &fileName);
    void stopSaveToFile();
    void saveToStdout(bool readable = true);
    void StopSaveToStdout();

    void log(const std::string &serieName, const std::variant<double, std::string> &value);
    void logStatic(const std::string &serieName, const std::variant<double, std::string> &value);

    void clear();
    std::vector<std::string> getSeriesList();

protected:
private:
    std::string m_fileName;
    std::string m_recordName;
    bool m_readableStdout;
    bool m_toStdout;
    bool m_toFile;
    std::unordered_map<std::string, TelemetrySeries> m_series;
    std::chrono::steady_clock::time_point m_startTime;
    std::vector<TelemetryData> m_buffer;
    std::ofstream m_outFile;

    void writeBuffer(TelemetryData value);
    static bool checkValueType(const TelemetrySeries &serie, const TelemetryData &value);
};
