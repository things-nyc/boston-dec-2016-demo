#ifndef MBED_DHT22_H
#define MBED_DHT22_H

#include "mbed.h"

class DHT22 {
private:
    int _temperature,_humidity;
    PinName _data_pin;
public:
    DHT22(PinName);
    bool sample();
    int getTemperature();
    int getHumidity();
};

#endif