# eVTOL Simulation Software Design Description (SDD)

## 1. Introduction
This document describes the design of the eVTOL simulation software.

This directory contains project documentation and integrates both high-level architectural concepts described in this SDD and and low-level details automatically generated from the source code via [Doxygen](./doxygen/html/index.html).

### 1.1 Purpose
This project implements a multi-vehicle eVTOL simulation.

There are five companies developing eVTOL aircraft. The vehicles produced by each manufacturer have different characteristics. We simulate using these vehicle for multiple hours. There are a limited number of chargers available for all vehicles, and a single charger can only be used by one vehicle at a time.

The simulation tracks various statistics per vehicle type and outputs the results at the end of simulation.

### 1.2 References
- [Doxygen](./doxygen/html/index.html)

---

## 2. Requirements

TODO: I would iterate with the appropriate stakeholders (such as vehicle engineers, systems engineers, simulation customers, etc.) on these requirements to ensure all desired functionality is captured and update the code accordingly. These requirements are based on the brief as provided as well as my own assumptions (documented in the next section).

### Vehicle

### Vehicle Requirements

| Requirement ID   | Requirement Text                                                                 |
|------------------|----------------------------------------------------------------------------------|
| REQ-VEHICLE-1    | Each vehicle shall be associated with a specific manufacturer.                 |
| REQ-VEHICLE-2    | Each vehicle type shall define six properties: Cruise Speed (mph), Battery Capacity (kWh), Time to Charge (hours), Energy use at Cruise (kWh/mile), Passenger Count, Probability of fault per hour. |
| REQ-VEHICLE-3    | Each vehicle shall maintain and update its operational state (e.g., Flying, Queued) and current battery level. |
| REQ-VEHICLE-4    | Each vehicle shall queue for charging immediately when battery is depleted. |
| REQ-VEHICLE-5    | Each vehicle shall record statistics: flight time, distance traveled, charging time, faults, and passenger miles. |
| REQ-VEHICLE-6    | Each vehicle shall simulate faults during flight, based on probability per hour.  |
| REQ-VEHICLE-7    | Each vehicle shall start the simulation with a fully-charged battery.            |

### Simulation

| Requirement ID   | Requirement Text                                                                 |
|------------------|----------------------------------------------------------------------------------|
| REQ-SIM-1   | The simulation shall support random assignment of vehicle types, totaling a default 20 (configurable) vehicles across all types. |

---

## 3. Assumptions and Simplifications
The following assumptions and simplifications are made in the current state of the project:

* Each vehicle starts the simulation with a fully-charged battery.
* Each vehicle instantaneously reaches Cruise Speed.
* Each vehicle is airborne for the full use of the battery, and is immediately in line for the charger after running out of battery power.
* Each vehicle simply flies at constant cruise speed until out of charge, in other words there is no flight route or paths to consider.
* A vehicle fault can occur only when flying and for statistics purposes assume the fault occurs half way through the iteration.

---

## 4. Detailed Design

# Vehicle

```
Ready (Initial) → Flying (Automatic)
                   ↙     OR     ↘
Queued (When battery depleted)   Faulted* (When fault occurs during flight)
↓
Charging (Manual via startCharging())
↓
Ready (Automatic when fully charged)
↓
Flying (Automatic - cycle repeats)

* Faulted: Requires manual intervention (not yet simulated)
```

TODO: Time permitting I would write more details about the Vehicle class here, probably create an actual state transition diagram, and also discuss about how to extend it in future.

# Simulation
TODO: Same, for the Simulation