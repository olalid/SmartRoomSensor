# SmartRoomSensor
Smarter replacement for the RG20 room sensor that is an optional extra for some NIBE heat pumps.

The NIBE RG20 room sensor is a very simple thermostat like device that is connected to a NIBE heatpump and lets you control the temperature in your home more preciesely.
This replacement version with MQTT supports lets you control the setpoint and read back the current temperature from your home automation system. It also shows the status (heating or idle) of the heatpump.

The hardware is based on on an ESP8266 module, the ESP-12F, combining it with a digital potentiometer for the setpoint control and an NTC resistor for temperature reading.

PS. The uploaded HW version is untested, although an earlier version has been built and tested with mostly good results.
