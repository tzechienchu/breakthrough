/*****************************************************************************/
//	Function: control the seeedstudo Grove MP3 player
//      Hardware: Grove - Serial MP3 Player
/*******************************************************************************/
#include <SoftwareSerial.h>

#define LED1   4
#define LED2   5
#define FLASHLED2 {digitalWrite(LED2,LOW);delay(500);digitalWrite(LED2,HIGH);}
#define THUMB  output3[0]
#define POINT  output3[1]
#define MIDLE  output3[2]

//Max Number of MP3
#define MAXSOUND 30
//Key Repeat Time Every 2 seconds
#define REPEATCOUNT 200
#define WAITTICK 100

const int NUMReadings = 8;
const int NUMChannels = 5;

SoftwareSerial mp3(2, 3);//modify this with the connector you are using.

int index[NUMChannels];
int total[NUMChannels];
int average[NUMChannels];
int offset[NUMChannels];
int delta[NUMChannels];
int th1[NUMChannels];
int th2[NUMChannels];
byte output1[NUMChannels];
byte output2[NUMChannels];
byte output3[NUMChannels];
byte tickCount[NUMChannels];

int c1[NUMReadings];
int c2[NUMReadings];
int c3[NUMReadings];
int c4[NUMReadings];
int c5[NUMReadings];

int a1[NUMReadings];
int a2[NUMReadings];
int a3[NUMReadings];
int a4[NUMReadings];
int a5[NUMReadings];

int mp3DT1;
int mp3DT2;

long int tick;
int startMP3;

void setup()
{
        startMP3 = 0;
        pinMode(LED1,OUTPUT);
        pinMode(LED2,OUTPUT);
        
        digitalWrite(LED1,HIGH);
        digitalWrite(LED2,LOW);
        
        Serial.begin(115200); 
        initialMP3();
        sampleInitial();
}
void loop()
{
  //for (int i=0;i<1000;i++) 
  {
    //Round Trip around 1ms
    sampleADC(0,c1,a1);
    sampleADC(1,c2,a2);
    sampleADC(2,c3,a3);
    //sampleADC(3,c4,a4);
    //sampleADC(4,c5,a5);
    
    if ((output2[0] ==0) && (output2[1] ==0) && (startMP3 == 0) &&
        (output2[2] ==0) && (tick > 100)) 
    {
        startMP3 = 1;
        mp3DT1 = 0;
        mp3DT2 = 0;
        Serial.print("MP3 Start");
        digitalWrite(LED1,LOW);
        digitalWrite(LED2,HIGH);
    }
    //probeData(output3[0],output3[1],output3[2],output3[3],1);
    //probeData(a1[index[0]],delta[0],output2[0],output3[0],1);//ADC0
    //probeData(a2[index[1]],delta[1],output2[1],output3[1],1);//ADC1
    //Log All Data
    probeData(a1[index[0]],delta[0],output2[0],output3[0],0);
    probeData(a2[index[1]],delta[1],output2[1],output3[1],0);
    probeData(a3[index[2]],delta[2],output2[2],output3[1],1);
    //Check Output and Play MP3
    MP3Controller();
    //Round Trip 30ms
  }
  //while(1);
}
//ADC
void probeData(int d1,int d2,int d3,int d4,int mode)
{ 
  unsigned long time;
  time = millis();
  
  Serial.print(d1);
  Serial.print(" ");
  Serial.print(d2);
  Serial.print(" ");
  Serial.print(d3);
  Serial.print(" ");
  Serial.print(d4);
  Serial.print(" ");
  
  if (mode == 1) {
    Serial.print(mp3DT2);
    Serial.print(" ");
    Serial.print(mp3DT1);
    Serial.print(" ");
    
    Serial.print(THUMB);
    Serial.print(" ");
    Serial.println(time);
  }
}
void sampleInitial()
{
  for (int i=0;i<NUMChannels;i++) {
    index[i] = 0;
    total[i] = 0;
    average[i] = 0;
    offset[i] = 0;
    th1[i] = 48;
    th2[i] = 8;//16
    output1[i] = 0;
    output2[i] = 0;
    output3[i] = 0;
  }
  for (int i=0;i<NUMReadings;i++) {
    c1[i] = 0;
    c2[i] = 0;
    c3[i] = 0;
    c4[i] = 0;
  }
  
  mp3DT1=0;
  mp3DT2=0;
}
int sampleADC(int channel,int temp[],int tempout[])
{
  int indexlocal = index[channel];
  int totallocal = total[channel];
  int reading;
  int averagelocal;
  int offsetlocal;
  float deltalocal = 0.0;
  int head;
  int tail;
  int pout;
  int nout;
  
  tick++;
  totallocal= totallocal - temp[indexlocal];         
  // read from the sensor: 
  reading = analogRead(channel); 
  temp[indexlocal]  = reading;
  // add the reading to the total:
  totallocal= totallocal + reading;                                            
  averagelocal = totallocal / NUMReadings; 
  
  offsetlocal = (offset[channel]*7 + averagelocal)/8;
  
  tempout[indexlocal] = averagelocal;
  
  head = indexlocal;
  indexlocal = indexlocal + 1; 
  if (indexlocal >= NUMReadings) indexlocal = 0; 
  tail = indexlocal;
  
  index[channel] = indexlocal;
  total[channel] = totallocal;
  average[channel] = averagelocal;
  offset[channel] = offsetlocal;
  
  //Delta
  for(int i=0;i<(NUMReadings);i++) {
    deltalocal += tempout[i]-offsetlocal;
  }
  
  delta[channel] = (delta[channel]*3+deltalocal)/4;
  
  pout = output2[channel];
  //Shift into 8Bit Output1
  //Digital Filter
  int lastBit = bitRead(output1[channel],0);
  output1[channel] = output1[channel] << 1;
  if (lastBit == 1) {
    bitSet(output1[channel],0);
  }
  if (lastBit == 0) {
    bitClear(output1[channel],0);
  }
  if (delta[channel] > th1[channel]) {
    bitSet(output1[channel],0);
  }
  if (delta[channel] < th2[channel]) {
    bitClear(output1[channel],0);
  }  
 
  //4 Bits
  //if ((output1[channel] & 0x0F) == 0x00) output2[channel] = 0;
  //if ((output1[channel] & 0x0F) == 0x0F) output2[channel] = 1;
  //8 Bits
  if (output1[channel] == 0x00) {
    output2[channel] = 0;
    tickCount[channel] = 0;
  }
  if (output1[channel] == 0xFF) {
    output2[channel] = 1;
  }
  
  if ((pout == 0) && (output2[channel] == 0)) {
    output3[channel] = 0;
  }  
  if ((pout == 1) && (output2[channel] == 1)) {
    output3[channel] = 0;
  }   
  if ((pout == 0) && (output2[channel] == 1)) {
    output3[channel] = 1;
    if (channel > 0) {
      tickCount[channel] = 1;
    }
  }
  if ((pout == 1) && (output2[channel] == 0)) {
    output3[channel] = 2;
  }  
  if ((tickCount[channel] > 0) && (output2[channel] == 1)) {
    tickCount[channel]++;
  }
  if (tickCount[channel] == REPEATCOUNT) {
    if (channel > 0) {//Except the Thumb
      output3[channel] = 1;
      tickCount[channel] = 1;
    }
  }
  return averagelocal;
}
//Finger to MP3
void MP3Controller()
{
  if (startMP3 == 0) return;
  
  if (POINT ==   1) {
    mp3DT1++;
    FLASHLED2
  }
  if (mp3DT1 >= MAXSOUND) mp3DT1 = 0;
  
  if ((MIDLE == 1) && (mp3DT1 > 0)) {
    mp3DT1--;
    FLASHLED2
  } else {
    if ((MIDLE == 1) && (mp3DT1 == 0)) {
      mp3DT1 = MAXSOUND-1;
      FLASHLED2
    }
  }
  int soundIndex = mp3DT2*10+mp3DT1;
  
  if (THUMB == 1) { 
    SetMusicPlay(00,soundIndex);
    FLASHLED2
    //SetVolume(0x0E);
  }
}
//MP3
void testMP3()
{
  SetMusicPlay(00,01);
  delay(100);
  SetVolume(0x0E);
  delay(5000);
  SetMusicPlay(00,02);
  delay(100);
  SetVolume(0x0E);
  delay(5000);
  SetMusicPlay(00,03);
  delay(100);
  SetVolume(0x0E);
  delay(5000);
}
void initialMP3()
{
	mp3.begin(9600);
	delay(100);

//        if (true ==SetPlayMode(0x01))
//          Serial.println("Set The Play Mode to 0x01, Single Loop Mode.");
//        else
//          Serial.println("Playmode Set Error");
          
        if (true ==SetPlayMode(0x00))
          Serial.println("Play Single Track 1");
        else
          Serial.println("Playmode Set Error");     
          
        PauseOnOffCurrentMusic();  
        delay(1000); 
        SetPlayMode(0x00);//0x01 
        delay(1000);      
}
//Set the music index to play, the index is decided by the input sequence
//of the music;
//hbyte: the high byte of the index;
//lbyte: the low byte of the index;
boolean SetMusicPlay(uint8_t hbyte,uint8_t lbyte)
{
	mp3.write(0x7E);
	mp3.write(0x04);
	mp3.write(0xA0);
	mp3.write(hbyte);
	mp3.write(lbyte);
	mp3.write(0x7E);
        delay(10);
	 while(mp3.available())
	{
	if (0xA0==mp3.read())
	return true;
	else 
	return false;
        }
}
// Pause on/off  the current music
boolean PauseOnOffCurrentMusic(void)
{
   mp3.write(0x7E);
   mp3.write(0x02);
   mp3.write(0xA3);
   mp3.write(0x7E);
   delay(10);
   while(mp3.available())
   {
     if (0xA3==mp3.read())
       return true;
     else 
       return false;
   }
}

//Set the volume, the range is 0x00 to 0x1F
boolean SetVolume(uint8_t volume)
{
   mp3.write(0x7E);
   mp3.write(0x03);
   mp3.write(0xA7);
   mp3.write(volume);
   mp3.write(0x7E);
   delay(10);
   while(mp3.available())
   {
     if (0xA7==mp3.read())
       return true;
     else 
       return false;
   }
}

boolean SetPlayMode(uint8_t playmode)
{
  if (((playmode==0x00)|(playmode==0x01)|(playmode==0x02)|(playmode==0x03))==false)
  {
    Serial.println("PlayMode Parameter Error! ");
    return false;
  }
   mp3.write(0x7E);
   mp3.write(0x03);
   mp3.write(0xA9);
   mp3.write(playmode);
   mp3.write(0x7E);
   delay(10);
   while(mp3.available())
   {
     if (0xA9==mp3.read())
       return true;
     else 
       return false;
   }
}

