#ifndef DS18B20_H
#define DS18B20_H

#include "mbed.h"
#include <stdint.h>

/** A DS18B20 Dallas 1-wire digital thermometer interface.
 *
 * Currently supports 9, 10, 11 or 12-bit conversions. Fewer bits
 * require less conversion time from 93.75ms to 750ms. Also supports
 * reading back the 8-byte internal ROM. Orignal code copied from
 * DS18B20 C program by Niall Cooling (thanks!) and wrapped in C++
 * class by this library.
 *
 * @code
 * #include "mbed.h"
 * #include <stdint.h>
 * #include "DS18B20.h"
 *
 * Serial pc(USBTX, USBRX);     // serial comms over usb back to console
 * DS18B20 thermom(p16, DS18B20::RES_12_BIT); // Dallas 1-wire
 *
 * int main() {
 *   pc.printf("DS18B20 Configuration\n\r");
 *   
 *   DS18B20::ROM_Code_t ROM_Code;
 *   thermom.ReadROM(&ROM_Code);
 *   pc.printf("Family code: 0x%X\n\r", ROM_Code.BYTES.familyCode);
 *   pc.printf("Serial Number: ");
 *   for (unsigned i = 6; i != 0; --i) {
 *       pc.printf("%02X%s", ROM_Code.BYTES.serialNo[i-1], (i != 1)?":":"\r\n");
 *   }
 *   pc.printf("VTV: 0x%X\r\n", ROM_Code.BYTES.VTV);
 *   
 *   pc.printf("\n\rRunning temperature conversion...\n\r");
 *   while (1) {
 *       pc.printf("Temperature is: %.4fC\n\r", thermom.GetTemperature());
 *       wait(10);
 *   }
 * }
 * @endcode
 */

class DS18B20
{
public:
    /** Value to return when Reset() fails */
    enum {INVALID_TEMPERATURE = -10000};
    
    /** Temperature conversion dit width resolutions */
    enum RESOLUTION { RES_9_BIT=0x1f,    /**< 93.75ms */
                      RES_10_BIT=0x3f,   /**< 187.5ms */
                      RES_11_BIT=0x5f,   /**< 375ms */
                      RES_12_BIT=0x7f    /**< 750ms */
    };
    
    /** Holds 8-byte internal ROM */
    typedef union {
        uint8_t rom[8];
        struct {
            uint8_t familyCode;  /**< Family Code */
            uint8_t serialNo[6]; /**< Serial Number */
            uint8_t VTV;         /**< VTV check byte */
        } BYTES;
    } ROM_Code_t;
    
    /** Device onboard register layout (for reference only, not currently used) */
    typedef struct {
        uint8_t    LSB; /**< LSB of converted temperature */
        uint8_t    MSB; /**< MSB of converted temperature */
        uint8_t    Th; /**< Threshold for high alarm */
        uint8_t    Tl; /**< Threshold for low alarm */
        uint8_t    config; /**< Conversion resultion */
        uint8_t    reserved0xFF;
        uint8_t    reserved0xCH;
        uint8_t    reserved0x10;
        uint8_t    VTV; /**< VTV check byte */
    } ScratchPad_t;
    
    /** Create a Dallas DS18B20 1-wire interface
     *
     * @param pin         Pin to use for 1-wire interface (bidirectional I/O)
     * @param resolution  Sets the conversion bit width (using RESOLUTION enum)
     */
    DS18B20(PinName pin, unsigned resolution);
    
    /** Destructor */
    ~DS18B20();

    /** Performs conversion in DS18B20 and then reads and converts returned temperature
     *  to floating point. For many applications this is the only required method that
     *  needs to be used.
     */
    float GetTemperature();

    /** Performs conversion but does not read back temperature. Not needed if
     *  GetTemperature() is used as this calls DoConversion() itself. */
    unsigned DoConversion();
    
    /** The method that GetTemperature() calls to do all the conversion and reading
     *  but this method returns a 32-bit signed integer. The integer contains 4
     *  fractional LSBs. Sometimes referred to as s28.4 format. */
    int RawTemperature();
    
    /** Reads and returns the 8-byte internal ROM */
    int ReadROM(ROM_Code_t *ROM_Code);
    
    /** Sets the conversion resolution with RESOLUTION enum (9-12 bits signed) */
    unsigned SetResolution(unsigned resolution);

protected:

    // Timing delay for 1-wire serial standard option
    enum DELAY { A = 6, B = 64, C = 60, D = 10, E = 9, F = 55, G = 0, H = 480, I = 70, J = 410 };

    // Device byte commands over 1-wire serial
    enum COMMANDS { READ_ROM = 0x33, CONVERT = 0x44, READ_SCRATCHPAD = 0xBE,  WRITE_SCRATCHPAD = 0x4E, SKIP_ROM = 0xCC };
    
    // Methods from DS1Wire.h
    unsigned Reset();
    void WriteBit(unsigned bit);
    unsigned ReadBit();
    void WriteByte(unsigned byte);
    unsigned ReadByte();

    // The pin used for the Dallas 1-wire interface
    DigitalInOut _pin;
};

#endif
