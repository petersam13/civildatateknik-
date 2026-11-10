#include "mbed.h"

AnalogOut Aout(p18);
float i;

int main() {
    while(1) {
        for (i=0; i<2; i=i+0.021) {
            Aout = 0.5 + 0.3 * sin(i * 3.14159);
            wait(0.001);
        }
    }
}
