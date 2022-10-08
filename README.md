# Lego ESP32 Sun Tracker

This is a simple sun tracker, made using Lego Technic pieces for the structural components, and using Lego-compatible servos to control azimuth and elevation.

## Libraries

https://pubsubclient.knolleary.net/

https://github.com/madhephaestus/ESP32Servo

https://arduinojson.org/

A servo library tutorial is described at https://dronebotworkshop.com/esp32-servo/

## Green Servo Specifications:

| Characteristic       | Value                          |
|----------------------|--------------------------------|
| Color:               | green                          |
| Torsion:             | 0.7kg                          |
| Operating voltage    | 4.8-6V                         |
| Use Angle:           | 360 degrees, continuous        |
| Limit angle:         | None                           |
| Operating travel:    | 360° ±10° (in 500 → 2500 μsec) |
| PWM signal:          | 500-2500                       |
| Product speed        | 160/min @ 6V                   |
| Gear reduction ratio | 1:140                          |
| Wire length:         | 25 cm                          |
| Terminal type:       | DuPont terminal                |

## Gray Servo Specifications:

### Electrical Specification (Function of the Performance)：

| Characteristic               | 4.8V               | 6.0V               |
|------------------------------|--------------------|--------------------|
| Operating speed (at no load) | 0.14 ±0.01 sec/60° | 0.12 ±0.01 sec/60° |
| Running current (at no load) | 70 ±20 mA          | 90 ±20 mA          |
| Stall torque (at locked)     | 1.8 ±0.2 kg-cm     | 2 ±0.2 kg-cm       |
| Stall current (at locked)    | 0.8 ±0.1 A         | 0.9 ±0.1 A         |
| Idle current (at stopped)    | 7 ±1 mA            | 7 ±1 mA            |
| Running life(at no load)     | > 50,000 times     | > 40,000 times     |
| Temperature drift(at 25°C)   | ≤ 1°               | ≤ 1°               |

### Mechanical Specification：

| Characteristic          | value                                       |
|-------------------------|---------------------------------------------|
| Limit angle:            | 360°                                        |
| Weight                  | 20 ±1g                                      |
| Horn gear spline:       | Cross hole double output (1 Lego unit deep) |
| Reduction ratio:        | 1/300                                       |
| excessive play:         | ≤ 1°                                        |
| Control system:         | Change the pulse width                      |
| Amplifier type:         | Digital controller                          |
| Operating travel:       | 360° ±10° (in 500 → 2500 μsec)              |
| Left&Right travelling:  | ≤ 10°                                       |
| Centering deviation:    | ≤ 2°                                        |
| Neutral position:       | 1500 μsec                                   |
| Dead bandwidth:         | ≤ 4 μsec                                    |
| Rotating direction:     | counter-clockwise (1500 → 500 μsec)         |
| Pulse width range:      | 500 → 2500 μsec                             |
| Maximum travel          | 360°(500 → 2500 μsec)                       |
| Shock resistance:       | 250 ±30g                                    |
| Storage Temperature     | -20°C - 60°C                                |
| Operating Temperature   | -10°C - 50°C                                |

### Standard Test Environment

Every test at temperature: 25 ±5° C and relative humidity: 65 ±10％.
