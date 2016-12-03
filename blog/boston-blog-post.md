Smart Rain Gardens for Greener Cities
=====================================

TODO:

Intro, discuss need for rain gardens in cities,
and how IoT can help make them more awesome.

Overview of demo'd solution,
including block diagram

Discuss each piece of the system:

Why Smart Rain Gardens
-----

Overview of Solution
-------

![Block Diagram](block_diag.jpg)

Hardware
-------

The first thing to consider when building an Internet of Things device is,
what exactly is being measured? For a rain garden, 
we want to know the temperature and moisture of the growing soil,
the temperature of water leaving the drain spout and entering the garden,
as well as envronmental conditions like light level, air temperature, and humidity.

To monitor these variables, we will need a suite of sensors.
We selected the following sensors:

 - [Ambient Light Sensor](https://www.adafruit.com/products/439)  (TSL2561)
 - [Soil Temperature and Moisture Sensor](https://www.adafruit.com/products/1298) (SHT10)
 - [Water Temperature Sensor](https://www.adafruit.com/products/381) (DS18B20)
 - [Air Temperature and Humidity Sensor](https://www.adafruit.com/products/385) (DHT22)

Next we need a microcontroller to read from the sensors, and a radio to send sensor data up to the internet.
There are many options here. We settled on mDot from MultiTech because it combines a microcontroller and radio into an easy-to-use yet powerful package:

 - [MultiTech mDot](http://www.multitech.com/models/94557148LF) (MTDOT-915-X1P-SMA-1)
 - [mDot Developer Kit](http://www.multitech.com/models/94558010LF) (MTUDK2-ST-MDOT)
 
Lastly, we need just a few miscellaneous components:

 - Thin hookup wire (30AWG "wirewrap" wire works great)
 - 2x 4.7kOhm thru-hole resistors

And a few tools:

 - Soldering Tools (iron, solder, flux, needle-nose pliers)
 - Notebook for notes while wiring
 - Laptop for firmware development and debugging
 - Desk vise like [Panavise](http://www.panavise.com/) for holding circuit boards
 - Oscilloscope (optional) for hardware debugging 
 
TODO: Assembly instructions, with pinouts.

Firmware
-------

With the node constructed, it's time to write firmware to breathe life into the device!

We used the [ARM mBed](https://developer.mbed.org/) platform for wiring firmware. While traditional development environments (IDEs like Arduino, IAR, Keil) require a software installation, mBed works entirely in your web browser, making it easy to get started.

On mBed, the first step is to find an example application for using the mDot. We started with a [project](https://developer.mbed.org/users/merckeng/code/mDot_TTN_DHT11/) that demonstrated using the mDot to read a DHT11 sensor and send data to The Things Network. However, we still need to teach the mDot to read from the sensors we have selected. For this, we need *libraries*.

By using the mBed compiler's "import" tool, we found a library for each of our sensors:

 - [DHT22 Library by Julepalme](http://mbed.org/users/Julepalme/code/DHT22/)
 - [DS18B20 Library from Richard Lane](https://developer.mbed.org/users/richardlane/code/DS18B20_1wire/)
 - [SHTx Library courtesy of Ron of NegativeBlack](https://developer.mbed.org/users/richardlane/code/DS18B20_1wire/)
 - [TSL2561_I2C Library by Karl Maxwell](https://developer.mbed.org/users/karlmaxwell67/code/TSL2561_I2C/)

We imported each of the libraries into our mBed project, and added code to use the libraries, initialize the sensors, read data, and prepare the data for sending to The Things Network. To get started right away, you can [click here](https://developer.mbed.org/users/merckeng/code/mDot_TTN_DHT11_Boston16_CAM/) to fork the project repository on mBed.

This code reads from all sensors and sends a packet of data every 5 seconds. This is for testing and demonstration purposes only! In a real application, you will only want to send about one packet per hour to conserve spectrum for other users.

The Things Network
-----------------


Scriptr
-------

mqtt to rest bridge
dataviz


Call to Action
----

follow Scriptr, Multitech, and TTN NY on Twitter ?

