/**
 * @file logger.hpp
 * @brief Implementation file for the Logger class
 *
 * See logger.hpp for more information.
 */

#include "logger.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>

Logger::Logger(const std::string& filename, LogMode mode)
    : currentMode(mode), includeTimestampInFile(true) {
    logFileName = filename;

    // Only open file if we need to log to file
    if (currentMode == LogMode::FILE_ONLY || currentMode == LogMode::BOTH) {
        if (!filename.empty()) {
            logFile.open(filename, std::ios::app);
            if (!logFile.is_open()) {
                std::cerr << "Warning: Could not open log file " << filename << std::endl;
            }
        }
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

    // Only open file if we need to log to file
    if ((currentMode == LogMode::FILE_ONLY || currentMode == LogMode::BOTH) && !filename.empty()) {
        logFile.open(logFileName, std::ios::app);
        if (!logFile.is_open()) {
            std::cerr << "Warning: Could not open log file " << filename << std::endl;
        }
    }
}

std::string Logger::getLogFileName() const {
    return logFileName;
}

void Logger::setLogMode(LogMode mode) {
    LogMode oldMode = currentMode;
    currentMode = mode;

    // Handle file opening/closing based on mode change
    if (oldMode != currentMode) {
        if ((oldMode == LogMode::FILE_ONLY || oldMode == LogMode::BOTH) &&
            currentMode == LogMode::STDOUT_ONLY) {
            // We were logging to file, now we're not
            if (logFile.is_open()) {
                logFile.close();
            }
        } else if (oldMode == LogMode::STDOUT_ONLY &&
                   (currentMode == LogMode::FILE_ONLY || currentMode == LogMode::BOTH)) {
            // We weren't logging to file, now we are
            if (!logFileName.empty()) {
                logFile.open(logFileName, std::ios::app);
                if (!logFile.is_open()) {
                    std::cerr << "Warning: Could not open log file " << logFileName << std::endl;
                }
            }
        }
    }
}

Logger::LogMode Logger::getLogMode() const {
    return currentMode;
}

void Logger::setIncludeTimestampInFile(bool enable) {
    includeTimestampInFile = enable;
}

bool Logger::getIncludeTimestampInFile() const {
    return includeTimestampInFile;
}

std::string Logger::formatTableCell(const std::string& text, int width) {
    std::ostringstream oss;
    oss << std::setw(width) << text;
    return oss.str();
}

void Logger::log(const std::string& message, bool includeTimestamp) {
    // Prepare console output
    std::string consoleOutput = message;
    if (includeTimestamp) {
        consoleOutput = "[" + getCurrentTimestamp() + "] " + message;
    }

    // Prepare file output (may differ from console output)
    std::string fileOutput = message;
    if (includeTimestamp && includeTimestampInFile) {
        fileOutput = "[" + getCurrentTimestamp() + "] " + message;
    }

    // Output to stdout if mode allows it
    if (currentMode == LogMode::STDOUT_ONLY || currentMode == LogMode::BOTH) {
        std::cout << consoleOutput;
    }

    // Output to file if mode allows it and file is open
    if ((currentMode == LogMode::FILE_ONLY || currentMode == LogMode::BOTH) && logFile.is_open()) {
        logFile << fileOutput;
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