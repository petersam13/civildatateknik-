#include "mbed.h"
AnalogOut Aout(p18);

int main() {
    const float low  = 0.2f;
    const float high = 0.8f;
    const int period_ms = 10; // 100 Hz square
    while (true) {
        Aout.write(high);
        wait_ms(period_ms / 2);
        Aout.write(low);
        wait_ms(period_ms / 2);
    }
}
