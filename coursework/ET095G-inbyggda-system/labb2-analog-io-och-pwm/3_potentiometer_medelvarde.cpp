#include "mbed.h"
AnalogIn mypotentiometer(p20);

int main() {
    float total_v = 0;
    while (1) {
        total_v = 0;
        for (int i = 0; i < 20; i++) {
            total_v += mypotentiometer.read();
        }
        float avg = total_v / 20.0f;
        float voltage = avg * 3.3f;
        printf("Average voltage: %.3f V\r\n", voltage);
        wait(1.0);
    }
}
