// simulation.cpp
#include "simulation.hpp"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include <filesystem>


/* Constructor*/
Simulation::Simulation(
        int numVehicles,
        double simHours,
        int numChargers,
        double simTimeStepSeconds,
        int simLogVerbosity,
        bool randomizeVehicles)
    : numVehicles(numVehicles),
      simHours(simHours),
      numChargers(numChargers),
      simLogVerbosity(simLogVerbosity),
      simTimeStepSeconds(simTimeStepSeconds),
      randomizeVehicles(randomizeVehicles),
      chargingStations(numChargers, nullptr) {

        // Create output directory if it doesn't exist
        std::filesystem::create_directories("output");

        std::string filename = "output/eVTOL_sim_report_" + logger.getCurrentTimestamp() + ".txt";
        logger.setLogFile(filename);
        logger.setVerbosityLevel(simLogVerbosity);
}

/* Run Simulation*/
bool Simulation::runSimulation() {
    bool success = false;

    currentTime = 0.0;
    stepCount = 0;
    timeStep = nextTimeStep();

    printInitialStatus();
    initializeVehicles();

    // Save current logging mode and switch to file-only for detailed step output
    Logger::LogMode originalMode = logger.getLogMode();
    logger.setLogMode(Logger::LogMode::FILE_ONLY);
    while (currentTime < simHours) {
        logger.logSubSectionDivider(2, "Simulation Step " + std::to_string(stepCount + 1));
        logger.logLine(2, "Current Time: " + std::to_string(currentTime) + " hours (Delta +" + std::to_string(timeStep) + " hours from previous step)");

        processChargingVehicles();
        updateAllVehicles(timeStep);
        manageCharging();

        currentTime += timeStep;
        stepCount++;
        timeStep = nextTimeStep();

        // Update progress every few steps to avoid excessive output
        if (stepCount % 5 == 0 || currentTime >= simHours) {
            showProgress(currentTime, simHours);
        }
    }

    logger.setLogMode(Logger::LogMode::STDOUT_ONLY);
    logger.logLine("", false);
    // Restore original logging mode
    logger.setLogMode(originalMode);

    printStatsTable();
    printFaultStatsTable();
    printFinalStatus();

    return success;
}

double Simulation::nextTimeStep() const {
    return std::min(simTimeStepSeconds * SECONDS_TO_HOURS, simHours - currentTime);
}

void Simulation::updateAllVehicles(double timeStep) {
    for (const auto& vehicle : vehicles) {
            vehicle->updateState(timeStep);
        updateVehicleStats(vehicle.get());
    }
}

void Simulation::updateVehicleStats(Vehicle* vehicle) {
    const Vehicle::Manufacturer manufacturer = vehicle->getManufacturer();
    const auto& stepStats = vehicle->getStepStats();
    const auto& totalStats = vehicle->getTotalStats();

    // Track flight and charge counts for averages
    static std::map<Vehicle*, Vehicle::State> lastState;
    Vehicle::State currentState = vehicle->getCurrentState();

    // Initialize if first time
    if (lastState.find(vehicle) == lastState.end()) {
        lastState[vehicle] = currentState;
    }

    if (lastState[vehicle] == Vehicle::State::Charging && currentState != Vehicle::State::Charging) {
        // Vehicle just finished charging
        typeStats[manufacturer].totalCharges++;
    } else if (lastState[vehicle] == Vehicle::State::Flying && currentState != Vehicle::State::Flying) {
        // Vehicle just finished flying
        // TODO: This would count even Faulted flight as flights, which may not be desired
        // need to decide if we want to count Faulted flights
        typeStats[manufacturer].totalFlights++;
    }

    // Update type statistics
    auto& typeData = typeStats[manufacturer];

    typeData.totalFlightTime += stepStats.flightTime;
    typeData.totalDistance += stepStats.distanceTraveled;
    typeData.totalChargingTime += stepStats.chargingTime;
    typeData.totalFaults += stepStats.faults;
    typeData.totalPassengerMiles += stepStats.passengerMiles;

    lastState[vehicle] = currentState;

    printVehicleStats(vehicle, stepStats, totalStats);
}

void Simulation::manageCharging() {

    logger.logLine(2);
    logger.logLine(2, "Manage Charging Queue");

    // Add vehicles that need charging to queue
    for (const auto& vehicle : vehicles) {
        if (vehicle->getCurrentState() == Vehicle::State::Queued) {
            // If a vehicle is in queued state add it
            // to the charging queue if it is not already in the queue
            if (queuedVehicles.find(vehicle.get()) == queuedVehicles.end()) {
                chargingQueue.push(vehicle.get());
                queuedVehicles.insert(vehicle.get());
            }
        }
    }

    printChargingQueue();
    logger.logLine(2);

    assignAvailableChargers();
}

void Simulation::assignAvailableChargers() {

    logger.logLine(2);
    logger.logLine(2, "Assign Available Chargers");

    // Assign available chargers to queued vehicles
    for (int i = 0; i < numChargers && !chargingQueue.empty(); i++) {
        if (chargingStations[i] == nullptr) {
            Vehicle* vehicle = chargingQueue.front();
            chargingQueue.pop();
            queuedVehicles.erase(vehicle);

            if (vehicle->getCurrentState() == Vehicle::State::Queued) {
                chargingStations[i] = vehicle;
                vehicle->startCharging();
            }
        }
    }

    printChargingStations();
    logger.logLine(2);
}

void Simulation::processChargingVehicles() {
    for (int i = 0; i < numChargers; ++i) {
        if (chargingStations[i] != nullptr) {
            Vehicle* vehicle = chargingStations[i];

            if (vehicle->getCurrentState() != Vehicle::State::Charging) {
                    // Vehicle is no longer charging, free the charger
                    chargingStations[i] = nullptr;
            }
        }
    }
}

/* Initialization */
std::unique_ptr<Vehicle> Simulation::createVehicle(int type) {
    switch (type) {
        case 0: return std::make_unique<AlphaCompanyVehicle>();
        case 1: return std::make_unique<BravoCompanyVehicle>();
        case 2: return std::make_unique<CharlieCompanyVehicle>();
        case 3: return std::make_unique<DeltaCompanyVehicle>();
        case 4: return std::make_unique<EchoCompanyVehicle>();
        default: return nullptr;
    }
}

void Simulation::initializeVehicles() {

    logger.logSectionDivider("Initialize Simulation Vehicles");

    vehicles.clear();
    typeStats.clear();

    std::map<std::string, int> vehicleCounts;

    // Generate vehicles
    for(int i = 0; i < numVehicles; ++i) {
        int type;
        if (randomizeVehicles) {
            type = rng.uniformInt(0, NUM_VEHICLE_TYPES - 1);
        } else {
            type = i % NUM_VEHICLE_TYPES; // Round-robin selection (equal distribution, for testing)
        }

        auto vehicle = createVehicle(type);
        if (vehicle) {
            std::string name = vehicle->getManufacturerString();
            vehicleCounts[name]++;
            vehicles.push_back(std::move(vehicle));
        }

    }

    // Initialize type statistics
    for (const auto& vehicle : vehicles) {
        if (typeStats.find(vehicle->getManufacturer()) == typeStats.end()) {
            typeStats[vehicle->getManufacturer()].manufacturerName = vehicle->getManufacturerString();
            typeStats[vehicle->getManufacturer()].expectedFaultRate = vehicle->getFaultProbability();
        }
        typeStats[vehicle->getManufacturer()].vehicleCount++;
    }

    logger.logLine("Vehicle type counts:");
    int totalVehicles = 0;
    for (const auto& pair : vehicleCounts) {
        logger.logLine("  " + pair.first + ": " + std::to_string(pair.second));
        totalVehicles += pair.second;
    }
    logger.logLine("  Total vehicles: " + std::to_string(totalVehicles));
    logger.logLine();
}


/* Print Helpers */
void Simulation::printVehicleStats(const Vehicle* vehicle, const VehicleStats& stepStats, const VehicleStats& totalStats) {
    int batteryPercent = static_cast<int>(vehicle->getBatteryPercent() + 0.5);
    std::string batteryDisplay = "Battery " + std::to_string(batteryPercent) + "%";
    std::string stepSection = stepStats.toShortString();
    std::string totalSection = totalStats.toLongString();

    const int stepWidth = 40;
    const int totalWidth = 140;

    stepSection.resize(stepWidth, ' ');   // Pad with spaces
    totalSection.resize(totalWidth, ' '); // Pad with spaces

    logger.logLine(2, logger.formatFixedWidth("Vehicle " + std::to_string(vehicle->getId()) + " (" + vehicle->getManufacturerString() + ")   ", 30) +
                   "[" + logger.formatFixedWidth(vehicle->getStateString(), 8) + "]   " +
                   "[" + logger.formatFixedWidth(batteryDisplay, 12) + "]   " +
                   "Step: " + stepSection + " | Total: " + totalSection);
}

void Simulation::showProgress(double currentTime, double totalTime) {
    const int barWidth = 50;
    double progress = currentTime / totalTime;
    int pos = static_cast<int>(barWidth * progress);

    std::cout << "\r[" << logger.getCurrentTimestamp() << "] [";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << std::fixed << std::setprecision(1)
              << (progress * 100.0) << "% ("
              << std::setprecision(2) << currentTime << "/" << totalTime << " hours)";
    std::cout.flush();
}

void Simulation::printInitialStatus() {

    logger.logSectionDivider("eVTOL Simulation START");

    logger.logLine("Inputs:");
    logger.logLine("  Number of vehicles: " + std::to_string(numVehicles));
    logger.logLine("  Simulation hours: " + std::to_string(simHours));
    logger.logLine("  Number of chargers: " + std::to_string(numChargers));
    logger.logLine("  Time step: " + std::to_string(simTimeStepSeconds) + " seconds (" +
                                     std::to_string(simTimeStepSeconds / 3600.0) + " hours)");
    logger.logLine("  Log verbosity level: " + std::to_string(simLogVerbosity));
    logger.logLine("  Vehicle selection: " + std::string(randomizeVehicles ? "Random" : "Equal distribution"));
    logger.logLine();

    logger.logLine("Initial Status:");
    logger.logLine("  Time: " + std::to_string(currentTime));
    logger.logLine("  Step Count: " + std::to_string(stepCount));
    logger.logLine();
}

void Simulation::printStatsTable() {
    logger.logLine();
    logger.logSectionDivider("Simulation Results by Vehicle Type", true);

    // Table header
    const int colWidth = 12;
    std::string separator(32 + 6*colWidth, '-');
    logger.logLine();
    logger.logLine(separator);
    logger.log(logger.formatFixedWidth("Vehicle",            colWidth) + " | ");
    logger.log(logger.formatFixedWidth("Count",              colWidth) + " | ", false);
    logger.log(logger.formatFixedWidth("Avg Flight",         colWidth) + " | ", false);
    logger.log(logger.formatFixedWidth("Avg Dist",           colWidth) + " | ", false);
    logger.log(logger.formatFixedWidth("Avg Charge",         colWidth) + " | ", false);
    logger.log(logger.formatFixedWidth("Faults",             colWidth) + " | ", false);
    logger.logLine(logger.formatFixedWidth("PAX Miles",      colWidth) + " | ", false);

    logger.log(logger.formatFixedWidth("Type",               colWidth) + " | ");
    logger.log(logger.formatFixedWidth("",                   colWidth) + " | ", false);
    logger.log(logger.formatFixedWidth("Time (hrs)",         colWidth) + " | ", false);
    logger.log(logger.formatFixedWidth("(miles)",            colWidth) + " | ", false);
    logger.log(logger.formatFixedWidth("Time (hrs)",         colWidth) + " | ", false);
    logger.log(logger.formatFixedWidth("",                   colWidth) + " | ", false);
    logger.logLine(logger.formatFixedWidth("(miles)",        colWidth) + " | ", false);

    logger.logLine(separator);

    // Per-Type data
    for (const auto& pair : typeStats) {
        const auto& stats = pair.second;

        logger.log(logger.formatFixedWidth(stats.manufacturerName,                           colWidth) +  " | ");
        logger.log(logger.formatFixedWidth(std::to_string(stats.vehicleCount),               colWidth) +  " | ", false);
        logger.log(logger.formatFixedWidth(std::to_string(stats.avgFlightTimePerFlight()),   colWidth) +  " | ", false);
        logger.log(logger.formatFixedWidth(std::to_string(stats.avgDistancePerFlight()),     colWidth) +  " | ", false);
        logger.log(logger.formatFixedWidth(std::to_string(stats.avgChargingTimePerSession()),colWidth) +  " | ", false);
        logger.log(logger.formatFixedWidth(std::to_string(stats.totalFaults),                colWidth) +  " | ", false);
        logger.logLine(logger.formatFixedWidth(std::to_string(stats.totalPassengerMiles),    colWidth) +  " | ", false);
    }
    logger.logLine(separator);
}

void Simulation::printFinalStatus() {

    logger.logLine();
    logger.logLine("Final Status:");
    logger.logLine("  Time: " + std::to_string(currentTime));
    logger.logLine("  Step Count: " + std::to_string(stepCount));
    logger.logLine();

    logger.logLine("Outputs:");
    logger.logLine("  Log File: " + logger.getLogFile());
    logger.logLine();

    logger.logSectionDivider("eVTOL Simulation DONE");
}

void Simulation::printChargingQueue() {
    logger.log(2, "Charging Queue: [");

    std::queue<Vehicle*> tempQueue = chargingQueue;
    bool first = true;

    while (!tempQueue.empty()) {
        if (!first) {
            logger.log(2, ", ", false);
        }
        Vehicle* vehicle = tempQueue.front();
        tempQueue.pop();
        logger.log(2, "Vehicle " + std::to_string(vehicle->getId()), false);
        first = false;
    }

    logger.logLine(2, "]", false);
}

void Simulation::printChargingStations() {
    logger.log(2, "Charging Stations: ");

    for (int i = 0; i < numChargers; i++) {

        if (chargingStations[i] == nullptr) {
            logger.log(2, "[--]", false);
        } else {
            logger.log(2, "[Vehicle " + std::to_string(chargingStations[i]->getId()) + "]", false);
        }

        if (i < numChargers - 1) {
            logger.log(2, " ", false);
        }
    }

    logger.logLine(2, "", false);
}

void Simulation::printFaultStatsTable() {
    logger.logLine();
    logger.logSectionDivider("Fault Statistics by Vehicle Type", true);

    // Table header
    const int colWidth = 12;
    const int colWidthLarger = 15;
    std::string separator(23 + 6*colWidth, '-');
    logger.logLine();
    logger.logLine(separator);
    logger.log(logger.formatFixedWidth("Vehicle", colWidth) + " | ");
    logger.log(logger.formatFixedWidth("Count", colWidth) + " | ", false);
    logger.log(logger.formatFixedWidth("Total Faults", colWidth) + " | ", false);
    logger.log(logger.formatFixedWidth("Flight Hours", colWidth) + " | ", false);
    logger.log(logger.formatFixedWidth("Fault Prb", colWidthLarger) + " | ", false);
    logger.logLine(logger.formatFixedWidth("Actual Fault", colWidthLarger) + " | ", false);

    logger.log(logger.formatFixedWidth("Type", colWidth) + " | ");
    logger.log(logger.formatFixedWidth("", colWidth) + " | ", false);
    logger.log(logger.formatFixedWidth("", colWidth) + " | ", false);
    logger.log(logger.formatFixedWidth("", colWidth) + " | ", false);
    logger.log(logger.formatFixedWidth("Per Hour", colWidthLarger) + " | ", false);
    logger.logLine(logger.formatFixedWidth("Rate Per Hour", colWidthLarger) + " | ", false);

    logger.logLine(separator);

    // Per-Type fault data
    for (const auto& pair : typeStats) {
        const auto& stats = pair.second;

        logger.log(logger.formatFixedWidth(stats.manufacturerName, colWidth) + " | ");
        logger.log(logger.formatFixedWidth(std::to_string(stats.vehicleCount), colWidth) + " | ", false);
        logger.log(logger.formatFixedWidth(std::to_string(stats.totalFaults), colWidth) + " | ", false);
        logger.log(logger.formatFixedWidth(std::to_string(stats.totalFlightTime), colWidth) + " | ", false);
        logger.log(logger.formatFixedWidth(std::to_string(stats.expectedFaultRate), colWidthLarger) + " | ", false);
        logger.logLine(logger.formatFixedWidth(std::to_string(stats.getActualFaultRate()), colWidthLarger) + " | ", false);
    }
    logger.logLine(separator);
}
