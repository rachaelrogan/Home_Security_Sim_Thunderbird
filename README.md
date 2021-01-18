# Home Security Simulation

# Project Description
This project simulates a home security system. Upon startup, the program will print to the Serial Communication Interface the status of all the components it tracks: the temperature, the front door, the garage door, and the window. All these components start locked and closed. A potentiometer is used to get the temperature of the home. The SCI will update the temperature approximately every 20 seconds with the help of an interrupt service routine. The SCI will also be updated throughout the program as different circumstances occur. A hex keypad is used for the keyless entry of the front door, and a function separate from the main is used to get the input from the hex keypad. Once the correct four-digit number is entered through the keypad, the servo, which simulates the front door latch, will flip, and a welcome message will display to an LCD display. The SCI will show the user that the front door is unlocked. After ten seconds, the servo flips back to its original position, signifying the front door locking again, the message on the LCD will disappear, and the SCI will once again be updated to the front door (and all other components) being locked. The program then checks to see if the garage door opener switch has been pressed. If it has, the SCI is appropriately updated, an external LED lights up for the garage lights, and a stepper motor turns to simulate the garage door opening. The motor will turn for ten seconds. Those tens seconds of turning are executed using an interrupt service routine. After ten seconds, it remains still for ten seconds. Then the stepper motor will turn the opposite direction to simulate the garage door closing. It turns for another ten seconds (also executed using an interrupt service routine). Once it is finished turning, the LED turns off, and the SCI is updated signifying that all components are locked. The program will also to check to see if the window has been opened (which is simply the window switch being pressed). The SCI will be updated telling the user that the window has been opened. Then the buzzer will sound until the window switch is pressed again, signifying the window being closed. Once the window is closed, the SCI will again be updated. This program runs in a forever loop.

# Wiring
This project is implemented using a Thunderbird MC9S12DG256 and CodeWarrior IDE version 5.9.0. All of the components it uses are from the Freenove RFID Starter Kit for Raspberry Pi. The Thunderbird should be properly connected to power and ground. How each component was connected for this project is described below:
LCD Display: On the LCD, the spot marked VCC goes to the same power as the Thunderbird, and the spot marked GND should go to ground. The wire connected to SDA on the LCD display should be connected to PJ6 on the Thunderbird. The wire connected to SCL on the LCD display should be connected to PJ7 on the Thunderbird.
Potentiometer: There are three connections on the potentiometer. The middle connection connects to AN7 on the Thunderbird. For the other two connections, one needs to go to ground, and the other needs to go to the same power as the Thunderbird.
Hex Keypad: All eight connections from the hex keypad go to PA0, A1, A2, A3, A4, A5, A6, and PA7. Note: each hex keypad differs, so there must be proper code in the program’s asm file for the correct numbers to be enter through the keypad.
Garage Switch: If the Thunderbird is on the far left of the breadboard, place the switch directly to the right of the Thunderbird. The connection on the top left goes to ground. The connection on the top right connects to B4 on the Thunderbird. A 10k resistor goes from the top right connection of the switch to the same power line as the Thunderbird.
Window Switch: This switch is directly to the right of the garage switch. The top left connection of the switch goes to ground. The top right connection goes to B5 on the Thunderbird. A 10k resistor goes from the top right connection of the switch to the same power line as the Thunderbird.
External LED: The shorter connection of the LED (also the connection on the side of the LED that is slightly flattened when look at with the connections pointing toward you), is connected to ground. The longer connection has a 220ohm resistor, and then is connected to B6 on the Thunderbird.
Buzzer: The connection of the buzzer that is on the same side as the part of the sticker on top of it that has a plus (+) sign on it is connected to PB7. The other connection goes to ground.
Stepper Motor: The wires that come from the stepper motor connect to the stepping motor driver (the blue wire from the motor should be on the same side as the light labeled A on the driver). On the driver there are two pins marked – and +. The wire connected to – goes to ground. The wire connected to + goes to the external power (not the same power line that the Thunderbird is connected to). The wire connected to IN1 on the driver goes to PB0 on the Thunderbird, the wire connected to IN2 on the driver goes to B1 on the Thunderbird, IN3 goes to B2, IN4 goes to B3.
Servo Motor: The wire connected to the red wire on the servo goes to an external power supply (not the same power line as the Thunderbird). The wire that is connected to the brown part of the servo motor goes to ground. The wire that is connected to the orange part of the servo motor is connected to PP7 on the Thunderbird.
Note: An external power supply is needed to execute this project.

# References
Richard E. Haskell and Darrin M. Hanna’s book Programming the DRAGON-12-Plus-USB in C and Assembly Language Using Code Warrior was a major resource in the development of this project:
- Their code to read the potentiometer and convert it to a temperature (Example 20 on page 88) was used. 
- Their code to set up writing to the SCI (Example 23 on page 103) was used.
- Their code was to write strings to the LCD (Example 11, page 48) was modeled in this project.
- The main.asm file was included in this project.
The following assembly routines written by these authors were used in this project:
-	Clear_RTI_flag() 
-	PLL_init()
-	Lcd_init()
-	RTI_init()
-	Servo76_init()
-	Keypad_enable()
-	Ad0_enable()
-	SCI0_init(int)
-	Set_servo(int)
-	Outchar0(int)
-	Keyscan()
-	Set_lcd)addr(int)
-	Type_lcd(char)
-	Clear_lcd()
-	Ms_delay(int)
-	Data8(char)

Professor Fazekas helped in the logic of the interrupt service routine and provided, along with classmates, help in the wiring of the project.
The internet was also used to confirm the logic flow set up.
