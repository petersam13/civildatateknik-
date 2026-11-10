#include "mbed.h"


DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
InterruptIn button(p14); // joystick center
Ticker blink; // för 1 Hz blinkning

// växla LED1 varje 0.5 s → 1 Hz
void toggle_led1() { 
    led1 = !led1; 
}

// knapp trycks ned
void onRise() { 
    led2 = !led2; // växla LED2 vid stigande flank
}

// knapp släpps upp
void onFall() { 
    led3 = !led3; // växla LED3 vid fallande flank
}

int main() {
    blink.attach(&toggle_led1, 0.5); // 0.5 s → 1 Hz
    button.rise(&onRise);
    button.fall(&onFall);
    while (true) { } // inget i loopen
}
