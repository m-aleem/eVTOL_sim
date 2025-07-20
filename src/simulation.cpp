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
        double simTimeStepSeconds)
    : numVehicles(numVehicles),
      simHours(simHours),
      numChargers(numChargers),
      simTimeStepSeconds(simTimeStepSeconds),
      chargingStations(numChargers, nullptr) {

        // Create output directory if it doesn't exist
        std::filesystem::create_directories("output");

        std::string filename = "output/eVTOL_sim_report_" + logger.getCurrentTimestamp() + ".txt";
        logger.setLogFile(filename);
}

/* Run Simulation*/
bool Simulation::runSimulation() {
    bool success = false;

    currentTime = 0.0;
    stepCount = 0;
    timeStep = std::min(simTimeStepSeconds * SECONDS_TO_HOURS, simHours - currentTime);

    printInitialStatus();
    initializeVehicles();

    while (currentTime < simHours) {
        // Save current logging mode and switch to file-only for detailed step output
        Logger::LogMode originalMode = logger.getLogMode();
        logger.setLogMode(Logger::LogMode::FILE_ONLY);

        logger.logSubSectionDivider("Simulation Step " + std::to_string(stepCount + 1));
        logger.logLine("Current Time: " + std::to_string(currentTime) + " hours");

        updateAllVehicles(timeStep);
        manageCharging();
        processChargingVehicles(timeStep);

        // Restore original logging mode
        logger.setLogMode(originalMode);

        currentTime += timeStep;
        stepCount++;

        // Update progress every few steps to avoid excessive output
        if (stepCount % 5 == 0 || currentTime >= simHours) {
            showProgress(currentTime, simHours);
        }
    }

    logger.logLine("", false);
    printStatsTable();
    printFinalStatus();

    return success;
}

void Simulation::updateAllVehicles(double timeStep) {
    for (const auto& vehicle : vehicles) {
            vehicle->updateState(timeStep);
        updateVehicleStats(vehicle.get());
    }
}

void Simulation::updateVehicleStats(Vehicle* vehicle) {
    const Vehicle::Manufacturer manufacturer = vehicle->getManufacturer();
    const auto& stats = vehicle->getStatistics();

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
    const auto& vStats = vehicle->getStatistics();

    typeData.totalFlightTime += vStats.lastUpdateFlightTime;
    typeData.totalDistance += vStats.lastUpdateDistanceTraveled;
    typeData.totalChargingTime += vStats.lastUpdateChargingTime;
    typeData.totalFaults += vStats.lastUpdateFaults;
    typeData.totalPassengerMiles += vStats.lastUpdatePassengerMiles;

    lastState[vehicle] = currentState;

    // // FIXME DEBUG
    // std::cout << "Iterate thru every vehicle: " << std::endl;
    // if (manufacturer == Vehicle::Manufacturer::Echo) {
    //     std::cout << vStats.toString() << std::endl;
    //     std::cout << typeStats[Vehicle::Manufacturer::Echo].toString() << std::endl;
    // }

    logger.logLine(logger.formatFixedWidth("Vehicle " + std::to_string(vehicle->getId()) + " (" + vehicle->getManufacturerString() + ")   ", 30) +
                   "[" + logger.formatFixedWidth(vehicle->getStateString(), 8) + "]   " +
                   "[" + logger.formatFixedWidth(std::to_string(vehicle->getBatteryLevel()), 12) + "]   " +
                   vStats.lastUpdatesToString());
}

void Simulation::manageCharging() {

    // Add vehicles that need charging to queue
    logger.logLine();
    printChargingQueue();
    logger.logLine("Manage Charging Queue");
    for (const auto& vehicle : vehicles) {
        if (vehicle->getCurrentState() == Vehicle::State::Queued) {
            // Check if already in queue
            bool alreadyQueued = false;
            std::queue<Vehicle*> tempQueue = chargingQueue;
            while (!tempQueue.empty()) {
                if (tempQueue.front() == vehicle.get()) {
                    alreadyQueued = true;
                    break;
                }
                tempQueue.pop();
            }

            if (!alreadyQueued) {
                chargingQueue.push(vehicle.get());
            }
        }
    }
    printChargingQueue();
    logger.logLine();
    assignAvailableChargers();
}

void Simulation::assignAvailableChargers() {
    logger.logLine();
    printChargingStations();
    logger.logLine("Assign Available Chargers");
    // Assign available chargers to queued vehicles
    for (int i = 0; i < numChargers && !chargingQueue.empty(); i++) {
        if (chargingStations[i] == nullptr) {
            Vehicle* vehicle = chargingQueue.front();
            chargingQueue.pop();

            if (vehicle->getCurrentState() == Vehicle::State::Queued) {
                chargingStations[i] = vehicle;
                vehicle->startCharging();
            }
        }
    }

    printChargingStations();
    logger.logLine();
}

void Simulation::processChargingVehicles(double timeStep) {
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
        std::srand(time(0));
        int type = rng.uniformInt(0, NUM_VEHICLE_TYPES - 1); // Randomly select vehicle type
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
    logger.log(logger.formatFixedWidth("Total Faults",       colWidth) + " | ", false);
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
        logger.log(logger.formatFixedWidth(std::to_string(stats.totalFaults) + "(" + std::to_string(stats.getFaultRate()) + ")",                colWidth) +  " | ", false);
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
    logger.log("Charging Queue:");
    std::queue<Vehicle*> tempQueue = chargingQueue;
    while (!tempQueue.empty()) {
        Vehicle* vehicle = tempQueue.front();
        tempQueue.pop();
        logger.log("  Vehicle " + std::to_string(vehicle->getId()), false);
    }
    logger.logLine("", false);
}

void Simulation::printChargingStations() {
    logger.log("Charging Stations:");
    for (int i = 0; i < numChargers; i++) {
        if (chargingStations[i] == nullptr) {
            logger.log("  Charger " + std::to_string(i) + ": Available", false);
        } else {
            logger.log("  Charger " + std::to_string(i) + ": Vehicle " + std::to_string(chargingStations[i]->getId()), false);
        }
    }
    logger.logLine("", false);
}
