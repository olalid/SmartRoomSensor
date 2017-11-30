# SmartRoomSensor
Smarter replacement for the RG20 room sensor that is an optional extra for some older NIBE heat pumps.

The NIBE RG20 room sensor is a very simple thermostat like device that is connected to a NIBE heatpump and lets you control the temperature in your home more preciesely. A little simplified the original consists of a potentiometer and an NTC resistor.

This replacement version has MQTT support and lets you control the setpoint and read back the current temperature from your home automation system. It also shows the status (heating or idle) of the heatpump.
It connects to the same interface in the heatpump as the RG20 does plus that it steals some power from the heatpump internal PSU (which is around 20-24V).

The hardware is based on on an ESP8266 module, the ESP-12F, combining it with a digital potentiometer for the setpoint control and an NTC resistor for temperature reading.
Ths software is based on Arduino and support libraries WiFiManager and PubSubClinet.

PS. The uploaded HW version is untested, although an earlier version has been built and tested with mostly good results. This version improves the power supply (DC/DC instead of all linear) and adds the support for measuring the supply voltage (which actually enables us to understand if the heatpump is working or not since the voltage will drop significantly when it does). The SW needs some work to add support for the heating/idle indication that is not supported by the current HW.

# How to connect to the heatpump
The sensor has a screw terminal that is used to connect wires coming from the heatpump.
The marking on the PCB is the same as the original RG20, plus the additional terminal for the power.
See the installation manual for the original sensor for connection to the heat pump.
http://www.nibe.se/nibedocuments/10013/011022-6.pdf
The power supply should connect to the power in the heatpump, located at the IDC header that is used to connect the front panel display with the relay board. The simplest solution is to use a probe clip such as part number 523525 from Farnell to connect this in the heatpump.
