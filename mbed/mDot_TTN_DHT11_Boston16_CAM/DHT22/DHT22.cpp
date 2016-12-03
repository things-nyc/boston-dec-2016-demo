#include "DHT22.h"


DHT22::DHT22(PinName pin) {
    _data_pin = pin;
}

int DHT22::getTemperature() {
    return _temperature;
}

int DHT22::getHumidity() {
    return _humidity;
}

bool DHT22::sample() {
    DigitalInOut DHT22(_data_pin);
    int dht22_dat [5];
    DHT22.output();
    DHT22.write(0);
    wait_ms(18);
    DHT22.write(1);
    DHT22.input();
    wait_us(40);
    wait_us(80);
    int i,j,result=0;
    for (i=0; i<5; i++) {
        result=0;
        for (j=0; j<8; j++) {
            while (DHT22);
            while (!DHT22);
            wait_us(50);
            int p;
            p=DHT22;
            p=p <<(7-j);
            result=result|p;
        }
        dht22_dat[i] = result;
    }
    int dht22_check_sum;
    dht22_check_sum=dht22_dat[0]+dht22_dat[1]+dht22_dat[2]+dht22_dat[3];
    dht22_check_sum= dht22_check_sum%256;
    if (dht22_check_sum==dht22_dat[4]) {
        _humidity=dht22_dat[0]*256+dht22_dat[1];
        _temperature=dht22_dat[2]*256+dht22_dat[3];
        return true;
    }
    return false;
}
