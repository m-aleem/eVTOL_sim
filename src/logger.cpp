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

Logger::Logger(const std::string& filename, LogMode mode, int verbosity)
    : currentMode(mode), includeTimestampInFile(true), verbosityLevel(verbosity) {
    logFileName = filename;

    // Only open file if we need to log to file
    if (currentMode == LogMode::FILE_ONLY || currentMode == LogMode::BOTH) {
        if (!filename.empty()) {
            openLogFile();
        }
    }
}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void Logger::openLogFile() {
    logFile.open(logFileName, std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Warning: Could not open log file " << logFileName << std::endl;
    }
}

void Logger::setLogFile(const std::string& filename) {
    if (logFile.is_open()) {
        logFile.close();
    }
    logFileName = filename;

    // Only open file if we need to log to file
    if ((currentMode == LogMode::FILE_ONLY || currentMode == LogMode::BOTH) && !filename.empty()) {
        openLogFile();
    }
}

std::string Logger::getLogFile() const {
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
                openLogFile();
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

void Logger::setVerbosityLevel(int level) {
    verbosityLevel = level;
}

int Logger::getVerbosityLevel() const {
    return verbosityLevel;
}

std::string Logger::formatFixedWidth(const std::string& text, int width, bool rightAlign) {
    std::ostringstream oss;
    if (rightAlign) {
        oss << std::right << std::setw(width) << text;
    } else {
        oss << std::left << std::setw(width) << text;
    }
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

void Logger::log(int verbosity, const std::string& message, bool includeTimestamp) {
    if (verbosity <= verbosityLevel) {
        log(message, includeTimestamp);
    }
}

void Logger::logLine(int verbosity, const std::string& message, bool includeTimestamp) {
    if (verbosity <= verbosityLevel) {
        logLine(message, includeTimestamp);
    }
}

void Logger::logSectionDivider(int verbosity, const std::string& message, bool includeTimestamp) {
    if (verbosity <= verbosityLevel) {
        logSectionDivider(message, includeTimestamp);
    }
}

void Logger::logSubSectionDivider(int verbosity, const std::string& message, bool includeTimestamp) {
    if (verbosity <= verbosityLevel) {
        logSubSectionDivider(message, includeTimestamp);
    }
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
