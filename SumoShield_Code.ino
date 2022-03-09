/*
    SumoShield_Code.ino
    Copyright 2011 Solarbotics Ltd.

    Written by Jeremie Boulianne 2011
    Ported from .C code by Bob Cook, based on the original by Grant McKee
    Cleaned up and revised by Dan Gates 2006
    Assistance from: Mark Martens

    Designed for Rev 1.0 SumoShield.
    Intended to be compiled with Arduino 1.0 (or later).

    Solarbotics: http://www.solarbotics.com/
    Arduino    : http://www.arduino.cc/

    This example code is in the public domain.

    Description: Minisumo/Linefollower program Ver 1.0

    If outside edge sensors see black during startup the program will branch
    to sumo after a 5 second delay, indicated by the green LEDS. 
    If outside edge sensors see white during startup the program will
    branch to Linefollower.

    Sumo mode is fairly basic:

        - Wait 5 seconds before moving
        - After 5 seconds go straight forward
        - If a white line is detected on outside Left sensor
            * Reverse both motors briefly
            * Stop left motor
            * Reverse Right motor
            * After set time continue straight forward
        - If a white line is detected on outside Right sensor
            * Reverse both motors briefly
            * Stop Right motor
            * Reverse Left motor
            * After set time continue straight forward
        - If opponent detected on right Side
            * Stop Right motor
        - If opponent detected on Left Side
            * Stop Left motor
        - If both sensors detect opponent
           * Turn on both motors forward

    Linefollower mode:

        - Start immediately
        - If center sensor sees black all is well- go straight forward
        - If center right sensor sees black, make a gentle right turn by
          slowing down right motor
        - If center left sensor sees black, make a gentle left turn by slowing
          down left motor
        - If far right sensor sees black, make a very sharp right turn by
          reversing right motor
        - If far left sensor sees black, make a very sharp left turn by
          reversing left motor
        - If all is white, all is lost - go looking for the line!

 */

/*
 *  Definitions
 *
 */

/* White line sensitivity; higher is more sensitive (1024 max) */
int kWhiteThreshold = 128; //Original value 128
int linesensor_left;
int linesensor_midleft;
int linesensor_center;
int linesensor_midright;
int linesensor_right;
int irsensors;
int PD0;
int PD1;


void setup(){
  
pinMode(7, INPUT); /* Front IR sensor inputs */
pinMode(8, INPUT);

/* the SumoShield has five green leds that we can use to display things in our 
   program; we need to set the LED pins as outputs for them to turn on*/

pinMode(2, OUTPUT);                      /* Pins 2-6 = green leds */
pinMode(3, OUTPUT); 
pinMode(5, OUTPUT);
pinMode(4, OUTPUT); 
pinMode(6, OUTPUT); 
    
/* the sumovore has two motors that also require output pins */

pinMode(9, OUTPUT);                    
pinMode(10, OUTPUT); 
pinMode(12, OUTPUT); 
pinMode(13, OUTPUT); 


}

/*
 *  Main (program entry point)
 *
 */

void loop()
{
    
/* make sure all LEDs are off until we're ready */

    digitalWrite(2, LOW);
    digitalWrite(3, LOW);	              /* green LEDs off */
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
    digitalWrite(6, LOW);


/* make sure motors are off until we're ready. */

	digitalWrite(9, LOW);                   /* right motor disable */
	digitalWrite(10, LOW);	              /* left motor disable */

    /* read the line sensors to decide whether we should behave like a
       sumo wrestler or like a line chaser */

    linesensor_left  = analogRead(A0);  //~34 on white ~700+ on black  threshold 128
    linesensor_right = analogRead(A4);  //~34 on white ~700+ on black

    if ( linesensor_left  < kWhiteThreshold && linesensor_right < kWhiteThreshold  )
    {
        /* both line sensors see white */

        LINEFOLLOWER();
      
    }
    else
    {
        /* one or both line sensors see black */

        SUMO();
    }

    /* this point of the program is never reached, but we satisfy the
       compiler's warnings by saying we'd return zero */

    return;
}


/**********************************************************************
 *  SUMO
 *
 *  Find and push our opponent out of the ring!
 *
 */

void SUMO()
{
  
   /* start with a five second countdown */

    digitalWrite(2, HIGH); 		/* turn on LED 1 */
    delay(650);    	 	           
    
    digitalWrite(3, HIGH);		/* turn on LED 2 */
    delay(650);                        /* wait 1000 milliseconds = 2nd second */
           		
    digitalWrite(4, HIGH);		/* turn on LED 3 */
    delay(650);                          /* another 1000 milliseconds = 3rd second */
          		
    digitalWrite(5, HIGH);		/* turn on LED 4 */
    delay(650);                          /* another 1000 milliseconds = 4th second */
           		
    digitalWrite(6, HIGH);		/* turn on LED 5 */
    delay(650);                         /* another 1000 milliseconds = 5th second */
         		

    /* match begins! turn off all the leds */

    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);			        /* all LEDs off */
    digitalWrite(5, LOW);
    digitalWrite(6, LOW);

    /* turn on both motors going full forward */

    digitalWrite(9, HIGH);    				/* left  motor enable  */
    digitalWrite(10, HIGH);     			/* right motor enable  */
    digitalWrite(12, HIGH);    				/* left  motor forward */
    digitalWrite(13, HIGH); 				/* right motor forward */

    /* loop here forever, chasing our opponent out of the ring */

    while(1)
    {
      /* read the line sensors to see if we've hit the ring edge */

         linesensor_left  = analogRead(A0);
          linesensor_right = analogRead(A4);

        if ( linesensor_right  < kWhiteThreshold )
        {
            /* ring edge seen on right side; reverse both motors to prevent
               sliding over the edge */

            digitalWrite(12, LOW);
            digitalWrite(13, LOW);
            delay(300); 

            /* now turn around fast by changing the right motor direction */

            digitalWrite(13, HIGH);
            delay(400);
        }

        if ( linesensor_left  < kWhiteThreshold )
        {
            /* ring edge seen on left side; reverse both motors to prevent
               sliding over the edge */

            digitalWrite(12, LOW);
            digitalWrite(13, LOW); 
            delay(300);

            /* now turn around fast by changing the left motor direction */

            digitalWrite(12, HIGH);
            delay(400);
        }

        /* read the forward-looking sensors looking for our opponent */
        
        PD0 = digitalRead(7);
        PD1 = digitalRead(8);
        
        irsensors = ((PD0 * PD1) + PD0 + (PD1 * 2));
        //irsensors = (PD0 & PD1); /* just PD0, PD1 */

        switch ( irsensors )
        {
            case 0:
            {
                /* both sensors see something; full steam ahead! */

                digitalWrite(2, HIGH);        /* turn on LED 1 */
                digitalWrite(6, HIGH);       /* turn on LED 5 */

                digitalWrite(9, HIGH);      /* left  motor enable  */
                digitalWrite(10, HIGH);     /* right motor enable  */
                digitalWrite(12, HIGH);     /* left  motor forward */
                digitalWrite(13, HIGH);     /* right motor forward */
                delay(50);
                break;
            }

            case 1:
            {
                /* opponent in the right sensor */

                digitalWrite(2, LOW);      /* turn off LED 1 */
                digitalWrite(6, HIGH);     /* turn on  LED 5 */

                digitalWrite(10, LOW);     /* right motor disable */
                
                digitalWrite(9, HIGH);    /* left motor enable  */
                digitalWrite(12, HIGH);   /* left motor forward */
                delay(50);
                break;
            }

            case 2:
            {
                /* opponent in the left sensor */

                digitalWrite(2, HIGH);       /* turn on  LED 1 */
                digitalWrite(6, LOW);       /* turn off LED 5 */

                digitalWrite(9, LOW);      /* left  motor disable */

                digitalWrite(10, HIGH);     /* right motor enable  */
                digitalWrite(13, HIGH);     /* right motor forward */
                delay(50);
                break;
            }

            case 4:
            {
                // neither sensor sees anything
                

                digitalWrite(2, LOW);      // turn off LED 1
                digitalWrite(6, LOW);      // turn off LED 5

                digitalWrite(9, HIGH);     // left  motor enable  
                digitalWrite(10, HIGH);    // right motor enable  
                digitalWrite(12, HIGH);    // left  motor forward 
                digitalWrite(13, HIGH);    // right motor forward
                delay(50);
                break;
            }
           
        }
        
        /* loop back to check sensors again */
    }
    /* NEVER REACHED */
}


/*
 *  LINEFOLLOWER
 *
 *  Chase the black line as fast as we can!
 *
 */

void LINEFOLLOWER()
{
    

/* set up Timer1 to provide PWM output to drive the motors */

  
    /* loop here forever, following the black line */

    for ( ;; )
    {
        

        /* read the current sensor values */
        
        linesensor_left     = analogRead(A0);
        linesensor_midleft  = analogRead(A1);
        linesensor_center   = analogRead(A2);
        linesensor_midright = analogRead(A3);
        linesensor_right    = analogRead(A4);

        /* check the outer right sensor */

        if ( linesensor_right < kWhiteThreshold )
        {
            /* seeing white on this sensor */

            digitalWrite(6, HIGH); /* turn on LED 5 */
        }
        else
        {
            /* seeing black on this sensor */

            digitalWrite(6, LOW); /* turn off LED 5 */
            digitalWrite(13, LOW); /* left motor reverse */
            delayMicroseconds(1300);
        }

        /* check the middle-right sensor */

        if ( linesensor_midright < kWhiteThreshold )
        {
            /* seeing white on this sensor */

            digitalWrite(5, HIGH); /* turn on LED 4 */
        }
        else
        {
            /* seeing black on this sensor */

            digitalWrite(5, LOW); /* turn off LED 4 */
            analogWrite(10, 127);
            delayMicroseconds(1300);
        }

        /* check the center sensor */

        if ( linesensor_center < kWhiteThreshold )
        {
            /* seeing white on this sensor */

            digitalWrite(4, HIGH); /* turn on LED 3 */
        }
        else
        {
            /* seeing black on this sensor */

            digitalWrite(4, LOW); /* turn off LED 3 */

            /* both motors forward at max speed */

            digitalWrite(12, HIGH);
            digitalWrite(13, HIGH);
            analogWrite(10, 255);
            analogWrite(9, 255);
            delayMicroseconds(1300);
        }

        /* check the left-middle sensor */

        if ( linesensor_midleft < kWhiteThreshold )
        {
            /* seeing white on this sensor */

            digitalWrite(3, HIGH); /* turn on LED 2 */
        }
        else
        {
            /* seeing black on this sensor */

            digitalWrite(3, LOW); /* turn off LED 2 */
            analogWrite(9, 127);            /* slow down left motor */
            delayMicroseconds(1300);
        }

        /* check the outer left sensor */

        if ( linesensor_left < kWhiteThreshold )
        {
            /* seeing white on this sensor */

            digitalWrite(2, HIGH);  /* turn on LED 1 */
        }
        else
        {
            /* seeing black on this sensor */

            digitalWrite(2, LOW);     /* turn off LED 1 */
            digitalWrite(12, LOW);   /* left motor reverse */
            delayMicroseconds(1300);
        }

        /* loop back to check sensors again */
    }
    /* NEVER REACHED */
}

/************************************ END ***************************************/

