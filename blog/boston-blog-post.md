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
 - Notebook
 - Laptop
 - Oscilloscope (optional)

Firmware
-------



The Things Network
-----------------


Scriptr
-------

mqtt to rest bridge
dataviz


Call to Action
----

follow Scriptr, Multitech, and TTN NY on Twitter ?

