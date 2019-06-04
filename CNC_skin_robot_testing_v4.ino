/*
 * Adding height compensation in V3 
 * 
 */
int delaymachine = 2000;
int PIN_PUMP = 31;
int PIN_SOLENOID = 30;
int PIN_PRESSURE_SENSOR = A13;
int PIN_IR_SENSOR = A14;
volatile unsigned long incomingByte = 0;
bool startingUp = false;
bool firstPos = true;
bool isSensorAttached = false;
int currentX, currentY;
int sequence = 0;
int pumpDelay = 4000;
int attachment_tries = 0;

int TOTAL_SEQUENCES = 13;
int TOTAL_PASSES = 6;

//make timer that checks serial input
IntervalTimer myTimer;
IntervalTimer sensorReadTimer;

void checkTimer() { 
  if (Serial.available()) {
    incomingByte = Serial.read();
    // do something with incomingByte
    if (incomingByte == 'a') { 
       Serial1.println("G91");     // string
        Serial1.println("G0 Z+0.1"); 
    }
    if (incomingByte == 'z') { 
       Serial1.println("G91");     // string
        Serial1.println("G0 Z-0.1"); 
    }

    if (incomingByte == 's') { 
       Serial1.println("G91");     // string
        Serial1.println("G0 X-1"); 
    }

    if (incomingByte == 'f') { 
       Serial1.println("G91");     // string
        Serial1.println("G0 X+1"); 
    }

    if (incomingByte == 'e') { 
       Serial1.println("G91");     // string
        Serial1.println("G0 Y+1"); 
    }

    if (incomingByte == 'd') { 
       Serial1.println("G91");     // string
        Serial1.println("G0 Y-1"); 
    }

    if (incomingByte =='1') { 
       startingUp = true;
    }

    if (incomingByte=='t') { 
        Serial1.println("G91");     // string
        Serial1.println("G0 Z+5"); 
    }

        if (incomingByte=='g') { 
        Serial1.println("G91");     // string
        Serial1.println("G0 Z-5"); 
    }
        
    
  }
}

void readSensors() {
 int temp = analogRead(PIN_IR_SENSOR); 
 int temp2 =analogRead(PIN_PRESSURE_SENSOR);
 Serial.print("S"); 
 Serial.print(",");
 Serial.print(temp);  
 Serial.print(","); 
 Serial.print(temp2); 
 Serial.print(",");
 Serial.print(currentX); 
 Serial.print(",");
 Serial.print(currentY);
 Serial.print(",");
 Serial.print(sequence);
 Serial.print(",");
 Serial.print(attachment_tries);
 Serial.print(",");
 Serial.println(millis());
}


void sendStopRecording() {
 Serial.print("A"); 
 Serial.print(",");
 Serial.println(millis());
}


void setup() {
  // put your setup code here, to run once:
  Serial1.begin(115200);
  pinMode(PIN_PUMP, OUTPUT); 
  pinMode(PIN_SOLENOID, OUTPUT);
  myTimer.begin(checkTimer, 150000);  // blinkLED to run every 0.15 seconds
  sensorReadTimer.begin(readSensors, 100000);//reading sensor timer
  analogReadResolution(14);

 for (int i=0; i<6; i++) { 
    digitalWrite(PIN_SOLENOID, 0) ;
    delay(300);
    digitalWrite(PIN_SOLENOID, 1); 
    delay(300);
 }
  digitalWrite(PIN_PUMP, 0); 
  digitalWrite(PIN_SOLENOID, 0);




}


void loop() {
  

 if (startingUp) { 
     sequence=0;
     Serial.println("start");
     //push Z-motor slightly down
     // Serial1.println("G0 Z-1"); 

   //number of total tries
   for(int m=0; m<TOTAL_SEQUENCES; m++) {

        //Do the first testing 
        firstPos = true;
        currentX = 0;
        currentY = 0;

        //take the measurement
        //adjust the height first
        check_attachment(); 
             
        //one testing sequence here
        //move suction cup to right position - manual and down on the first spot
        for(int g=0; g<TOTAL_PASSES; g++) {
          for (int i=0; i<11; i++) { 
            Serial1.println("G91");     // string
            Serial1.println("G0 Z+5"); 
            Serial1.println("G91");     // string
            Serial1.println("G0 Y+5"); 
            Serial1.println("G91");     // string
            Serial1.println("G0 Z-5"); 
    
            //testing
            currentX++;

            //take the measurement
            //adjust the height first
            check_attachment();
            
            firstPos = false;
          }
      
          Serial1.println("G0 Z+5"); 
          Serial1.println("G91");     // string
          Serial1.println("G91");     // string
          Serial1.println("G0 X+5"); 
          Serial1.println("G91");     // string
          Serial1.println("G0 Z-5"); 
      
          //testing
          currentY++;
          check_attachment();
        
          for (int h=0; h<11; h++) { 
            Serial1.println("G91");     // string
            Serial1.println("G0 Z+5"); 
            Serial1.println("G91");     // string
            Serial1.println("G0 Y-5"); 
            Serial1.println("G91");     // string
            Serial1.println("G0 Z-5"); 
            
            Serial.print(h); 
            Serial.print(",");
            Serial.println(g+1);

            currentX--;
            check_attachment();
         }

          //Move to next column
          Serial1.println("G0 Z+5"); 
          Serial1.println("G91");     // string
          Serial1.println("G91");     // string
          Serial1.println("G0 X+5"); 
          Serial1.println("G91");     // string
          Serial1.println("G0 Z-5");  
          currentY++;
          //if (g<(TOTAL_PASSES-1) && h<){
            check_attachment();        
        }
        startingUp = false;
        sequence++;
        
        //return to the original position
        Serial1.println("G91");     // string
        Serial1.println("G0 Z+5"); 
        Serial1.println("G91");     // string
        Serial1.println("G0 X-60"); 
        Serial1.println("G91");     // string
        Serial1.println("G0 Z-5"); 
     }//end sequence.
     //done recording
     sendStopRecording();
     Serial1.println("G91");     // string
     Serial1.println("G0 Z+1.5"); 
 }//end starting up


}//end loop

void do_testing() { 
}

void check_attachment() { 

  delay(1000);
  digitalWrite(PIN_PUMP, 1); 
  digitalWrite(PIN_SOLENOID, 0);
  
  while(!isSensorAttached && attachment_tries<20) { 
    attachment_tries++;
    Serial1.println("G91");     // string
    Serial1.println("G0 Z-0.1"); 
    delay(500);
    int temp3 = analogRead(PIN_PRESSURE_SENSOR);
    if (temp3<10500) { 
      isSensorAttached = true;
    }     
  }  
  isSensorAttached = false;

  //Preventing the material from moving up. Experimental ! 
  Serial1.println("G91");     // string
  Serial1.println("G0 Z-0.3"); 
  
  //delay(1000);

  delay(pumpDelay);
  digitalWrite(PIN_SOLENOID, 1);
  digitalWrite(PIN_PUMP, 0); 
  Serial1.println("G91");     // string
  Serial1.println("G0 Z+1.3"); 
  delay(1000);
  attachment_tries = 0;
  
}
