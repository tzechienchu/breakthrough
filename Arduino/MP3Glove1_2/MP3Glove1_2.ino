//2014-10-08
//Add LCD 
#include <SoftwareSerial.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
//IR Use Pin3
#include <IRremote.h> 
IRsend irsend;
#define TV_SAMSUNG_POWER    0xE0E040BF //(samsung 32 bits)
#define TV_SAMSUNG_VOL_UP   0xE0E0E01F //(samsung 32 bits)
#define TV_SAMSUNG_VOL_DOWN 0xE0E0D02F //(samsung 32 bits)
#define TV_SAMSUNG_CH_UP    0xE0E048B7 //(samsung 32 bits)
#define TV_SAMSUNG_CH_DOWN  0xE0E008F7 //(samsung 32 bits)
#define TV_SAMSUNG_1        0xE0E020DF
#define TV_SAMSUNG_2        0xE0E0A05F
#define TV_SAMSUNG_3        0xE0E0609F
#define TV_SAMSUNG_4        0xE0E010EF
#define TV_SAMSUNG_5        0xE0E0906F
#define TV_SAMSUNG_6        0xE0E050AF
#define TV_SAMSUNG_7        0xE0E030CF
#define TV_SAMSUNG_8        0xE0E0B04F
#define TV_SAMSUNG_9        0xE0E0708F
#define TV_SAMSUNG_0        0xE0E08877

#define TFT_CS     6
#define TFT_RST    8  
#define TFT_DC     7
#define TFT_SCLK   4   
#define TFT_MOSI   5   
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

#define LED1   A6
#define LED2   A7

#define FLASHLED2 {digitalWrite(LED2,LOW);delay(500);digitalWrite(LED2,HIGH);}
#define THUMB  output3[0]
#define POINT  output3[1]
#define MIDLE  output3[2]

#define ROWINDEXMAX 2
//Max Number of MP3
#define MAXSOUND    30
//Key Repeat Time Every 2 seconds
#define REPEATCOUNT 200
#define WAITTICK    100
#define MAXCHANNEL  30

const int NUMReadings = 8;
const int NUMChannels = 5;

SoftwareSerial mp3(9, 10);

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

byte startSystem;
int rowIndex;
int columnIndex;
int columnIndexMax[ROWINDEXMAX];

int mp3Channel;
int TVChannel;
int TVPresetIndex;
int TVVol;
int TVPresetChannel[MAXCHANNEL];

long int tick;

void setup()
{
        startSystem = 0;
        pinMode(LED1,OUTPUT);
        pinMode(LED2,OUTPUT);
        
        digitalWrite(LED1,HIGH);
        digitalWrite(LED2,LOW);
        
        Serial.begin(115200); 
        initLCD();
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
    
    if ((output2[0] ==0) && (output2[1] ==0) && (startSystem == 0) &&
        (output2[2] ==0) && (tick > 100)) 
    {
        startSystem = 1;
        rowIndex =0;
        columnIndex = 0;
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
    FunctionController();
    
    tftPrintTest();
    mediabuttons();
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
    Serial.print(rowIndex);
    Serial.print(" ");
    Serial.print(columnIndex);
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
  
  rowIndex = 0;
  columnIndex = 0;
  columnIndexMax[0] = 3;//MP3,UP/DOWN,PLAY
  columnIndexMax[1] = 4;//TV,ON/OFF,CHL,VOL
  //Test Only
  for(int i=0;i<MAXCHANNEL;i++) {
    TVPresetChannel[i] = i+7;
  }
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

void FunctionController()
{
  if (THUMB == 1) {
    columnIndex++;
    if (columnIndex > columnIndexMax[rowIndex]) columnIndex = 0;
    return;
  }
  //At the Header
  if (columnIndex == 0) {
    if (POINT == 1) {
      rowIndex++;
      if (rowIndex >= ROWINDEXMAX) rowIndex = 0;
      return;
    }
    if (MIDLE == 1) {
      if (rowIndex >0) {
        rowIndex--;
      } else {
        if (rowIndex == 0) rowIndex = (ROWINDEXMAX-1);
      }
      return;
    }
  } else {
    if (POINT == 1) {
      rowIndex++;
    }    
    if (MIDLE == 1) {
      rowIndex--;
    }
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

//LCD
void initLCD() {
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST7735_BLACK);
  
}
void tftPrintTest() {
  tft.setTextWrap(false);
  tft.setRotation(1);
  tft.setCursor(0, 0);
  tft.fillScreen(ST7735_BLACK);
  
  tft.setCursor(15, 20);
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(12);
  tft.println("TV");
  delay(1000);
  
  tft.setRotation(1);
  tft.setCursor(0, 0);
  tft.fillScreen(ST7735_BLACK);
  
  tft.setCursor(15, 30);
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(8);
  tft.println("MP3");
  
  tft.setRotation(1);
  tft.setCursor(0, 0);
  tft.fillScreen(ST7735_BLACK);
  
  tft.setCursor(15, 30);
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(8);
  tft.println("VOL");
  
  tft.setRotation(1);
  tft.setCursor(0, 0);
  tft.fillScreen(ST7735_BLACK);
  
  tft.setCursor(15, 30);
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(8);
  tft.println("CHL");
  
  for(int i=0;i<200;i+=31) {
  tft.setRotation(1);
  tft.setCursor(0, 0);
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(15, 30);
  tft.setTextColor(ST7735_BLUE);
  tft.setTextSize(8);
  tft.print(i);
  delay(200);
  }
}

void showButton(int code) 
{
  switch(code) {
    case 1:
      tft.fillScreen(ST7735_BLACK);
      tft.fillRoundRect(25, 10, 78, 60, 8, ST7735_WHITE);
      tft.fillTriangle(42, 20, 42, 60, 90, 40, ST7735_RED);
      break;
    case 2:
      tft.fillRoundRect(25, 90, 78, 60, 8, ST7735_WHITE);
      tft.fillRoundRect(39, 98, 20, 45, 5, ST7735_GREEN);
      tft.fillRoundRect(69, 98, 20, 45, 5, ST7735_GREEN);
      break;
  }
}

void LCDShowControl(int code)
{
  switch(code) {
    case 1:
      tft.setCursor(15, 20);
      tft.setTextColor(ST7735_GREEN);
      tft.setTextSize(12);
      tft.println("TV");
      break;
    case 2:
      tft.setCursor(15, 30);
      tft.setTextColor(ST7735_GREEN);
      tft.setTextSize(8);
      tft.println("MP3");
      break;
  }
}

void MP3Control(int code)
{
  switch(code) {
    case 1 :
       SetMusicPlay(00,mp3Channel);
       break;
    case 2 :
       PauseOnOffCurrentMusic();
       break;
    case 3 :
       mp3Channel = max(mp3Channel++,MAXSOUND);  
       break;
    case 4 :
       mp3Channel = min(mp3Channel--,0);
       break;
  }
}

void TVControl(int code)
{
  
switch(code){
    case 1 : 
      irsend.sendSAMSUNG(TV_SAMSUNG_POWER, 32);
      break;
    case 2 : 
      irsend.sendSAMSUNG(TV_SAMSUNG_VOL_UP, 32);
      break;
    case 3 : 
      irsend.sendSAMSUNG(TV_SAMSUNG_VOL_DOWN, 32);
      break;
    case 4 : 
      //irsend.sendSAMSUNG(TV_SAMSUNG_CH_UP, 32);
      //Next Preset Channel
      break;
    case 5 : 
      //irsend.sendSAMSUNG(TV_SAMSUNG_CH_DOWN, 32);
      break;
  }
}

void showTVChannel(int channel) 
{
  tft.setRotation(1);
  tft.setCursor(0, 0);
  tft.fillScreen(ST7735_BLACK);
  if (channel < 10) {
    tft.setCursor(30, 30);
  } else {
    tft.setCursor(15, 30);
  }
  tft.setTextColor(ST7735_BLUE);
  tft.setTextSize(8);
  tft.print(channel);
  
}

void mediabuttons() {
  // play
  tft.fillScreen(ST7735_BLACK);
  tft.fillRoundRect(25, 10, 78, 60, 8, ST7735_WHITE);
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST7735_RED);
  delay(500);
  // pause
  tft.fillRoundRect(25, 90, 78, 60, 8, ST7735_WHITE);
  tft.fillRoundRect(39, 98, 20, 45, 5, ST7735_GREEN);
  tft.fillRoundRect(69, 98, 20, 45, 5, ST7735_GREEN);
  delay(500);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST7735_BLUE);
  delay(50);
  // pause color
  tft.fillRoundRect(39, 98, 20, 45, 5, ST7735_RED);
  tft.fillRoundRect(69, 98, 20, 45, 5, ST7735_RED);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST7735_GREEN);
}

