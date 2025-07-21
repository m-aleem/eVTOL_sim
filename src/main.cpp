#include "simulation.hpp"
#include <iostream>
#include <string>
#include <cstdlib>

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]\n";
    std::cout << "\nOptions:\n";
    std::cout << "  -v, --vehicles <num>     Number of vehicles (default: " << DEFAULT_NUM_VEHICLES << ")\n";
    std::cout << "  -h, --hours <hours>      Simulation duration in hours, decimal values supported (default: " << DEFAULT_HRS_SIM << ")\n";
    std::cout << "  -c, --chargers <num>     Number of charging stations (default: " << DEFAULT_CHARGERS << ")\n";
    std::cout << "  -t, --timestep <sec>     Time step in seconds (default: " << DEFAULT_TIME_STEP_SECONDS << ")\n";
    std::cout << "  -l, --logVerbosity <num> Log verbosity level [1, 2] (default: " << DEFAULT_VERBOSITY << ")\n";
    std::cout << "                           This controls verbosity in log output file (not to console).\n";
    std::cout << "                           Recommend to use > 1 only for short/smaller simulations and\n";
    std::cout << "                           debugging. \n";
    std::cout << "  -e, --equal              Use equal distribution instead of random (default: random)\n";
    std::cout << "  --help                   Show this help message\n";
    std::cout << "\nExamples:\n";
    std::cout << "  " << programName << "                              # Use all defaults\n";
    std::cout << "  " << programName << " -v 50 -h 6                   # 50 vehicles, 6 hours simulation\n";
    std::cout << "  " << programName << " --vehicles 30 --chargers 5   # 30 vehicles, 5 chargers\n";
    std::cout << "  " << programName << " -v 10 -h 4.5 -c 8 -t 0.5     # 10 vehicles, 4.5 hours, 8 chargers, 0.5s timestep\n";
}

int main(int argc, char* argv[]) {
    // Default values
    int numVehicles = DEFAULT_NUM_VEHICLES;
    double simHours = DEFAULT_HRS_SIM;
    int numChargers = DEFAULT_CHARGERS;
    double simTimeStepSeconds = DEFAULT_TIME_STEP_SECONDS;
    int simLogVerbosity = DEFAULT_VERBOSITY;
    bool randomizeVehicles = true;

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "--help") {
            printUsage(argv[0]);
            return 0;
        }
        else if ((arg == "-v" || arg == "--vehicles") && i + 1 < argc) {
            numVehicles = std::atoi(argv[++i]);
            if (numVehicles <= 0) {
                std::cerr << "Error: Number of vehicles must be positive\n";
                return 1;
            }
        }
        else if ((arg == "-h" || arg == "--hours") && i + 1 < argc) {
            simHours = std::atof(argv[++i]);
            if (simHours <= 0) {
                std::cerr << "Error: Simulation hours must be positive\n";
                return 1;
            }
        }
        else if ((arg == "-c" || arg == "--chargers") && i + 1 < argc) {
            numChargers = std::atoi(argv[++i]);
            if (numChargers <= 0) {
                std::cerr << "Error: Number of chargers must be positive\n";
                return 1;
            }
        }
        else if ((arg == "-t" || arg == "--timestep") && i + 1 < argc) {
            simTimeStepSeconds = std::atof(argv[++i]);
            if (simTimeStepSeconds <= 0) {
                std::cerr << "Error: Time step must be positive\n";
                return 1;
            }
        }
        else if ((arg == "-l" || arg == "--logVerbosity") && i + 1 < argc) {
            simLogVerbosity = std::atoi(argv[++i]);
            if (simLogVerbosity <= 0) {
                std::cerr << "Error: Time step must be positive\n";
                return 1;
            }
        }
        else if (arg == "-e" || arg == "--equal") {
            randomizeVehicles = false;
        }
        else {
            std::cerr << "Error: Unknown argument '" << arg << "'\n";
            std::cerr << "Use --help for usage information\n";
            return 1;
        }
    }

    // Create and run simulation with parsed parameters
    Simulation simulation(numVehicles, simHours, numChargers, simTimeStepSeconds, simLogVerbosity, randomizeVehicles);
    simulation.runSimulation();

    return 0;
}
