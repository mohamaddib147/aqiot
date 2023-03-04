IK1332 Project: IoT for air quality monitoring
1 Background
1.1 Sensor Applications
A sensor network application consists of several components interacting with each other:
• Sensors that measure physical quantities and send measurement data onto a network.
• A sensor network that forms a network infrastructure that the sensors use to transfer sensor
data to the Internet.
• Servers in the cloud, where sensor data is stored. Data is stored in a database, where
applications can access it through an API.
• A sensor application protocol for end-to-end communication between sensors and
database servers.
• Applications that access and process the data on the database servers.
1.2 Project Objectives
Air-Quality (AQ) is one of our bigger health challenges. Transportation, manufacturing, climate change,
urbanization and population growth are major contributors. This is a huge area to address and to
understand.
Particles (PM measurements) are monitored in many of big cities and environmental thresholds are
exceeded in many places. Today alarming reports are now seen for smaller particles than those
measured in today’s air quality monitoring systems. In this project we intend to build system for
measurement of smaller particles design an alert system these small particles.
2 Project Components
This project consists of designing and implementing a complete sensor network with the five parts
above. Next follows more detailed descriptions of the different parts.
2.1 Sensors
Through recent developments in technology, small and low-cost air quality sensors for particle
measurements have become available, and thereby possible to use in sensor network applications.
The project is performed with RSS2 sensor nodes, which are equipped with AtMega256RFR2
microcontrollers and communicate over IEEE 802.15.4. Air quality sensor modules from Plantower, the
PMS5003 series, are connected to the sensor node over a serial connect (I2C or RS-232).
2.2 Sensor network
In KTH Electrum, there is an infrastructure with gateways through which sensors can connect to the
Internet using IEEE 802.15.4 – a technology for low-rate wireless personal area networks (LR-WPAN).
Internet communication is over IPv6, using 6LoWPAN and RPL (IPv6 Routing Protocol for Low-Power
and Lossy Networks).
2.3 Servers
The choice of server technology is project-specific, left as a task for the student group to resolve.
2.4 Sensor application protocol
The communication between sensors and database servers takes place over an application protocol that
supports communication according to the publish-subscribe message pattern. A popular protocol for
this is MQTT, and recently CoAP pubsub has emerged as an interesting alternative. There is currently a
more developed and mature ecosystem for MQTT, so for this project we will use MQTT. More
specifically we will use the variant MQTT-SN, which uses UDP instead of TCP as the transport protocol.
2.5 Applications
Using low-cost sensors for air quality measurements is becoming increasingly popular. The usage of
such units in sensor networks is quite new, and there are many potential applications of different
character. The Plantower sensor module can be used for high resolution outdoor measurements of
particles of different sizes (PM-1, PM-2.5, and PM-10). It can monitor on a time scale of down to a few
measurements per minute, which means that it is possible to quickly detect local changes in particle
levels in the air. The Plantower sensors have an option to measure small particles, i.e., 0.3 and 0.5
micrometer particles, which is a mechanism we will use in this project.
Within the GreenIoT project, support was developed for basic air quality reporting over MQTT. Here,
we will take those results further and develop an intelligent sensor for air quality measurements. An
intelligent sensor can do a certain amount of data processing on the sensor. In this project, the sensor
node should measure and implement alarms for small particles, i.e., particles less than 1 um. The sensor
node should report concentration average and deviations/alarms from average. The project also inlucde
to discuss limitations and possible improvements.
3 Project Components
There are several generic tasks that should be addressed before starting the actual development work:
• Study the necessary documentation for PMS5003 air quality sensor modules.
• Set up a toolchain for code development and a testbed for the 6LoWPAN IoT infrastructure in
KTH Electrum.
• RIOT-OS study and installation
Cross compilation platform, also known as toolchain. Install build/flash/test.
• Study of MQTT and MQTT-SN
• Study documentation of project sensor for air quality
• Understand 6LowPAN GW setup.
Sensor network setup and configuration.
• Verification and test of example MQTT-SN code
• Verification and test of sensor example code for the air quality sensor module
• Setup git project repository at KTH to host results and track work-in-progress

Here are the details of the MQTT infrastructure that you need in your project.

Ethernet gateway for the sensor network (running IPv6/RPL over 6LoWPAN)
Wireless interface IPv6: 2001:6b0:32:23::1
Ethernet interface IPv6: 2001:6b0:32:13:fcc2:3dff:fe01:856b
It is in the lab room to the right of the 4th floor, B entrance.
A sensor node needs to run RPL to connect to the Ethernet gateway. It can ping the wireless interface but not the Ethernet interface. You must sit and work at the tables by the B entrance to get wireless coverage.

MQTT-SN gateway (running on UDP port 10000)
NAME (for IPv4 only): ik1332-sn-gw.ssvl.kth.se
IPv4: 192.16.125.236
IPv6: 2001:6b0:32:13::236

MQTT broker (running on the default TCP port 1883)
NAME (for IPv4 only): ik1332-broker.ssvl.kth.se
IPv4: 192.16.125.237
IPv6: 2001:6b0:32:13::237

MQTT broker publisher/subscriber must authenticate with the credentials below to use the service.
USERNAME: student
PASSWORD: IK1332-group3

You can use any MQTT client to publish/subscribe to our broker.
For instance, on Ubuntu, you can install mosquitto-clients with the command below:
sudo apt install mosquitto-clients

I also recommend you use a topic name specific to your group.
For example, you should always use a topic beginning with "group3" such as "group3/sensor1/temperature" or "group3/AQI"

Example subscribe command from an Internet host:
mosquitto_sub -h 192.16.125.237 -u student -P IK1332-group3 -t "group3"

Example publish command from an Internet host:
mosquitto_sub -h 192.16.125.237 -u student -P IK1332-group3 -t "group3" -m "hello"

On your sensor node, you will need
- RPL to communicate with the Ethernet gateway
- MQTT-SN client to publish your sensor data
As a starting point, you can copy the RIOT's examples/emcute-mqttsn and modify it to fit your need.
Enclosed is the Makefile I used to enable the RPL networking with the emcute-mqttsn example code.
You need to verify that the sensor can connect to the Ethernet gateway and then test with the shell commands: con to connect to the MQTT-SN gateway and pub to publish the data.

As I explained, you will probably need to write your application code to verify that the sensor node can ping the IPv6 address of the MQTT-SN gateway before you make the MQTT-SN connect and publish the data.
