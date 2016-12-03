// TODO: add license header

#ifndef MDOT_H
#define MDOT_H

#include "mbed.h"
#include "rtos.h"
#include <vector>
#include <map>
#include <string>

class LoRaMacEvent;
class LoRaConfig;
class LoRaMac;
class MdotRadio;

class mDot {

    private:

        mDot();
        ~mDot();

        static void idle(void const* args) {
            while (1)
                __WFI();
        }

        void setLastError(const std::string& str);

        static bool validateBaudRate(const uint32_t& baud);
        static bool validateFrequencySubBand(const uint8_t& band);
        bool validateDataRate(const uint8_t& dr);

        int32_t joinBase(const uint32_t& retries);
        int32_t sendBase(const std::vector<uint8_t>& data, const bool& confirmed = false, const bool& blocking = true, const bool& highBw = false);
        void waitForPacket();
        void waitForLinkCheck();

        void setActivityLedState(const uint8_t& state);
        uint8_t getActivityLedState();

        void blinkActivityLed(void) {
            if (_activity_led) {
                int val = _activity_led->read();
                _activity_led->write(!val);
            }
        }

        mDot(const mDot&);
        mDot& operator=(const mDot&);

        uint32_t RTC_ReadBackupRegister(uint32_t RTC_BKP_DR);
        void RTC_WriteBackupRegister(uint32_t RTC_BKP_DR, uint32_t Data);

        void wakeup();

        static mDot* _instance;

        LoRaMac* _mac;
        MdotRadio* _radio;
        LoRaMacEvent* _events;
        LoRaConfig* _config;
        Thread _idle_thread;
        std::string _last_error;
        static const uint32_t _baud_rates[];
        uint8_t _activity_led_state;
        Ticker _tick;
        DigitalOut* _activity_led;
        bool _activity_led_enable;
        PinName _activity_led_pin;
        bool _activity_led_external;
        uint8_t _linkFailCount;
        uint8_t _class;
        InterruptIn* _wakeup;
        PinName _wakeup_pin;

        typedef enum {
            OFF, ON, BLINK,
        } state;

    public:

        typedef enum {
            FM_APPEND = (1 << 0),
            FM_TRUNC = (1 << 1),
            FM_CREAT = (1 << 2),
            FM_RDONLY = (1 << 3),
            FM_WRONLY = (1 << 4),
            FM_RDWR = (FM_RDONLY | FM_WRONLY),
            FM_DIRECT = (1 << 5)
        } FileMode;

        typedef enum {
            MDOT_OK = 0,
            MDOT_INVALID_PARAM = -1,
            MDOT_TX_ERROR = -2,
            MDOT_RX_ERROR = -3,
            MDOT_JOIN_ERROR = -4,
            MDOT_TIMEOUT = -5,
            MDOT_NOT_JOINED = -6,
            MDOT_ENCRYPTION_DISABLED = -7,
            MDOT_NO_FREE_CHAN = -8,
            MDOT_ERROR = -1024,
        } mdot_ret_code;

        enum JoinMode {
            MANUAL, OTA, AUTO_OTA
        };

        enum Mode {
            COMMAND_MODE, SERIAL_MODE
        };

        enum RX_Output {
            HEXADECIMAL, BINARY
        };

        enum DataRates {
            SF_12, SF_11, SF_10, SF_9, SF_8, SF_7, SF_7H, SF_50
        };

        enum FrequencyBands {
            FB_868, FB_915
        };

        enum FrequencySubBands {
            FSB_ALL, FSB_1, FSB_2, FSB_3, FSB_4, FSB_5, FSB_6, FSB_7, FSB_8
        };

        enum JoinByteOrder {
            LSB, MSB
        };

        enum wakeup_mode {
            RTC_ALARM, INTERRUPT
        };

        typedef struct {
                int16_t fd;
                char name[30];
                uint32_t size;
        } mdot_file;

        typedef struct {
                uint32_t Up;
                uint32_t Down;
                uint32_t Joins;
                uint32_t JoinFails;
                uint32_t MissedAcks;
        } mdot_stats;

        typedef struct {
                int16_t last;
                int16_t min;
                int16_t max;
                int16_t avg;
        } rssi_stats;

        typedef struct {
                int8_t last;
                int8_t min;
                int8_t max;
                int8_t avg;
        } snr_stats;

        typedef struct {
                bool status;
                int32_t dBm;
                uint32_t gateways;
                std::vector<uint8_t> payload;
        } link_check;

        typedef struct {
                int32_t status;
                int16_t rssi;
                int16_t snr;
        } ping_response;

        static const uint8_t MaxLengths_915[];
        static const uint8_t MaxLengths_868[];

        static std::string JoinModeStr(uint8_t mode);
        static std::string ModeStr(uint8_t mode);
        static std::string RxOutputStr(uint8_t format);
        static std::string DataRateStr(uint8_t rate);
        static std::string FrequencyBandStr(uint8_t band);
        static std::string FrequencySubBandStr(uint8_t band);

        /** Get a handle to the singleton object
         * @returns pointer to mDot object
         */
        static mDot* getInstance();

        /** Get library version information
         * @returns string containing library version information
         */
        std::string getId();

        /** Perform a soft reset of the system
         */
        void resetCpu();

        /** Reset config to factory default
         */
        void resetConfig();

        /** Save config data to non volatile memory
         * @returns true if success, false if failure
         */
        bool saveConfig();

        /** Set the log level for the library
         * options are:
         *  NONE_LEVEL - logging is off at this level
         *  FATAL_LEVEL - only critical errors will be reported
         *  ERROR_LEVEL
         *  WARNING_LEVEL
         *  INFO_LEVEL
         *  DEBUG_LEVEL
         *  TRACE_LEVEL - every detail will be reported
         * @param level the level to log at
         * @returns MDOT_OK if success
         */
        int32_t setLogLevel(const uint8_t& level);

        /** Get the current log level for the library
         * @returns current log level
         */
        uint8_t getLogLevel();

        /** Enable or disable the activity LED.
         * @param enable true to enable the LED, false to disable
         */
        void setActivityLedEnable(const bool& enable);

        /** Find out if the activity LED is enabled
         * @returns true if activity LED is enabled, false if disabled
         */
        bool getActivityLedEnable();

        /** Use a different pin for the activity LED.
         * The default is XBEE_RSSI.
         * @param pin the new pin to use
         */
        void setActivityLedPin(const PinName& pin);

        /** Use an external DigitalOut object for the activity LED.
         * The pointer must stay valid!
         * @param pin the DigitalOut object to use
         */
        void setActivityLedPin(DigitalOut* pin);

        /** Find out what pin the activity LED is on
         * @returns the pin the activity LED is using
         */
        PinName getActivityLedPin();

        /** Get list of channel frequencies currently in use
         * @returns vector of channels currently in use
         */
        std::vector<uint32_t> getChannels();

        /** Get frequency band
         * @returns FB_915 if configured for United States, FB_868 if configured for Europe
         */
        uint8_t getFrequencyBand();

        /** Set frequency sub band
         * only applicable if frequency band is set for United States (FB_915)
         * sub band 0 will allow the radio to use all 64 channels 
         * sub band 1 - 8 will allow the radio to use the 8 channels in that sub band
         * for use with Conduit gateway and MTAC_LORA, use sub bands 1 - 8, not sub band 0
         * @param band the sub band to use (0 - 8)
         * @returns MDOT_OK if success
         */
        int32_t setFrequencySubBand(const uint8_t& band);

        /** Get frequency sub band
         * @returns frequency sub band currently in use
         */
        uint8_t getFrequencySubBand();

        /** Enable/disable public network mode
         * @param on should be true to enable public network mode
         * @returns MDOT_OK if success
         */
        int32_t setPublicNetwork(const bool& on);

        /** Get public network mode
         * @returns true if public network mode is enabled
         */
        bool getPublicNetwork();

        /** Get the device ID
         * @returns vector containing the device ID (size 8)
         */
        std::vector<uint8_t> getDeviceId();

        /** Set network address
         * for use with MANUAL network join mode, will be assigned in OTA & AUTO_OTA modes
         * @param addr a vector of 4 bytes
         * @returns MDOT_OK if success
         */
        int32_t setNetworkAddress(const std::vector<uint8_t>& addr);

        /** Get network address
         * @returns vector containing network address (size 4)
         */
        std::vector<uint8_t> getNetworkAddress();

        /** Set network session key
         * for use with MANUAL network join mode, will be assigned in OTA & AUTO_OTA modes
         * @param key a vector of 16 bytes
         * @returns MDOT_OK if success
         */
        int32_t setNetworkSessionKey(const std::vector<uint8_t>& key);

        /** Get network session key
         * @returns vector containing network session key (size 16)
         */
        std::vector<uint8_t> getNetworkSessionKey();

        /** Set data session key
         * for use with MANUAL network join mode, will be assigned in OTA & AUTO_OTA modes
         * @param key a vector of 16 bytes
         * @returns MDOT_OK if success
         */
        int32_t setDataSessionKey(const std::vector<uint8_t>& key);

        /** Get data session key
         * @returns vector containing data session key (size 16)
         */
        std::vector<uint8_t> getDataSessionKey();

        /** Set network name
         * for use with OTA & AUTO_OTA network join modes
         * generates network ID (crc64 of name) automatically
         * @param name a string of of at least 8 bytes and no more than 128 bytes
         * @return MDOT_OK if success
         */
        int32_t setNetworkName(const std::string& name);

        /** Get network name
         * @return string containing network name (size 8 to 128)
         */
        std::string getNetworkName();

        /** Set network ID
         * for use with OTA & AUTO_OTA network join modes
         * setting network ID via this function sets network name to empty
         * @param id a vector of 8 bytes
         * @returns MDOT_OK if success
         */
        int32_t setNetworkId(const std::vector<uint8_t>& id);

        /** Get network ID
         * @returns vector containing network ID (size 8)
         */
        std::vector<uint8_t> getNetworkId();

        /** Set network passphrase
         * for use with OTA & AUTO_OTA network join modes
         * generates network key (cmac of passphrase) automatically
         * @param name a string of of at least 8 bytes and no more than 128 bytes
         * @return MDOT_OK if success
         */
        int32_t setNetworkPassphrase(const std::string& passphrase);

        /** Get network passphrase
         * @return string containing network passphrase (size 8 to 128)
         */
        std::string getNetworkPassphrase();

        /** Set network key
         * for use with OTA & AUTO_OTA network join modes
         * setting network key via this function sets network passphrase to empty
         * @param id a vector of 16 bytes
         * @returns MDOT_OK if success
         */
        int32_t setNetworkKey(const std::vector<uint8_t>& id);

        /** Get network key
         * @returns a vector containing network key (size 16)
         */
        std::vector<uint8_t> getNetworkKey();

        /** Set join byte order
         * @param order 0:LSB 1:MSB
         */
        uint32_t setJoinByteOrder(uint8_t order);

        /** Get join byte order
         * @returns byte order to use in joins 0:LSB 1:MSB
         */
        uint8_t getJoinByteOrder();

        /** Attempt to join network
         * retries according to configuration set by setJoinRetries()
         * @returns MDOT_OK if success
         */
        int32_t joinNetwork();

        /** Attempts to join network once
         * @returns MDOT_OK if success
         */
        int32_t joinNetworkOnce();

        /** Resets current network session, essentially disconnecting from the network
         * has no effect for MANUAL network join mode
         */
        void resetNetworkSession();

        /** Set number of times joining will retry before giving up
         * @param retries must be between 0 - 255
         * @returns MDOT_OK if success
         */
        int32_t setJoinRetries(const uint8_t& retries);

        /** Set number of times joining will retry before giving up
         * @returns join retries (0 - 255)
         */
        uint8_t getJoinRetries();

        /** Set network join mode
         * MANUAL: set network address and session keys manually
         * OTA: User sets network name and passphrase, then attempts to join
         * AUTO_OTA: same as OTA, but network sessions can be saved and restored
         * @param mode MANUAL, OTA, or AUTO_OTA
         * @returns MDOT_OK if success
         */
        int32_t setJoinMode(const uint8_t& mode);

        /** Get network join mode
         * @returns MANUAL, OTA, or AUTO_OTA
         */
        uint8_t getJoinMode();

        /** Get network join status
         * @returns true if currently joined to network
         */
        bool getNetworkJoinStatus();

        /** Do a network link check
         * application data may be returned in response to a network link check command
         * @returns link_check structure containing success, dBm above noise floor, gateways in range, and packet payload
         */
        link_check networkLinkCheck();

        /** Set network link check count to perform automatic link checks every count packets
         * only applicable if ACKs are disabled
         * @param count must be between 0 - 255
         * @returns MDOT_OK if success
         */
        int32_t setLinkCheckCount(const uint8_t& count);

        /** Get network link check count
         * @returns count (0 - 255)
         */
        uint8_t getLinkCheckCount();

        /** Set network link check threshold, number of link check failures or missed acks to tolerate
         * before considering network connection lost
         * @pararm count must be between 0 - 255
         * @returns MDOT_OK if success
         */
        int32_t setLinkCheckThreshold(const uint8_t& count);

        /** Get network link check threshold
         * @returns threshold (0 - 255)
         */
        uint8_t getLinkCheckThreshold();

        /** Get UpLinkCounter
         * @returns number of packets sent to the gateway during this network session (sequence number)
         */
        uint32_t getUpLinkCounter();

        /** Enable/disable AES encryption
         * AES encryption must be enabled for use with Conduit gateway and MTAC_LORA card
         * @param on true for AES encryption to be enabled
         * @returns MDOT_OK if success
         */
        int32_t setAesEncryption(const bool& on);

        /** Get AES encryption
         * @returns true if AES encryption is enabled
         */
        bool getAesEncryption();

        /** Get RSSI stats
         * @returns rssi_stats struct containing last, min, max, and avg RSSI
         */
        rssi_stats getRssiStats();

        /** Get SNR stats
         * @returns snr_stats struct containing last, min, max, and avg SNR
         */
        snr_stats getSnrStats();

        /** Get ms until next free channel
         * only applicable for European models, US models return 0
         * @returns time (ms) until a channel is free to use for transmitting
         */
        uint32_t getNextTxMs();

        /** Get data pending
         * only valid after sending data to the gateway
         * @returns true if server has available packet(s)
         */
        bool getDataPending();

        /** Get transmitting 
         * @returns true if currently transmitting
         */
        bool getIsTransmitting();

        /** Set TX data rate
         * data rates affect maximum payload size
         * @param dr SF_7 - SF_12 for Europe, SF_7 - SF_10 for United States
         * @returns MDOT_OK if success
         */
        int32_t setTxDataRate(const uint8_t& dr);

        /** Get TX data rate
         * @returns current TX data rate (SF_7 - SF_12)
         */
        uint8_t getTxDataRate();

        /** Set TX power
         * power affects maximum range
         * @param power 2 dBm - 20 dBm
         * @returns MDOT_OK if success
         */
        int32_t setTxPower(const uint32_t& power);

        /** Get TX power
         * @returns TX power (2 dBm - 20 dBm)
         */
        uint32_t getTxPower();

        /** Enable/disable TX waiting for rx windows
         * when enabled, send calls will block until a packet is received or RX timeout
         * @param enable set to true if expecting responses to transmitted packets
         * @returns MDOT_OK if success
         */
        int32_t setTxWait(const bool& enable);

        /** Get TX wait
         * @returns true if TX wait is enabled
         */
        bool getTxWait();

        /** Get time on air
         * @returns the amount of time (in ms) it would take to send bytes bytes based on current configuration
         */
        uint32_t getTimeOnAir(uint8_t bytes);

        /** Get min frequency
         * @returns minimum frequency based on current configuration
         */
        uint32_t getMinFrequency();

        /** Get max frequency
         * @returns maximum frequency based on current configuration
         */
        uint32_t getMaxFrequency();

        /** Set forward error correction bytes
         * @param bytes 1 - 4 bytes
         * @returns MDOT_OK if success
         */
        int32_t setFec(const uint8_t& bytes);

        /** Get forward error correction bytes
         * @returns bytes (1 - 4)
         */
        uint8_t getFec();

        /** Enable/disable CRC checking of packets
         * CRC checking must be enabled for use with Conduit gateway and MTAC_LORA card
         * @param on set to true to enable CRC checking
         * @returns MDOT_OK if success
         */
        int32_t setCrc(const bool& on);

        /** Get CRC checking
         * @returns true if CRC checking is enabled
         */
        bool getCrc();

        /** Set ack
         * @param retries 0 to disable acks, otherwise 1 - 8
         * @returns MDOT_OK if success
         */
        int32_t setAck(const uint8_t& retries);

        /** Get ack
         * @returns 0 if acks are disabled, otherwise retries (1 - 8)
         */
        uint8_t getAck();

        /** Send data to the gateway
         * validates data size (based on spreading factor)
         * @param data a vector of up to 242 bytes (may be less based on spreading factor)
         * @returns MDOT_OK if packet was sent successfully (ACKs disabled), or if an ACK was received (ACKs enabled)
         */
        int32_t send(const std::vector<uint8_t>& data, const bool& blocking = true, const bool& highBw = false);

        /** Fetch data received from the gateway
         * this function only checks to see if a packet has been received - it does not open a receive window
         * send() must be called before recv()
         * @param data a vector to put the received data into
         * @returns MDOT_OK if packet was successfully received
         */
        int32_t recv(std::vector<uint8_t>& data);


        /** Ping
         * status will be MDOT_OK if ping succeeded
         * @returns ping_response struct containing status, RSSI, and SNR
         */
        ping_response ping();

        /** Get return code string
         * @returns string containing a description of the given error code
         */
        static std::string getReturnCodeString(const int32_t& code);

        /** Get last error
         * @returns string explaining the last error that occured
         */
        std::string getLastError();

        /** Go to sleep
         * @param interval the number of seconds to sleep before waking up if wakeup_mode == RTC_ALARM, else ignored
         * @param wakeup_mode RTC_ALARM, INTERRUPT
         *      if RTC_ALARM the real time clock is configured to wake the device up after the specified interval
         *      if INTERRUPT the device will wake up on the rising edge of the interrupt pin
         * @param deepsleep if true go into deep sleep mode (lowest power, all memory and registers are lost, peripherals turned off)
         *                  else go into sleep mode (low power, memory and registers are maintained, peripherals stay on)
         *
         * ** CURRENTLY ONLY DEEPSLEEP MODE IS AVAILABLE **
         *
         * in sleep mode, the device can be woken up on any of the XBEE pins or by the RTC alarm
         * in deepsleep mode, the device can only be woken up using the WKUP pin (PA0, XBEE_DIO7) or by the RTC alarm
         */
        void sleep(const uint32_t& interval, const uint8_t& wakeup_mode = RTC_ALARM, const bool& deepsleep = true);

        /** Set wake pin
         * @param pin the pin to use to wake the device from sleep mode
         */
        void setWakePin(const PinName& pin);

        /** Get wake pin
         * @returns the pin to use to wake the device from sleep mode
         */
        PinName getWakePin();

        /******************************************
         * THESE FEATURES ARE NOT FULLY IMPLEMENTED
         *****************************************/
        // get/set adaptive data rate
        // configure data rates and power levels based on signal to noise information from gateway
        // true == adaptive data rate is on
        // set function returns MDOT_OK if success
        int32_t setAdr(const bool& on);
        bool getAdr();

        /*************************************************************************
         * The following functions are only used by the AT command application and
         * should not be used by standard applications consuming the mDot library
         ************************************************************************/

        // set/get configured baud rate for command port
        // only for use in conjunction with AT interface
        // set function returns MDOT_OK if success
        int32_t setBaud(const uint32_t& baud);
        uint32_t getBaud();

        // set/get baud rate for debug port
        // set function returns MDOT_OK if success
        int32_t setDebugBaud(const uint32_t& baud);
        uint32_t getDebugBaud();

        // set/get command terminal echo
        // set function returns MDOT_OK if success
        int32_t setEcho(const bool& on);
        bool getEcho();

        // set/get command terminal verbose mode
        // set function returns MDOT_OK if success
        int32_t setVerbose(const bool& on);
        bool getVerbose();

        // set/get startup mode
        // COMMAND_MODE (default), starts up ready to accept AT commands
        // SERIAL_MODE, read serial data and send it as LoRa packets
        // set function returns MDOT_OK if success
        int32_t setStartUpMode(const uint8_t& mode);
        uint8_t getStartUpMode();

        int32_t setRxDataRate(const uint8_t& dr);
        uint8_t getRxDataRate();

        // get/set TX/RX frequency
        // if frequency band == FB_868 (Europe), must be between 863000000 - 870000000
        // if frequency band == FB_915 (United States), must be between 902000000-928000000
        // if set to 0, device will hop frequencies
        // set function returns MDOT_OK if success
        int32_t setTxFrequency(const uint32_t& freq);
        uint32_t getTxFrequency();
        int32_t setRxFrequency(const uint32_t& freq);
        uint32_t getRxFrequency();

        // get/set TX/RX inverted
        // true == signal is inverted
        // set function returns MDOT_OK if success
        int32_t setTxInverted(const bool& on);
        bool getTxInverted();
        int32_t setRxInverted(const bool& on);
        bool getRxInverted();

        // get/set RX output mode
        // valid options are HEXADECIMAL and BINARY
        // set function returns MDOT_OK if success
        int32_t setRxOutput(const uint8_t& mode);
        uint8_t getRxOutput();

        // get/set serial wake interval
        // valid values are 2 s - INT_MAX (2147483647) s
        // set function returns MDOT_OK if success
        int32_t setWakeInterval(const uint32_t& interval);
        uint32_t getWakeInterval();

        // get/set serial wake delay
        // valid values are 2 ms - INT_MAX (2147483647) ms
        // set function returns MDOT_OK if success
        int32_t setWakeDelay(const uint32_t& delay);
        uint32_t getWakeDelay();

        // get/set serial receive timeout
        // valid values are 0 ms - 65000 ms
        // set function returns MDOT_OK if success
        int32_t setWakeTimeout(const uint16_t& timeout);
        uint16_t getWakeTimeout();

        // get/set serial wake mode
        // valid values are INTERRUPT or RTC_ALARM
        // set function returns MDOT_OK if success
        int32_t setWakeMode(const uint8_t& delay);
        uint8_t getWakeMode();

        // Save user file data to flash
        // file - name of file max 30 chars
        // data - data of file
        // size - size of file
        bool saveUserFile(const char* file, void* data, uint32_t size);

        // Append user file data to flash
        // file - name of file max 30 chars
        // data - data of file
        // size - size of file
        bool appendUserFile(const char* file, void* data, uint32_t size);

        // Read user file data from flash
        // file - name of file max 30 chars
        // data - data of file
        // size - size of file
        bool readUserFile(const char* file, void* data, uint32_t size);

        // Move a user file in flash
        // file     - name of file
        // new_name - new name of file
        bool moveUserFile(const char* file, const char* new_name);

        // Delete user file data from flash
        // file - name of file max 30 chars
        bool deleteUserFile(const char* file);

        // Open user file in flash, max of 4 files open concurrently
        // file - name of file max 30 chars
        // mode - combination of FM_APPEND | FM_TRUNC | FM_CREAT |
        //                       FM_RDONLY | FM_WRONLY | FM_RDWR | FM_DIRECT
        // returns - mdot_file struct, fd field will be a negative value if file could not be opened
        mDot::mdot_file openUserFile(const char* file, int mode);

        // Seek an open file
        // file - mdot file struct
        // offset - offset in bytes
        // whence - where offset is based SEEK_SET, SEEK_CUR, SEEK_END
        bool seekUserFile(mDot::mdot_file& file, size_t offset, int whence);

        // Read bytes from open file
        // file - mdot file struct
        // data - mem location to store data
        // length - number of bytes to read
        // returns - number of bytes written
        int readUserFile(mDot::mdot_file& file, void* data, size_t length);

        // Write bytes to open file
        // file - mdot file struct
        // data - data to write
        // length - number of bytes to write
        // returns - number of bytes written
        int writeUserFile(mDot::mdot_file& file, void* data, size_t length);

        // Close open file
        // file - mdot file struct
        bool closeUserFile(mDot::mdot_file& file);

        // List user files stored in flash
        std::vector<mDot::mdot_file> listUserFiles();

        // Move file into the firmware upgrade path to be flashed on next boot
        // file - name of file
        bool moveUserFileToFirmwareUpgrade(const char* file);

        // get current statistics
        // Join Attempts, Join Fails, Up Packets, Down Packets, Missed Acks
        mdot_stats getStats();

        // reset statistics
        // Join Attempts, Join Fails, Up Packets, Down Packets, Missed Acks
        void resetStats();

        // Convert pin number 2-8 to pin name DIO2-DI8
        static PinName pinNum2Name(uint8_t num);

        // Convert pin name DIO2-DI8 to pin number 2-8
        static uint8_t pinName2Num(PinName name);

        // Convert pin name DIO2-DI8 to string
        static std::string pinName2Str(PinName name);


        uint64_t crc64(uint64_t crc, const unsigned char *s, uint64_t l);

        // MTS_RADIO_DEBUG_COMMANDS

        void openRxWindow(uint32_t timeout, uint8_t bandwidth = 0);
        void sendContinuous();
        int32_t setDeviceId(const std::vector<uint8_t>& id);
        int32_t setFrequencyBand(const uint8_t& band);
        bool saveProtectedConfig();
        void resetRadio();
        int32_t setRadioMode(const uint8_t& mode);
        std::map<uint8_t, uint8_t> dumpRegisters();
        void eraseFlash();


        // deprecated - use setWakeInterval
        int32_t setSerialWakeInterval(const uint32_t& interval);
        // deprecated - use getWakeInterval
        uint32_t getSerialWakeInterval();

        // deprecated - use setWakeDelay
        int32_t setSerialWakeDelay(const uint32_t& delay);
        // deprecated - use setWakeDelay
        uint32_t getSerialWakeDelay();

        // deprecated - use setWakeTimeout
        int32_t setSerialReceiveTimeout(const uint16_t& timeout);
        // deprecated - use getWakeTimeout
        uint16_t getSerialReceiveTimeout();

    private:
        mdot_stats _stats;

};

#endif
