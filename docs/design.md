# eVTOL Simulation Software Design Description (SDD)

## Introduction
This document describes the design of the eVTOL simulation project, a command-line application that models electric vertical takeoff and landing aircraft from multiple manufacturers.

The simulation uses discrete time-stepping to model scenarios where vehicles fly until their batteries are depleted, queue for available charging stations, and return to flight once recharged. Each vehicle type has unique manufacturer-specific characteristics.

This document describes the design of the eVTOL simulation software.

This directory contains project documentation and integrates both high-level architectural concepts described in this SDD and and low-level details automatically generated from the source code via [Doxygen](./doxygen/html/index.html).

### Purpose
This project implements a multi-vehicle eVTOL simulation.

There are five companies developing eVTOL aircraft. The vehicles produced by each manufacturer have different characteristics. We simulate using these vehicle for multiple hours. There are a limited number of chargers available for all vehicles, and a single charger can only be used by one vehicle at a time.

The simulation tracks various statistics per vehicle type and outputs the results at the end of simulation.

### References
- [Requirements Document](requirements.md)
- [Doxygen](./doxygen/html/index.html)
- [C++ Bernoulli Distribution](https://en.cppreference.com/w/cpp/numeric/random/bernoulli_distribution.html)


## System Architecture

### Component Overview
The system consists of several key components:
- **Vehicle Classes** - Base and manufacturer-specific vehicle implementations
- **Simulation Engine** - Core time-stepping simulation
- **Logger System** - Configurable output and reporting
- **Random Number Generation** - To help with fault simulation and vehicle type selection


## Detailed Design

### Vehicle
The `Vehicle` class serves as the base class for eVTOL vehicles. It implements a state machine that models flying, queuing, charging, and faults. There are currently five manufacturer-specific implementations (derived classes). Currently there is no functionality that is unique to particular vehicle manufacturer, but given this design pattern future updates to this simulation would make this possible as well as make adding additional vehicle types straight forward.

#### State Machine

The following is a simple diagram of the Vehicle state machine, which handles both automatic and manual transitions:

```
Ready (Initial) → Flying (Automatic)
                   ↙            OR     ↘
Queued* (When battery depleted)          Faulted* (When fault occurs during flight)
  ↓
Charging (Manual via startCharging())
  ↓
Ready (Automatic when fully charged)
  ↓
Flying (Automatic - cycle repeats)

--

* Requires manual intervention
   Faulted → <Any>: Not yet simulated
   Queued → Charging: Simulation to transition to charging
```

The current Vehicle pattern allows for future extensibility for more complex simulation logic such as additional states and more custom transition logic.

#### Fault Injection
A vehicle can enter the Faulted state based on its manufacturer-defined fault probability and a random number generator which simulates the likelihood of a failure occurring during flight. This fault model uses a Bernoulli distribution, where the fault probability is scaled by the flight duration. For convenience, a default static instance of the random number generator is provided but a 'mock' random number generator can be injected for testability.


#### Statistics
For each vehicle object, statistics are collected in a member structure `stats`. Again, this allows for easy addition of new/additional metrics.

TODO: Time permitting I would write more details about the Vehicle class here, probably create an actual state transition diagram, and also discuss about how to extend it in future (and also do those code updates!).


### Simulation
The `Simulation` class manages the overall simulation execution, coordinating multiple vehicles, charging infrastructure, and time progression.

The simulation operates on a discrete time-stepping model where each iteration represents a configurable time slice. During each step, the simulation processes all vehicles, manages charging resources, and collects statistics before advancing to the next time interval.

#### Time-Stepping Algorithm

The simulation follows this sequence during each discrete time step:

1. **Process Charging Vehicles**: Free up chargers from vehicles that have completed charging
2. **Update All Vehicles**: Each vehicle processes its automatic state transitions based on available time slice
3. **Manage Charging Queue**: Add vehicles needing charge to queue and assign newly available charging stations
4. **Advance Time**: Increment simulation clock

Throughout this duration, it will also collect statistics and log details to console and the simulation report.

The following is a simple flow diagram of the Simulation:

```
      START
        ↓
Process Charging → Update All Vehicles → Manage Charging
    Vehicles                                  Queue
        |                                       ↓
        |                                  Advance Time
        |                                       ↓
        |                                Time < Duration?
        |                                       |
        └-------------------------------Yes-----┴---------- No → DONE
```


TODO: Same, for the Simulation, I would add more details. Also will note here that I think the Simulation class could use refactoring on a longer term project. Right now we have a simple implicit flow. As I wrote the documentation I realized I think it could benefit from similarly being a more explicit state machine with each of the above squares as states if we were to want to support step control and pause/resume simulation. But for the current focus, the simple flow architecture suffices.


## Design Constraints and Context
This design is based on the requirements, assumptions, and simplifications documented in the [Requirements Document](requirements.md).

The current design prioritizes simplicity and clarity over maximum simulation fidelity, focusing on core eVTOL fleet dynamics rather than comprehensive real-world modeling, while still having a high-level architecture supports future enhancements.