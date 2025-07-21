# eVTOL Simulation

## Introduction

This project implements a multi-vehicle eVTOL simulation that models the operational behavior of electric vertical takeoff and landing aircraft from five different manufacturers. The simulation is a command-line application that provides vehicle statistics over configurable time periods.

The simulator uses discrete time-stepping to model scenarios where vehicles fly until their batteries are depleted, queue for available charging stations, and return to flight once recharged. Each vehicle type has unique (manufacturer-specific) characteristics such as cruise speed, battery capacity, and fault probability, creating operational diversity in the fleet/each iteration of the simulation.

## Background

There are five companies developing eVTOL aircraft. The vehicles produced by each manufacturer have different characteristics. We simulate using these vehicle for multiple hours. There are a limited number of chargers available for all vehicles, and a single charger can only be used by one vehicle at a time.

The simulation progresses through discrete time steps (configurable in seconds), during which each vehicle updates its state based on current conditions and available resources. Vehicles follow a state machine transitioning between Ready, Flying, Queued (for charging), Charging, and Faulted states. The simulation includes fault injection based on manufacturer-specific probabilities and tracks detailed statistics.

The output incldues is both a summary of the simulation results to the console as well as a detailed simulation report that provides information for each time step.

## File Structure

* `docs` Contains documentation, including requirements.
* `src` Contains simulation program source files.
* `tests` Contains test files.
* `output` Contains generated simulation log files.

## Usage

### Dependencies
The following are required to be able to build and run this simulation project:
* [cmake](https://cmake.org/download/) (3.10+)

#### Installing Dependencies
On macOS:
```
brew install cmake
```

### Build
To build this simulation project:

1) Clone project.
2) Run `make`.

### Run

#### Simulation
After building, to run the simulation project:
```
./eVTOL_sim
```

#### Tests
To run the simulation tests:
```
./eVTOL_tests
```

### Input
Currently there are no required inputs, however if desired various simulation properties can be configured using command-line options. For more information see the help documentation and examples below.

#### Show help
```
./eVTOL_sim --help
```

#### Using all defaults
```
./eVTOL_sim
```

#### Custom vehicles and duration
```
./eVTOL_sim -v 50 -h 6
```

### Output
The output of the program is a simulation report which includes various statistics per vehicle type. This output is shown both on the console and saved to a timestamped log file in the `output/` directory. Console output provides high-level progress and final results, while the log file, if verbosity is high enough, will also contain detailed step-by-step information including individual vehicle states, charging queue status, and charging station assignments.

## Future Work

The following are various suggested paths for future work associated with this simulation both in terms of improving the current state of the project (there is always more to test...) and further extension of capabilities/fidelity of the simulation.

### Vehicle
* Add more realistic state transition support such as takeoff/landing phases
* Model emergency landing (after fault) and recovery procedures
* Add support for 'flight paths' and route planning between destinations

### Simulation
* Clean up Simulation implementation, including cleaner separation of logging from time stepping flow
* Add a GUI
    * Real-time visualization of vehicle state
    * Implement simulation state pause/resume/saving
* Add support for multiple simulation runs with statistical analysis
* Implement multi-threaded simulation for performance improvements
* Add configuration file support for simulation parameters and also for manufacturer information import
* Implement data export to various formats (CSV, JSON, database)

### Testing
* Add assertions and comprehensive error handling
* Add unit tests for random number generator and logger
* Add unit tests for program input interface
* Add more unit tests for vehicle, simulation, including focus on edge cases/error checking
