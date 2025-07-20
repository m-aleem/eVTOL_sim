#include "logger.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>



Logger::Logger(const std::string& filename) {
    logFileName = filename;
    logFile.open(filename, std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Warning: Could not open log file " << filename << std::endl;
    }
}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void Logger::setLogFileName(const std::string& filename) {
    if (logFile.is_open()) {
        logFile.close();
    }
    logFileName = filename;
    logFile.open(logFileName, std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Warning: Could not open log file " << filename << std::endl;
    }
}

std::string Logger::getLogFileName() const {
    return logFileName;
}

std::string Logger::formatTableCell(const std::string& text, int width) {
    std::ostringstream oss;
    oss << std::setw(width) << text;
    return oss.str();
}


void Logger::log(const std::string& message, bool includeTimestamp) {
    std::string output = message;

    if (includeTimestamp) {
        output = "[" + getCurrentTimestamp() + "] " + message;
    }

    std::cout << output;
    if (logFile.is_open()) {
        logFile << output;
        logFile.flush();
    }
}

void Logger::logLine(const std::string& message, bool includeTimestamp) {
    log(message + "\n", includeTimestamp);
}

void Logger::logSectionDivider(const std::string& message, bool includeTimestamp) {
    std::string divider = std::string(110, '=');
    logLine(divider, includeTimestamp);
    if (!message.empty()) {
        logLine(message, includeTimestamp);
    }
    logLine(divider, includeTimestamp);
}

void Logger::logSubSectionDivider(const std::string& message, bool includeTimestamp) {
    std::string divider = std::string(60, '=');
    logLine(divider, includeTimestamp);
    if (!message.empty()) {
        logLine(message, includeTimestamp);
    }
    logLine(divider, includeTimestamp);
}