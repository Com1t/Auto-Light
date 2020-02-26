# Auto-Light
Auto light is consisted by two parts
1. inertial remote controller
2. control board to control the LED strips

# Inertial remote controller
Using a GR-KURUMI(A more enegy efficient arduino) to manage with
Inertial sensor(MPU6050)
Radio module
While the acceleration is grater than 0.3G and maintain 0.1G after 50ms is sends signal to LED strips control board
No acceleration
![image](https://github.com/Com1t/Auto-Light/blob/master/DEMO/No%20acceleration.jpg)
Accelerated
![image](https://github.com/Com1t/Auto-Light/blob/master/DEMO/Accelerated.jpg)
Back
![image](https://github.com/Com1t/Auto-Light/blob/master/DEMO/Back.jpg)

# RGB_LED
Provides a button to switch the LED strips into different modes(No restriction),
and a receiver in pair with the remote controller
(while receiving signal from remote controller the LED strips can only be lit on at right time, and right ambient light).
# RGB_LED_without_button
Removed a button to switch the LED strips into different modes(to reduce the control board size, also, it's redundant),
and a receiver in pair with the remote controller
(while receiving signal from remote controller the LED strips can only be lit on at right time, and right ambient light).
Control board
![image](https://github.com/Com1t/Auto-Light/blob/master/DEMO/Control%20board.jpg)
MAX44009
![image](https://github.com/Com1t/Auto-Light/blob/master/DEMO/MAX44009.jpg)
Whole LED part
![image](https://github.com/Com1t/Auto-Light/blob/master/DEMO/Whole%20LED%20part.jpg)
Light on
![image](https://github.com/Com1t/Auto-Light/blob/master/DEMO/LED%20light%20on.jpg)
