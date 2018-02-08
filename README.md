# SmartRoomSensor
Smarter replacement for the RG20 room sensor that is an optional extra for some older NIBE heat pumps.

The NIBE RG20 room sensor is a very simple thermostat like device that is connected to a NIBE heatpump and lets you control the temperature in your home more preciesely. A little simplified the original consists of a potentiometer and an NTC resistor.

This replacement version has MQTT support and lets you control the setpoint and read back the current temperature from your home automation system. It also shows the status (heating or idle) of the heatpump.
It connects to the same interface in the heatpump as the RG20 does plus that it steals some power from the heatpump internal PSU (which is around 20-24V).

The hardware is based on on an ESP8266 module, the ESP-12F, combining it with a digital potentiometer for the setpoint control and an NTC resistor for temperature reading. The heatpump power supply has reduced voltage when the heatpump is active, so measuring that voltage makes it possible to detect if the heatpumpt is active or not.

Ths software is based on Arduino and support libraries WiFiManager and PubSubClinet.
A minor modification to PubSubClient is needed, so the modified file is also included.

PS. This is currently work in progress. The uploaded version of schematics and PCB has not been built as a complete prototype, but rather as a patched version of an older PCB revision. I.e. it is not 100% guranteed that it works, even if the risk that it does not should be small.

# Prerequisites
To use this unit, you need to have:

A NIBE heatpump that is compatible with the RG20 room sensor (Doh!)

An MQTT server (like mosquitto) running on a server somewhere.

Some kind of home automation software to display a user interface and possibly atuomate the control of the unit (like Home Assistant).

# How to connect to the heatpump
The sensor has a screw terminal that is used to connect wires coming from the heatpump.
The marking on the PCB is the same as the original RG20, plus the additional terminal for the power.
See the installation manual for the original sensor for connection to the heat pump.
http://www.nibe.se/nibedocuments/10013/011022-6.pdf

One thing is different, the power connection is marked with "+" next to its screw terminal on the PCB, but in the heat pump there is no easy accessible terminal for this. So, instead we connect to one of the solder pins on an IDC header that carries the voltage we want to access. A probe clip, such as Farnell part number 523525 makes this easier.

# How to configure the unit
When intially powered up the unit will start in access point mode, creating a WiFi network called "SmartRoomSensor".
Connect to this network using a computer or a smartphone.

Normally a web browser with the configuration interface should open automatically and you can enter the configuration details needed.

You will need to enter the name of your Wifi network, the password, the MQTT server IP or name, MQTT server username/password (if any) and the topics for MQTT.

There are 4 MQTT topics:

tset: the topic the unit will subscribe to get updates on target temperature.

tget: the topic where the unit will publish the configured target temperature.

cget: the topic where the unit will publish the current measured temperature.

mode: the topic where the unit will publish the current heatpump mode (heat or idle).

# Known problems
The unit does heat up a bit, compared to the surrounding room temperature, which results in a temperature reading that is 1-2 degrees more than the actual temperature. So the target temperature needs to be increased accordingly. I.e. if you want to have 21 degrees in the room, set 22 or 23 degrees.
This could possibly be compensated by increasing the value of R2 slightly, but this has not (yet) been tested.
