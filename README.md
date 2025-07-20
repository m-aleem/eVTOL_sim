# eVTOL_sim
eVTOL Simulation

## Introduction

This project implements a multi-vehicle eVTOL simulation.

## Background

There are five companies developing eVTOL aircraft. The vehicles produced by each manufacturer have different characteristics. We simulate using these vehicle for multiple hours. There are a limited number of chargers available for all vehicles, and a single charger can only be used by one vehicle at a time.

The simulation tracks various statistics per vehicle type and outputs the results at the end of simulation.

## File Structure

* `docs` Contains documentation, including requirements.
* `src` Contains simulation program source files.
* `tests` Contains test files.

## Usage

### Dependencies
The following are required to be able to build and run this simulation project:
* [cmake](https://cmake.org/download/) (3.10+)
* [googletest](https://google.github.io/googletest/)

#### Installing Dependencies
On macOS:
```
brew install cmake
brew install googletest
```

### Build
To build this simulation project:

1) Clone project.
2) Run `make`.

### Run
After building, to run the simulation project:
1) Run `make run`

### Input
Currently there are no required inputs.

### Output
The output of the program is a **simulation report** which includes various statistics per vehicle type.

## Version
Version # | Status | Description
-- | -- | --
0.1 | In Development | Initial Development