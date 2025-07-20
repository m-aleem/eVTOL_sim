#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "vehicle.hpp"
#include "interface_rng.hpp"
#include <stdexcept>

class MockRandomGenerator : public RandomGenerator {
    public:
        MOCK_METHOD(bool, bernoulli, (double p), (override));
        MOCK_METHOD(int, uniformInt, (int min, int max), (override));
};


class VehicleTest : public ::testing::Test {
protected:
    MockRandomGenerator mockRng;

    AlphaCompanyVehicle* alpha;
    BravoCompanyVehicle* bravo;
    CharlieCompanyVehicle* charlie;
    DeltaCompanyVehicle* delta;
    EchoCompanyVehicle* echo;

    void SetUp() override {
        // By default, no faults will occur during tests
        EXPECT_CALL(mockRng, bernoulli(::testing::_))
            .WillRepeatedly(::testing::Return(false));

        // Pass the mock RNG to each vehicle
        alpha   = new AlphaCompanyVehicle(mockRng);
        bravo   = new BravoCompanyVehicle(mockRng);
        charlie = new CharlieCompanyVehicle(mockRng);
        delta   = new DeltaCompanyVehicle(mockRng);
        echo    = new EchoCompanyVehicle(mockRng);
    }

    void TearDown() override {
        delete alpha;
        delete bravo;
        delete charlie;
        delete delta;
        delete echo;

        alpha = nullptr;
        bravo = nullptr;
        charlie = nullptr;
        delta = nullptr;
        echo = nullptr;
    }
};


class VehicleTestFault : public ::testing::Test {
protected:
    MockRandomGenerator mockRng;

    AlphaCompanyVehicle* alpha;
    BravoCompanyVehicle* bravo;
    CharlieCompanyVehicle* charlie;
    DeltaCompanyVehicle* delta;
    EchoCompanyVehicle* echo;

    void SetUp() override {
        // Faults will occur during these tests
        EXPECT_CALL(mockRng, bernoulli(::testing::_))
            .WillRepeatedly(::testing::Return(true));

        // Pass the mock RNG to each vehicle
        alpha   = new AlphaCompanyVehicle(mockRng);
        bravo   = new BravoCompanyVehicle(mockRng);
        charlie = new CharlieCompanyVehicle(mockRng);
        delta   = new DeltaCompanyVehicle(mockRng);
        echo    = new EchoCompanyVehicle(mockRng);
    }

    void TearDown() override {
        delete alpha;
        delete bravo;
        delete charlie;
        delete delta;
        delete echo;

        alpha = nullptr;
        bravo = nullptr;
        charlie = nullptr;
        delta = nullptr;
        echo = nullptr;
    }
};


class VehicleParameterizedTest : public ::testing::TestWithParam<std::string> {
protected:
    MockRandomGenerator mockRng;
    Vehicle* vehicle;

    void SetUp() override {
        // By default, no faults will occur during tests
        EXPECT_CALL(mockRng, bernoulli(::testing::_))
            .WillRepeatedly(::testing::Return(false));

        // Create the appropriate vehicle based on the parameter
        std::string vehicleType = GetParam();
        if (vehicleType == "Alpha") {
            vehicle = new AlphaCompanyVehicle(mockRng);
        } else if (vehicleType == "Bravo") {
            vehicle = new BravoCompanyVehicle(mockRng);
        } else if (vehicleType == "Charlie") {
            vehicle = new CharlieCompanyVehicle(mockRng);
        } else if (vehicleType == "Delta") {
            vehicle = new DeltaCompanyVehicle(mockRng);
        } else if (vehicleType == "Echo") {
            vehicle = new EchoCompanyVehicle(mockRng);
        }
    }

    void TearDown() override {
        delete vehicle;
        vehicle = nullptr;
    }
};


class VehicleParameterizedTestFault : public ::testing::TestWithParam<std::string> {
protected:
    MockRandomGenerator mockRng;
    Vehicle* vehicle;

    void SetUp() override {
        // Faults will occur during these tests
        EXPECT_CALL(mockRng, bernoulli(::testing::_))
            .WillRepeatedly(::testing::Return(true));

        // Create the appropriate vehicle based on the parameter
        std::string vehicleType = GetParam();
        if (vehicleType == "Alpha") {
            vehicle = new AlphaCompanyVehicle(mockRng);
        } else if (vehicleType == "Bravo") {
            vehicle = new BravoCompanyVehicle(mockRng);
        } else if (vehicleType == "Charlie") {
            vehicle = new CharlieCompanyVehicle(mockRng);
        } else if (vehicleType == "Delta") {
            vehicle = new DeltaCompanyVehicle(mockRng);
        } else if (vehicleType == "Echo") {
            vehicle = new EchoCompanyVehicle(mockRng);
        }
    }

    void TearDown() override {
        delete vehicle;
        vehicle = nullptr;
    }
};


INSTANTIATE_TEST_SUITE_P(
    AllVehicleTypes,
    VehicleParameterizedTest,
    ::testing::Values("Alpha", "Bravo", "Charlie", "Delta", "Echo")
);


INSTANTIATE_TEST_SUITE_P(
    AllVehicleTypes,
    VehicleParameterizedTestFault,
    ::testing::Values("Alpha", "Bravo", "Charlie", "Delta", "Echo")
);

/* Initialization Tests */
TEST_P(VehicleParameterizedTest, InitialStateIsReady) {
    SCOPED_TRACE("REQ-VEHICLE-3: Verifies vehicle starts in Ready state.");
    EXPECT_EQ(vehicle->getCurrentState(), Vehicle::State::Ready);
}

TEST_P(VehicleParameterizedTest, InitialBatteryLevelIsFull) {
    SCOPED_TRACE("REQ-VEHICLE-3, REQ-VEHICLE-7: Verifies initial battery level is set to full capacity.");
    EXPECT_DOUBLE_EQ(vehicle->getBatteryLevel(), vehicle->getBatteryCapacity());
}

TEST_P(VehicleParameterizedTest, InitialStatisticsAreZero) {
    SCOPED_TRACE("REQ-VEHICLE-5: Verifies all statistics are initialized to zero at start.");

    // Test total stats are zero
    const auto& totalStats = vehicle->getTotalStats();
    EXPECT_DOUBLE_EQ(totalStats.flightTime, 0.0);
    EXPECT_DOUBLE_EQ(totalStats.distanceTraveled, 0.0);
    EXPECT_DOUBLE_EQ(totalStats.chargingTime, 0.0);
    EXPECT_DOUBLE_EQ(totalStats.queuedTime, 0.0);
    EXPECT_DOUBLE_EQ(totalStats.faultedTime, 0.0);
    EXPECT_EQ(totalStats.faults, 0);
    EXPECT_DOUBLE_EQ(totalStats.passengerMiles, 0.0);

    // Test step stats are zero
    const auto& stepStats = vehicle->getStepStats();
    EXPECT_DOUBLE_EQ(stepStats.flightTime, 0.0);
    EXPECT_DOUBLE_EQ(stepStats.distanceTraveled, 0.0);
    EXPECT_DOUBLE_EQ(stepStats.chargingTime, 0.0);
    EXPECT_DOUBLE_EQ(stepStats.queuedTime, 0.0);
    EXPECT_DOUBLE_EQ(stepStats.faultedTime, 0.0);
    EXPECT_EQ(stepStats.faults, 0);
    EXPECT_DOUBLE_EQ(stepStats.passengerMiles, 0.0);

    // Test individual getter methods (for backward compatibility)
    EXPECT_DOUBLE_EQ(vehicle->getTotalFlightTime(), 0.0);
    EXPECT_DOUBLE_EQ(vehicle->getTotalDistanceTraveled(), 0.0);
    EXPECT_DOUBLE_EQ(vehicle->getTotalChargingTime(), 0.0);
    EXPECT_DOUBLE_EQ(vehicle->getTotalQueuedTime(), 0.0);
    EXPECT_DOUBLE_EQ(vehicle->getTotalFaultedTime(), 0.0);
    EXPECT_EQ(vehicle->getTotalFaults(), 0);
    EXPECT_DOUBLE_EQ(vehicle->getTotalPassengerMiles(), 0.0);
}

TEST_F(VehicleTest, ManufacturerSpecificProperties) {
    SCOPED_TRACE("REQ-VEHICLE-1, REQ-VEHICLE-2: Verifies manufacturer and six distinct properties for each vehicle type.");
    EXPECT_EQ(alpha->getManufacturer(), Vehicle::Manufacturer::Alpha);
    EXPECT_EQ(alpha->getCruiseSpeed(), 120);
    EXPECT_EQ(alpha->getBatteryCapacity(), 320);
    EXPECT_EQ(alpha->getTimeToCharge(), 0.6);
    EXPECT_EQ(alpha->getEnergyUsePerMile(), 1.6);
    EXPECT_EQ(alpha->getPassengerCount(), 4);
    EXPECT_EQ(alpha->getFaultProbability(), 0.25);

    EXPECT_EQ(bravo->getManufacturer(), Vehicle::Manufacturer::Bravo);
    EXPECT_EQ(bravo->getCruiseSpeed(), 100);
    EXPECT_EQ(bravo->getBatteryCapacity(), 100);
    EXPECT_EQ(bravo->getTimeToCharge(), 0.2);
    EXPECT_EQ(bravo->getEnergyUsePerMile(), 1.5);
    EXPECT_EQ(bravo->getPassengerCount(), 5);
    EXPECT_EQ(bravo->getFaultProbability(), 0.10);

    EXPECT_EQ(charlie->getManufacturer(), Vehicle::Manufacturer::Charlie);
    EXPECT_EQ(charlie->getCruiseSpeed(), 160);
    EXPECT_EQ(charlie->getBatteryCapacity(), 220);
    EXPECT_EQ(charlie->getTimeToCharge(), 0.8);
    EXPECT_EQ(charlie->getEnergyUsePerMile(), 2.2);
    EXPECT_EQ(charlie->getPassengerCount(), 3);
    EXPECT_EQ(charlie->getFaultProbability(), 0.05);

    EXPECT_EQ(delta->getManufacturer(), Vehicle::Manufacturer::Delta);
    EXPECT_EQ(delta->getCruiseSpeed(), 90);
    EXPECT_EQ(delta->getBatteryCapacity(), 120);
    EXPECT_EQ(delta->getTimeToCharge(), 0.62);
    EXPECT_EQ(delta->getEnergyUsePerMile(), 0.8);
    EXPECT_EQ(delta->getPassengerCount(), 2);
    EXPECT_EQ(delta->getFaultProbability(), 0.22);

    EXPECT_EQ(echo->getManufacturer(), Vehicle::Manufacturer::Echo);
    EXPECT_EQ(echo->getCruiseSpeed(), 30);
    EXPECT_EQ(echo->getBatteryCapacity(), 150);
    EXPECT_EQ(echo->getTimeToCharge(), 0.3);
    EXPECT_EQ(echo->getEnergyUsePerMile(), 5.8);
    EXPECT_EQ(echo->getPassengerCount(), 2);
    EXPECT_EQ(echo->getFaultProbability(), 0.61);
}

/* State Transition Tests */

TEST_P(VehicleParameterizedTest, ReadyToFlyingZero) {
    SCOPED_TRACE("REQ-VEHICLE-3: Verifies transition from Ready to Flying state when starting a flight with time zero.");
    EXPECT_EQ(vehicle->getCurrentState(), Vehicle::State::Ready);
    vehicle->updateState(0.0);
    EXPECT_EQ(vehicle->getCurrentState(), Vehicle::State::Flying);
}

TEST_P(VehicleParameterizedTest, ReadyToFlyingSmall) {
    SCOPED_TRACE("REQ-VEHICLE-3: Verifies transition from Ready to Flying state when starting a flight with time greater than 0 but not enough to deplete battery.");

    // Start at Ready
    EXPECT_EQ(vehicle->getCurrentState(), Vehicle::State::Ready);

    // Fly but not enough to deplete battery
    double time_delta =  0.1;
    double max_flight_time = vehicle->getMaxFlightTime();
    double flight_time = max_flight_time - time_delta;
    vehicle->updateState(flight_time);

    // Should still be in flying
    EXPECT_EQ(vehicle->getCurrentState(), Vehicle::State::Flying);

    // Statistics should be updated
    // TODO: These statics do math on double values, so we need to use EXPECT_NEAR
    // to compare and I am using the same EPSILON value as in the original code for Vehicle
    // but as noted this would be a tolerance discussed with the team
    EXPECT_NEAR(vehicle->getTotalFlightTime(), flight_time, EPSILON);
    EXPECT_NEAR(vehicle->getTotalDistanceTraveled(), flight_time * vehicle->getCruiseSpeed(), EPSILON);
    EXPECT_NEAR(vehicle->getTotalQueuedTime(), 0, EPSILON);
    EXPECT_NEAR(vehicle->getTotalChargingTime(), 0, EPSILON);
    EXPECT_NEAR(vehicle->getTotalPassengerMiles(), flight_time * vehicle->getPassengerCount() * vehicle->getCruiseSpeed(), EPSILON);
    EXPECT_NEAR(vehicle->getTotalFaultedTime(), 0, EPSILON);
    EXPECT_EQ(vehicle->getTotalFaults(), 0);
}

TEST_P(VehicleParameterizedTest, ReadyToFlyingToQueued) {
    SCOPED_TRACE("REQ-VEHICLE-3, REQ-VEHICLE-4: Verifies transition from Ready to Flying state to Queued when starting a flight with time greater than 0 and enough to deplete battery.");
    // Start at Ready
    EXPECT_EQ(vehicle->getCurrentState(), Vehicle::State::Ready);

    // Fly but not enough to deplete battery
    double time_delta =  0.05;
    double max_flight_time = vehicle->getMaxFlightTime();
    double flight_time = max_flight_time + time_delta;
    vehicle->updateState(flight_time);

    // Should now be in queued state
    EXPECT_EQ(vehicle->getCurrentState(), Vehicle::State::Queued);

    // Statistics should be updated
    EXPECT_NEAR(vehicle->getTotalFlightTime(), max_flight_time, EPSILON);
    EXPECT_NEAR(vehicle->getTotalDistanceTraveled(), max_flight_time * vehicle->getCruiseSpeed(), EPSILON);
    EXPECT_NEAR(vehicle->getTotalQueuedTime(), time_delta, EPSILON);
    EXPECT_NEAR(vehicle->getTotalChargingTime(), 0, EPSILON);
    EXPECT_NEAR(vehicle->getTotalPassengerMiles(), max_flight_time * vehicle->getPassengerCount() * vehicle->getCruiseSpeed(), EPSILON);
    EXPECT_NEAR(vehicle->getTotalFaultedTime(), 0, EPSILON);
    EXPECT_EQ(vehicle->getTotalFaults(), 0);

    // Now try to fly again, should remain in Queued state
    double flight_time2 = 0.2;
    vehicle->updateState(flight_time2);
    EXPECT_EQ(vehicle->getCurrentState(), Vehicle::State::Queued);
    EXPECT_NEAR(vehicle->getTotalFlightTime(), max_flight_time, EPSILON);
    EXPECT_NEAR(vehicle->getTotalDistanceTraveled(), max_flight_time * vehicle->getCruiseSpeed(), EPSILON);
    EXPECT_NEAR(vehicle->getTotalQueuedTime(), time_delta + flight_time2, EPSILON);
    EXPECT_NEAR(vehicle->getTotalChargingTime(), 0, EPSILON);
    EXPECT_NEAR(vehicle->getTotalPassengerMiles(), max_flight_time * vehicle->getPassengerCount() * vehicle->getCruiseSpeed(), EPSILON);
    EXPECT_NEAR(vehicle->getTotalFaultedTime(), 0, EPSILON);
    EXPECT_EQ(vehicle->getTotalFaults(), 0);

}

TEST_P(VehicleParameterizedTestFault, ReadyToFaulted) {
    SCOPED_TRACE("REQ-VEHICLE-3, REQ-VEHICLE-6: Verifies transition from Ready to Faulted state.");

    // Start at Ready
    EXPECT_EQ(vehicle->getCurrentState(), Vehicle::State::Ready);

    double flight_time = vehicle->getMaxFlightTime();
    vehicle->updateState(flight_time);

    // Should still be in flying
    EXPECT_EQ(vehicle->getCurrentState(), Vehicle::State::Faulted);

    // Statistics should be updated with assumption that fault occurs halfway through flight
    EXPECT_NEAR(vehicle->getTotalFlightTime(), flight_time/2, EPSILON);
    EXPECT_NEAR(vehicle->getTotalDistanceTraveled(), (flight_time * vehicle->getCruiseSpeed())/2, EPSILON);
    EXPECT_NEAR(vehicle->getTotalQueuedTime(), 0, EPSILON);
    EXPECT_NEAR(vehicle->getTotalChargingTime(), 0, EPSILON);
    EXPECT_NEAR(vehicle->getTotalPassengerMiles(), (flight_time * vehicle->getPassengerCount() * vehicle->getCruiseSpeed())/2, EPSILON);
    EXPECT_NEAR(vehicle->getTotalFaultedTime(), flight_time/2, EPSILON);
    EXPECT_EQ(vehicle->getTotalFaults(), 1);

    // Now try to fly again, should remain in Faulted state
    double flight_time2 = 0.5;
    vehicle->updateState(flight_time2);
    EXPECT_EQ(vehicle->getCurrentState(), Vehicle::State::Faulted);
    EXPECT_NEAR(vehicle->getTotalFlightTime(), flight_time/2, EPSILON);
    EXPECT_NEAR(vehicle->getTotalDistanceTraveled(), (flight_time * vehicle->getCruiseSpeed())/2, EPSILON);
    EXPECT_NEAR(vehicle->getTotalQueuedTime(), 0, EPSILON);
    EXPECT_NEAR(vehicle->getTotalChargingTime(), 0, EPSILON);
    EXPECT_NEAR(vehicle->getTotalPassengerMiles(), (flight_time * vehicle->getPassengerCount() * vehicle->getCruiseSpeed())/2, EPSILON);
    EXPECT_NEAR(vehicle->getTotalFaultedTime(), flight_time/2. + flight_time2, EPSILON);
    EXPECT_EQ(vehicle->getTotalFaults(), 1);

}

// TODO: Add tests for Queued and then get called to charge scenarios
// TODO: Add tests for Charging complete then Ready and Flying scenarios

TEST_P(VehicleParameterizedTest, StepAndTotalStatsAccumulation) {
    SCOPED_TRACE("REQ-VEHICLE-5: Verifies step stats are properly accumulated into total stats.");

    // Perform first update - fly for some time
    double firstFlightTime = 0.5; // 30 minutes
    vehicle->updateState(firstFlightTime);

    // Check that step stats contain only the latest operation
    const auto& stepStats1 = vehicle->getStepStats();
    EXPECT_GT(stepStats1.flightTime, 0.0);
    EXPECT_GT(stepStats1.distanceTraveled, 0.0);
    EXPECT_GT(stepStats1.passengerMiles, 0.0);

    // Check that total stats accumulated the first step
    const auto& totalStats1 = vehicle->getTotalStats();
    double firstFlightActual = stepStats1.flightTime;
    double firstDistanceActual = stepStats1.distanceTraveled;
    double firstPassengerMilesActual = stepStats1.passengerMiles;

    EXPECT_DOUBLE_EQ(totalStats1.flightTime, firstFlightActual);
    EXPECT_DOUBLE_EQ(totalStats1.distanceTraveled, firstDistanceActual);
    EXPECT_DOUBLE_EQ(totalStats1.passengerMiles, firstPassengerMilesActual);

    // Perform second update - fly some more
    double secondFlightTime = 0.3; // 18 minutes
    vehicle->updateState(secondFlightTime);

    // Check that step stats contain only the second operation (should be reset)
    const auto& stepStats2 = vehicle->getStepStats();
    EXPECT_GT(stepStats2.flightTime, 0.0);
    EXPECT_LT(stepStats2.flightTime, firstFlightActual); // Should be less than first flight
    EXPECT_GT(stepStats2.distanceTraveled, 0.0);
    EXPECT_GT(stepStats2.passengerMiles, 0.0);

    // Check that total stats accumulated both steps
    const auto& totalStats2 = vehicle->getTotalStats();
    EXPECT_DOUBLE_EQ(totalStats2.flightTime, firstFlightActual + stepStats2.flightTime);
    EXPECT_DOUBLE_EQ(totalStats2.distanceTraveled, firstDistanceActual + stepStats2.distanceTraveled);
    EXPECT_DOUBLE_EQ(totalStats2.passengerMiles, firstPassengerMilesActual + stepStats2.passengerMiles);

    // Verify step stats are isolated from total stats
    EXPECT_NE(stepStats2.flightTime, totalStats2.flightTime);
    EXPECT_NE(stepStats2.distanceTraveled, totalStats2.distanceTraveled);
    EXPECT_NE(stepStats2.passengerMiles, totalStats2.passengerMiles);
}

TEST_P(VehicleParameterizedTest, StepStatsResetBetweenUpdates) {
    SCOPED_TRACE("REQ-VEHICLE-5: Verifies step stats are reset between updateState calls.");

    // Perform first flight
    double flightTime = 0.5;
    vehicle->updateState(flightTime);

    // Capture step stats values after first update
    double firstStepFlightTime = vehicle->getStepStats().flightTime;
    double firstStepDistance = vehicle->getStepStats().distanceTraveled;
    double firstStepPassengerMiles = vehicle->getStepStats().passengerMiles;

    // Verify step stats have values from first update
    EXPECT_GT(firstStepFlightTime, 0.0);
    EXPECT_GT(firstStepDistance, 0.0);
    EXPECT_GT(firstStepPassengerMiles, 0.0);

    // Perform another update
    vehicle->updateState(0.3);

    // Check step stats after second update - should be different values (reset and recalculated)
    double secondStepFlightTime = vehicle->getStepStats().flightTime;
    double secondStepDistance = vehicle->getStepStats().distanceTraveled;
    double secondStepPassengerMiles = vehicle->getStepStats().passengerMiles;

    // Step stats should be reset and contain only the second update's values
    EXPECT_GT(secondStepFlightTime, 0.0);
    EXPECT_GT(secondStepDistance, 0.0);
    EXPECT_GT(secondStepPassengerMiles, 0.0);

    // Total stats should contain accumulated values from both updates
    const auto& totalStats = vehicle->getTotalStats();
    EXPECT_DOUBLE_EQ(totalStats.flightTime, firstStepFlightTime + secondStepFlightTime);
    EXPECT_DOUBLE_EQ(totalStats.distanceTraveled, firstStepDistance + secondStepDistance);
    EXPECT_DOUBLE_EQ(totalStats.passengerMiles, firstStepPassengerMiles + secondStepPassengerMiles);
}

// Test fixture for VehicleStats struct
class VehicleStatsTest : public ::testing::Test {
protected:
    VehicleStats stats1;
    VehicleStats stats2;

    void SetUp() override {
        // Initialize stats1 with some values
        stats1.flightTime = 10.0;
        stats1.queuedTime = 2.0;
        stats1.distanceTraveled = 100.0;
        stats1.chargingTime = 5.0;
        stats1.faultedTime = 1.0;
        stats1.faults = 2;
        stats1.passengerMiles = 400.0;

        // Initialize stats2 with different values
        stats2.flightTime = 5.0;
        stats2.queuedTime = 1.0;
        stats2.distanceTraveled = 50.0;
        stats2.chargingTime = 3.0;
        stats2.faultedTime = 0.5;
        stats2.faults = 1;
        stats2.passengerMiles = 200.0;
    }
};

TEST_F(VehicleStatsTest, ResetMethod) {
    SCOPED_TRACE("Verifies VehicleStats reset() method clears all fields to zero.");

    // Verify stats1 has non-zero values
    EXPECT_GT(stats1.flightTime, 0.0);
    EXPECT_GT(stats1.faults, 0);

    // Reset and verify all fields are zero
    stats1.reset();

    EXPECT_DOUBLE_EQ(stats1.flightTime, 0.0);
    EXPECT_DOUBLE_EQ(stats1.queuedTime, 0.0);
    EXPECT_DOUBLE_EQ(stats1.distanceTraveled, 0.0);
    EXPECT_DOUBLE_EQ(stats1.chargingTime, 0.0);
    EXPECT_DOUBLE_EQ(stats1.faultedTime, 0.0);
    EXPECT_EQ(stats1.faults, 0);
    EXPECT_DOUBLE_EQ(stats1.passengerMiles, 0.0);
}

TEST_F(VehicleStatsTest, AddMethod) {
    SCOPED_TRACE("Verifies VehicleStats add() method correctly accumulates values.");

    // Save original values
    double originalFlightTime = stats1.flightTime;
    double originalDistance = stats1.distanceTraveled;
    int originalFaults = stats1.faults;

    // Add stats2 to stats1
    stats1.add(stats2);

    // Verify accumulation
    EXPECT_DOUBLE_EQ(stats1.flightTime, originalFlightTime + stats2.flightTime);
    EXPECT_DOUBLE_EQ(stats1.queuedTime, 2.0 + 1.0);
    EXPECT_DOUBLE_EQ(stats1.distanceTraveled, originalDistance + stats2.distanceTraveled);
    EXPECT_DOUBLE_EQ(stats1.chargingTime, 5.0 + 3.0);
    EXPECT_DOUBLE_EQ(stats1.faultedTime, 1.0 + 0.5);
    EXPECT_EQ(stats1.faults, originalFaults + stats2.faults);
    EXPECT_DOUBLE_EQ(stats1.passengerMiles, 400.0 + 200.0);

    // Verify stats2 is unchanged
    EXPECT_DOUBLE_EQ(stats2.flightTime, 5.0);
    EXPECT_EQ(stats2.faults, 1);
}