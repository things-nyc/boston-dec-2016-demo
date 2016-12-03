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

#include "i2c.hpp"

namespace SHTx {
	I2C::I2C(PinName sda, PinName scl) :
	scl_pin(scl), sda_pin(sda), frequency(10) {
		this->sda_pin.output();
		this->scl_pin.output();
	}

	void
	I2C::setFrequency(uint32_t hz) {
		this->frequency = (1000000 / hz);
	}

	void
	I2C::start(void) {
	    this->output();
		this->sda(1);
		this->scl(1);
		this->sda(0);
		this->scl(0);
		this->scl(1);
		this->sda(1);
		this->scl(0);
	}

	void
	I2C::stop(void) {
	    this->output();
		this->sda(0);
		this->scl(1);
		this->sda(1);
	}

	bool
	I2C::wait(void) {
		bool ack = false;
	
		this->input();
		for (uint8_t i = 0; i < 500 && !ack; i++) {
			wait_ms(1);
			ack = !this->sda_pin;
		}
	
		return ack;
	}
	
	void
	I2C::reset(void) {
		this->output();
		for (uint8_t i = 9; i; i--) {
			this->shift_out(1);
		}
		this->start();
		this->scl(1);
	}

	bool
	I2C::write(uint8_t data) {
	    bool ack;

	    this->output();
		for (uint8_t i = 8; i; i--) {
		    this->shift_out(data & 0x80);
		    data <<= 1;
		}
    
	    this->input();
	    ack = !this->shift_in();
    
		return ack;
	}

	uint8_t
	I2C::read(bool ack) {
		uint8_t data = 0;
	
	    this->input();
		for (uint8_t i = 8; i; i--) {
		    data <<= 1;
			data  |= this->shift_in();
		}
	
	    this->output();
		this->shift_out(!ack);
	
		return data;
	}

	void
	I2C::output(void) {
		this->sda_pin.output();
	}

	void
	I2C::input(void) {
		this->sda_pin.input();
	}

	void
	I2C::sda(bool value) {
		this->sda_pin = value;
		wait_us(this->frequency);
	}

	void
	I2C::scl(bool value) {
		this->scl_pin = value;
		wait_us(this->frequency);
	}

	void
	I2C::shift_out(bool bit) {
	    this->sda(bit);
		this->scl(1);
		this->scl(0);
	}

	bool
	I2C::shift_in(void) {
	    wait_us(this->frequency);
		this->scl(1);
		bool bit = this->sda_pin;
		this->scl(0);
		return bit;
	}
}