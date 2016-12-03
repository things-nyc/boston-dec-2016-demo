/**
 * Copyright (c) 2010 Roy van Dam <roy@negative-black.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
 
#ifndef _SHT15_HPP_
#define _SHT15_HPP_

#include "mbed.h"
#include "i2c.hpp"

namespace SHTx {    
    /**
     * Class: SHT15
     *  Humidity and Temperature Sensor - SHT15
     *  High level software interface.
     */
    class SHT15 {
    public: // CM private
        I2C i2c;
        
        bool ready;
        bool scale;
    
        uint8_t  status_register;
        uint16_t humidity;
        uint16_t temperature;

        struct coefficients {
            // Humidity conversion coefficients
            float c1[2], c2[2], c3[2];

            // Temperature conversion coefficients
            float dc[2], df[2], dv[2];

            // Temperature compensation coefficients
            float t1[2], t2[2];

            // Initialize table
            coefficients(void) {
                c1[0] = -2.046f;        c2[1] = -2.046f;
                c2[0] =  0.036f;        c2[1] =  0.587f;
                c3[0] = -0.0000015955f; c3[1] = -0.00040845f;

                dc[0] =  0.01f;         dc[1] =  0.04f;
                df[0] =  0.018f;        df[1] =  0.072f;
                dv[0] = -39.7f;         dv[1] = -39.5f;

                t1[0] =  0.01f;         t1[1] =  0.01f;
                t2[0] =  0.00008f;      t2[1] =  0.00128f;
            };
        } coefficient;
    
        enum cmd_list {
            cmd_read_temperature = 0x03,
            cmd_read_humidity    = 0x05,
            cmd_read_register    = 0x07,
            cmd_write_register   = 0x06,
            cmd_reset_device     = 0x1E
        };

        enum flag_list {
            flag_resolution = 0x01,
            flag_otp_reload = 0x02,
            flag_heater     = 0x04,
            flag_battery    = 0x40
        };
    
    public:
        /*
         * Initializes I2C interface and waits
         * the 11ms device initialization time.
         */
        SHT15(PinName sda, PinName scl);

        /**
         * Function: getTemperature
         *  Returns the current temperature.
         *
         * Values:
         *  returns - current temperature.
         */
        float getTemperature(void);
    
        /**
         * Function: getHumidity
         *  Returns the current relative humidity.
         *
         * Values:
         *  returns - relative humidity.
         */
        float getHumidity(void);

        /**
         * Function: checkBattery
         *  Returns true if battery voltage drops
         *    below 2.4 volt.
         *
         * Values:
         *  returns - true on low battery, false otherwise
         */
        bool checkBattery(void);
    
        /**
         * Function: setHeater
         *  Enable on chip heating element. The heater may 
         *  increase the temperature of the sensor by 5C to
         *  10C beyond ambient temperature.
         *
         * Values:
         *  value - true->on, false->off
         *  return - operation result
         */
        bool setHeater(bool value);

        /**
         * Function: setResolution
         *  Set lower measurement resolution to allow
         *  faster update frequencies.
         *
         * Resolutions
         *  low:   8bit humid. - 12bit temp.
         *  high: 12bit humid. - 14bit temp.
         *
         * Values:
         *  value - true->low, false->high
         *  return - operation result
         */
        bool setResolution(bool value);

		/**
         * Function: setOTPReload
         *  With this operation the calibration data is 
		 *  uploaded to the register before each measurement. This 
         *  may be deactivated for reducing measurement time by 
         *  about 10ms
         *
         * Values:
         *  value - true->enabled, false->disabled
         */
		bool setOTPReload(bool value);

        /**
         * Function: setScale
         *  Sets output scale to fahrenheit or celcius.
         *
         * Values:
         *  value - true->fahrenheit, false->celcius
         */
        void setScale(bool value);
    
        /**
         * Function: update
         *  Performs humidity and temperature
         *  sensor readout of the chip.
         *
         * Values:
         *  return - operation result
         */
        bool update(void);
    
        /**
         * Function: reset
         *  Resets the interface, clears the 
		 *  status register to default values.
		 *  And waits for a minimum of 11ms.
         *
         * Values:
         *  return - operation result
         */
        bool reset(void);

		/**
         * Function: connectionReset
         *  If communication with the device is lost
		 *  the command will reset the serial interface
         */
		void connectionReset(void);
    
    private:
    
        /**
         * Function: convertTemperature
         *  Convert sensor data to human readable value
         *  on the farenheit or celcius scale.
         *
         * Values:
         *  sot    - raw temperatue sensor output
         *  res    - true->low, false->high 
         *  scale  - true->fahrenheit, false->celcius 
         *  return - temperature
         */
        float convertTemperature(uint16_t sot, bool res = false, bool scale = false);
    
        /**
         * Function: convertHumidity
         *  Convert sensor data to relative humidity
         *  in percentage.
         *
         * Values:
         *  sohr   - raw humidity sensor output
         *  sot    - raw temperatue sensor output
         *  res    - true->low, false->high
         *  return - relative humidity
         */
        float convertHumidity(uint16_t sohr, uint16_t sot, bool res = false);
      
        /**
         * Function: writeRegister
         *  Write internal chip register.
         *
         * Values:
         *  return - operation result
         */
        bool writeRegister(void);
    
        /**
         * Function: readRegister
         *  Reads internal chip register
         *
         * Values:
         *  command - register to be accessed
         *  return  - operation result
         */
        bool readRegister(cmd_list command);
    
        /**
         * Function: setFlag
         *  Modify local register flag.
         *
         * Values:
         *  flag  - flag to be modified
         *  value - value to be assigned
         */
        void setFlag(flag_list flag, bool value);
    
        /**
         * Function: getFlag
         *  Get local register flag.
         *
         * Values:
         *  returns - flag value
         */
        bool getFlag(flag_list flag);
    };
}

/* !_SHT15_HPP_ */
#endif