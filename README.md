# Car-Collision-Detection-System-IoT-
This IoT project is a car collision avoidance and detection system. It uses Node MCU as the microcontroller which has an inbuilt wifi connection.
We have made use of ultrasonic distance sensors and an accelerometer for collision avoidance. We have made use of impact sensor for collision detection. 
The location of the collision is found using GPS sensor. In case of collision,it will automatically send the car's location to Firebase cloud via MQTT protocol and from there 
it will be sent to emergency services or close relatives via sms.
