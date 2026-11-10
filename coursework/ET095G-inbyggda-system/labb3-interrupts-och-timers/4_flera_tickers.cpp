#include "mbed.h"
#include <stdio.h>
DigitalOut myled1(LED1);
DigitalOut myled2(LED2);
DigitalOut myled3(LED3);
DigitalOut myled4(LED4);
void blink1();
void blink2();
void blink3();
void blink4();
Ticker trickerLed1;
Ticker trickerLed2;
Ticker trickerLed3;
Ticker trickerLed4;

int main() {
    trickerLed1.attach(&blink1, 0.7);
    trickerLed2.attach(&blink2, 1.2);
    trickerLed3.attach(&blink3, 2.3);
    trickerLed4.attach(&blink4, 3.9);
    
    while(1) {
    }
}
void blink1(){
    myled1 = !myled1;
}
void blink2(){
    myled2 = !myled2;
}
void blink3(){
    myled3 = !myled3;
}
void blink4(){
    myled4 = !myled4;
}