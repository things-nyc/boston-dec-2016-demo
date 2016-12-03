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

#ifndef _I2C_HPP_
#define _I2C_HPP_

#include "mbed.h"

namespace SHTx {
    /**
     * Class: I2C
     *  Humidity and Temperature Sensor - SHT15
     *  I2C Bit-bang master driver.
     */
    class I2C {
    private:
        DigitalInOut scl_pin;
        DigitalInOut sda_pin;
    
        uint32_t frequency;
    
    public:    
        /**
         * Constructor: SHTx::I2C
           *  Create an I2C Master interface, connected to the specified pins.
         *  Bit-bang I2C driver to get around the lousy I2C implementation in the
         *  SHTx interface... 
         *
         * Variables:
         *  sda - I2C data line pin
         *  scl - I2C clock line pin
         */
        I2C(PinName sda, PinName scl);
    
        /**
         * Function: setFrequency
         *  Set the frequency of the SHTx::I2C interface
         *
         * Variables:
         *  hz - The bus frequency in hertz
         */
        void setFrequency(uint32_t hz);

        /**
         * Function: start
         *  Issue start condition on the SHTx::I2C bus
         */
        void start(void);
    
        /**
         * Function: stop
         *  Issue stop condition on the SHTx::I2C bus
         */
        void stop(void);
    
        /**
         * Function: wait
         *  Wait for SHT15 to complete measurement.
         *  Max timeout 500ms.
         *
         * Variables:
         *  returns - true if an ACK was received, false otherwise
         */
        bool wait(void);

		/**
		 * Function: reset
		 *  If communication with the device is lost
		 *  the command will reset the serial interface
		 */
		void reset(void);
    
        /**
         * Function: write
         *  Write single byte out on the SHTx::I2C bus
         *
         * Variables:
         *  data     - data to write out on bus
         *  returns - true if an ACK was received, false otherwise
         */
        bool write(uint8_t data);
    
        /**
         * Function: write
         *  Read single byte form the I2C bus
         *
         * Variables:
         *  ack     - indicates if the byte is to be acknowledged
         *  returns - the read byte
         */
        uint8_t read(bool ack);

    private:
        /**
         * Function: output
         *  Configures sda pin as output
         */
        void output(void);
    
        /**
         * Function: input
         *  Configures sda pin as input
         */
        void input(void);
    
        /**
         * Function: sda
         *  Drive sda pin.
         *
         * Variables:
         *  value - drive pin high or low
         */
        void sda(bool value);
    
        /**
         * Function: scl
         *  Drive scl pin.
         *
         * Variables:
         *  value - drive pin high or low
         */
        void scl(bool value);
    
        /**
         * Function: shift_out
         *  Write single bit out on the SHTx::I2C bus
         *
         * Variables:
         *  bit - value of the bit to be written.
         */
        void shift_out(bool bit);
    
        /**
         * Function: shift_in
         *  Read single bit from the SHTx::I2C bus
         *
         * Variables:
         *  return - value of the bit read.
         */
        bool shift_in(void);
    };
}

/* !_I2C_HPP_ */
#endif