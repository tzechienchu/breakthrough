// Pro_Graph2.pde

import processing.serial.*;

Serial myPort;        // The serial port

int lineHeight = 200;
int xHOffset = 30;
int yBOffset = 100;

int dataOffset = 200;
int dataGain = 10;

//Variables to draw a continuous line.
int[] xPos       = new int[4]; 
int[] lastxPos   = new int[4];
int[] lastheight = new int[4];
int[] basePos    = new int[4];
color[] lineColor = new color[4];
float[] data      = new float[4];

int dataLeng=4;
void setup () {
  
  for (int i=0; i<4; i++) {
    xPos[i]       = 1;
    lastxPos[i]   = 1;
    lastheight[i] = 0;
  }
  basePos[0] = 0;
  basePos[1] = lineHeight;
  basePos[2] = lineHeight*2;
  basePos[3] = lineHeight*3;
  
  lineColor[0] = color(131, 255, 20);
  lineColor[1] = color(232, 158, 12);
  lineColor[2] = color(255, 0, 0);
  lineColor[3] = color(62, 12, 232);
  
  // set the window size:
  size(1024, lineHeight*4+yBOffset);        

  // List all the available serial ports
  println(Serial.list());
  // Check the listed serial ports in your machine
  // and use the correct index number in Serial.list()[].

  myPort = new Serial(this, Serial.list()[5], 115200);  //

  // A serialEvent() is generated when a newline character is received :
  myPort.bufferUntil('\n');
  background(0);      // set inital background:
}
void draw () {
  // everything happens in the serialEvent()
}

void serialEvent (Serial myPort) {
  // get the ASCII string:
  String inString = myPort.readStringUntil('\n');
  if (inString != null) {
    
    inString = trim(inString);                // trim off whitespaces.
    println(inString);
    String[] nums = split(inString, ' ');
 
    dataLeng = nums.length;
    if (nums.length >= 4) dataLeng = 4;
    
    for(int i=0;i<dataLeng;i++) {
      
      //Data Preprocessing
      if (i==0) {
        data[i] = (float(nums[i])-dataOffset)*dataGain;  
        //data[i] = (float(nums[i])-0)*256; 
      } 
      if (i==1) {
        data[i] = (float(nums[i])-0)*dataGain;  
        //data[i] = (float(nums[i])-0)*256; 
      } 
      if (i==2) {
        //data[i] = (float(nums[i])-0)*dataGain;
        data[i] = (float(nums[i])-0)*256;  
      }
      if (i==3) {
        //data[i] = (float(nums[i])-0)*dataGain;
        data[i] = (float(nums[i])-0)*256;  
      }
      
      data[i] = map(data[i], 0, 1023, 0, lineHeight); //map to the screen height.
   
      //Drawing a line from Last inByte to the new one.
      stroke(lineColor[i]);    //stroke color
      strokeWeight(2
      );        //stroke wider
      line(lastxPos[i]+xHOffset, lastheight[i]+basePos[i], xPos[i]+xHOffset, (lineHeight - data[i])+basePos[i]); 
      
      lastxPos[i]= xPos[i];
      lastheight[i]= int(lineHeight-data[i]);
  
      // at the edge of the window, go back to the beginning:
      if (xPos[i] >= width-xHOffset) {
        xPos[i] = 0;
        lastxPos[i]= 0;
        background(0);  //Clear the screen.
      } 
      else {
        // increment the horizontal position:
        xPos[i]++;
      }
    }
  }
}

