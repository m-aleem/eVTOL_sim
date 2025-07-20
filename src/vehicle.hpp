/**
 * @file vehicle.hpp
 * @brief Header file for the Vehicle class and its derived classes.
 *
 * Vehicle class represents an eVTOL vehicle with properties such as cruise speed,
 * battery capacity, energy usage, and fault probability. It includes methods for
 * managing the vehicle's state, such as starting flights, flying, charging, and
 * faults. Derived classes represent specific manufacturers' vehicles.
 */

#ifndef VEHICLE_HPP
#define VEHICLE_HPP

#include <string>
#include <functional>
#include "std_rng.hpp"


const double EPSILON = 1e-10; // epsilon for floating point comparisons to 0

/**
 * @brief Statistics structure to hold vehicle metrics.
 */
struct VehicleStatistics {
    double totalFlightTime = 0.0;
    double totalQueuedTime = 0.0;
    double totalDistanceTraveled = 0.0;
    double totalChargingTime = 0.0;
    double totalFaultedTime = 0.0;
    int totalFaults = 0;
    double totalPassengerMiles = 0.0;

    double lastUpdateFlightTime = 0;
    double lastUpdateQueuedTime = 0;
    double lastUpdateDistanceTraveled = 0.0;
    double lastUpdateChargingTime = 0;
    double lastUpdateFaultedTime = 0;
    int lastUpdateFaults = 0;
    double lastUpdatePassengerMiles = 0;

    void resetTotals() {
        totalFlightTime = 0.0;
        totalDistanceTraveled = 0.0;
        totalChargingTime = 0.0;
        totalChargingTime = 0.0;
        totalFaultedTime = 0.0;
        totalFaults = 0;
        totalPassengerMiles = 0.0;
    }

    void resetStep() {
        lastUpdateFlightTime = 0.0;
        lastUpdateDistanceTraveled = 0.0;
        lastUpdateDistanceTraveled = 0.0;
        lastUpdateChargingTime = 0.0;
        lastUpdateFaultedTime = 0.0;
        lastUpdateFaults = 0;
        lastUpdatePassengerMiles = 0.0;
    }

    std::string toString() const {
        return (" (Total)  Flight Time: " + std::to_string(totalFlightTime) +
               ", Queued Time: " + std::to_string(totalQueuedTime) +
               ", Distance: " + std::to_string(totalDistanceTraveled) +
               ", Charging Time: " + std::to_string(totalChargingTime) +
               ", Faulted Time: " + std::to_string(totalFaultedTime) +
               ", Faults: " + std::to_string(totalFaults) +
               ", Passenger Miles: " + std::to_string(totalPassengerMiles) +
               "\n (Last)   Flight Time: " + std::to_string(lastUpdateFlightTime) +
               ", Queued Time: " + std::to_string(lastUpdateQueuedTime) +
               ", Distance: " + std::to_string(lastUpdateDistanceTraveled) +
               ", Charging Time: " + std::to_string(lastUpdateChargingTime) +
               ", Faulted Time: " + std::to_string(lastUpdateFaultedTime) +
               ", Faults: " + std::to_string(lastUpdateFaults) +
               ", Passenger Miles: " + std::to_string(lastUpdatePassengerMiles));
    }


    std::string totalsToString() const {
        return ("Flight Time: " + std::to_string(totalFlightTime) +
               ", Queued Time: " + std::to_string(totalQueuedTime) +
               ", Distance: " + std::to_string(totalDistanceTraveled) +
               ", Charging Time: " + std::to_string(totalChargingTime) +
               ", Faulted Time: " + std::to_string(totalFaultedTime) +
               ", Faults: " + std::to_string(totalFaults) +
               ", Passenger Miles: " + std::to_string(totalPassengerMiles));
    }


    std::string lastUpdatesToString() const {
        return ("Flight Time: " + std::to_string(lastUpdateFlightTime) +
               ", Queued Time: " + std::to_string(lastUpdateQueuedTime) +
               ", Distance: " + std::to_string(lastUpdateDistanceTraveled) +
               ", Charging Time: " + std::to_string(lastUpdateChargingTime) +
               ", Faulted Time: " + std::to_string(lastUpdateFaultedTime) +
               ", Faults: " + std::to_string(lastUpdateFaults) +
               ", Passenger Miles: " + std::to_string(lastUpdatePassengerMiles));
    }


};

/**
 * @brief Base class for eVTOL vehicles.
 */
class Vehicle {
public:

    enum class State {
        Ready,    // Vehicle is ready for flight (pre-flight state)
        Flying,   // Vehicle is currently in flight
        Queued,   // Vehicle is queued for charging (post-flight state)
        Charging, // Vehicle is actively charging
        Faulted   // Vehicle has encountered a fault and cannot operate
    };

    enum class Manufacturer {
        Alpha,
        Bravo,
        Charlie,
        Delta,
        Echo,
        NumManufacturers // Used to determine number of vehicle types
    };

    // Default random number generator instance for the Vehicle class
    static RandomGenerator& defaultRng();

    Vehicle(Manufacturer manufacturer,
            double cruiseSpeed,
            double batteryCapacity,
            double timeToCharge,
            double energyUsePerMile,
            int passengerCount,
            double faultProbability,
            RandomGenerator& rng = Vehicle::defaultRng());

    virtual ~Vehicle() = default;

    // Property getters
    Manufacturer getManufacturer() const;
    double getCruiseSpeed() const;
    double getBatteryCapacity() const;
    double getTimeToCharge() const;
    double getEnergyUsePerMile() const;
    int getPassengerCount() const;
    double getFaultProbability() const;

    // State getter & setter
    State getCurrentState() const;
    void setCurrentState(State state);

    // Battery level getter & setter
    double getBatteryLevel() const;
    void setBatteryLevel(double level);

    // Statistics getters
    const VehicleStatistics& getStatistics() const { return stats; }
    VehicleStatistics& getStatistics() { return stats; }

    // Individual statistics getters
    double getTotalFlightTime() const { return stats.totalFlightTime; }
    double getTotalDistanceTraveled() const { return stats.totalDistanceTraveled; }
    double getTotalQueuedTime() const { return stats.totalQueuedTime; }
    double getTotalChargingTime() const { return stats.totalChargingTime; }
    double getTotalFaultedTime() const { return stats.totalFaultedTime; }
    int getTotalFaults() const { return stats.totalFaults; }
    double getTotalPassengerMiles() const { return stats.totalPassengerMiles; }

    // State machine transitions and helpers
    /**
     * @brief State machine update method.
     *
     * This method handles transitions based on the current state and input time. It also
     * coordinates charging and flight operations based on the input time and the vehicle's
     * current state and properties.
     *
     * @param hours The time in hours to update the state.
     */
    void updateState(double hours);


    /**
     * @brief This method simulates flying the vehicle.
     *
     * This method transitions the vehicle to the Flying state and checks for any follow-up transitions.
     * It must be called when the vehicle is in the Ready state.
     */

     /**
     * @brief This method simulates flying the vehicle.
     *
     * This method handles simulating the vehicle's flight for a specified duration. It simulates flying
     * either for the full energy available or for the requested time, whichever is less and also checks
     * for faults during the flight. If a fault occurs, the vehicle will transition to the Faulted state.
     * If the battery is depleted, the vehicle will transition to the Queued state.
     * If the flight is successful and battery remains, the vehicle will remain in the Flying state.
     * Statistics such as flight time, distance traveled, and passenger miles are updated accordingly.
     *
     * @param hours The time in hours to fly.
     * @return The actual time flown, which may be less than requested if a fault occurs
     * or if the battery is depleted.
     */
    double fly(double hours);

    /**
     * @brief Start charging the vehicle.
     *
     * This method transitions the vehicle to the Charging state and checks for any follow-up transitions.
     * This method is required because there is no automatic transition from Queued to Charging, i.e.
     * the vehicle must be explicitly told to start charging.
     */
    void startCharging();

    /**
     * @brief This method simulates charging the vehicle's battery.
     *
     * This method handles charging the vehicle's battery for a specified duration.
     * It calculates how much energy can be added based on the charging rate and the time provided. If the battery is fully
     * charged, it will transition to the Ready state.
     *
     * @param hours The time in hours to charge the vehicle.
     * @return The actual time used for charging, which may be less than requested if the
     * battery is already full or if the charging time exceeds the vehicle's charging capacity.
     */
    double charge(double hours);

    /**
     * @brief Check if a fault occurs during the given time period.
     *
     * This method uses the random number generator to determine if a fault occurs based on the vehicle's
     * fault probability and the input time.
     *
     * @param hours The time in hours to check for faults.
     * @return True if a fault occurs, false otherwise.
     */
    bool checkFault(double hours);

    // Battery helpers
    double getPowerConsumptionRate() const {
        // [kWh/mile] * [mile/hour] = [kWh/hour]
        return (energyUsePerMile * cruiseSpeed);
    }

    double getMaxFlightTime() const {
        // [kWh] / ([kWh/mile] * [mile/hour]) = [hours]
        return (batteryLevel / (energyUsePerMile * cruiseSpeed));
    }

    // Utility methods
    std::string getStateString() const;
    std::string getManufacturerString() const;
    int getId() const { return id; }

private:
    int id; // Unique ID for the vehicle instance
    static int nextId;            // Static counter for unique IDs
    Manufacturer manufacturer;
    double cruiseSpeed;           // mph
    double batteryCapacity;       // kWh
    double timeToCharge;          // hours to full charge
    double energyUsePerMile;      // kWh/mile
    int passengerCount;           // number of passengers
    double faultProbability;      // faults per hour
    RandomGenerator& rng;         // Random number generator for fault simulation

    State currentState;
    double batteryLevel;          // current battery level in kWh

    VehicleStatistics stats;      // statistics for the vehicle
};

// Derived classes for each manufacturer
// TODO: Should probably read these constructor constants from a configuration file or database
//       in the future and move them to their own files for better organization in a larger project.

/**
 * @brief Alpha Company Vehicle class.
 */
class AlphaCompanyVehicle : public Vehicle {
public:
    AlphaCompanyVehicle(RandomGenerator& rng = Vehicle::defaultRng())
        : Vehicle(Manufacturer::Alpha, 120, 320, 0.6, 1.6, 4, 0.25, rng) {}
};

/**
 * @brief Bravo Company Vehicle class.
 */
class BravoCompanyVehicle : public Vehicle {
public:
    BravoCompanyVehicle(RandomGenerator& rng = Vehicle::defaultRng())
        : Vehicle(Manufacturer::Bravo, 100, 100, 0.2, 1.5, 5, 0.10, rng) {}
};

/**
 * @brief Charlie Company Vehicle class.
 */
class CharlieCompanyVehicle : public Vehicle {
public:
    CharlieCompanyVehicle(RandomGenerator& rng = Vehicle::defaultRng())
        : Vehicle(Manufacturer::Charlie, 160, 220, 0.8, 2.2, 3, 0.05, rng) {}
};

/**
 * @brief Delta Company Vehicle class.
 */
class DeltaCompanyVehicle : public Vehicle {
public:
    DeltaCompanyVehicle(RandomGenerator& rng = Vehicle::defaultRng())
        : Vehicle(Manufacturer::Delta, 90, 120, 0.62, 0.8, 2, 0.22, rng) {}
};

/**
 * @brief Echo Company Vehicle class.
 */
class EchoCompanyVehicle : public Vehicle {
public:
    EchoCompanyVehicle(RandomGenerator& rng = Vehicle::defaultRng())
        : Vehicle(Manufacturer::Echo, 30, 150, 0.3, 5.8, 2, 0.61, rng) {}
};

#endif