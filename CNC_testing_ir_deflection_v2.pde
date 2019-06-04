/*
Procedure: 
1) move to the left buttom corner
2) Make sure that the piece is compeltely straight in x and y. Do this by moving in x and y directions 
3) Move the suction cup to 1700 IR value
4) start
5) rotate the piece counterclocwise - repeat
6) Do total of 4 times
*/

import processing.serial.*;

Serial myPort;        // The serial port 

float irReading, irReadingPrev, irReadingScaled, irReadingScaledPrev;
float pressureReading, pressurePrev, pressureScaled,pressureScaledPrev;
PrintWriter ch1FileWriter;
int x_pos, y_pos;
PFont f;
PFont fsmall;

boolean done_recording = false; 

void setup()
{
   size(displayWidth, 950, P3D); // animation is much smoother in P2D; text looks better with JAVA2D
   background(0);
   for (int i=0; i<Serial.list().length; i++) { 
     System.out.println(i + " : " + Serial.list()[i]); 
   }//end for
    
    System.out.println("ok");
    myPort = new Serial(this, Serial.list()[13], 1000000);
    frameRate(120);
    
    f = createFont("Arial",90,true);
    fsmall = createFont("Arial",30,true);
    
    textFont(f);  
    
    String shour = String.valueOf(hour());
    String smin = String.valueOf(minute());
     String sday = String.valueOf(day());
     String smonth = String.valueOf(month());
    ch1FileWriter = createWriter(smonth + "_" + sday + "_" + shour + "_" + smin + ".csv"); 
    
    ch1FileWriter.println("n,ir_data, pressure, x_location, y_location, sequence, attachment_tries, time"); //file header
    ch1FileWriter.flush(); // Writes the remaining data to the file
}
void draw() { 
    irReadingScaled = height- map(irReading, 0,4000, 0,height);
    irReadingScaledPrev = height- map(irReadingPrev, 0,4000, 0,height);
    pressureScaled = height - map(pressureReading, 0,16000, 0,height);
    pressureScaledPrev = height - map(pressurePrev, 0,16000, 0,height);

  
   strokeWeight(8);
   stroke(205, 0, 0);      //color of the plotVal graphed
   line((frameCount-1), irReadingScaledPrev, (frameCount), irReadingScaled );
   
   stroke(0, 0, 220);      //color of the plotVal graphed
   line((frameCount-1), pressureScaledPrev, (frameCount), pressureScaled );
   
   irReadingPrev = irReading;
   pressurePrev = pressureReading;
   noStroke();
   fill(0,0,0);
   rect(100+350, 20 + 100-90, 200.0,150.0);
   fill(200,200,200);
   text("position: " + Integer.toString(x_pos) + "," + Integer.toString(y_pos),100, 100);
   
   noStroke();
   fill(50,0,0);
   rect(500+350, 20 + 100-90, 200.0,150.0);
   fill(200,200,200);
   text("IR: " + Float.toString(irReading),700, 100);

   if(done_recording) { 
     text("DONE!!! ",width/2, height/2);
   }
   
    if(frameCount>(width)) {
        frameCount=0;
        clear();
    } 
}


public void keyPressed()
{  
    println("Key code pressed: " + keyCode);  
     //start the sequence
    if (key == '1')
    {
      myPort.write('1'); //begining
    }
   
   //z axis up
    else if (key =='a') { 
      myPort.write('a'); 
    }
    
    //z axis down
    else if (key =='z') { 
      myPort.write('z'); 
    }  
    //x left
   else if (key =='s') { 
      myPort.write('s'); 
    } 
    //x right
   else if (key =='f') { 
      myPort.write('f'); 
   }
   
   //y left
   else if (key =='e') { 
      myPort.write('e'); 
    } 
    //y right
   else if (key =='d') { 
      myPort.write('d'); 
   }
   
   else if (key =='t') { 
      myPort.write('t'); 
   }
   
   else if (key =='g') { 
      myPort.write('g'); 
   }
    
    
}


public void serialEvent (Serial myPort) {
     try {     //Use try, catch to avoid crashing
       // get the ASCII string:
       String inString = myPort.readStringUntil('\n');        
       //System.out.println(inString.charAt(0));
         //System.out.println(inString);
             
         if (inString !=null ){
                inString = trim(inString);
                System.out.println(inString);
                //System.out.println(inString.length());
         }
         
         if (inString != null && inString.length()>0 && inString.charAt(0) == 'S' ) {
           // trim off any whitespace:
           inString = trim(inString);
           String[] list = split(inString, ',');             
           irReading = (float)Integer.parseInt(list[1]); //LSB/100   
           pressureReading = (float)Integer.parseInt(list[2]);
           x_pos = Integer.parseInt(list[3]);
           y_pos = Integer.parseInt(list[4]);
           //System.out.println(irReading);
             //ch1FileWriter.println(inString); // Write the coordinate to the file
             //ch1FileWriter.flush(); // Writes the remaining data to the file
           ch1FileWriter.println(inString); // Write the coordinate to the file
           ch1FileWriter.flush(); // Writes the remaining data to the file
         }//end if        
         
         //finished command - stop recording
         if (inString != null && inString.length()>0 && inString.charAt(0) == 'A' ) {
             ch1FileWriter.flush(); // Writes the remaining data to the file
             ch1FileWriter.close(); // Finishes the file
             done_recording = true;
         }

      } catch (Exception e){ 
        System.out.println("ErrorSerial: " + e);
      }  
}//end SerialEvent
