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
    EXPECT_DOUBLE_EQ(vehicle->getStatistics().totalFlightTime, 0.0);
    EXPECT_DOUBLE_EQ(vehicle->getStatistics().totalDistanceTraveled, 0.0);
    EXPECT_DOUBLE_EQ(vehicle->getStatistics().totalChargingTime, 0.0);
    EXPECT_EQ(vehicle->getStatistics().totalFaults, 0);
    EXPECT_DOUBLE_EQ(vehicle->getStatistics().totalPassengerMiles, 0.0);
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
