#include "DS18B20.h"

DS18B20::DS18B20(PinName pin, unsigned resolution) :
    _pin(pin) {
    SetResolution(resolution);
}

DS18B20::~DS18B20() {
}

// Reset 1-wire interface.
unsigned DS18B20::Reset() {
    _pin.output();
    _pin = 0;    // drive bus low
    wait_us(H);
    _pin.input(); // release bus
    wait_us(I);
    unsigned result = _pin;  // read bus value
    wait_us(J);
    return result;
}

// Write bit to 1-wire.
void DS18B20::WriteBit(unsigned bit) {
    _pin.output();
    if (bit) {
        _pin = 0;        // drive bus low
        wait_us(A);        // delay A
        _pin.input();      // release bus
        wait_us(B);        // delay B
    } else {
        _pin = 0;    // drive bus low
        wait_us(C);    // delay C
        _pin.input();  // release bus
        wait_us(D);    // delay D
    }
}

// Read bit from 1-wire.
unsigned DS18B20::ReadBit() {
    unsigned bit_value;
    _pin.output();
    _pin = 0;        // drive bus low
    wait_us(A);        // delay A
    _pin.input();      // release bus
    wait_us(E);        // delay E
    bit_value = _pin;    // master sample bus
    wait_us(F);
    return bit_value;
}

// Write byte to 1-wire.
void DS18B20::WriteByte(unsigned byte) {
    for (unsigned bit = 0; bit < 8; ++bit) {
        WriteBit(byte & 0x01); // lsb to msb
        byte >>= 1;    // right shift by 1-bit
    }
}

// Read byte from 1-wire.
unsigned DS18B20::ReadByte() {
    unsigned byte = 0;
    for (unsigned bit = 0; bit < 8; ++bit) {
        byte |= (ReadBit() << bit);    // Reads lsb to msb
    }
    return byte;
}

// Set number of bits in the conversion.
unsigned DS18B20::SetResolution(unsigned resolution) {
    if (Reset() != 0)
        return 1;
    else {
        WriteByte(SKIP_ROM);            // Skip ROM
        WriteByte(WRITE_SCRATCHPAD);    // WRITE_SCRATCHPAD
        WriteByte(0x7f);                // Alarm TH
        WriteByte(0x80);                // Alarm TL
        WriteByte(resolution);          // 0xx11111 xx=resolution (9-12 bits)
    }
    return 0;
}

// Trigger a temperature conversion but don't read the temperature.
unsigned DS18B20::DoConversion() {
    if (Reset() != 0)
        return 1;
    else {
        WriteByte(SKIP_ROM);            // Skip ROM
        WriteByte(CONVERT);             // Convert
        while (ReadBit() == 0)
            ; // wait for conversion to complete
    }
    return 0;
}

// Do Conversion and get temperature as s8.4 sign-extended to 16-bits.
int DS18B20::RawTemperature() {
    // Perform the temperature conversion.
    if (DoConversion() != 0)
        return INVALID_TEMPERATURE;
    // Read the temperature back.
    if (Reset() != 0)
        return INVALID_TEMPERATURE;
    else {
        WriteByte(SKIP_ROM);    // Skip ROM
        WriteByte(READ_SCRATCHPAD);    // Read Scrachpad
        unsigned LSB = ReadByte();
        unsigned MSB = ReadByte();
        // Terminate read as we only want temperature
        Reset();
        // Ensure correct sign-extension.
        return (int)((int16_t)((MSB << 8) | LSB));
    }
}

// Read temperature in floating point format.
float DS18B20::GetTemperature() {
    int temperature = RawTemperature();
    return ((float)temperature) / 16.0;
}

// Read back DS18B20 ROM.
int DS18B20::ReadROM(DS18B20::ROM_Code_t *ROM_Code) {
    if (Reset() != 0)
        return 1;
    else {
        WriteByte(READ_ROM);    // Read ROM
        for (unsigned i = 0; i < 8; ++i) {
            ROM_Code->rom[i] = ReadByte();
        }
    }
    return 0;
}
