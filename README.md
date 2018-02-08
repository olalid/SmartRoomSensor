# SmartRoomSensor
Smarter replacement for the RG20 room sensor that is an optional extra for some older NIBE heat pumps.

The NIBE RG20 room sensor is a very simple thermostat like device that is connected to a NIBE heatpump and lets you control the temperature in your home more preciesely. A little simplified the original consists of a potentiometer and an NTC resistor.

This replacement version has MQTT support and lets you control the setpoint and read back the current temperature from your home automation system. It also shows the status (heating or idle) of the heatpump.
It connects to the same interface in the heatpump as the RG20 does plus that it steals some power from the heatpump internal PSU (which is around 20-24V).

The hardware is based on on an ESP8266 module, the ESP-12F, combining it with a digital potentiometer for the setpoint control and an NTC resistor for temperature reading.
Ths software is based on Arduino and support libraries WiFiManager and PubSubClinet.
A minor modification to PubSubClient is needed, so the modified file is also included.

PS. This is currently work in progress. The uploaded version of schematics and PCB has not been built as a complete prototype, but rather as a patched version of an older PCB revision. I.e. it is not 100% guranteed that it works, even if the risk that it does not should be small.

# How to connect to the heatpump
The sensor has a screw terminal that is used to connect wires coming from the heatpump.
The marking on the PCB is the same as the original RG20, plus the additional terminal for the power.
See the installation manual for the original sensor for connection to the heat pump.
http://www.nibe.se/nibedocuments/10013/011022-6.pdf

One thing is different, the power connection is marked with "+" next to its screw terminal on the PCB, but in the heat pump there is no easy accessible terminal for this. So, instead we connect to one of the solder pins on an IDC header that carries the voltage we want to access. A probe clip, such as Farnell part number 523525 makes this easier.
