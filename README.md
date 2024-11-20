# ISS Rendez-vous Simulation

A C++ simulation program for calculating and visualizing spacecraft rendezvous trajectories with the International Space Station (ISS).

## Overview

This project simulates orbital mechanics and rendezvous operations with the ISS using real-time TLE (Two-Line Element) data. It calculates optimal trajectories considering orbital dynamics and Hohmann transfer orbits.

## Key Features

- Real-time ISS orbital data fetching
- Keplerian orbital elements calculation
- State vector transformation
- Hohmann transfer orbit computation
- 3D trajectory visualization

## Mathematical Foundation

### Orbital Elements to State Vectors

#### Kepler's Equation

![M](https://latex2image-output.s3.amazonaws.com/img-HEHkVtHt3UGF.png)

![](https://latex2image-output.s3.amazonaws.com/img-Pwmf4gETu6nR.png)

#### Position Vector Components

![r](https://latex2image-output.s3.amazonaws.com/img-JrYh1ZpNPEJC.png)

![x](https://latex2image-output.s3.amazonaws.com/img-W1fvE6FTHGqQ.png)

![y](https://latex2image-output.s3.amazonaws.com/img-8REnkESaeJJ4.png)

![z](https://latex2image-output.s3.amazonaws.com/img-WkY2CAs4wdRW.png)

![](https://latex2image-output.s3.amazonaws.com/img-H96YWS2dm1kA.png)

### Hohmann Transfer Orbit

#### Transfer Orbit Semi-major Axis

![a_t](https://latex2image-output.s3.amazonaws.com/img-MewJxHjDGWnn.png)

#### Delta-V Calculations

![deltav1](https://latex2image-output.s3.amazonaws.com/img-ynZYkntx4pbh.png)

![deltav2](https://latex2image-output.s3.amazonaws.com/img-pQmWNEuJ6mUX.png)

![](https://latex2image-output.s3.amazonaws.com/img-BMxFxmpJPxRr.png)

## Requirements

To compile this project, you will need:

- A C++ compiler that supports C++17 or higher
- CMake version 3.10 or higher
- Qt Framework (ensure that the Qt development libraries are installed)

## Dependencies

- Qt Framework
- C++17 or higher

## Building and Running

1. Clone the repository
    ```bash
    git clone https://github.com/yourusername/rvSim.git
    ```

2. Build the project
    ```bash
    cd rvSim && mkdir build && cd build && cmake .. && make
    ```

3. Run the simulation
    ```bash
    ./rvSim
    ```

## Authors

- Recep Kamrak
- Emil Mammadli
- Burak Saglam

Middle East Technical University

## License

MIT License
