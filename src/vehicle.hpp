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
struct VehicleStats {
    double flightTime = 0.0;
    double queuedTime = 0.0;
    double distanceTraveled = 0.0;
    double chargingTime = 0.0;
    double faultedTime = 0.0;
    int faults = 0;
    double passengerMiles = 0.0;

    void reset() {
        flightTime = 0.0;
        queuedTime = 0.0;
        distanceTraveled = 0.0;
        chargingTime = 0.0;
        faultedTime = 0.0;
        faults = 0;
        passengerMiles = 0.0;
    }

    void add(const VehicleStats& other) {
        flightTime += other.flightTime;
        queuedTime += other.queuedTime;
        distanceTraveled += other.distanceTraveled;
        chargingTime += other.chargingTime;
        faultedTime += other.faultedTime;
        faults += other.faults;
        passengerMiles += other.passengerMiles;
    }

    std::string toString() const {
        return ("Flight Time: " + std::to_string(flightTime) +
               ", Queued Time: " + std::to_string(queuedTime) +
               ", Distance: " + std::to_string(distanceTraveled) +
               ", Charging Time: " + std::to_string(chargingTime) +
               ", Faulted Time: " + std::to_string(faultedTime) +
               ", Faults: " + std::to_string(faults) +
               ", Passenger Miles: " + std::to_string(passengerMiles));
    }

    std::string toShortString() const {
        std::vector<std::string> parts;

        if (flightTime > EPSILON) {
            char buffer[40];
            snprintf(buffer, sizeof(buffer), "Flew      %.6fh", flightTime);
            parts.push_back(std::string(buffer));
        }
        if (distanceTraveled > EPSILON) {
            char buffer[40];
            snprintf(buffer, sizeof(buffer), "Distance  %.3fmi", distanceTraveled);
            parts.push_back(std::string(buffer));
        }
        if (chargingTime > EPSILON) {
            char buffer[40];
            snprintf(buffer, sizeof(buffer), "Charged   %.3fh", chargingTime);
            parts.push_back(std::string(buffer));
        }
        if (queuedTime > EPSILON) {
            char buffer[40];
            snprintf(buffer, sizeof(buffer), "Queued    %.3fh", queuedTime);
            parts.push_back(std::string(buffer));
        }
        if (faultedTime > EPSILON) {
            char buffer[40];
            snprintf(buffer, sizeof(buffer), "Faulted   %.3fh", faultedTime);
            parts.push_back(std::string(buffer));
        }

        if (parts.empty()) {
            return "";
        }

        // Join parts
        std::string result;
        for (size_t i = 0; i < parts.size(); ++i) {
            if (i > 0) result += " ";
            result += parts[i];
        }

        return result;
    }

    std::string toLongString() const {
        std::vector<std::string> parts;

        char buffer[50];
        snprintf(buffer, sizeof(buffer), "Flew      %8.3fh", flightTime);
        parts.push_back(std::string(buffer));

        snprintf(buffer, sizeof(buffer), "Distance  %8.1fmi", distanceTraveled);
        parts.push_back(std::string(buffer));

        snprintf(buffer, sizeof(buffer), "Charged   %8.3fh", chargingTime);
        parts.push_back(std::string(buffer));

        snprintf(buffer, sizeof(buffer), "Queued    %8.3fh", queuedTime);
        parts.push_back(std::string(buffer));

        snprintf(buffer, sizeof(buffer), "Faulted   %8.3fh", faultedTime);
        parts.push_back(std::string(buffer));

        snprintf(buffer, sizeof(buffer), "Faults    %8d", faults);
        parts.push_back(std::string(buffer));

        snprintf(buffer, sizeof(buffer), "PAX Miles %8.1fmi", passengerMiles);
        parts.push_back(std::string(buffer));

        // Join parts
        std::string result;
        for (size_t i = 0; i < parts.size(); ++i) {
            if (i > 0) result += " ";
            result += parts[i];
        }

        return result;

        // // Show all values for totals with aligned labels (10 chars each)
        // char buffer[300];
        // snprintf(buffer, sizeof(buffer),
        //          "Flew      %.3fh  Distance  %.3fmi  Charged   %.3fh  Queued    %.3fh  Faulted   %.3fh  Faults    %d  PAX Miles :%2fmi",
        //          flightTime,                             // Flight hours
        //          distanceTraveled,                       // Distance miles
        //          chargingTime,                           // Charging hours
        //          queuedTime,                             // Queue hours
        //          faultedTime,                            // Faulted hours
        //          faults,                                 // Fault count
        //          passengerMiles);      // Passenger miles
        // return std::string(buffer);
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
    const VehicleStats& getStepStats() const { return stepStats; }
    VehicleStats& getStepStats() { return stepStats; }
    const VehicleStats& getTotalStats() const { return totalStats; }
    VehicleStats& getTotalStats() { return totalStats; }

    // Individual statistics getters
    double getTotalFlightTime() const { return totalStats.flightTime; }
    double getTotalQueuedTime() const { return totalStats.queuedTime; }
    double getTotalDistanceTraveled() const { return totalStats.distanceTraveled; }
    double getTotalChargingTime() const { return totalStats.chargingTime; }
    double getTotalFaultedTime() const { return totalStats.faultedTime; }
    int getTotalFaults() const { return totalStats.faults; }
    double getTotalPassengerMiles() const { return totalStats.passengerMiles; }

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

    double getBatteryPercent() const {
        return (batteryLevel / batteryCapacity) * 100.0;
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

    VehicleStats stepStats;       // statistics for the current step
    VehicleStats totalStats;      // statistics for the total simulation
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