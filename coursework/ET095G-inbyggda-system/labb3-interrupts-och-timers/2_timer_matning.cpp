#include "mbed.h"
#include <cstdio>
#include <cstring>

int main() {
    Timer t; 
    char str[100]; 

    printf("Skriv en text: ");
    scanf("%s", str); // läs in text

    t.start();
    printf("Du skrev: %s\n", str); // skriv ut
    t.stop();

    float total = t.read(); // total tid i sekunder
    float avg = total / strlen(str); // tid per tecken

    printf("Total tid: %.6f s\n", total);
    printf("Tid per tecken: %.6f s\n", avg);
}
