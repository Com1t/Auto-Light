# Auto-Light
Auto light is consisted by two parts
1. inertial remote controller
2. control board to control the LED strips

# Inertial remote controller
Using a GR-KURUMI(A more enegy efficient arduino) to manage with
Inertial sensor(MPU6050)
Radio module
While the acceleration is grater than 0.3G and maintain 0.1G after 50ms is sends signal to LED strips control board

# RGB_LED
Provides a button to switch the LED strips into different modes(No restriction),
and a receiver in pair with the remote controller
(while receiving signal from remote controller the LED strips can only be lit on at right time, and right ambient light).
# RGB_LED_without_button
Removed a button to switch the LED strips into different modes(to reduce the control board size, also, it's redundant),
and a receiver in pair with the remote controller
(while receiving signal from remote controller the LED strips can only be lit on at right time, and right ambient light).
