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

$$ M = E - e \sin(E) $$

Where:
- $M$: Mean anomaly
- $E$: Eccentric anomaly
- $e$: Eccentricity

#### Position Vector Components

$$ r = a(1 - e \cos(E)) $$

$$ x = r(\cos(\Omega)\cos(\omega+\nu) - \sin(\Omega)\sin(\omega+\nu)\cos(i)) $$

$$ y = r(\sin(\Omega)\cos(\omega+\nu) + \cos(\Omega)\sin(\omega+\nu)\cos(i)) $$

$$ z = r(\sin(i)\sin(\omega+\nu)) $$

Where:
- $a$: Semi-major axis
- $\Omega$: Right ascension of ascending node
- $\omega$: Argument of periapsis
- $\nu$: True anomaly
- $i$: Inclination

### Hohmann Transfer Orbit

#### Transfer Orbit Semi-major axis

$$ a_t = \frac{r_1 + r_2}{2} $$

#### Delta-V Calculations

$$ \Delta V_1 = \sqrt{\frac{\mu}{r_1}} \left(\sqrt{\frac{2r_2}{r_1+r_2}} - 1\right) $$

$$ \Delta V_2 = \sqrt{\frac{\mu}{r_2}} \left(1 - \sqrt{\frac{2r_1}{r_1+r_2}}\right) $$

Where:
- $\mu$: Earth's gravitational parameter (398600.4418 km³/s²)
- $r_1$: Initial orbit radius
- $r_2$: Target orbit radius

## Orbit Generation Mathematics

The orbit generation module calculates initial position and velocity vectors for spacecraft orbits using fundamental orbital mechanics principles.

### Key Parameters
- Earth's gravitational parameter ($\mu$): $3.986004418 \times 10^{14} \text{ m}^3/\text{s}^2$
- Earth radius (R): $6,371,000 \text{ m}$
- Altitude range: $100-1000 \text{ km}$
- Inclination range: $[-90\degree, 90\degree]$

### Orbital Calculations

The initial position and velocity vectors are computed using:

#### Semi-major axis
$$a = \frac{r_1 + r_2}{2}$$

where:
- $r_1$ = initial radius (Earth radius + x altitude)
- $r_2$ = final radius (Earth radius + y altitude)

#### Initial Velocity Magnitude
Using the vis-viva equation:

$$v = \sqrt{\mu(\frac{2}{r} - \frac{1}{a})}$$

#### Initial State Vectors
Position vector:

$$\vec{r_0} = [x_r, 0, 0]$$

Velocity vector:

$$\vec{v_0} = [0, v\cos(i), v\sin(i)]$$

where $i$ is the orbital inclination in degrees.

## Time Conversion Mathematics

### TAI Modified Julian Date Conversion

The function converts calendar date to TAI (International Atomic Time) Modified Julian Date using the following algorithm:

#### Calendar to Julian Date
For year $y$, month $m$, day $d$, hour $h$, minute $min$, second $s$:

$$a = \lfloor\frac{14-m}{12}\rfloor$$
$$y' = y + 4800 - a$$
$$m' = m + 12a - 3$$

Julian Date calculation:
$$JD = d + \frac{h-12}{24} + \frac{min}{1440} + \frac{s}{86400} + \frac{153m' + 2}{5} + 365y' + \frac{y'}{4} - \frac{y'}{100} + \frac{y'}{400} - 32045$$

#### TAI MJD Conversion
$$TAI_{offset} = \frac{37}{86400}$$ (leap seconds)

$$TAI_{MJD} = JD - 2430000.5 + TAI_{offset}$$

This converts civil time to TAI Modified Julian Date, accounting for leap seconds and time standard differences.

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
