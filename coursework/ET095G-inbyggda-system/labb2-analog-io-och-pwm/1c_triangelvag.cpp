#include "mbed.h"
AnalogOut Aout(p18);

int main() {
    const float v_min = 0.2f, v_max = 0.8f;
    const int   steps = 400;     // up or down steps
    const int   us_step = 50;    // update rate -> sets frequency
    while (true) {
        // ramp up
        for (int i = 0; i <= steps; ++i) {
            Aout.write(v_min + (v_max - v_min) * (float)i / steps);
            wait_us(us_step);
        }
        // ramp down
        for (int i = steps; i >= 0; --i) {
            Aout.write(v_min + (v_max - v_min) * (float)i / steps);
            wait_us(us_step);
        }
    }
}
