// Final Project - Home Security Simulation by Rachael Rogan
#include <hidef.h>      /* common defines and macros */
#include <mc9s12dg256.h>     /* derivative information */
#pragma LINK_INFO DERIVATIVE "mc9s12dg256b"

#include "main_asm.h" /* interface to the assembly module */

#define garage 16    //Garage door switch
#define ExLED 64     //Garage door light
#define window 32    //Intruder detector
#define buzzer 128   //Alarm for intruder
int Gcounter = 1000; //Counter for the garage to open
int Gflag = 1;       //Tells the main when the garage should be done turning
int SCounter = 2000; //Everytime this reaches zero, we update the SCI
int SFlag = 0;       //Tells the main when we are finishing counting down           

//Gets four numbers from the hex keypad.
//Returns true if they form the password, and false if they don't.
int hex_entry (void);

//Gets the temperature from the trimpot and displays it to the SCI.
void printTemp(void);

//Prints the status (locked or unlocked) of the front door to the SCI.
//If status = 1, the front door is locked. If status = 0, the door is unlocked.
void printFront(int);

//Prints the status (open or closed) of the window to the SCI.
//If status = 1, the window is closed. If status = 0, the window is open.
void printWindow(int );

//Prints the status (open or closed) of the garage door to the SCI.
//If status = 1, the garage door is closed. If status = 0, the garage is open.
void printGarage(int );

//Clears the lines on the SCI that describe the states of the front door, garage door, and window
//and carriage returns so the SCI will be ready for updating on the next iteration of the main loop.
void prepForNext(void);

//This ISR decrements one counter for the garage door turning and another for the amount of time
//between when the temperature is read again and updated. Each counter flips a flag to zero to indicate
//the time has run out.
void interrupt 7 handler(){
  Gcounter--;
  if(Gcounter == 0){
    Gcounter = 1000;
    Gflag = 0;  
  }
  SCounter--;
  if(SCounter == 0){
     SFlag = 1;
  }
  clear_RTI_flag();
}

void main(void) {

  char* welcome = "Welcome Home!"; //Will be used for the keyless entry
  char* wrong = "Incorrect PIN";   //Will be used for the keyless entry
  int lock = 3300;    //servo position representing the door being locked
  int unlock = 5700;  //servo position representing the door being unlocked
  int sciReset = 0;   //Used to count how many characters we need to delete to update the SCI
  PLL_init();
  lcd_init();         //enable lcd
  RTI_init();         //initialize interrupt service routine
  servo76_init();     //set up the servo
  keypad_enable();    //enable keyboard
  ad0_enable();         //enable a/d converter 0
  SCI0_init(9600);      //initialize SCI0 at 9600 baud
  PORTB = 0;          //initialize portb to zero
  DDRB |= ExLED;      //programs bit 6 for ouput for external led
  DDRB |= 0x0F;        //Programs lower nibble on portb for output
  DDRB |= buzzer;     //Programs bit 7 for output (the buzzer)
  
  set_servo76(lock);  //The door starts locked
  printTemp();        //Print the temperature
  outchar0(0xA);
  printFront(1);     //Print status of the front door
  outchar0(0xA);
  printGarage(1);    //Print status of garage 
  outchar0(0xA);
  printWindow(1);     //Print status of the window
  
  while(1){           //Keeps program running forever 
    if(!(keyscan() == 16)){  //if the hex keypad is getting input
       if(hex_entry()){       //we see if they entered the right password
           set_lcd_addr(0x01);//set the lcd cursor so the words will be centered
           type_lcd(welcome); //Tell them they entered the right password
           set_servo76(unlock); //Unlock the door
           /* In the next three lines of code we will backspace all 
           the lines below where the state of the front door is 
           printed so we can update the front door status.*/
           sciReset = 33;
           for(sciReset; sciReset > 0; sciReset--){
              outchar0(0x08);                
           }
           outchar0(0xD);      //we will write over the previous status of the front door
           printFront(0);      //Tell the SCI the door is unlocked.
           outchar0(0xA);      //Go to the next line in the SCI
           printGarage(1);     //Print the status of the garage door
           outchar0(0xA);      //Go to the next line in the SCI
           printWindow(1);     //Print the status of the window
           ms_delay(10000);     //wait for the person to enter the hourse
           set_servo76(lock);   //Lock the door
           /* In the next three lines of code we will backspace all 
           the lines below where the state of the front door is 
           printed so we can update the front door status.*/
           sciReset = 33;
           for(sciReset; sciReset > 0; sciReset--){ 
              outchar0(0x08);                       
           }
           outchar0(0xD);      //we will write over the previous status of the front door
           printFront(1);      //Tell the SCI the door is locked.
           outchar0(0xA);      //Go to the next line in the SCI
           printGarage(1);     //Print the status of the garage door
           outchar0(0xA);      //Go to the next line in the SCI
           printWindow(1);     //Print the status of the window
           clear_lcd();        //Clear the lcd of the message
           
       } else{
           set_lcd_addr(0x01);//centers cursor on lcd
           type_lcd(wrong);   //Tell user they had the wrong pin
           ms_delay(1000);    //Leave the message on the lcd for a second
           clear_lcd();       
       }  
       
     } //end of hex part
     ms_delay(10);         //Accounts for the speed in which a person can press a button.
     if(!(PORTB&garage)){  //if the button is pressed
       PORTB |= ExLED;    //turn on garage light
       Gflag = 1;        //tells the ISR that we will keep track of the garage door flag
       Gcounter = 1000;  //Ensures that the motor will turn for 10 seconds.
       sciReset = 15;   //We have to backspace 15 spots to be able to udpate the garage status
       for(sciReset; sciReset > 0; sciReset--){ 
         outchar0(0x08);                       
       }
       outchar0(0xD);    //We will write over the previous status of the garage
       printGarage(0);   //Tell the SCI the garage is open
       outchar0(0xA);      //Go to the next line in the SCI
       printWindow(1);     //Print the status of the window
       PORTB |= 3;       //initializes the turning of the garage door
       ms_delay(5); 
       while(Gflag){     //We turn the motor clockwise until the ISR tells us to stop
          PORTB ^= 5; 
          ms_delay(5);
          PORTB ^= 10;
          ms_delay(5);
          PORTB ^= 5;
          ms_delay(5);
          PORTB ^= 10;
          ms_delay(5); 
       }//end of garage opening
       ms_delay(10000);  //Let the person/car enter the garage
       Gflag = 1;        //We will again keep track of what the garage door flag
       Gcounter = 1000;  //This ensures that the motor turns the opposite direction for 10 seconds.
       while(Gflag){     //Turns the motor the counter clockwise.
         PORTB ^= 10;
          ms_delay(5);
          PORTB ^= 5;
          ms_delay(5);
          PORTB ^= 10;
          ms_delay(5);
          PORTB ^= 5;
          ms_delay(5);    
       }//end of garage closing
        sciReset = 15;   //We have to backspace 15 spots to be able to udpate the garage status
       for(sciReset; sciReset > 0; sciReset--){ 
         outchar0(0x08);                       
       }
       outchar0(0xD);    //We will write over the previous status of the garage
       printGarage(1);   //Tell the SCI the garage is closed
       outchar0(0xA);    //Go to the next line in the SCI
       printWindow(1);   //Print the status of the window
       PORTB ^= 3;      //Turn off the motor
       PORTB ^= ExLED;  //Turn off the garage light because the door has closed.
       outchar0(0xD);   //Clear the SCI to write the following message.
     }//end of garage part 
     ms_delay(10);
  if(!(PORTB&window)){ //if the window is open
      while(!(PORTB&window)){} //wait till the window isn't being moved
      outchar0(0xD);          //write over the previous message about the window
      printWindow(0);         //Tell the SCI the window is open
      while(PORTB&window){ //while the window is still open, beep the alarm
        PORTB ^= buzzer;
        ms_delay(50);
        PORTB ^= buzzer;
        ms_delay(50);
      } //end of buzzing
      ms_delay(50);           //gives the user time to lift thier finger from the button
      outchar0(0xD);          //write over the previous message about the window
      printWindow(1);         //Tell the SCI the window is closed
    //The user has pressed the button
   }//end of intruder part
   ms_delay(10);
   if(SFlag == 1){        //If it has been 10 seconds since we last updated everything in the SCI
      prepForNext();      //We clear what was there before
      printTemp();        //Print the temperature
      outchar0(0xA);       //Go to the next line on the SCI.
      printFront(1);       //Print the status of the front door
      outchar0(0xA);       //Go to the next line on the SCI.
      printGarage(1);       //Print the status of the front door
      outchar0(0xA);       //Go to the next line on the garage.
      printWindow(1);      //Print the status of the window
      SCounter = 2000;
      SFlag = 0; 
   }
   ms_delay(10); 
 }//end of while 
}//end of main 

//This subroutine gets four numbers from the hex keypad, and returns a 1 if they  
//match the password (1969).
int hex_entry (void){ 
  //These four variables hold the numbers that the user enters to open the front door 
  char first;        
  char second;       
  char third;        
  char fourth;
  set_lcd_addr(0x06);  //sets cursor of the lcd to the first row and first column
  
  first = getkey();   //gets the first number from the user       
  wait_keyup();       //waits until the key is not being pressed 
  data8('*');         //displays '*' to tell user their input has been received                                       
  
  second = getkey();   //gets the second number from the user
  wait_keyup();       //waits until the key is not being pressed
  data8('*');          //displays '*' to tell user their input has been received 
  
  third = getkey();     //gets the third number from the user
  wait_keyup();       //waits until the key is not being pressed
  data8('*');            //displays '*' to tell user their input has been received 
  
  fourth = getkey();   //gets fourth number from the user
  wait_keyup();       //waits until the key is not being pressed
  data8('*');           //displays '*' to tell user their input has been received
  ms_delay(50);       //waits so the * can be seen on the lcd
  
  //If the user has entered the correct password, returns true
  if(first == 1 && second == 9 && third == 6 && fourth == 9){
    return 1;
  } else{
    return 0;  
  }
} //end of hex_entry

//Get the temperature from the trimpot, and print it to the SCI.
//Note: The reading in and calculating of the temperature is from the book.
void printTemp(void){
  int val, Tc, Tf;
  int ones;
  int tens;             
  val = ad0conv(7);   //read pot on channel 7
  val = val >> 2;     //shift 2 bit right (divide by 4)
  Tc = val -20;       //Degrees C
  Tf = Tc*9/5 + 32;   //Degrees F
  outchar0('T');      //Tell the SCI what we're talking about
  outchar0('e');
  outchar0('m');
  outchar0('p');
  outchar0(':');
  outchar0(' ');
  //If the temperature has three digits, we get each digit and print it.
  if(Tf > 99){
    int hundreds;
    ones = Tf%10;                   //gets the digit in the ones place
    tens = (Tf/10)%10;              //gets the digit in the tens place
    hundreds = Tf/100;              //gets the digit in the hundreds place
    outchar0(hex2asc(hundreds));    //prints the number
    outchar0(hex2asc(tens));
    outchar0(hex2asc(ones));
  }
  //If the temperature is only one digit, we just print that.
  else if(Tf < 10){
    outchar0(hex2asc(Tf));  
  }
  //If the temperature is two digits, we get each and print them.
  else {
    ones = Tf%10;
    tens = Tf/10;
    outchar0(hex2asc(tens));
    outchar0(hex2asc(ones));
  }
  outchar0(' ');
  outchar0('F');
} //end of printTemp

//Prints the status of the front door to the SCI.
//If status = 1, the front door is locked. If status = 0, the door is unlocked.
void printFront(int status){
   outchar0('F');
   outchar0('r');
   outchar0('o');
   outchar0('n');
   outchar0('t');
   outchar0(' ');
   outchar0('D');
   outchar0('o');
   outchar0('o');
   outchar0('r');
   outchar0(':');
   outchar0(' ');
   if(status){      //if the front door is locked, we tell the SCI
     outchar0('L');
     outchar0('o');
     outchar0('c');
     outchar0('k');
     outchar0('e');
     outchar0('d');
   } else{          //tells the SCI the door is unlocked
     outchar0('U');
     outchar0('n');
     outchar0('l');
     outchar0('o');
     outchar0('c');
     outchar0('k');
     outchar0('e');
     outchar0('d');
   }

}//end of printFront


//Prints the status (open or closed) to the SCI.
//If status = 1, the window is closed. If status = 0, the window is open.
void printWindow(int status){
   outchar0('W');       //We are telling the status of the window.
   outchar0('i');
   outchar0('n');
   outchar0('d');
   outchar0('o');
   outchar0('w');
   outchar0(':');
   outchar0(' ');
   if(status){         //If the window is closed, we tell the SCI such.
      outchar0('C');
      outchar0('l');
      outchar0('o');
      outchar0('s');
      outchar0('e');
      outchar0('d'); 
   } else{            //Otherwise the window is open and we tell the SCI such.
      outchar0('O');
      outchar0('P');
      outchar0('E');
      outchar0('N');
   }
}


//Prints the status (open or closed) of the garage door to the SCI.
//If status = 1, the garage door is closed. If status = 0, the garage is open.
void printGarage(int status){
  outchar0('G');
  outchar0('a');
  outchar0('r');
  outchar0('a');
  outchar0('g');
  outchar0('e');
  outchar0(':');
  outchar0(' '); 
  if(status){         //If the garage door is closed, we tell the SCI such.
      outchar0('C');
      outchar0('l');
      outchar0('o');
      outchar0('s');
      outchar0('e');
      outchar0('d'); 
   } else{            //Otherwise the garage door is open and we tell the SCI such.
      outchar0('O');
      outchar0('p');
      outchar0('e');
      outchar0('n');
   }
}//end of printGarage


//Clears the lines on the SCI that describe the states of the front door, garage door, and window
//and carriage returns so the SCI will be ready for updating on the next iteration of the main loop.
void prepForNext(void){
  short i = 0;
  for(i; i< 54; i++){ //Delete the 54 characters on the SCI
    outchar0(0x8);  
  }
  outchar0(0xD);//Puts the cursor to the beginning of the SCI so we can print the temp when we
                //go back to the main loop
} //end of prepForNext





