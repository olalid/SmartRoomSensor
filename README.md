# SmartRoomSensor
Smarter replacement for the RG20 room sensor that is an optional extra for some older NIBE heat pumps.

The NIBE RG20 room sensor is a very simple thermostat like device that is connected to a NIBE heatpump and lets you control the temperature in your home more preciesely. A little simplified the original consists of a potentiometer and an NTC resistor.

This replacement version has MQTT support and lets you control the setpoint and read back the current temperature from your home automation system. It also shows the status (heating or idle) of the heatpump.
It connects to the same interface in the heatpump as the RG20 does plus that it steals some power from the heatpump internal PSU (which is around 20-24V).

The hardware is based on on an ESP8266 module, the ESP-12F, combining it with a digital potentiometer for the setpoint control and an NTC resistor for temperature reading.
Ths software is based on Arduino and support libraries WiFiManager and PubSubClinet.

PS. The uploaded HW version is untested, although an earlier version has been built and tested with mostly good results. This version improves the power supply (DC/DC instead of all linear), possibility to measure the supply voltage (which actually enables us to understand if the heatpump is working or not since the voltage will drop significantly when it does). The SW needs some work to add support for the heating/idle indication that is not supported by the current HW.
