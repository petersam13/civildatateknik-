# Tilt Maze

An embedded game for the **NXP LPC1768 (Mbed) with the Mbed Application Board**. Tilt the board to roll a ball across the LCD and reach the goal before the 10-second timer runs out. Five rounds with increasing speed.

Final project in *ET095G – Introduction to Embedded Systems Programming*, Mid Sweden University, autumn 2025.

## Hardware used

| Component | Interface | Role in the game |
|---|---|---|
| MMA7660 accelerometer | I²C | Tilt in x/y moves the ball |
| Potentiometer | ADC (p19) | Difficulty: scales ball speed 1.0×–3.0× |
| C12832 LCD (128×32) | SPI | Ball, goal, timer, score, menus |
| Speaker | PWM (p26) | Start tone, round-win melody, game-over sound |
| 4 on-board LEDs | GPIO | Victory animation after round 5 |
| Joystick centre button | Interrupt (p14) | Start game (debounced) |
| Timer | – | 10 s limit per round, time-based score |

## Gameplay

1. Press the button on the start screen.
2. Tilt the board to steer the ball to the randomly placed goal.
3. Remaining time × 10 is added to the score for each round won.
4. Run out of time → game over. Win all five rounds → LED show and victory sound.

## Build

Import `main.cpp` into Keil Studio Cloud / Mbed Online Compiler (Mbed OS 2) together with the `C12832` and `MMA7660` libraries, compile for **LPC1768**, and copy the `.bin` file to the board.

## Report

Full design and implementation write-up, including a system block diagram: [Tilt-Maze-rapport.pdf](Tilt-Maze-rapport.pdf).
