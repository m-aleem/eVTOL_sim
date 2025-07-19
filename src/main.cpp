#include <iostream>
#include <cstdlib>
#include <ctime>
#include <map>
#include <memory>
#include "vehicle.hpp"

const int DEFAULT_NUM_VEHICLES = 20; // Default number of vehicles to create
const int DEFAULT_HRS_SIM = 3; // Default hours for simulation
const int NUM_VEHICLE_TYPES = 5; // Number of different vehicle types

// Factory function for vehicles
std::unique_ptr<Vehicle> createVehicle(int type) {
    switch (type) {
        case 0: return std::make_unique<AlphaCompanyVehicle>();
        case 1: return std::make_unique<BravoCompanyVehicle>();
        case 2: return std::make_unique<CharlieCompanyVehicle>();
        case 3: return std::make_unique<DeltaCompanyVehicle>();
        case 4: return std::make_unique<EchoCompanyVehicle>();
        // Add new cases for new types here
        default: return nullptr;
    }
}

int main(int argc, char* argv[]) {
    std::srand(static_cast<unsigned int>(std::time(nullptr))); // Seed RNG

    int numVehicles = DEFAULT_NUM_VEHICLES; // Default number of vehicles
    if (argc > 1) {
        numVehicles = std::atoi(argv[1]);
        if (numVehicles <= 0) numVehicles = DEFAULT_NUM_VEHICLES;
    }

    std::map<std::string, int> vehicleCounts;


    // Print out the input parameters
    std::cout << "Simulation Inputs:" << std::endl;
    std::cout << "  Number of vehicles: " << numVehicles << std::endl;
    std::cout << "  Simulation hours: " << DEFAULT_HRS_SIM << std::endl;


    // Generate vehicles objects
    for(int i = 0; i < numVehicles; ++i) {
        int type = std::rand() % NUM_VEHICLE_TYPES; // Randomly select vehicle type
        auto vehicle = createVehicle(type);
        if (vehicle) {
            std::string name = vehicle->getManufacturerString();
            ++vehicleCounts[name];
        }
    }

    std::cout << "\nVehicle type counts:" << std::endl;
    int totalVehicles = 0;
    for (const auto& pair : vehicleCounts) {
        std::cout << "  " << pair.first << ": " << pair.second << std::endl;
        totalVehicles += pair.second;
    }
    std::cout << " Total vehicles: " << totalVehicles << std::endl;

    return 0;
}
