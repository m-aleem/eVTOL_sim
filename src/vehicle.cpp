/**
 * @file vehicle.cpp
 * @brief Implementation file for the Vehicle class and its derived classes.
 *
 * See vehicle.hpp for class documentation.
 */

#include "vehicle.hpp"
#include <stdexcept>
#include <random>
#include <cmath>

RandomGenerator& Vehicle::defaultRng() {
    static StdRandomGenerator instance;
    return instance;
}

/* Constructor */
int Vehicle::nextId = 1; // Initialize static ID counter

Vehicle::Vehicle(Manufacturer manufacturer,
                 double cruiseSpeed,
                 double batteryCapacity,
                 double timeToCharge,
                 double energyUsePerMile,
                 int passengerCount,
                 double faultProbability,
                 RandomGenerator& rng)
    : manufacturer(manufacturer),
      cruiseSpeed(cruiseSpeed),
      batteryCapacity(batteryCapacity),
      timeToCharge(timeToCharge),
      energyUsePerMile(energyUsePerMile),
      passengerCount(passengerCount),
      faultProbability(faultProbability),
      currentState(State::Ready), // Always start Ready
      batteryLevel(batteryCapacity),
      rng(rng) {
        stepStats.reset();
        totalStats.reset();
        id = nextId++;
}

/* Getters */
Vehicle::Manufacturer Vehicle::getManufacturer() const { return manufacturer; }
double Vehicle::getCruiseSpeed() const { return cruiseSpeed; }
double Vehicle::getBatteryCapacity() const { return batteryCapacity; }
double Vehicle::getTimeToCharge() const { return timeToCharge; }
double Vehicle::getEnergyUsePerMile() const { return energyUsePerMile; }
int Vehicle::getPassengerCount() const { return passengerCount; }
double Vehicle::getFaultProbability() const { return faultProbability; }
double Vehicle::getBatteryLevel() const { return batteryLevel; }
Vehicle::State Vehicle::getCurrentState() const { return currentState; }

/* Setters */
void Vehicle::setBatteryLevel(double level) {
    if (level < 0.0) level = 0.0;
    if (level > batteryCapacity) level = batteryCapacity;
    batteryLevel = level;
}

void Vehicle::setCurrentState(State state) {
    currentState = state;
}

/* State Machine */
void Vehicle::updateState(double hours) {
    // Single loop handles both automatic transitions and time-consuming actions
    bool continueProcessing = true;
    double remainingTime = hours;

    while (continueProcessing && remainingTime >= 0) {
        continueProcessing = false;
        stepStats.reset(); // Reset step statistics for this update

        switch (currentState) {
            case State::Ready:
                // Automatic transition: Ready to Flying if battery > 0
                if (batteryLevel > 0) {
                    setCurrentState(State::Flying);
                    continueProcessing = true; // Process Flying state immediately
                }
                break;

            case State::Flying:
                // Time-consuming action: fly for remaining time
                if (remainingTime > 0) {
                    double timeUsed = fly(remainingTime);
                    remainingTime -= timeUsed; // Only consume time actually used

                    // Check if we transitioned to Queued (battery depleted) and have remaining time
                    if (currentState == State::Queued && remainingTime > 0) {
                        // Could potentially auto-start charging here if simulation allows
                        // For now, just consume remaining time waiting in queue
                        // Update queued time in statistics
                        stepStats.queuedTime += remainingTime;
                        remainingTime = 0;
                    }
                    // If we faulted, remaining time is lost (vehicle grounded)
                    else if (currentState == State::Faulted) {
                        // Update faulted time in statistics
                        stepStats.faultedTime += remainingTime;
                        remainingTime = 0;
                    }
                }
                break;

            case State::Charging:
                // Time-consuming action: charge for remaining time
                if (remainingTime > 0) {
                    double timeUsed = charge(remainingTime);
                    remainingTime -= timeUsed; // Only consume time actually used

                    // Check if charge completed and we transitioned to Ready
                    if (currentState == State::Ready) {
                        continueProcessing = true; // Process Ready state with remaining time
                    }
                }
                // Automatic transition check as we assume charging is done in one go
                // If battery is full, transition to Ready
                else if (batteryLevel >= batteryCapacity) {
                    setBatteryLevel(batteryCapacity);
                    setCurrentState(State::Ready);
                    continueProcessing = true;
                }
                break;

            case State::Queued:
                // Time-consuming action: wait (consume time but do nothing)
                if (remainingTime > 0) {
                    // Update queued time in statistics
                    stepStats.queuedTime += remainingTime;
                    remainingTime = 0; // Time consumed waiting
                }
                break;

            case State::Faulted:
                // Time-consuming action: stay faulted (consume time but do nothing)
                stepStats.faultedTime += remainingTime;
                setCurrentState(State::Faulted); // Remain in Faulted state
                continueProcessing = false; // No further processing needed
                // This could be extended to allow manual repairs or retries in a more complex simulation
                remainingTime = 0; // Time consumed in faulted state
                break;
        }
    }

    // Accumulate step stats into total stats
    totalStats.add(stepStats);
}

double Vehicle::fly(double hours) {
    if (currentState != State::Flying) {
        throw std::runtime_error("Vehicle must be in Flying state to fly");
    }

    if (hours <= 0) {
        return 0.0;
    }

    // Calculate maximum flight time based on available battery
    double maxDistance = batteryLevel / energyUsePerMile;
    double maxFlightTime = maxDistance / cruiseSpeed;

    // Determine actual flight time (either requested time or what battery allows)
    double actualFlightTime = std::min(hours, maxFlightTime);

    if (actualFlightTime <= 0) {
        // No battery left, go to Queued immediately
        setBatteryLevel(0.0);
        setCurrentState(State::Queued);
        return 0.0;
    }

    // Check for faults based on actual flight time
    bool faultOccurred = checkFault(actualFlightTime);
    double flightTimeBeforeFault = actualFlightTime;

    if (faultOccurred) {
        // Assume fault occurs halfway through the flight
        flightTimeBeforeFault = actualFlightTime * 0.5;
    }

    // Calculate flight statistics based on time flown before fault (if any)
    double distanceFlown = cruiseSpeed * flightTimeBeforeFault;
    double energyConsumed = distanceFlown * energyUsePerMile;
    double passengerMiles = distanceFlown * passengerCount;

    // Update battery and statistics
    setBatteryLevel(batteryLevel - energyConsumed);
    stepStats.flightTime += flightTimeBeforeFault;
    stepStats.distanceTraveled += distanceFlown;
    stepStats.passengerMiles += passengerMiles;

    // Handle state transitions
    if (faultOccurred) {
        stepStats.faults++;
        setCurrentState(State::Faulted);
        return flightTimeBeforeFault;
    }

    // Check if battery is depleted after this flight
    if (batteryLevel <= EPSILON) {
        setBatteryLevel(0.0);
        setCurrentState(State::Queued);
    }
    // Otherwise, remain in Flying state

    return actualFlightTime;
}

bool Vehicle::checkFault(double hours) {
    // Use the random number generator to determine if a fault occurs
    // This simulates the probability of a fault occurring that is proportional
    // to the fault probability and the duration of the flight.
    // If a fault occurs, the vehicle will transition to the Faulted state.
    return rng.bernoulli(faultProbability * hours);
}

void Vehicle::startCharging() {
    if (currentState != State::Queued) {
        throw std::runtime_error("Vehicle must be Queued to start charging");
    }
    setCurrentState(State::Charging);
    updateState(0.0); // Check for any follow-up transitions
}

double Vehicle::charge(double hours) {
    if (currentState != State::Charging) {
        throw std::runtime_error("Vehicle must be Charging to charge. Call startCharging() first.");
    }

    if (hours <= 0) {
        return 0.0;
    }

    // Calculate how much energy we can add and how much we actually need
    double chargeRate = batteryCapacity / timeToCharge; // kWh per hour
    double energyNeeded = batteryCapacity - batteryLevel; // How much to reach full
    double maxEnergyCanAdd = chargeRate * hours; // How much we could add with available time

    double actualEnergyToAdd = std::min(energyNeeded, maxEnergyCanAdd);
    double timeActuallyUsed = actualEnergyToAdd / chargeRate;

    setBatteryLevel(batteryLevel + actualEnergyToAdd);
    stepStats.chargingTime += timeActuallyUsed;

    // Check if fully charged and transition to Ready
    if (batteryLevel >= batteryCapacity) {
        setBatteryLevel(batteryCapacity);
        setCurrentState(State::Ready);
    }

    return timeActuallyUsed;
}

/* Other */
std::string Vehicle::getStateString() const {
    switch (currentState) {
        case State::Ready: return "Ready";
        case State::Flying: return "Flying";
        case State::Queued: return "Queued";
        case State::Charging: return "Charging";
        case State::Faulted: return "Faulted";
        default: return "Unknown";
    }
}

std::string Vehicle::getManufacturerString() const {
    switch (manufacturer) {
        case Manufacturer::Alpha: return "Alpha";
        case Manufacturer::Bravo: return "Bravo";
        case Manufacturer::Charlie: return "Charlie";
        case Manufacturer::Delta: return "Delta";
        case Manufacturer::Echo: return "Echo";
        default: return "Unknown";
    }
}