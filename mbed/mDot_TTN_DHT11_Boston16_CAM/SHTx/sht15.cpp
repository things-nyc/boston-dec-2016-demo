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
 
#include "sht15.hpp"

namespace SHTx {    
    SHT15::SHT15(PinName sda, PinName scl): i2c(sda, scl) {        
        this->ready = true;
        wait_ms(11);
    }

    float
    SHT15::getTemperature(void) {
        return this->convertTemperature(
            this->temperature,
            this->getFlag(flag_resolution),
            this->scale
        );
    }

    float
    SHT15::getHumidity(void) {
        return this->convertHumidity(
            this->humidity,
            this->temperature,
            this->getFlag(flag_resolution)
        );
    }

    bool
    SHT15::checkBattery(void) {
        this->readRegister(cmd_read_register);
        return this->getFlag(flag_battery);
    }

    bool
    SHT15::setHeater(bool value) {
        this->setFlag(flag_heater, value);
        return this->writeRegister();
    }

    bool
    SHT15::setResolution(bool value) {
        this->setFlag(flag_resolution, value);
        return this->writeRegister();
    }

	bool
	SHT15::setOTPReload(bool value) {
		this->setFlag(flag_otp_reload, !value);
		return this->writeRegister();
	}

    void
    SHT15::setScale(bool value) {
        this->scale = value;
    }

    bool
    SHT15::update(void) {
        if ((this->ready == false) ||
            !this->readRegister(cmd_read_temperature) ||
            !this->readRegister(cmd_read_humidity)) {
            return false;
        }

        return true;
    }

    bool
    SHT15::reset(void) {
        while (this->ready == false) {
            continue;
        }
    
        this->ready = false;    
        this->i2c.start();
        bool ack = this->i2c.write(cmd_reset_device);
        this->i2c.stop();
        this->ready = true;
    
        if (ack) {
            this->status_register = 0;
            this->humidity = 0;
            this->temperature = 0;            
			wait_ms(11);
            return true;
        }
    
        return false;
    }

	void
	SHT15::connectionReset(void) {
		this->i2c.reset();
	}

    float
    SHT15::convertTemperature(uint16_t sot, bool res, bool scale) {
        // Temperature conversion coefficients
        float d1 = this->coefficient.dv[scale];
        float d2 = ((scale) ? this->coefficient.df[res]
                            : this->coefficient.dc[res]);
        
        // Temperature data conversion
        return d1 + (d2 * (float)(sot));
    }

    float
    SHT15::convertHumidity(uint16_t sohr, uint16_t sot, bool res) {
        // Humidity conversion coefficients
        float c1 = this->coefficient.c1[res];
        float c2 = this->coefficient.c2[res];
        float c3 = this->coefficient.c3[res];
        
        // Temperature compensation coefficients
        float t1 = this->coefficient.t1[res];
        float t2 = this->coefficient.t2[res];
    
        // Temperature data conversion to celcius
        float temp = this->convertTemperature(sot, res, false);
    
        // Humidity data conversion to relative humidity
        float humid = c1 + (c2 * (float)(sohr)) + (c3 * (float)(sohr * sohr));
    
        // Calculate temperature compensation    
        return (temp - 25) + (t1 + (t2 * (float)(sohr)) + humid);
    }

    bool
    SHT15::writeRegister(void) {
        while (this->ready == false) {
            continue;
        }
    
        this->ready = false;    
        this->i2c.start();

        if (this->i2c.write(cmd_write_register)) {
            this->i2c.write(this->status_register);
        }

        this->i2c.stop();
        this->ready = true;

        return true;
    }

    bool
    SHT15::readRegister(cmd_list command) {
        while (this->ready == false) {
            continue;
        }
    
        this->ready = false;
        this->i2c.start();
    
        if (!this->i2c.write(command)) {
            this->i2c.stop();
            return false;
        }
    
        switch (command) {
            case cmd_read_temperature: {
                if (!this->i2c.wait()) {
                    this->i2c.stop();
                    return false;
                }
            
                this->temperature  = this->i2c.read(1) << 8;
                this->temperature |= this->i2c.read(0);
            } break;
        
            case cmd_read_humidity: {
                if (!this->i2c.wait()) {
                     this->i2c.stop();
                    return false;
                }
            
                this->humidity  = this->i2c.read(1) << 8;
                this->humidity |= this->i2c.read(0);
            } break;
        
            case cmd_read_register: {
                this->status_register = this->i2c.read(0);
            } break;
        }
    
        this->i2c.stop();
        this->ready = true;
    
        return true;
    }

    bool
    SHT15::getFlag(SHT15::flag_list flag) {
        return (this->status_register & flag) ? true : false;
    }

    void
    SHT15::setFlag(SHT15::flag_list flag, bool value) {
        if (value) {
            this->status_register |= flag;
        } else {
            this->status_register &= ~flag;
        }
    }
}
