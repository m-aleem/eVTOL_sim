#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "simulation.hpp"
#include "vehicle.hpp"
#include <memory>
#include <filesystem>
#include <fstream>

TEST(SimulationTest, Initialization) {
    SCOPED_TRACE("REQ-SIM-001, REQ-SIM-002, REQ-SIM-003, REQ-SIM-004: Verifies constructor arg to parameter assignment.");

    // Test default parameters
    {
        Simulation defaultSim;
        EXPECT_EQ(defaultSim.numVehicles, DEFAULT_NUM_VEHICLES);     // REQ-SIM-001: 20 vehicles
        EXPECT_DOUBLE_EQ(defaultSim.simHours, DEFAULT_HRS_SIM);      // REQ-SIM-002: 3 hours
        EXPECT_EQ(defaultSim.numChargers, DEFAULT_CHARGERS);         // REQ-SIM-003: 3 chargers
        EXPECT_DOUBLE_EQ(defaultSim.simTimeStepSeconds, DEFAULT_TIME_STEP_SECONDS); // REQ-SIM-004: 1 second
    }

    // Test custom parameters
    {
        int testVehicles = 15;
        double testHours = 2.5;
        int testChargers = 5;
        double testTimeStep = 0.5;

        Simulation customSim(testVehicles, testHours, testChargers, testTimeStep);
        EXPECT_EQ(customSim.numVehicles, testVehicles);
        EXPECT_DOUBLE_EQ(customSim.simHours, testHours);
        EXPECT_EQ(customSim.numChargers, testChargers);
        EXPECT_DOUBLE_EQ(customSim.simTimeStepSeconds, testTimeStep);
    }
}

TEST(SimulationTest, CreateVehicles) {
    SCOPED_TRACE("REQ-SIM-005: Verifies create vehicles method supports all manufacturers.");

    Simulation sim(1, 1.0, 1, 1.0);

    // Test creating each vehicle type
    for (int type = 0; type < NUM_VEHICLE_TYPES; ++type) {
        auto vehicle = sim.createVehicle(type);

        EXPECT_NE(vehicle.get(), nullptr);
        EXPECT_EQ(static_cast<int>(vehicle->getManufacturer()), type);

        // Vehicle should start in Ready state with full battery
        EXPECT_EQ(vehicle->getCurrentState(), Vehicle::State::Ready);
        EXPECT_DOUBLE_EQ(vehicle->getBatteryLevel(), vehicle->getBatteryCapacity());
    }
}

TEST(SimulationTest, TimeStep) {
    SCOPED_TRACE("REQ-SIM-006: Verifies discrete time-stepping using nextTimeStep helper method.");

    // Test time step conversion and boundary conditions
    double testTimeStepSeconds = 5.0;
    Simulation sim(1, 1.0, 1, testTimeStepSeconds);

    // Test normal time step calculation using the actual helper method
    sim.currentTime = 0.0;
    double expectedTimeStepHours = testTimeStepSeconds * SECONDS_TO_HOURS;
    double calculatedStep = sim.nextTimeStep();
    EXPECT_DOUBLE_EQ(calculatedStep, expectedTimeStepHours);

    // Test time step when remaining time > time step size
    sim.currentTime = 0.9; // Close to end of 1 hour simulation
    calculatedStep = sim.nextTimeStep();
    EXPECT_LE(calculatedStep, sim.simHours - sim.currentTime);
    EXPECT_GT(calculatedStep, 0.0);
    // Should return time step size (5 seconds)
    EXPECT_DOUBLE_EQ(calculatedStep, expectedTimeStepHours);

    // Test when remaining time < time step size
    sim.currentTime = sim.simHours - (expectedTimeStepHours / 2); // Half a time step from end
    calculatedStep = sim.nextTimeStep();
    double expectedRemainingTime = sim.simHours - sim.currentTime;
    EXPECT_DOUBLE_EQ(calculatedStep, expectedRemainingTime); // Should return remaining time
    EXPECT_LT(calculatedStep, expectedTimeStepHours); // Should be less than full time step

    // Test edge case - exactly at end
    sim.currentTime = sim.simHours;
    calculatedStep = sim.nextTimeStep();
    EXPECT_DOUBLE_EQ(calculatedStep, 0.0);

    // Test with different time step size
    Simulation largeSim(1, 2.0, 1, 10.0); // 10 second steps, 2 hour duration
    largeSim.currentTime = 0.0;
    double largeStep = largeSim.nextTimeStep();
    EXPECT_DOUBLE_EQ(largeStep, 10.0 * SECONDS_TO_HOURS);

    // Test conversion factor accuracy
    EXPECT_DOUBLE_EQ(SECONDS_TO_HOURS, 1.0/3600.0);
    EXPECT_DOUBLE_EQ(3600.0 * SECONDS_TO_HOURS, 1.0); // 1 hour conversion
}

TEST(SimulationTest, ChargingQueue) {
    SCOPED_TRACE("REQ-SIM-007:Verifies charging queue");

    Simulation sim(3, 1.0, 1, 1.0); // 3 vehicles, 1 charger to force queuing
    sim.initializeVehicles();

    // Get references to vehicles for testing
    Vehicle* vehicle1 = sim.vehicles[0].get();
    Vehicle* vehicle2 = sim.vehicles[1].get();
    Vehicle* vehicle3 = sim.vehicles[2].get();

    // Manually add vehicles to charging queue to test FIFO order
    sim.chargingQueue.push(vehicle1);
    sim.chargingQueue.push(vehicle2);
    sim.chargingQueue.push(vehicle3);


    EXPECT_EQ(sim.chargingQueue.front(), vehicle1);
    EXPECT_EQ(sim.chargingQueue.size(), 3);

    Vehicle* first = sim.chargingQueue.front();
    sim.chargingQueue.pop();
    EXPECT_EQ(first, vehicle1);

    Vehicle* second = sim.chargingQueue.front();
    sim.chargingQueue.pop();
    EXPECT_EQ(second, vehicle2);

    Vehicle* third = sim.chargingQueue.front();
    sim.chargingQueue.pop();
    EXPECT_EQ(third, vehicle3);

    EXPECT_TRUE(sim.chargingQueue.empty());
}
