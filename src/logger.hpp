/**
 * @file logger.hpp
 * @brief Header file for the Logger class
 *
 * Simple logger class for dual output (console + file)
 */

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <fstream>
#include <sstream>

class Logger {
public:
    /**
     * @brief Enumeration for different logging output modes
     */
    enum class LogMode {
        STDOUT_ONLY,  // Log to std::cout only
        FILE_ONLY,    // Log to file only
        BOTH          // Log to both std::cout and file (default)
    };

private:
    std::ofstream logFile;
    std::string logFileName;
    LogMode currentMode;
    bool includeTimestampInFile;

    void openLogFile();

public:
    Logger(const std::string& filename = "", LogMode mode = LogMode::BOTH);
    ~Logger();

    void log(const std::string& message, bool includeTimestamp = true);
    void logLine(const std::string& message = "", bool includeTimestamp = true);


    void logSectionDivider(const std::string& message = "", bool includeTimestamp = true);
    void logSubSectionDivider(const std::string& message = "", bool includeTimestamp = true);

    std::string formatFixedWidth(const std::string& text, int width, bool rightAlign = true);

    void setLogFile(const std::string& filename);
    std::string getLogFile() const;

    void setLogMode(LogMode mode);
    LogMode getLogMode() const;

    void setIncludeTimestampInFile(bool enable);
    bool getIncludeTimestampInFile() const;

    static std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;

        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d_%H:%M:%S");
        ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }
};

#endif