/*
** TECHNICAL TEST, 2025
** telemetry
** File description:
** TelemetryLogger
*/

#include "TelemetryLogger.hpp"

TelemetryLogger::TelemetryLogger(std::string recordName)
    : m_recordName(std::move(recordName))
{
    // start the timer
}

TelemetryLogger::~TelemetryLogger() {}

void TelemetryLogger::declareSeries(const std::string& name, const std::string& unit, TelemetryType type)
{
    // create a serie and assign it a unti and type
}

void TelemetryLogger::saveToFile(const std::string &fileName)
{
    // print the buffer into the file when it's called
    // after, print info directly into the file
}

void TelemetryLogger::to_stdout()
{
    //print the buffer to stdout
    // after, print info directly into the file
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

void clear()
{
    // clear all the series
}

std::vector<std::string> getSeriesList()
{
    // return the list of series name
    return {};
}
