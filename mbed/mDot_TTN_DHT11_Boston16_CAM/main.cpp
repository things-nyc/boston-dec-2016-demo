/** mDot_TTN_Raingarden -- Rain Garden Sensor by The Things Network New York
 * 
 *  This firmware runs an mDot dev board, with the following sensors:
 *   - DHT22 - air temp & humidity
 *   - TSL2561 - ambient light
 *   - SHT10 - soil temp & humidity
 *   - DS18B20 - water temp
 * 
 * Uses MultiTech mDot developer board http://www.multitech.com/models/94558010LF
 * Requires a MultiTech MultiConnect Conduit http://www.multitech.com/models/94557203LF
 * http://www.multitech.net/developer/software/lora/conduit-mlinux-convert-to-basic-packet-forwarder/
 * http://forum.thethingsnetwork.org/t/setting-up-multitech-conduit-gateway-for-ttn/216/35
 *
 * To receive and visualize this data:
 *
 */

#include "mbed.h"
#include "mDot.h"
#include "MTSLog.h"
#include "MTSText.h"
#include "DHT22.h"
#include "TSL2561_I2C.h"
#include "sht15.hpp"
#include "DS18B20.h"
#include <string>
#include <vector>

using namespace mts;

/* Pin definitions */

// air temp/humid sensor
#define DHT_PIN PA_1
DHT22 dht(DHT_PIN);

// water temp sensor
#define DS_PIN PA_11
DS18B20 thermom(DS_PIN, DS18B20::RES_12_BIT); 

// soil temp/humid sensor
#define SHT_DATA_PIN PA_4
#define SHT_SCK_PIN PC_13
SHTx::SHT15 sht(SHT_DATA_PIN, SHT_SCK_PIN);

// light sensor
#define TSL_DATA_PIN PC_9
#define TSL_SCK_PIN PA_8
TSL2561_I2C tsl(TSL_DATA_PIN, TSL_SCK_PIN);

// LEDs
#define STATUS PB_1

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))


/** ABP
 * Register your device and update these values:
 * https://account.thethingsnetwork.org/
 */

uint8_t AppSKey[16] = /* replace with Application Session Key in MSB format from TTN dashboard */;
uint8_t NwkSKey[16] = /* replace with Network Session Key in MSB format from TTN dashboard */;
uint8_t NetworkAddr[4] = /* replace with 4-byte Device Address from TTN dashboard */;

// Some defines for the LoRa configuration
#define LORA_SF mDot::SF_7
#define LORA_ACK 0
#define LORA_TXPOWER 20
static uint8_t config_frequency_sub_band = 2;

// functions for ensuring network endianness (little-endian)
uint16_t hton16(const uint16_t x)
{
  uint16_t t = x;
  uint8_t * a = (uint8_t*)&t;
  a[0] = x>>(8*1);
  a[1] = x>>(8*0);
  return t;
}
void hton16(uint16_t * x)
{
  *x = hton16(*x);
}



// build a transmit buffer (from https://raw.githubusercontent.com/mcci-catena/Catena4410-Sketches/master/catena4410_sensor1/catena4410_sensor1.ino)
class TxBuffer_t
        {
public:
        uint8_t buf[32];   // this sets the largest buffer size
        uint8_t *p;

        TxBuffer_t() : p(buf) {};
        void begin()
                {
                p = buf;
                }
        void put(uint8_t c)
                {
                if (p < buf + sizeof(buf))
                        *p++ = c;
                }
        void put1u(int32_t v)
                {
                if (v > 0xFF)
                        v = 0xFF;
                else if (v < 0)
                        v = 0;
                put((uint8_t) v);
                }
        void put2(uint32_t v)
                {
                if (v > 0xFFFF)
                        v = 0xFFFF;

                put((uint8_t) (v >> 8));
                put((uint8_t) v);
                }
        void put2(int32_t v)
                {
                if (v < -0x8000)
                        v = -0x8000;
                else if (v > 0x7FFF)
                        v = 0x7FFF;

                put2((uint32_t) v);
                }
        void put3(uint32_t v)
                {
                if (v > 0xFFFFFF)
                        v = 0xFFFFFF;

                put((uint8_t) (v >> 16));
                put((uint8_t) (v >> 8));
                put((uint8_t) v);
                }
        void put2u(int32_t v)
                {
                if (v < 0)
                        v = 0;
                else if (v > 0xFFFF)
                        v = 0xFFFF;
                put2((uint32_t) v);
                }
        void put3(int32_t v)
                {
                if (v < -0x800000)
                        v = -0x800000;
                else if (v > 0x7FFFFF)
                        v = 0x7FFFFF;
                put3((uint32_t) v);
                }
        uint8_t *getp(void)
                {
                return p;
                }
        size_t getn(void)
                {
                return p - buf;
                }
        uint8_t *getbase(void)
                {
                return buf;
                }
        void put2sf(float v)
                {
                int32_t iv;

                if (v > 32766.5f)
                        iv = 0x7fff;
                else if (v < -32767.5f)
                        iv = -0x8000;
                else
                        iv = (int32_t)(v + 0.5f);

                put2(iv);
                }
        void put2uf(float v)
                {
                uint32_t iv;

                if (v > 65535.5f)
                        iv = 0xffff;
                else if (v < 0.5f)
                        iv = 0;
                else
                        iv = (uint32_t)(v + 0.5f);

                put2(iv);
                }
        void put1uf(float v)
                {
                uint8_t c;

                if (v > 254.5)
                        c = 0xFF;
                else if (v < 0.5)
                        c = 0;
                else
                        c = (uint8_t) v;

                put(c);
                }
        void putT(float T)
                {
                put2sf(T * 256.0f + 0.5f);                
                }
        void putRH(float RH)
                {
                put1uf((RH / 0.390625f) + 0.5f);
                }
        void putV(float V)
                {
                put2sf(V * 4096.0f + 0.5f);
                }
        void putP(float P)
                {
                put2uf(P / 4.0f + 0.5f);
                }
        void putLux(float Lux)
                {
                put2uf(Lux);
                }
        };

/* the magic byte at the front of the buffer */
enum    {
        FormatSensor1 = 0x11,
        };

/* the flags for the second byte of the buffer */
enum    {
        FlagVbat = 1 << 0,
        FlagVcc = 1 << 1,
        FlagTPH = 1 << 2,
        FlagLux = 1 << 3,
        FlagWater = 1 << 4,
        FlagSoilTH = 1 << 5,
        };



// Serial via USB for debugging only
Serial pc(USBTX,USBRX);

int main()
{
    TxBuffer_t b;
    
    int32_t ret;
    mDot* dot;
    std::vector<uint8_t> send_data;
    std::vector<uint8_t> recv_data;
    std::vector<uint8_t> nwkSKey;
    std::vector<uint8_t> appSKey;
    std::vector<uint8_t> nodeAddr;
    std::vector<uint8_t> networkAddr;

    float temperature = 0.0;

    DigitalInOut status_led(STATUS);
    status_led.output();

    pc.baud(115200);
    pc.printf("TTN mDot LoRa Temperature & Humidity Sensor\n\r");

    // get a mDot handle
    dot = mDot::getInstance();

//  dot->setLogLevel(MTSLog::WARNING_LEVEL);
    dot->setLogLevel(MTSLog::TRACE_LEVEL);

    logInfo("Checking Config");

    // Test if we've already saved the config
    std::string configNetworkName = dot->getNetworkName();

    uint8_t *it = NwkSKey;
    for (uint8_t i = 0; i<16; i++)
        nwkSKey.push_back((uint8_t) *it++);
        
    it = AppSKey;
    for (uint8_t i = 0; i<16; i++)
        appSKey.push_back((uint8_t) *it++);

    it = NetworkAddr;
    for (uint8_t i = 0; i<4; i++)
        networkAddr.push_back((uint8_t) *it++);

    logInfo("Resetting Config");
    // reset to default config so we know what state we're in
    dot->resetConfig();

    // Set byte order - AEP less than 1.0.30
//    dot->setJoinByteOrder(mDot::LSB);
    dot->setJoinByteOrder(mDot::MSB);       // This is default for > 1.0.30 Conduit



    logInfo("Set TxPower");
    if((ret = dot->setTxPower( LORA_TXPOWER )) != mDot::MDOT_OK) {
        logError("Failed to set Tx Power %d:%s", ret, mDot::getReturnCodeString(ret).c_str());
    }

    logInfo("Set Public mode");
    if((ret = dot->setPublicNetwork(true)) != mDot::MDOT_OK) {
        logError("failed to set Public Mode %d:%s", ret, mDot::getReturnCodeString(ret).c_str());
    }

    logInfo("Set MANUAL Join mode");
    if((ret = dot->setJoinMode(mDot::MANUAL)) != mDot::MDOT_OK) {
        logError("Failed to set MANUAL Join Mode %d:%s", ret, mDot::getReturnCodeString(ret).c_str());
    }

    logInfo("Set Ack");
    // 1 retries on Ack, 0 to disable
    if((ret = dot->setAck( LORA_ACK)) != mDot::MDOT_OK) {
        logError("Failed to set Ack %d:%s", ret, mDot::getReturnCodeString(ret).c_str());
    }

    //Not applicable for 868MHz in EU
    if ((ret = dot->setFrequencySubBand(config_frequency_sub_band)) != mDot::MDOT_OK) {
        logError("failed to set frequency sub band", ret);
    }

    logInfo("Set Network Address");
    if ((ret = dot->setNetworkAddress(networkAddr)) != mDot::MDOT_OK) {
        logError("Failed to set Network Address %d:%s", ret, mDot::getReturnCodeString(ret).c_str());
    }

    logInfo("Set Data Session Key");
    if ((ret = dot->setDataSessionKey(appSKey)) != mDot::MDOT_OK) {
        logError("Failed to set Data Session Key %d:%s", ret, mDot::getReturnCodeString(ret).c_str());
    }

    logInfo("Set Network Session Key");
    if ((ret = dot->setNetworkSessionKey(nwkSKey)) != mDot::MDOT_OK) {
        logError("Failed to set Network Session Key %d:%s", ret, mDot::getReturnCodeString(ret).c_str());
    }

    logInfo("Saving Config");
    // Save config
    if (! dot->saveConfig()) {
        logError("failed to save configuration");
    }

    // Display what is set
    std::vector<uint8_t> tmp = dot->getNetworkSessionKey();
    pc.printf("Network Session Key: ");
    pc.printf("%s\r\n", mts::Text::bin2hexString(tmp, " ").c_str());

    tmp = dot->getDataSessionKey();
    pc.printf("Data Session Key: ");
    pc.printf("%s\r\n", mts::Text::bin2hexString(tmp, " ").c_str());

    pc.printf("Device ID ");
    std::vector<uint8_t> deviceId;
    deviceId = dot->getDeviceId();
    for (std::vector<uint8_t>::iterator it = deviceId.begin() ; it != deviceId.end(); ++it)
        pc.printf("%2.2x",*it );
    pc.printf("\r\n");

    std::vector<uint8_t> netAddress;

    pc.printf("Network Address ");
    netAddress = dot->getNetworkAddress();
    for (std::vector<uint8_t>::iterator it = netAddress.begin() ; it != netAddress.end(); ++it)
        pc.printf("%2.2x",*it );

    pc.printf("\r\n");

    // Display LoRa parameters
    // Display label and values in different colours, show pretty values not numeric values where applicable
    pc.printf("Public Network: %s\r\n", (char*)(dot->getPublicNetwork() ? "Yes" : "No") );
    pc.printf("Frequency: %s\r\n", (char*)mDot::FrequencyBandStr(dot->getFrequencyBand()).c_str() );
    pc.printf("Sub Band: %s\r\n", (char*)mDot::FrequencySubBandStr(dot->getFrequencySubBand()).c_str() );
    pc.printf("Join Mode: %s\r\n", (char*)mDot::JoinModeStr(dot->getJoinMode()).c_str() );
    pc.printf("Join Retries: %d\r\n", dot->getJoinRetries() );
    pc.printf("Join Byte Order: %s\r\n", (char*)(dot->getJoinByteOrder() == 0 ? "LSB" : "MSB") );
    pc.printf("Link Check Count: %d\r\n", dot->getLinkCheckCount() );
    pc.printf("Link Check Thold: %d\r\n", dot->getLinkCheckThreshold() );
    pc.printf("Tx Data Rate: %s\r\n", (char*)mDot::DataRateStr(dot->getTxDataRate()).c_str() );
    pc.printf("Tx Power: %d\r\n", dot->getTxPower() );
    pc.printf("TxWait: %s, ", (dot->getTxWait() ? "Y" : "N" ));
    pc.printf("CRC: %s, ", (dot->getCrc() ? "Y" : "N") );
    pc.printf("Ack: %s\r\n", (dot->getAck() ? "Y" : "N")  );

    logInfo("Joining Network");
    while ((ret = dot->joinNetwork()) != mDot::MDOT_OK) {
        logError("failed to join network [%d][%s]", ret, mDot::getReturnCodeString(ret).c_str());
        wait_ms(dot->getNextTxMs() + 1);
    }
    logInfo("Joined Network");
    wait_ms(500);
    
    /* Setup Sensors */
 
    logInfo("Configure Soil Sensor (SHT)");
/*    DigitalInOut sht_data(SHT_DATA_PIN);
    DigitalInOut sht_sck(SHT_SCK_PIN);
    sht_data.output();
    sht_sck.output();
    while (1) {
        sht_data.write(1);
        wait_ms(1);
        sht_data.write(0);
        wait_ms(1);
    }*/
 
    //logInfo("Configure Air Sensor (DHT)");
    // no config needed for DHT
    logInfo("Configure Water Sensor (DS)");
    wait_ms(500);
    DS18B20::ROM_Code_t ROM_Code;
    wait_ms(500);
    thermom.ReadROM(&ROM_Code);
    logInfo("Family code: 0x%X\n\r", ROM_Code.BYTES.familyCode);
    wait_ms(500);
    logInfo("Serial Number: ");
    for (unsigned i = 6; i != 0; --i) {
        logInfo("%02X%s", ROM_Code.BYTES.serialNo[i-1], (i != 1)?":":"\r\n");
    }
    logInfo("CRC: 0x%X\r\n", ROM_Code.BYTES.VTV);

    wait_ms(500);    
    logInfo("Configure Light Sensor (TSL)");
    tsl.enablePower();

    char dataBuf[50];
    uint16_t seq = 0;
    char * sf_str;
    while( 1 ) {
        
        /* cycle through spreading factors */
        uint8_t sf;
        seq = 0; /* force SF7 */
        switch (seq % 4) {
            case 0:
                sf = mDot::SF_7;
                sf_str = "SF7";
                break;
            case 1:
                sf = mDot::SF_8;
                sf_str = "SF8";
                break;
            case 2:
                sf = mDot::SF_9;
                sf_str = "SF9";
                break;
            case 3:
                sf = mDot::SF_10;
                sf_str = "SF10";
                break;
        }        
        // Set Spreading Factor, higher is lower data rate, smaller packets but longer range
        // Lower is higher data rate, larger packets and shorter range.
        logInfo("Set SF: %s",sf_str);
        if((ret = dot->setTxDataRate( sf )) != mDot::MDOT_OK) {
            logError("Failed to set SF %d:%s", ret, mDot::getReturnCodeString(ret).c_str());
        }
        
        /* set default data values */
        int temp = 0;
        int humid = -1;
        
        /* build packet */
        b.begin();
        uint8_t flag = 0;
        b.put(FormatSensor1);
        uint8_t * const pFlag = b.getp(); // save pointer to flag location
        b.put(0x00); // placeholder for flags
        
        // TODO: read battery voltage 
        b.putV(13.8);
        flag |= FlagVbat;
        
        // read from Bme280 sensor:
        float air_temp=0, air_humid=0;
        bool dht_success = dht.sample();    
        wait_ms(100);
        logInfo("Air Sensor Status: %s", dht_success?"OK":"ERROR");
        wait_ms(100);
        air_temp = (float)dht.getTemperature()/10.0;
        air_humid = (float)dht.getHumidity()/10.0;
        logInfo("Air Temp: %1.01fC  Air Humid: %1.01f%%", air_temp, air_humid);
        
        
        b.putT(air_temp); // air temp
        b.putP(1010.0); // air pressure
        b.putRH(air_humid); // air humidity
        flag |= FlagTPH;
        
        wait_ms(100);
        // read from light sensor
        float lux = tsl.getLux();
        logInfo("Ambient Light: %.4f", lux);
        wait_ms(100);
        b.putLux(lux*100); // ambient light
        flag |= FlagLux;
        
        // read water temperature
        wait_ms(100);
        logInfo("Running temperature conversion...");
        float water_temp = thermom.GetTemperature();
        logInfo("Water Temperature: %.4fC", water_temp);         
        wait_ms(100);
        b.putT(water_temp); // water temperature
        flag |= FlagWater;
        
        // read soil sensor
        wait_ms(100);
        logInfo("sht.ready = %s", sht.ready?"true":"false");
        sht.ready = true; // override error... 
        sht.reset();
        wait_ms(50);
        bool sht_success = sht.update();
        logInfo("Soil Sensor Status: %s", sht_success?"OK":"ERROR");
        float soil_temp = (float)sht.getTemperature();  
        float soil_humid = (float)(sht.humidity)/(float)35.0;
        logInfo("sht->humid = %d",sht.humidity);
        logInfo("Soil Temp: %1.01fC  Soil Humid: %1.01f%%", soil_temp, soil_humid);
        b.putT(soil_temp); // soil temperature
        b.putRH(soil_humid); // soil humidityupdate
        
        flag |= FlagSoilTH;
        
        // write flag byte
        *pFlag = flag;
        
        /* load vector */
        send_data.clear();
        uint8_t c;
        int n = b.getn();
        for( int i=0; i< n; i++ ) {
            c = b.buf[i];
            send_data.push_back( c );
        }

        wait_ms(100);
        /* send packet */
        if ((ret = dot->send(send_data)) != mDot::MDOT_OK) {
            logError("failed to send: [%d][%s]", ret, mDot::getReturnCodeString(ret).c_str());
        } else {
            logInfo("data len: %d,  send data: %s", n, Text::bin2hexString(send_data).c_str());
        }

        /* sleep */
        uint32_t sleep_time = MAX((dot->getNextTxMs() / 1000), 10 /* use 6000 for 10min */);
        logInfo("going to sleep for %d seconds", sleep_time);
        
        status_led.write(1);
        wait_ms(1*1000);
        status_led.write(0);
        wait_ms(4*1000);
        
        seq++;
    }

    return 0;
}
