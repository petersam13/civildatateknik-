# ET095G – Introduction to Embedded Systems Programming

Lab work on the **NXP LPC1768 (Mbed)** with the Mbed Application Board, written in C++ with Mbed OS 2. The course final project is [Tilt Maze](../../projects/tilt-maze).

| Lab | Topics | Files |
|---|---|---|
| [Lab 1 – LCD & joystick](labb1-lcd-och-joystick) | ADC voltmeter on the C12832 LCD, moving a bitmap with the joystick, live temperature graph from the LM75B sensor | `1_voltmeter_lcd.cpp`, `2_joystick_bitmap.cpp`, `3_temperaturgraf_lcd.cpp` |
| [Lab 2 – Analog I/O & PWM](labb2-analog-io-och-pwm) | Sine, square and triangle waves on the DAC (`AnalogOut`), RGB LED fade with PWM, averaging potentiometer readings | `1a_sinusvag.cpp` … `3_potentiometer_medelvarde.cpp` |
| [Lab 3 – Interrupts & timers](labb3-interrupts-och-timers) | `InterruptIn` on rising/falling edges, `Ticker`, `Timeout`, measuring execution time with `Timer`, several independent tickers | `1_interruptin_ticker.cpp` … `4_flera_tickers.cpp` |
| [Lab 4 – I²C temperature logger](labb4-i2c-temperaturlogg) | Reading the LM75B over raw I²C, decoding the 11-bit two's-complement value, logging to the on-board flash with `LocalFileSystem` | `main.cpp` |

## Build

Each file is a standalone `main` program. Import it into Keil Studio Cloud / Mbed Online Compiler, add the `C12832` / `LM75B` libraries where they are included, and compile for **LPC1768**.
