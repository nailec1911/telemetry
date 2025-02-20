/*
** TECHNICAL TEST, 2025
** telemetry
** File description:
** TelemetryLogger
*/

#pragma once

#include <string>
#include <vector>

enum class TelemetryType { DOUBLE, STRING };

class TelemetryLogger {
public:
    TelemetryLogger() = delete;
    TelemetryLogger(const TelemetryLogger &) = default;
    TelemetryLogger(TelemetryLogger &&) = delete;
    TelemetryLogger &operator=(const TelemetryLogger &) = default;
    TelemetryLogger &operator=(TelemetryLogger &&) = delete;
    TelemetryLogger(std::string recordName = "logs");
    ~TelemetryLogger();


    void declareSeries(const std::string& name, const std::string& unit, TelemetryType type);
    void saveToFile(const std::string &fileName);
    void to_stdout();

    void log(const std::string &serieName, double value);
    void logStatic(const std::string &serieName, double value);

    void clear();
    std::vector<std::string> getSeriesList();

protected:
private:
    std::string m_fileName;
    std::string m_recordName;
};
