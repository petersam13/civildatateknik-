#include "mbed.h"

I2C i2c(p28, p27);

const int LM75B_ADDR = 0x48 << 1;


LocalFileSystem local("local");


float readTempLM75B() {
    char reg = 0x00;
    char data[2] = {0, 0};

    i2c.write(LM75B_ADDR, &reg, 1);  
    i2c.read(LM75B_ADDR, data, 2);   

    
    int16_t raw = (data[0] << 3) | (data[1] >> 5);

    
    if (raw & 0x400) {
        raw |= 0xF800;
    }

    return raw * 0.125f;  
}

int main() {
    while (1) {
        float temp = readTempLM75B();

        FILE *f = fopen("/local/temp.txt", "a");
        if (f) {
            fprintf(f, "%.2f\n", temp);
            fclose(f);
        }

        wait(10); // 10 sekunder
    }
}
