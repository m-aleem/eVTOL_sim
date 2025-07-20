// simulation.hpp
/**
 * @file simulation.hpp
 * @brief Header file for the Simulation class
 *
 * The Simulation class manages multiple eVTOL vehicles, charging stations,
 * and provides comprehensive statistics reporting with progress tracking.
 */

#ifndef SIMULATION_HPP
#define SIMULATION_HPP


#include <vector>
#include <queue>
#include <map>
#include <unordered_set>

#include "vehicle.hpp"
#include "logger.hpp"

#include <gtest/gtest_prod.h>

const int DEFAULT_NUM_VEHICLES = 20; // Default number of vehicles to create
const int DEFAULT_HRS_SIM = 3; // Default hours for simulation
const int DEFAULT_CHARGERS = 3; // Default number of charging stations
const double DEFAULT_TIME_STEP_SECONDS = 1; //  Default time step in seconds
const double SECONDS_TO_HOURS = 1/3600.0; // Conversion factor from seconds to hours

const int NUM_VEHICLE_TYPES = static_cast<int>(Vehicle::Manufacturer::NumManufacturers); // Number of different vehicle types

// Structure to hold aggregated statistics per vehicle type
struct VehicleTypeStats {
    Vehicle::Manufacturer manufacturer;
    std::string manufacturerName;
    int vehicleCount = 0;
    int totalFlights = 0;
    int totalCharges = 0;
    double totalFlightTime = 0.0;
    double totalDistance = 0.0;
    double totalChargingTime = 0.0;
    int totalFaults = 0;
    double totalPassengerMiles = 0.0;

    double getFaultRate() const {
        return totalFaults > 0 ? static_cast<double>(totalFaults) / vehicleCount : 0.0;
    }

    double avgFlightTimePerFlight() const {
        return totalFlights > 0 ? totalFlightTime / totalFlights : 0.0;
    }

    double avgDistancePerFlight() const {
        return totalFlights > 0 ? totalDistance / totalFlights : 0.0;
    }

    double avgChargingTimePerSession() const {
        return totalCharges > 0 ? totalChargingTime / totalCharges : 0.0;
    }

    void reset() {
        vehicleCount = 0;
        totalFlights = 0;
        totalCharges = 0;
        totalFlightTime = 0.0;
        totalDistance = 0.0;
        totalChargingTime = 0.0;
        totalFaults = 0;
        totalPassengerMiles = 0.0;
    }

    std::string toString() const {
        return "Manufacturer: " + manufacturerName +
               ", Vehicles: " + std::to_string(vehicleCount) +
               ", Flights: " + std::to_string(totalFlights) +
               ", Charges: " + std::to_string(totalCharges) +
               ", Flight Time: " + std::to_string(totalFlightTime) +
               ", Distance: " + std::to_string(totalDistance) +
               ", Charging Time: " + std::to_string(totalChargingTime) +
               ", Faults: " + std::to_string(totalFaults) +
               ", Passenger Miles: " + std::to_string(totalPassengerMiles);
    }

};

class Simulation {
public:
    Simulation(
        int numVehicles = DEFAULT_NUM_VEHICLES,
        double simHours = DEFAULT_HRS_SIM,
        int numChargers = DEFAULT_CHARGERS,
        double simTimeStepSeconds = DEFAULT_TIME_STEP_SECONDS);

    ~Simulation() = default;

    bool runSimulation(); // Main simulation loop

    // Allow access to private members for testing
    friend class SimulationTest;
    FRIEND_TEST(SimulationTest, Initialization);
    FRIEND_TEST(SimulationTest, CreateVehicles);
    FRIEND_TEST(SimulationTest, TimeStep);
    FRIEND_TEST(SimulationTest, ChargingQueue);

private:
    // Configuration
    int numVehicles;
    double simHours;
    int numChargers;
    double simTimeStepSeconds;
    StdRandomGenerator rng;

    double currentTime;
    double timeStep;
    int stepCount;

    // Simulation state
    std::vector<std::unique_ptr<Vehicle>> vehicles;
    std::queue<Vehicle*> chargingQueue;
    std::unordered_set<Vehicle*> queuedVehicles;
    std::vector<Vehicle*> chargingStations; // nullptr = available, Vehicle* = occupied

    // Logging
    Logger logger;

    // Statistics tracking
    std::map<Vehicle::Manufacturer, VehicleTypeStats> typeStats;

    // Helper methods
    std::unique_ptr<Vehicle> createVehicle(int type);
    void initializeVehicles();
    void updateVehicleStats(Vehicle* vehicle);
    void manageCharging();
    void assignAvailableChargers();
    void processChargingVehicles(double timeStep);
    void updateAllVehicles(double timeStep);
    double nextTimeStep() const;


    void showProgress(double currentTime, double totalTime);

    void printInitialStatus();
    void printChargingQueue();
    void printChargingStations();
    void printFinalStatus();
    void printStatsTable();

};

#endif

