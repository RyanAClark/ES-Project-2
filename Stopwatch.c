ES-Project-2
============

//Program in C for a stopwatch with an 8051 microcontroller

//Port number and pin number you have used for input/output 

sbit h0  = P1^0;    //h0-3 are used for the hours
sbit h1  = P1^1;
sbit h2  = P1^2;
sbit h3  = P1^3;

sbit sw0 = P1^4;  //Switch 0
sbit sw1 = P1^5;  //Switch 1
sbit sw2 = P1^6;  //Switch 2
sbit apm = P1^7;  //am/pm

sbit m0  = P2^0;  //Minutes are P2^0-5
sbit m1  = P2^1;
sbit m2  = P2^2;
sbit m3  = P2^3;
sbit m4  = P2^4;
sbit m5  = P2^5;

sbit cm0 = P2^6;  //Current Mode 
sbit cm1 = P2^7;

sbit s0  = P3^0;  //Seconds
sbit s1  = P3^1;
sbit s2  = P3^2;
sbit s3  = P3^3;
sbit s4  = P3^4;
sbit s5  = P3^5;

sbit blink = P3^6;  //Alarm Blink







//Project #2 Ryan Clark

#include "Main.H"
#include "Simple_EOS.H"
#include "Stopwatch.H"
#include "Time.H"
#include "Elapsed_time.H"
#include "Ports.H"

void main(void)
{
	//P1=0xFF;
	//P2=0xFF;
	//P3=0xFF;

    // Inits watch set to clock mode by default
    watch_Init(clock_state);  
		time_Init();
		//prepare for elapsed time measurement
	Elapsed_Time_Init();



   // Set up simple EOS (10 ms ticks)
   sEOS_Init_Timer2(10);
	 	
   
   while(1) // Super Loop
   {
      // Enter idle mode to save power
      sEOS_Go_To_Sleep();  
   }
}

// Project #2, Ryan Clark

#include "Main.H"
#include "Elapsed_time.H"
#include "Time.H"
#include "Stopwatch.H"
#include "Ports.H"

// Easy to change logic here
#define ON  0
#define OFF 1

static watch_State watch_state_G;
 
void watch_Init(const watch_State Start_state)
{
  watch_state_G = Start_state;  // Decide on initial state
}

void watch_Update(void)
   {
   switch (watch_state_G)
      {
      case clock_state:     // clock mode by default
         {
         
					displaytime();  //displays clock time

					cm0=0; //sets mode
					cm1=0;
					 
					 
				 
         if (sw0 == ON)
            {
            watch_state_G = stop_state;
            sw0 = OFF;     // reset to be pressed again
						sw1 = OFF; 
						sw2 = OFF; 
            }

         break;
         }

      case stop_state:   //stop watch set mode
         {
				cm0=1;//clockmode
				cm1=0;
					 
				sw1=OFF;
			Elapsed_Time_Init(); //resets time to 00:00:00
					 
				while(sw0==OFF){	 		
				if(sw1==ON){
				while(sw1==ON){
								Elapsed_Time_Update();   	 
					edisplaytime();   //displays updated stop watch time hit sw1 to turn off
				}
				}
				if(sw2==ON){
					
					sw2=OFF;
					Elapsed_Time_Init();   //sets time back to 00:00:00
				}
			}
       
         if (sw0 == ON)   // switches to new state if sw0 is pressed
            {
							sw1=OFF;
							sw2=OFF;
							watch_state_G = alarm_state;//changes alarm state
							sw0 = OFF;     // reset to be pressed again
            }

         break;
         }

      case alarm_state:  // alarm set mode
         {
        	cm0=0;
			   	cm1=1;     //updates current mode
		  //time_Update();
			sw1=OFF;
					 
					 
			while(sw0==OFF){	
				if(sw1==ON){     //Press sw1 to enter alarm
						alarm();
				}
			}
		
		         if (sw0 == ON)
		            {
				sw1=OFF;
				sw2=OFF;
		            watch_state_G =time_state;   // changes to new state
		            sw0 = OFF;     // reset to be pressed again
		            }

         break;
         }

      case time_state: // time set mode
         {
			
			cm0=1;//sets mode
			cm1=1;
			sw1=OFF;
			 
				while(sw0==OFF){	
				if(sw1==ON){     //Press sw1 to enter alarm
						changetime();
				}
			}
							
         if (sw0 == ON)
            {
	sw1=OFF
	sw2=OFF;
            watch_state_G = clock_state;
            sw0 = OFF;     // reset to be pressed again
            }

         break;
         }
      }
   }
/*------------------------------------------------------------------*-
// Project #2, Ryan Clark
-*------------------------------------------------------------------*/

#include "Main.H"
#include "Simple_EOS.H"
#include "Elapsed_time.H"
#include "Stopwatch.H"
#include "Time.H"

void sEOS_ISR() interrupt INTERRUPT_Timer_2_Overflow
   {
   TF2 = 0;  // Must manually reset the T2 flag   

		Elapsed_Time_Update();   	 
		// Call update function
		time_Update();
	
    watch_Update();   
   }
	 
void sEOS_Init_Timer2(const tByte TICK_MS)
   {
   tLong Inc;
   tWord Reload_16;
   tByte Reload_08H, Reload_08L;

   // Timer 2 is configured as a 16-bit timer,
   // which is automatically reloaded when it overflows
   T2CON   = 0x04;   // Load Timer 2 control register

   // Number of timer increments required (max 65536)
   Inc = ((tLong)TICK_MS * (OSC_FREQ/1000)) / (tLong)OSC_PER_INST;   

   // 16-bit reload value
   Reload_16 = (tWord) (65536UL - Inc);

   // 8-bit reload values (High & Low)
   Reload_08H = (tByte)(Reload_16 / 256);
   Reload_08L = (tByte)(Reload_16 % 256);

   // Used for manually checking timing (in simulator)
   //P2 = Reload_08H;
   //P3 = Reload_08L;

   TH2     = Reload_08H;   // Load Timer 2 high byte
   RCAP2H  = Reload_08H;   // Load Timer 2 reload capt. reg. high byte
   TL2     = Reload_08L;   // Load Timer 2 low byte
   RCAP2L  = Reload_08L;   // Load Timer 2 reload capt. reg. low byte

   // Timer 2 interrupt is enabled, and ISR will be called 
   // whenever the timer overflows.
   ET2     = 1;

   // Start Timer 2 running
   TR2   = 1;     

   EA = 1;            // Globally enable interrupts
   }

	 
	
	 
/*-----------------------------------------------------------------*-
  sEOS_Go_To_Sleep()
-*------------------------------------------------------------------*/
void sEOS_Go_To_Sleep(void)
   {
   PCON |= 0x01;    // Enter idle mode (generic 8051 version)
   }
// Project #2, Ryan Clark


#include "Main.H"
#include "Elapsed_time.H"
#include "Ports.H"
#include <stdio.h>


tByte Houe;//elapsed time variables
tByte Mine;
tByte Sece;

void Elapsed_Time_Init(void)
   {
   Houe = 0;
   Mine = 0;
   Sece = 0;
		 	s0 = 0;
		s1 = 0;
		s2 = 0;
		s3 = 0;
		s4 = 0;
		s5 = 0;
		//minutes
		m0 = 0;
		m1 = 0;
		m2 = 0;
		m3 = 0;
		m4 = 0;
		m5 = 0;
		//hours
		h0 = 0;
		h1 = 0;
		h2 = 0;
		h3 = 0;
   }

//display
	 
void edisplaytime()
{  // update clock display

		s0 = (Sece & (1 << 0))!=0;
		s1 = (Sece & (1 << 1))!=0;
		s2 = (Sece & (1 << 2))!=0;
		s3 = (Sece & (1 << 3))!=0;
		s4 = (Sece & (1 << 4))!=0;
		s5 = (Sece & (1 << 5))!=0;
		//minutes
		m0 = (Mine & (1 << 0))!=0;
		m1 = (Mine & (1 << 1))!=0;
		m2 = (Mine & (1 << 2))!=0;
		m3 = (Mine & (1 << 3))!=0;
		m4 = (Mine & (1 << 4))!=0;
		m5 = (Mine & (1 << 5))!=0;
		//hours
		h0 = (Houe & (1 << 0))!=0;
		h1 = (Houe & (1 << 1))!=0;
		h2 = (Houe & (1 << 2))!=0;
		h3 = (Houe & (1 << 3))!=0;

		if(Houe <12 || Houe == 24)
		{
			apm = 0;
		}
  		else{
			apm = 1;
		}
}

void Elapsed_Time_Update(void)     
 {
 

   if (++Sece == 60)  
      { 
      Sece = 0;
      
      if (++Mine == 60)  
         {
         Mine = 0;
           
         if (++Houe == 24)  
            { 
            Houe = 0;
            }
         }
      }
  edisplaytime(); 
   }
//endoffile
// Project #2, Ryan Clark


#include "Main.H"
#include "Time.H"
#include "Ports.H"
#include <stdio.h>

// ------ Public variable definitions ------------------------------

 tByte Hou_G;
 tByte Min_G;
 tByte Sec_G;
	tByte tapm;
 tByte tHou_G;
 tByte tMin_G;
 tByte tSec_G;

    int  temp = 0;       /* Temporary variable for calculations */
		int temp1=0;
    int temp2=0;

#define ON  0
#define OFF 1

void time_Init(void)
   {
   Hou_G = 10;
   Min_G = 30;
   Sec_G = 0;

		s0 = 0;
		s1 = 0;
		s2 = 0;
		s3 = 0;
		s4 = 0;
		s5 = 0;
		//minutes
		m0 = 0;
		m1 = 1;
		m2 = 1;
		m3 = 1;
		m4 = 1;
		m5 = 0;
		//hours
		h0 = 0;
		h1 = 1;
		h2 = 0;
		h3 = 1;
  }

//display
void displaytime()
{
  // update clock display
		s0 = (Sec_G & (1 << 0))!=0;
		s1 = (Sec_G & (1 << 1))!=0;
		s2 = (Sec_G & (1 << 2))!=0;
		s3 = (Sec_G & (1 << 3))!=0;
		s4 = (Sec_G & (1 << 4))!=0;
		s5 = (Sec_G & (1 << 5))!=0;
		//minutes
		m0 = (Min_G & (1 << 0))!=0;
		m1 = (Min_G & (1 << 1))!=0;
		m2 = (Min_G & (1 << 2))!=0;
		m3 = (Min_G & (1 << 3))!=0;
		m4 = (Min_G & (1 << 4))!=0;
		m5 = (Min_G & (1 << 5))!=0;
		//hours
		h0 = (Hou_G & (1 << 0))!=0;
		h1 = (Hou_G & (1 << 1))!=0;
		h2 = (Hou_G & (1 << 2))!=0;
		h3 = (Hou_G & (1 << 3))!=0;


		if(Hou_G <12 || Hou_G == 24)// set to am or pm
		{
			apm = 0;
		}
  		else{
			apm = 1;
		}
}
 
void changetime(void)     //Change clock time function
{
		while(sw0==OFF){
			while(sw1==1){    //0 for am 1 for pm		
					if (sw0 == ON)    
            {
							sw1=OFF;
							sw2=OFF;
							sw0 = OFF;     // reset to be pressed again
							break;
            }		
						apm=sw2;
						displaytime();		
				}
				sw1=1;    //1 = OFF      
				while(sw1==1){  // increments hours press switch one to exit loop
						if(sw2==0){
							Hou_G++;
							sw2=1;
							displaytime();
							if (sw0 == ON)
            {
							sw1=OFF;
							sw2=OFF;
							sw0 = OFF;     // reset to be pressed again
							break;
            }
						}
				}
				sw1=1;
				while(sw1==1){   // increments minutes press sw1 to exit 
						if(sw2==0){
							Min_G++;
							sw2=1;
						displaytime();
						
						}
						if (sw0 == ON)
            {
							sw1=OFF;
							sw2=OFF;
							sw0 = OFF;     // reset to be pressed again
							break;
            }
				}
				sw1=1;
					while(sw1==1){   // increments seconds press sw1 to exit
						if(sw2==0){
							Sec_G++;
							sw2=1;
							displaytime();
						}
						if (sw0 == ON)
            {
							sw1=OFF;
							sw2=OFF;
							sw0 = OFF;     // reset to be pressed again
							break;
            }
				}
				sw1=1;
			}
			if (sw0 == ON)
            {
							sw1=OFF;
							sw2=OFF;
							sw0 = OFF;     // reset to be pressed again
            }		
}

void time_Update(void)     //updates time
   {
 

   if (++Sec_G == 60)  
      { 
      Sec_G = 0;
      
      if (++Min_G == 60)  
         {
         Min_G = 0;
           
         if (++Hou_G == 24)  
            { 
            Hou_G = 0;
            }
         }
      }
			displaytime();
}
	
void alarm(void)     //alarm
{
 
	while(sw0==ON){
		while(sw1==OFF){    //0 for am 1 for pm	
						
							tapm=sw2;					
				}
				sw1=OFF;
				            
				while(sw1==OFF){  // increments hours press switch one to exit loop
						if(sw2==ON){
							tHou_G++;
							sw2=OFF;
						}
						if (sw0 == ON)
            {
							sw1=OFF;
							sw2=OFF;
							sw0 = OFF;     // reset to be pressed again
							break;
            }
				}
				sw1=OFF;
				while(sw1==OFF){   // increments minutes press sw1 to exit 
						if(sw2==ON){
							tMin_G++;
							sw2=OFF;
						}
						if (sw0 == ON)
            {
							sw1=OFF;
							sw2=OFF;
							sw0 = OFF;     // reset to be pressed again
							break;
            }
				}
				sw1=OFF;
					while(sw1==OFF){   // increments seconds press sw1 to exit
						if(sw2==ON){
							tSec_G++;
							sw2=OFF;
						}
						if (sw0 == ON)
            {
							sw1=OFF;
							sw2=OFF;
							sw0 = OFF;     // reset to be pressed again
							break;
            }
				}
				sw1=OFF;	
				if(tapm == apm){
					if(tHou_G==Hou_G){
						if(tMin_G==Min_G){
							if(tSec_G==Sec_G){
								if(blink==1) ///Blinks when alarm goes off
								{
									blink=0;
								}
								else
								{
									blink=1;
								}
							}
						}
					}
				}
			}
			if (sw0 == ON)
            {
							sw1=OFF;
							sw2=OFF;
							sw0 = OFF;     // reset to be pressed again
						
            }		
   }
// End of file
