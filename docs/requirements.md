# eVTOL Simulation Requirements

## 1. Introduction

This document specifies the requirements for the eVTOL simulation and also documentions assumptions and constraints.

## 2. Requirements

### 2.1 Vehicle Requirements

| Requirement ID | Requirement Text |
|----------------|------------------|
| REQ-VEH-001 | Each vehicle shall be associated with a specific manufacturer |
| REQ-VEH-002 | Each vehicle type shall define cruise speed (mph), battery capacity (kWh), time to charge (hours), energy use at cruise (kWh/mile), passenger count, and probability of fault per hour |
| REQ-VEH-003 | Each vehicle shall maintain and update its operational state (Ready, Flying, Queued, Charging, Faulted) and current battery level |
| REQ-VEH-004 | Each vehicle shall queue for charging immediately when battery is depleted |
| REQ-VEH-005 | Each vehicle shall record statistics: flight time, distance traveled, charging time, faults, and passenger miles |
| REQ-VEH-006 | Each vehicle shall simulate faults during flight based on manufacturer-specific probability per hour |
| REQ-VEH-007 | Each vehicle shall start the simulation with a fully-charged battery |

### 2.2 Simulation Requirements

| Requirement ID | Requirement Text |
|----------------|------------------|
| REQ-SIM-001 | The simulation shall support configurable number of vehicles (default: 20) |
| REQ-SIM-002 | The simulation shall support configurable simulation duration in hours (default: 3) |
| REQ-SIM-003 | The simulation shall support configurable number of charging stations (default: 3) |
| REQ-SIM-004 | The simulation shall support configurable time step resolution in seconds (default: 1) |
| REQ-SIM-005 | The simulation shall randomly assign vehicle types across all manufacturers |
| REQ-SIM-006 | The simulation shall process all vehicles each time step using discrete time-stepping |
| REQ-SIM-007 | The simulation shall manage charging queue using first-in-first-out ordering |

### 2.3 Output Requirements

| Requirement ID | Requirement Text |
|----------------|------------------|
| REQ-OUT-001 | The system shall output final statistics by vehicle manufacturer |

## 3. Assumptions and Constraints

- Each vehicle starts the simulation with a fully-charged battery
- Each vehicle instantaneously reaches cruise speed
- Each vehicle flies at constant cruise speed until battery depletion
- Vehicle faults can only occur during flight operations
- Upon fault vehicle immediately stops flight
- A single charger can only be used by one vehicle at a time
- No geographic or routing considerations are modeled
- Limited to five predefined vehicle manufacturers
